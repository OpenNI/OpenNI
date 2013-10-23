/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnModuleLoader.h"
#include "XnRecorderImpl.h"
#include "XnPlayerImpl.h"
#include <XnOpenNI.h>
#include <XnOS.h>
#include <XnLog.h>
#include <XnVersion.h>
#include "XnXml.h"
#include "XnEnum.h"
#include "XnInternalTypes.h"
#include <XnInternalDefs.h>
#include <XnCppWrapper.h>
#include <XnCodecIDs.h>
#include "XnLicensingInternal.h"
#include "XnMockNotifier.h"
#include "XnNodeWatcher.h"
#include "xnInternalFuncs.h"
#include <math.h>
#include <XnPropNames.h>
#include "XnTypeManager.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_FRAME_SYNC_THRESHOLD		3000
#define XN_NODE_FPS_CALC_SAMPLES	90
#define XN_MASK_FPS					"FPS"
#define XN_DUMP_MASK_REF_COUNT		"OpenNIRefCount"
#define XN_DUMP_MASK_DATA_FLOW		"OpenNIDataFlow"
#define XN_BASE_NUMBER_OF_GESTURES 2

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------

#define XN_VALIDATE_FUNC_PTR_RET(ptr, retVal)	\
	if (ptr == NULL)							\
	{											\
		return retVal;							\
	}

#define XN_VALIDATE_FUNC_PTR(ptr)											\
	XN_VALIDATE_FUNC_PTR_RET(ptr, XN_STATUS_INVALID_OPERATION)

#define XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, nodeType, retVal)						\
	if (!hInstance->pTypeHierarchy->IsSet(nodeType))	\
	{																					\
		return retVal;																	\
	}

#define XN_VALIDATE_INTERFACE_TYPE(hInstance, nodeType)	\
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, nodeType, XN_STATUS_INVALID_OPERATION)

#define XN_VALIDATE_CHANGES_ALLOWED(hInstance)	\
	if (!xnAreChangesAllowed(hInstance))		\
	{											\
		return (XN_STATUS_NODE_IS_LOCKED);		\
	}

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
static void xnContextDestroy(XnContext* pContext, XnBool bForce = FALSE);
static XnStatus xnStartGeneratingImpl(XnNodeHandle hInstance);
static XnStatus xnUpdateDataImpl(XnNodeHandle hInstance);
static void xnUpdateMetaData(XnNodeHandle hNode);
static XnStatus xnFreeProductionNodeImpl(XnNodeHandle hNode, XnStatus nRetVal = XN_STATUS_OK);
static void xnDestroyProductionNodeImpl(XnNodeHandle hNode);
static XnBool xnIsNewDataAvailableImpl(XnNodeHandle hInstance, XnUInt64* pnTimestamp);
static XnStatus xnRegisterToLockChangeImpl(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
static void xnUnregisterFromLockChangeImpl(XnNodeHandle hInstance, XnCallbackHandle hCallback);
static XnBool xnGetLockStateImpl(XnNodeHandle hInstance);
static XnStatus xnCreateMockNodeImpl(XnContext* pContext, XnProductionNodeType type, const XnChar* strName, XnNodeHandle* phNode);
static XnStatus xnInitCodec(XnNodeHandle hCodec, XnNodeHandle hInitializerNode);
static XnBool xnIsFrameSyncedWithImpl(XnNodeHandle hInstance, XnNodeHandle hOther);

//---------------------------------------------------------------------------
// Static Variables
//---------------------------------------------------------------------------
XnLogger* g_logger = xnLoggerOpen(XN_MASK_OPEN_NI);

//---------------------------------------------------------------------------
// Initialization / Deinitialization
//---------------------------------------------------------------------------

static void xnDumpRefCount(XnContext* pContext, XnNodeHandle hNode, XnUInt32 nRefCount, const XnChar* strComment)
{
	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);
	const XnChar* strName = "Context";
	if (hNode != NULL)
	{
		strName = hNode->pNodeInfo->strInstanceName;
	}
	const XnChar* strActualComment = strComment;
	if (strActualComment == NULL)
	{
		strActualComment = "";
	}
	xnDumpFileWriteString(pContext->pDumpRefCount, "%llu,%s,%u,%s\n", nNow, strName, nRefCount, strActualComment);
}

XN_C_API XnStatus xnInit(XnContext** ppContext)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_OUTPUT_PTR(ppContext);

	// make sure xnOS is initialized
	nRetVal = xnOSInit();
	if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_OS_ALREADY_INIT)
	{
		return (nRetVal);
	}

	// and also log system
	xnLogInitSystem();

	*ppContext = NULL;

	// allocate context
	XnContext* pContext;
	XN_VALIDATE_NEW(pContext, XnContext);

	// initialize members
	pContext->bGlobalMirrorSet = FALSE;
	pContext->bGlobalMirror = FALSE;
	pContext->globalErrorState = XN_STATUS_OK;
	pContext->hNewDataEvent = NULL;
	pContext->nLastLockID = 0;
	pContext->nRefCount = 1;
	pContext->hLock = 0;
	pContext->pDumpRefCount = xnDumpFileOpen(XN_DUMP_MASK_REF_COUNT, "RefCount.csv");
	pContext->pDumpDataFlow = xnDumpFileOpen(XN_DUMP_MASK_DATA_FLOW, "DataFlow.csv");
	pContext->hPlayerNode = NULL;

	xnDumpFileWriteString(pContext->pDumpRefCount, "Timestamp,Object,RefCount,Comment\n");
	xnDumpFileWriteString(pContext->pDumpDataFlow, "Timestamp,Action,Object,DataTimestamp\n");

	nRetVal = xnFPSInit(&pContext->readFPS, XN_NODE_FPS_CALC_SAMPLES);
	if (nRetVal != XN_STATUS_OK)
	{
		xnContextDestroy(pContext);
		return (nRetVal);
	}

	nRetVal = xnOSCreateEvent(&pContext->hNewDataEvent, FALSE);
	if (nRetVal != XN_STATUS_OK)
	{
		xnContextDestroy(pContext);
		return (nRetVal);
	}

	nRetVal = xnOSCreateCriticalSection(&pContext->hLock);
	if (nRetVal != XN_STATUS_OK)
	{
		xnContextDestroy(pContext);
		return (nRetVal);
	}

	nRetVal = xnNodeInfoListAllocate(&pContext->pOwnedNodes);
	if (nRetVal != XN_STATUS_OK)
	{
		xnContextDestroy(pContext);
		return (nRetVal);
	}

	// Initialize module loader
	nRetVal = pContext->moduleLoader.Init();
	if (nRetVal != XN_STATUS_OK)
	{
		xnContextDestroy(pContext);
		return (nRetVal);
	}

	// load global licenses
	nRetVal = xnLoadGlobalLicenses(pContext);
	if (nRetVal != XN_STATUS_OK)
	{
		xnContextDestroy(pContext);
		return (nRetVal);
	}

	xnDumpRefCount(pContext, NULL, 1, "Create");

	// return to caller
	*ppContext = pContext;
	
	return (XN_STATUS_OK);
}

static void xnFindValidName(XnContext* pContext, const XnChar* strBaseName, XnChar* strName)
{
	XnUInt i = 1;

	XnInternalNodeData* pNode;

	for (;;)
	{
		sprintf(strName, "%s%u", strBaseName, i);

		// check if name already exists
		if (pContext->nodesMap.Get(strName, pNode) != XN_STATUS_OK)
		{
			// found it
			break;
		}

		++i;
	}
}

static void xnFindValidNameForType(XnContext* pContext, XnProductionNodeType Type, XnChar* strName)
{
	// get type string
	const XnChar* strTypeName = xnProductionNodeTypeToString(Type);
	xnFindValidName(pContext, strTypeName, strName);
}

static void xnMarkOwnedNode(XnContext* pContext, XnNodeHandle hNode)
{
	hNode->bIsOwnedByContext = TRUE;
	xnNodeInfoListAddNode(pContext->pOwnedNodes, hNode->pNodeInfo);
	xnProductionNodeAddRef(hNode);
}

static void xnResetVisitState(XnContext* pContext)
{
	for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
	{
		it->Value()->bWasVisited = FALSE;
	}
}

XN_C_API XnStatus xnContextRunXmlScriptFromFile(XnContext* pContext, const XnChar* strFileName, XnEnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnNodeHandle hScriptNode;
	nRetVal = xnContextRunXmlScriptFromFileEx(pContext, strFileName, pErrors, &hScriptNode);
	XN_IS_STATUS_OK(nRetVal);

	// take ownership of created node
	xnMarkOwnedNode(pContext, hScriptNode);
	xnProductionNodeRelease(hScriptNode);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnContextRunXmlScriptFromFileEx(XnContext* pContext, const XnChar* strFileName, XnEnumerationErrors* pErrors, XnNodeHandle* phScriptNode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_INPUT_PTR(strFileName);
	XN_VALIDATE_OUTPUT_PTR(phScriptNode);

	*phScriptNode = NULL;

	XnNodeHandle hScriptNode;
	nRetVal = xnCreateScriptNode(pContext, XN_SCRIPT_FORMAT_XML, &hScriptNode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnLoadScriptFromFile(hScriptNode, strFileName);
	if (nRetVal != XN_STATUS_OK)
	{
		xnProductionNodeRelease(hScriptNode);
		return (nRetVal);
	}

	nRetVal = xnScriptNodeRun(hScriptNode, pErrors);
	if (nRetVal != XN_STATUS_OK)
	{
		xnProductionNodeRelease(hScriptNode);
		return (nRetVal);
	}

	*phScriptNode = hScriptNode;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnContextRunXmlScriptEx(XnContext* pContext, const XnChar* xmlScript, XnEnumerationErrors* pErrors, XnNodeHandle* phScriptNode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_INPUT_PTR(xmlScript);
	XN_VALIDATE_OUTPUT_PTR(phScriptNode);

	*phScriptNode = NULL;

	XnNodeHandle hScriptNode;
	nRetVal = xnCreateScriptNode(pContext, XN_SCRIPT_FORMAT_XML, &hScriptNode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnLoadScriptFromString(hScriptNode, xmlScript);
	if (nRetVal != XN_STATUS_OK)
	{
		xnProductionNodeRelease(hScriptNode);
		return (nRetVal);
	}

	nRetVal = xnScriptNodeRun(hScriptNode, pErrors);
	if (nRetVal != XN_STATUS_OK)
	{
		xnProductionNodeRelease(hScriptNode);
		return (nRetVal);
	}

	*phScriptNode = hScriptNode;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnContextRunXmlScript(XnContext* pContext, const XnChar* xmlScript, XnEnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnNodeHandle hScriptNode;
	nRetVal = xnContextRunXmlScriptEx(pContext, xmlScript, pErrors, &hScriptNode);
	XN_IS_STATUS_OK(nRetVal);

	// take ownership of created node
	xnMarkOwnedNode(pContext, hScriptNode);
	xnProductionNodeRelease(hScriptNode);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnInitFromXmlFileEx(const XnChar* strFileName, XnContext** ppContext, XnEnumerationErrors* pErrors, XnNodeHandle* phScriptNode)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(strFileName);
	XN_VALIDATE_OUTPUT_PTR(ppContext);
	XN_VALIDATE_OUTPUT_PTR(phScriptNode);

	*ppContext = NULL;
	*phScriptNode = NULL;

	nRetVal = xnLogInitFromXmlFile(strFileName);
	XN_IS_STATUS_OK(nRetVal);

	XnContext* pContext;
	nRetVal = xnInit(&pContext);
	XN_IS_STATUS_OK(nRetVal);

 	nRetVal = xnContextRunXmlScriptFromFileEx(pContext, strFileName, pErrors, phScriptNode);
	if (nRetVal != XN_STATUS_OK)
	{
		xnContextRelease(pContext);
		return (nRetVal);
	}

	*ppContext = pContext;
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnInitFromXmlFile(const XnChar* strFileName, XnContext** ppContext, XnEnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnNodeHandle hScriptNode;
	nRetVal = xnInitFromXmlFileEx(strFileName, ppContext, pErrors, &hScriptNode);
	XN_IS_STATUS_OK(nRetVal);
	
	// take ownership of created nodes
	xnMarkOwnedNode(*ppContext, hScriptNode);
	xnProductionNodeRelease(hScriptNode);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnContextAddRef(XnContext* pContext)
{
	XN_VALIDATE_INPUT_PTR(pContext);
	XnAutoCSLocker lock(pContext->hLock);
	pContext->nRefCount++;
	xnDumpRefCount(pContext, NULL, pContext->nRefCount, NULL);
	return XN_STATUS_OK;
}

XN_C_API void xnContextRelease(XnContext* pContext)
{
	XN_ASSERT(pContext != NULL);

	// decrease ref count in a lock
	XnAutoCSLocker lock(pContext->hLock);
	pContext->nRefCount--;
	xnDumpRefCount(pContext, NULL, pContext->nRefCount, NULL);

	if (pContext->nRefCount == 0)
	{
		lock.Unlock();
		xnContextDestroy(pContext);
	}
}

/** Checks if a node is needed by another. */
static XnBool xnIsNeeded(XnContext* pContext, XnNodeHandle hNode)
{
	for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
	{
		// search all needed ones to see if one of them is what we're looking for
		XnInternalNodeData* pData = it->Value();
		for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pData->pNodeInfo->pNeededTrees);
			xnNodeInfoListIteratorIsValid(it);
			it = xnNodeInfoListGetNext(it))
		{
			if (hNode->pNodeInfo == xnNodeInfoListGetCurrent(it))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

static void xnContextDestroy(XnContext* pContext, XnBool bForce /* = FALSE */)
{
	if (pContext != NULL)
	{
		xnDumpRefCount(pContext, NULL, 0, "Destroy");
		xnDumpFileClose(pContext->pDumpRefCount);

		// we have to destroy nodes from top to bottom. So we'll go over the list, each time removing
		// nodes that nobody needs, until the list is empty
		while (!pContext->nodesMap.IsEmpty())
		{
			for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
			{
				if (!xnIsNeeded(pContext, it->Value()))
				{
					xnDestroyProductionNodeImpl(it->Value());
					break;
				}
			}
		}

		if (bForce)
		{
			// raise the shutdown event *after* all nodes have been destroyed (some nodes might call the
			// context or other nodes when destroying)
			pContext->shutdownEvent.Raise(pContext);
		}

		xnLoggerInfo(g_logger, "Destroying context");

		xnNodeInfoListFree(pContext->pOwnedNodes);
		xnOSCloseCriticalSection(&pContext->hLock);
		xnOSCloseEvent(&pContext->hNewDataEvent);
		xnFPSFree(&pContext->readFPS);
		XN_DELETE(pContext);

#ifdef XN_MEM_PROFILING
	#ifdef _WIN32
		xnOSWriteMemoryReport("C:\\xnMemProf.txt");
	#else
		//TODO: Something for Linux
	#endif
#endif
	}
}

XN_C_API void xnShutdown(XnContext* pContext)
{
	xnForceShutdown(pContext);
}

XN_C_API void xnForceShutdown(XnContext* pContext)
{
	xnContextDestroy(pContext, TRUE);
}

XN_C_API XnStatus xnContextRegisterForShutdown(XnContext* pContext, XnContextShuttingDownHandler pHandler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_INPUT_PTR(pHandler);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	return pContext->shutdownEvent.Register(pHandler, pCookie, *phCallback);
}

XN_C_API void xnContextUnregisterFromShutdown(XnContext* pContext, XnCallbackHandle hCallback)
{
	XN_ASSERT(pContext != NULL);
	XN_ASSERT(hCallback != NULL);

	if (pContext == NULL || hCallback == NULL)
	{
		return;
	}

	XnStatus nRetVal = pContext->shutdownEvent.Unregister(hCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_ASSERT(FALSE);
	}
}

XN_C_API XnStatus xnContextOpenFileRecordingEx(XnContext* pContext, const XnChar* strFileName, XnNodeHandle* phPlayerNode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_INPUT_PTR(strFileName);
	XN_VALIDATE_OUTPUT_PTR(phPlayerNode);

	*phPlayerNode = NULL;

	const char* strExt = strrchr(strFileName, '.');
	if (strExt == NULL)
	{
		//No extension
		return XN_STATUS_BAD_FILE_EXT;
	}
	strExt++;

	XnNodeHandle hPlayer;
	nRetVal = xnCreatePlayer(pContext, strExt, &hPlayer);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetPlayerSource(hPlayer, XN_RECORD_MEDIUM_FILE, strFileName);
	if (nRetVal != XN_STATUS_OK)
	{
		xnProductionNodeRelease(hPlayer);
		return (nRetVal);
	}

	*phPlayerNode = hPlayer;
	return XN_STATUS_OK;
}

XN_C_API XnStatus xnContextOpenFileRecording(XnContext* pContext, const XnChar* strFileName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnNodeHandle hPlayer;
	nRetVal = xnContextOpenFileRecordingEx(pContext, strFileName, &hPlayer);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnNodeInfoListAddNode(pContext->pOwnedNodes, hPlayer->pNodeInfo);
	if (nRetVal != XN_STATUS_OK)
	{
		xnProductionNodeRelease(hPlayer);
		return (nRetVal);
	}
	
	return (XN_STATUS_OK);
}

//---------------------------------------------------------------------------
// Utilities
//---------------------------------------------------------------------------

XN_C_API XnStatus xnGetVersion(XnVersion* pVersion)
{
	XN_VALIDATE_INPUT_PTR(pVersion);
	pVersion->nMajor = XN_MAJOR_VERSION;
	pVersion->nMinor = XN_MINOR_VERSION;
	pVersion->nMaintenance = XN_MAINTENANCE_VERSION;
	pVersion->nBuild = XN_BUILD_VERSION;
	return (XN_STATUS_OK);
}

XN_C_API const XnChar* xnProductionNodeTypeToString(XnProductionNodeType Type)
{
	const XnChar* strName;
	XnStatus nRetVal = TypeManager::GetInstance().GetTypeName(Type, &strName);
	if (nRetVal != XN_STATUS_OK)
	{
		return "Unknown";
	}

	return strName;
}

XN_C_API XnStatus xnProductionNodeTypeFromString(const XnChar* strType, XnProductionNodeType* pType)
{
	XN_VALIDATE_INPUT_PTR(strType);
	XN_VALIDATE_OUTPUT_PTR(pType);

	return TypeManager::GetInstance().GetTypeByName(strType, pType);
}

XN_C_API XnStatus xnAutoEnumerateOverSingleInput(XnContext* pContext, XnNodeInfoList* pList, XnProductionNodeDescription* pDescription, const XnChar* strCreationInfo, XnProductionNodeType InputType, XnEnumerationErrors* pErrors, XnNodeQuery* pQuery)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pList);
	XN_VALIDATE_INPUT_PTR(pDescription);

	// enumerate needed node
	XnNodeInfoList* pInputList;
	nRetVal = xnEnumerateProductionTrees(pContext, InputType, pQuery, &pInputList, pErrors);
	if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_NO_NODE_PRESENT)
	{
		return (nRetVal);
	}

	if (nRetVal == XN_STATUS_OK)
	{
		// now, for each found node, add one to the list
		for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pInputList);
			xnNodeInfoListIteratorIsValid(it);
			it = xnNodeInfoListGetNext(it))
		{
			// create needed nodes list
			XnNodeInfoList* pNeeded;
			nRetVal = xnNodeInfoListAllocate(&pNeeded);
			if (nRetVal != XN_STATUS_OK)
			{
				xnNodeInfoListFree(pInputList);
				return (nRetVal);
			}

			// add this input node to it
			nRetVal = xnNodeInfoListAddNodeFromList(pNeeded, it);
			if (nRetVal != XN_STATUS_OK)
			{
				xnNodeInfoListFree(pInputList);
				xnNodeInfoListFree(pNeeded);
				return (nRetVal);
			}

			// and add to result list
			nRetVal = xnNodeInfoListAdd(pList, pDescription, strCreationInfo, pNeeded);
			if (nRetVal != XN_STATUS_OK)
			{
				xnNodeInfoListFree(pInputList);
				xnNodeInfoListFree(pNeeded);
				return (nRetVal);
			}

			xnNodeInfoListFree(pNeeded);
		}

		// free input nodes list
		xnNodeInfoListFree(pInputList);
	}
	
	return (XN_STATUS_OK);
}

XnStatus xnGetNeededNodeData(XnNodeHandle hNode, XnNodeHandle hNeededNode, XnNeededNodeData** ppData)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnNeededNodeData* pData;
	if (XN_STATUS_OK != hNode->pNeededNodesDataHash->Get(hNeededNode, pData))
	{
		XN_VALIDATE_CALLOC(pData, XnNeededNodeData, 1);
		nRetVal = hNode->pNeededNodesDataHash->Set(hNeededNode, pData);
		if (nRetVal != XN_STATUS_OK)
		{
			xnOSFree(pData);
			return (nRetVal);
		}
	}

	*ppData = pData;
	
	return (XN_STATUS_OK);
}

/*
XN_C_API XnStatus xnIsNewDataAvailableFromAnyNeededNode(XnContext* pContext, const XnChar* strInstanceName, XnNodeHandle* aNeededNodes, XnUInt nCount)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// find this node
	XnNodeHandle hNode;
	nRetVal = xnGetNodeHandleByName(pContext, strInstanceName, &hNode);
	XN_IS_STATUS_OK(nRetVal);	

	// check if needed nodes were provided
	XnNodeHandle aTempNeeded[200];
	if (aNeededNodes == NULL)
	{
		nCount = 0;

		// add needed ones
		XnNodeInfoList* pNeeded = hNode->pNodeInfo->pNeededTrees;
		for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pNeeded);
			xnNodeInfoListIteratorIsValid(it);
			it = xnNodeInfoListGetNext(it))
		{
			aTempNeeded[nCount++] = xnNodeInfoListGetCurrent(it)->hNode;
		}

		aNeededNodes = aTempNeeded;
	}

	// now, for each needed node, wait for new data
	for (XnUInt i = 0; i < nCount; ++i)
	{
		// take its data
		XnNeededNodeData* pData;
		nRetVal = xnGetNeededNodeData(hNode, aNeededNodes[i], &pData);
		XN_IS_STATUS_OK(nRetVal);	

		// check timestamp
		if (xnGetTimestamp(aNeededNodes[i]) == pData->nLastReadTimestamp)
		{
			// timestamp hasn't changed. wait for new data
			nRetVal = xnWaitAndUpdateData(aNeededNodes[i]);
			XN_IS_STATUS_OK(nRetVal);

			// update our stored timestamp
			pData->nLastReadTimestamp = xnGetTimestamp(aNeededNodes[i]);
		}
	}
	
	return (XN_STATUS_OK);
}
*/

typedef struct XnResolutionInfo
{
	XnResolution Res;
	XnUInt32 nXRes;
	XnUInt32 nYRes;
	const XnChar* strName;
} XnResolutionInfo;

#define _XN_REGISTER_RES(x)	\
	{ XN_RES_##x, XN_##x##_X_RES, XN_##x##_Y_RES, XN_STRINGIFY(x) },

static XnResolutionInfo g_resInfo[] = 
{
	_XN_REGISTER_RES(QQVGA)
	_XN_REGISTER_RES(CGA)
	_XN_REGISTER_RES(QVGA)
	_XN_REGISTER_RES(VGA)
	_XN_REGISTER_RES(SVGA)
	_XN_REGISTER_RES(XGA)
	_XN_REGISTER_RES(720P)
	_XN_REGISTER_RES(SXGA)
	_XN_REGISTER_RES(UXGA)
	_XN_REGISTER_RES(1080P)
	_XN_REGISTER_RES(QCIF)
	_XN_REGISTER_RES(240P)
	_XN_REGISTER_RES(CIF)
	_XN_REGISTER_RES(WVGA)
	_XN_REGISTER_RES(480P)
	_XN_REGISTER_RES(576P)
	_XN_REGISTER_RES(DV)
};

XN_C_API XnUInt32 xnResolutionGetXRes(XnResolution resolution)
{
	for (XnUInt i = 0; i < sizeof(g_resInfo)/sizeof(XnResolutionInfo); ++i)
	{
		if (g_resInfo[i].Res == resolution)
		{
			return g_resInfo[i].nXRes;
		}
	}

	return 0;
}

XN_C_API XnUInt32 xnResolutionGetYRes(XnResolution resolution)
{
	for (XnUInt i = 0; i < sizeof(g_resInfo)/sizeof(XnResolutionInfo); ++i)
	{
		if (g_resInfo[i].Res == resolution)
		{
			return g_resInfo[i].nYRes;
		}
	}

	return 0;
}

XN_C_API XnResolution xnResolutionGetFromXYRes(XnUInt32 xRes, XnUInt32 yRes)
{
	for (XnUInt i = 0; i < sizeof(g_resInfo)/sizeof(XnResolutionInfo); ++i)
	{
		if (g_resInfo[i].nXRes == xRes && g_resInfo[i].nYRes == yRes)
		{
			return g_resInfo[i].Res;
		}
	}

	return XN_RES_CUSTOM;
}

XN_C_API XnResolution xnResolutionGetFromName(const XnChar* strName)
{
	for (XnUInt i = 0; i < sizeof(g_resInfo)/sizeof(XnResolutionInfo); ++i)
	{
		if (strcmp(g_resInfo[i].strName, strName) == 0)
		{
			return g_resInfo[i].Res;
		}
	}

	return XN_RES_CUSTOM;
}

XN_C_API const XnChar* xnResolutionGetName(XnResolution resolution)
{
	for (XnUInt i = 0; i < sizeof(g_resInfo)/sizeof(XnResolutionInfo); ++i)
	{
		if (g_resInfo[i].Res == resolution)
		{
			return g_resInfo[i].strName;
		}
	}

	return "Unknown";
}

void xnMarkFPSFrame(XnContext* pContext, XnFPSData* pFPS)
{
	if (!xnLogIsEnabled(XN_MASK_FPS, XN_LOG_VERBOSE))
		return;

	static XnUInt64 nLastPrint = 0;

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);

	xnFPSMarkFrame(pFPS, nNow);

	// get current time in seconds
	nNow /= 1000000;

	if (nNow != nLastPrint)
	{
		nLastPrint = nNow;

		// build string
		XnChar strFPS[1024];
		strFPS[0] = '\0';
		XnUInt32 nSize = 0;
		nSize = sprintf(strFPS, "[FPS] ");

		for (XnNodesMap::ConstIterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
		{
			XnInternalNodeData* pNode = it->Value();
			if (pNode->pModuleInstance->pLoaded->pInterface->HierarchyType.IsSet(XN_NODE_TYPE_GENERATOR))
			{
				nSize += sprintf(strFPS + nSize, "%s (I: %5.2f, O: %5.2f) ", pNode->pNodeInfo->strInstanceName, xnFPSCalc(&pNode->genFPS), xnFPSCalc(&pNode->readFPS));
			}
		}

		sprintf(strFPS + nSize, "ReadCalls: %5.2f", xnFPSCalc(&pContext->readFPS));

		xnLogVerbose(XN_MASK_FPS, strFPS);
	}
}

//---------------------------------------------------------------------------
// ProductionNode
//---------------------------------------------------------------------------

XnStatus xnNodeInfoAllocate(const XnProductionNodeDescription* pDescription, const XnChar* strCreationInfo, XnNodeInfoList* pNeededNodes, XnNodeInfo** ppNodeInfo)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pDescription);
	XN_VALIDATE_OUTPUT_PTR(ppNodeInfo);

	XnNodeInfo* pInfo;
	XN_VALIDATE_CALLOC(pInfo, XnNodeInfo, 1);

	pInfo->nRefCount = 1;
	pInfo->Description = *pDescription;

	if (strCreationInfo != NULL)
	{
		strncpy(pInfo->strCreationInfo, strCreationInfo, XN_MAX_CREATION_INFO_LENGTH);
	}

	// allocate a list for this node
	nRetVal = xnNodeInfoListAllocate(&pInfo->pNeededTrees);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pInfo);
		return (nRetVal);
	}

	// and copy all elements from original list
	if (pNeededNodes != NULL)
	{
		for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pNeededNodes);
			xnNodeInfoListIteratorIsValid(it);
			it = xnNodeInfoListGetNext(it))
		{
			xnNodeInfoListAddNodeFromList(pInfo->pNeededTrees, it);
		}
	}

	*ppNodeInfo = pInfo;
	
	return (XN_STATUS_OK);
}

void xnNodeInfoFree(XnNodeInfo* pNodeInfo)
{
	if (pNodeInfo != NULL)
	{
		// decrease ref count
		--pNodeInfo->nRefCount;

		// if ref count reached 0, destroy
		if (pNodeInfo->nRefCount == 0)
		{
			if (pNodeInfo->hNode != NULL)
			{
				xnProductionNodeRelease(pNodeInfo->hNode);
			}

			// free additional data
			if (pNodeInfo->pAdditionalDataFreeFunc != NULL)
			{
				pNodeInfo->pAdditionalDataFreeFunc(pNodeInfo->pAdditionalData);
			}

			// free all needed nodes
			xnNodeInfoListFree(pNodeInfo->pNeededTrees);
			// and free memory
			xnOSFree(pNodeInfo);
		}
	}
}

XN_C_API XnStatus xnNodeInfoSetInstanceName(XnNodeInfo* pNodeInfo, const XnChar* strInstanceName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pNodeInfo);
	XN_VALIDATE_INPUT_PTR(strInstanceName);

	if (pNodeInfo->hNode != NULL)
	{
		return (XN_STATUS_INVALID_OPERATION);
	}

	nRetVal = xnOSStrCopy(pNodeInfo->strInstanceName, strInstanceName, XN_MAX_NAME_LENGTH);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnNodeInfoSetAdditionalData(XnNodeInfo* pNodeInfo, const void* pAdditionalData, XnFreeHandler pFreeHandler)
{
	XN_VALIDATE_INPUT_PTR(pNodeInfo);

	pNodeInfo->pAdditionalData = pAdditionalData;
	pNodeInfo->pAdditionalDataFreeFunc = pFreeHandler;

	return XN_STATUS_OK;
}

XN_C_API const XnProductionNodeDescription* xnNodeInfoGetDescription(XnNodeInfo* pNodeInfo)
{
	XN_VALIDATE_PTR(pNodeInfo, NULL);
	return &pNodeInfo->Description;
}

XN_C_API const XnChar* xnNodeInfoGetInstanceName(XnNodeInfo* pNodeInfo)
{
	XN_VALIDATE_PTR(pNodeInfo, NULL);
	return pNodeInfo->strInstanceName;
}

XN_C_API const XnChar* xnNodeInfoGetCreationInfo(XnNodeInfo* pNodeInfo)
{
	XN_VALIDATE_PTR(pNodeInfo, NULL);
	return pNodeInfo->strCreationInfo;
}

XN_C_API XnNodeInfoList* xnNodeInfoGetNeededNodes(XnNodeInfo* pNodeInfo)
{
	XN_VALIDATE_PTR(pNodeInfo, NULL);
	return pNodeInfo->pNeededTrees;
}

XN_C_API XnNodeHandle xnNodeInfoGetHandle(XnNodeInfo* pNodeInfo)
{
	XN_VALIDATE_PTR(pNodeInfo, NULL);
	return pNodeInfo->hNode;
}

XN_C_API XnNodeHandle xnNodeInfoGetRefHandle(XnNodeInfo* pNodeInfo)
{
	XN_VALIDATE_PTR(pNodeInfo, NULL);
	if (pNodeInfo->hNode != NULL)
	{
		XnStatus nRetVal = xnProductionNodeAddRef(pNodeInfo->hNode);
		if (nRetVal != XN_STATUS_OK)
		{
			XN_ASSERT(FALSE);
	}
	}
	return pNodeInfo->hNode;
}

XN_C_API const void* xnNodeInfoGetAdditionalData(XnNodeInfo* pNodeInfo)
{
	return pNodeInfo->pAdditionalData;
}

XnStatus xnNodeInfoGetTreeStringRepresentationImpl(XnNodeInfo* pNodeInfo, XnChar* csResult, XnUInt32 nSize, XnUInt32 nDepth)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnUInt32 nLen = 0;
	XnUInt32 nWritten = 0;

	XnChar strIndentation[100] = "";
	if (nDepth >= 100)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_INTERNAL_BUFFER_TOO_SMALL;
	}

	for (XnUInt32 i = 0; i < nDepth; ++i)
	{
		strIndentation[i] = '\t';
	}

	nRetVal = xnOSStrCopy(csResult + nLen, strIndentation, nSize - nLen);
	XN_IS_STATUS_OK(nRetVal);

	nLen = xnOSStrLen(csResult);
	
	nRetVal = xnProductionNodeDescriptionToString(&pNodeInfo->Description, csResult + nLen, nSize - nLen);
	XN_IS_STATUS_OK(nRetVal);

	if (pNodeInfo->strInstanceName[0] != '\0')
	{
		nLen = xnOSStrLen(csResult);
		nRetVal = xnOSStrFormat(csResult + nLen, nSize - nLen, &nWritten, "[\"%s\"]", pNodeInfo->strInstanceName);
		XN_IS_STATUS_OK(nRetVal);
	}

	if (pNodeInfo->strCreationInfo[0] != '\0')
	{
		nLen = xnOSStrLen(csResult);
		nRetVal = xnOSStrFormat(csResult + nLen, nSize - nLen, &nWritten, "(%s)", pNodeInfo->strCreationInfo);
		XN_IS_STATUS_OK(nRetVal);
	}

	if (pNodeInfo->pNeededTrees != NULL)
	{
		// add list of needed nodes
		XnBool bFirst = TRUE;

		for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pNodeInfo->pNeededTrees); 
			xnNodeInfoListIteratorIsValid(it);
			it = xnNodeInfoListGetNext(it))
		{
			nLen = xnOSStrLen(csResult);

			if (bFirst)
			{
				nRetVal = xnOSStrFormat(csResult + nLen, nSize - nLen, &nWritten, " ->\n", strIndentation);
				XN_IS_STATUS_OK(nRetVal);
			}

			nLen = xnOSStrLen(csResult);

			nRetVal = xnNodeInfoGetTreeStringRepresentationImpl(xnNodeInfoListGetCurrent(it), csResult + nLen, nSize - nLen, nDepth + 1);
			XN_IS_STATUS_OK(nRetVal);

			bFirst = FALSE;
		}

		if (bFirst)
		{
			// no needed nodes. add new line ourselves
			nLen = xnOSStrLen(csResult);
			nRetVal = xnOSStrFormat(csResult + nLen, nSize - nLen, &nWritten, "\n", strIndentation);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeInfoGetTreeStringRepresentation(XnNodeInfo* pNodeInfo, XnChar* csResult, XnUInt32 nSize)
{
	XN_VALIDATE_INPUT_PTR(pNodeInfo);
	XN_VALIDATE_OUTPUT_PTR(csResult);

	return xnNodeInfoGetTreeStringRepresentationImpl(pNodeInfo, csResult, nSize, 0);
}

//---------------------------------------------------------------------------
// ProductionNodesList
//---------------------------------------------------------------------------

XN_C_API XnStatus xnNodeInfoListAllocate(XnNodeInfoList** ppList)
{
	XN_VALIDATE_OUTPUT_PTR(ppList);

	XN_VALIDATE_CALLOC(*ppList, XnNodeInfoList, 1);
	
	return (XN_STATUS_OK);
}

XN_C_API void xnNodeInfoListFree(XnNodeInfoList* pList)
{
	if (pList != NULL)
	{
		xnNodeInfoListClear(pList);
		xnOSFree(pList);
	}
}

XN_C_API XnStatus xnNodeInfoListAddNode(XnNodeInfoList* pList, XnNodeInfo* pNode)
{
	// allocate new list node
	XnNodeInfoListNode* pListNode;
	XN_VALIDATE_CALLOC(pListNode, XnNodeInfoListNode, 1);

	pListNode->pCurrent = pNode;
	++pNode->nRefCount;

	if (pList->pLast != NULL)
	{
		pList->pLast->pNext = pListNode;
		pListNode->pPrev = pList->pLast;
		pList->pLast = pListNode;
	}
	else
	{
		// this will be the only node in the list.
		pList->pFirst = pListNode;
		pList->pLast = pListNode;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeInfoListAdd(XnNodeInfoList* pList, const XnProductionNodeDescription* pDescription, const XnChar* strCreationInfo, XnNodeInfoList* pNeededNodes)
{
	return xnNodeInfoListAddEx(pList, pDescription, strCreationInfo, pNeededNodes, NULL, NULL);
}

XN_C_API XnStatus xnNodeInfoListAddEx(XnNodeInfoList* pList, const XnProductionNodeDescription* pDescription, const XnChar* strCreationInfo, XnNodeInfoList* pNeededNodes, const void* pAdditionalData, XnFreeHandler pFreeHandler)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pList);
	XN_VALIDATE_INPUT_PTR(pDescription);

	// allocate new node info
	XnNodeInfo* pNode;
	nRetVal = xnNodeInfoAllocate(pDescription, strCreationInfo, pNeededNodes, &pNode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnNodeInfoSetAdditionalData(pNode, pAdditionalData, pFreeHandler);
	if (nRetVal != XN_STATUS_OK)
	{
		xnNodeInfoFree(pNode);
		return (nRetVal);
	}

	nRetVal = xnNodeInfoListAddNode(pList, pNode);
	if (nRetVal != XN_STATUS_OK)
	{
		xnNodeInfoFree(pNode);
		return (nRetVal);
	}

	// free it
	xnNodeInfoFree(pNode);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeInfoListAddNodeFromList(XnNodeInfoList* pList, XnNodeInfoListIterator pOtherListIt)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pList);
	XN_VALIDATE_INPUT_PTR(pOtherListIt.pCurrent);

	// take node
	XnNodeInfo* pNode = xnNodeInfoListGetCurrent(pOtherListIt);

	// add it to list
	nRetVal = xnNodeInfoListAddNode(pList, pNode);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeInfoListRemove(XnNodeInfoList* pList, XnNodeInfoListIterator it)
{
	XN_VALIDATE_INPUT_PTR(pList);
	XN_VALIDATE_INPUT_PTR(it.pCurrent);

	XnNodeInfoListNode* pPrev = it.pCurrent->pPrev;
	XnNodeInfoListNode* pNext = it.pCurrent->pNext;

	// connect previous and next elements
	if (pPrev == NULL)
	{
		pList->pFirst = pNext;
	}
	else
	{
		pPrev->pNext = pNext;
	}

	if (pNext == NULL)
	{
		pList->pLast = pPrev;
	}
	else
	{
		pNext->pPrev = pPrev;
	}

	// free it
	it.pCurrent->pPrev = NULL;
	it.pCurrent->pNext = NULL;
	xnNodeInfoFree(it.pCurrent->pCurrent);
	xnOSFree(it.pCurrent);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeInfoListClear(XnNodeInfoList* pList)
{
	XN_VALIDATE_INPUT_PTR(pList);

	XnNodeInfoListNode* pNode = pList->pFirst;
	while (pNode != NULL)
	{
		XnNodeInfoListNode* pCurr = pNode;

		pNode = pNode->pNext;
		xnNodeInfoFree(pCurr->pCurrent);
		xnOSFree(pCurr);
	}

	pList->pFirst = NULL;
	pList->pLast = NULL;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeInfoListAppend(XnNodeInfoList* pList, XnNodeInfoList* pOther)
{
	XN_VALIDATE_INPUT_PTR(pList);
	XN_VALIDATE_INPUT_PTR(pOther);

	if (pList->pLast == NULL)
	{
		// if target list is empty, then just make it point to other list
		pList->pFirst = pOther->pFirst;
		pList->pLast = pOther->pLast;
	}
	else if (pOther->pFirst != NULL)
	{
		// both lists are not empty. start by connecting last and first
		pList->pLast->pNext = pOther->pFirst;
		pOther->pFirst->pPrev = pList->pLast;
		pList->pLast = pOther->pLast;
	}
	else
	{
		// other list is empty, nothing to be done here
	}

	// now "empty" target list
	pOther->pFirst = NULL;
	pOther->pLast = NULL;

	return (XN_STATUS_OK);
}

XN_C_API XnBool xnNodeInfoListIsEmpty(XnNodeInfoList* pList)
{
	return pList->pFirst == NULL;
}

XN_C_API XnNodeInfoListIterator xnNodeInfoListGetFirst(XnNodeInfoList* pList)
{
	XnNodeInfoListIterator it = { pList->pFirst };
	return it;
}

XN_C_API XnNodeInfoListIterator xnNodeInfoListGetLast(XnNodeInfoList* pList)
{
	XnNodeInfoListIterator it = { pList->pLast };
	return it;
}

XN_C_API XnBool xnNodeInfoListIteratorIsValid(XnNodeInfoListIterator it)
{
	return (it.pCurrent != NULL);
}

XN_C_API XnNodeInfo* xnNodeInfoListGetCurrent(XnNodeInfoListIterator it)
{
	return it.pCurrent->pCurrent;
}

XN_C_API XnNodeInfoListIterator xnNodeInfoListGetNext(XnNodeInfoListIterator it)
{
	XnNodeInfoListIterator result = { it.pCurrent->pNext };
	return (result);
}

XN_C_API XnNodeInfoListIterator xnNodeInfoListGetPrevious(XnNodeInfoListIterator it)
{
	XnNodeInfoListIterator result = { it.pCurrent->pPrev };
	return (result);
}

//---------------------------------------------------------------------------
// Production Trees
//---------------------------------------------------------------------------

XN_C_API XnStatus xnEnumerateProductionTrees(XnContext* pContext, XnProductionNodeType Type, const XnNodeQuery* pQuery, XnNodeInfoList** ppTreesList, XnEnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_OUTPUT_PTR(ppTreesList);

	XnNodeInfoList* pResult;
	nRetVal = xnNodeInfoListAllocate(&pResult);
	XN_IS_STATUS_OK(nRetVal);

	// first take existing ones
	for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
	{
		XnInternalNodeData* pNodeData = it->Value();
		if (pNodeData->pNodeInfo->Description.Type == Type)
		{
			nRetVal = xnNodeInfoListAddNode(pResult, pNodeData->pNodeInfo);
			if (nRetVal != XN_STATUS_OK)
			{
				xnNodeInfoListFree(pResult);
				return (nRetVal);
			}
		}
	}

	// find exported generators
	nRetVal = pContext->moduleLoader.Enumerate(pContext, Type, pResult, pErrors);
	if (nRetVal != XN_STATUS_OK)
	{
		xnNodeInfoListFree(pResult);
		return (nRetVal);
	}

	if (pQuery != NULL)
	{
		xnNodeQueryFilterList(pContext, pQuery, pResult);
	}

	// see if we have any results
	if (!xnNodeInfoListIteratorIsValid(xnNodeInfoListGetFirst(pResult)))
	{
		xnNodeInfoListFree(pResult);
		return (XN_STATUS_NO_NODE_PRESENT);
	}

	*ppTreesList = pResult;

	return (XN_STATUS_OK);
}

XnStatus xnCreateMetaData(XnInternalNodeData* pNodeData)
{
	if (pNodeData->pTypeHierarchy->IsSet(XN_NODE_TYPE_DEPTH))
	{
		pNodeData->pMetaData.Depth = xnAllocateDepthMetaData();
		XN_VALIDATE_ALLOC_PTR(pNodeData->pMetaData.Depth);
		pNodeData->pbMetaDataIsNewFlag = &pNodeData->pMetaData.Depth->pMap->pOutput->bIsNew;
	}
	else if (pNodeData->pTypeHierarchy->IsSet(XN_NODE_TYPE_IMAGE))
	{
		pNodeData->pMetaData.Image = xnAllocateImageMetaData();
		XN_VALIDATE_ALLOC_PTR(pNodeData->pMetaData.Image);
		pNodeData->pbMetaDataIsNewFlag = &pNodeData->pMetaData.Image->pMap->pOutput->bIsNew;
	}
	else if (pNodeData->pTypeHierarchy->IsSet(XN_NODE_TYPE_IR))
	{
		pNodeData->pMetaData.IR = xnAllocateIRMetaData();
		XN_VALIDATE_ALLOC_PTR(pNodeData->pMetaData.IR);
		pNodeData->pbMetaDataIsNewFlag = &pNodeData->pMetaData.IR->pMap->pOutput->bIsNew;
	}
	else if (pNodeData->pTypeHierarchy->IsSet(XN_NODE_TYPE_AUDIO))
	{
		pNodeData->pMetaData.Audio = xnAllocateAudioMetaData();
		XN_VALIDATE_ALLOC_PTR(pNodeData->pMetaData.Audio);
		pNodeData->pbMetaDataIsNewFlag = &pNodeData->pMetaData.Audio->pOutput->bIsNew;
	}
	else if (pNodeData->pTypeHierarchy->IsSet(XN_NODE_TYPE_SCENE))
	{
		pNodeData->pMetaData.Scene = xnAllocateSceneMetaData();
		XN_VALIDATE_ALLOC_PTR(pNodeData->pMetaData.Scene);
		pNodeData->pbMetaDataIsNewFlag = &pNodeData->pMetaData.Scene->pMap->pOutput->bIsNew;
	}

	xnUpdateMetaData(pNodeData);
	
	return (XN_STATUS_OK);
}

XnStatus xnCreatePrivateData(XnInternalNodeData* pNodeData)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (pNodeData->pTypeHierarchy->IsSet(XN_NODE_TYPE_RECORDER))
	{
		xn::RecorderImpl *pRecorderImpl = NULL;
		XN_VALIDATE_NEW(pRecorderImpl, xn::RecorderImpl);
		nRetVal = pRecorderImpl->Init(pNodeData);
		if (nRetVal != XN_STATUS_OK)
		{
			XN_DELETE(pRecorderImpl);
			return nRetVal;
		}

		pNodeData->pPrivateData = pRecorderImpl;
	}
	else if (pNodeData->pTypeHierarchy->IsSet(XN_NODE_TYPE_PLAYER))
	{
		xn::PlayerImpl *pPlayerImpl = NULL;
		XN_VALIDATE_NEW(pPlayerImpl, xn::PlayerImpl);
		nRetVal = pPlayerImpl->Init(pNodeData);
		if (nRetVal != XN_STATUS_OK)
		{
			XN_DELETE(pPlayerImpl);
			return nRetVal;
		}
		pNodeData->pPrivateData = pPlayerImpl;
	}
	else if (pNodeData->pTypeHierarchy->IsSet(XN_NODE_TYPE_DEPTH))
	{
		xn::DepthPrivateData* pDepthPrivateData = NULL;
		XN_VALIDATE_NEW(pDepthPrivateData, xn::DepthPrivateData);
		nRetVal = pDepthPrivateData->Init(pNodeData);
		if (nRetVal != XN_STATUS_OK)
		{
			XN_DELETE(pDepthPrivateData);
			return nRetVal;
		}
		pNodeData->pPrivateData = pDepthPrivateData;
	}
    else if(pNodeData->pTypeHierarchy->IsSet(XN_NODE_TYPE_USER))
    {
        xn::PosePrivateData * pPosePrivateData=NULL;

        if(xnIsCapabilitySupported(pNodeData,XN_CAPABILITY_POSE_DETECTION))
        {
            XN_VALIDATE_NEW(pPosePrivateData, xn::PosePrivateData);
            nRetVal = pPosePrivateData->Init(pNodeData);
            if (nRetVal != XN_STATUS_OK)
            {
                XN_DELETE(pPosePrivateData);
                return nRetVal;
            }
            pNodeData->pPrivateData = pPosePrivateData;
        }

    }
	return (XN_STATUS_OK);
}

void XN_CALLBACK_TYPE xnGeneratorHasNewData(XnNodeHandle hNode, void* /*pCookie*/)
{
	xnMarkFPSFrame(hNode->pContext, &hNode->genFPS);
	xnOSSetEvent(hNode->pContext->hNewDataEvent);
	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);
	xnDumpFileWriteString(hNode->pContext->pDumpDataFlow, "%llu,NewDataAvailable,%s,\n", nNow, hNode->pNodeInfo->strInstanceName);
}

void XN_CALLBACK_TYPE xnNodeLockChanged(XnNodeHandle hNode, void* /*pCookie*/)
{
	XnBool bLocked = xnGetLockStateImpl(hNode);
	// if it's locked, give it a special handle
	hNode->LockData.nCurrentLock = bLocked ? (XnUInt32)XN_MAX_UINT32 : 0;
}

void xnSetGlobalErrorState(XnContext* pContext, XnStatus errorState)
{
	if (pContext->globalErrorState != errorState)
	{
		if (errorState == XN_STATUS_OK)
		{
			xnLoggerInfo(g_logger, "Context has returned to normal state.");
		}
		else
		{
			xnLoggerInfo(g_logger, "Context has entered error state: %s", xnGetStatusString(errorState));
		}

		pContext->globalErrorState = errorState;
		pContext->globalErrorChangeEvent.Raise(errorState);
	}
}

void XN_CALLBACK_TYPE xnNodeErrorStateChanged(XnNodeHandle hNode, void* /*pCookie*/)
{
	XnContext* pContext = hNode->pContext;

	// check for all nodes errors
	XnStatus errorState = XN_STATUS_OK;

	for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
	{
		// check this node error state
		XnStatus nodeError = xnGetNodeErrorState(it->Value());

		if (nodeError != XN_STATUS_OK)
		{
			if (errorState == XN_STATUS_OK)
			{
				errorState = nodeError;
			}
			else
			{
				errorState = XN_STATUS_MULTIPLE_NODES_ERROR;
				// we don't care about the rest
				break;
			}
		}
	}

	xnSetGlobalErrorState(pContext, errorState);
}

void XN_CALLBACK_TYPE xnNodeFrameSyncChanged(XnNodeHandle hNode, void* /*pCookie*/)
{
	XnContext* pContext = hNode->pContext;

	// check if node is frame synced with any other node
	XnNodeHandle hFrameSyncedWith = NULL;

	for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
	{
		XnNodeHandle hOther = it->Value();
		if (hNode != hOther && xnIsFrameSyncedWithImpl(hNode, hOther))
		{
			hFrameSyncedWith = hOther;
			// For now, we support only one frame synced object
			break;
		}
	}
	
	// if we were previously synced with other node, remove this sync
	if (hNode->hFrameSyncedWith != NULL)
	{
		hNode->hFrameSyncedWith->hFrameSyncedWith = NULL;
	}

	hNode->hFrameSyncedWith = hFrameSyncedWith;

	if (hFrameSyncedWith != NULL)
	{
		hFrameSyncedWith->hFrameSyncedWith = hNode;
	}
}

static XnStatus xnCreateProductionNodeImpl(XnContext* pContext, XnNodeInfo* pTree, XnNodeHandle* phNode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	*phNode = NULL;
	
	// first of all, check if name is empty, if so - give it a default name
	if (pTree->strInstanceName[0] == '\0')
	{
		xnFindValidNameForType(pContext, pTree->Description.Type, pTree->strInstanceName);
	}

	XnChar strDescription[500];
	xnProductionNodeDescriptionToString(&pTree->Description, strDescription, 500);
	xnLoggerInfo(g_logger, "Creating node '%s' of type %s...", pTree->strInstanceName, strDescription);

	XnModuleInstance* pModuleInstance;
	nRetVal = pContext->moduleLoader.CreateRootNode(pContext, pTree, &pModuleInstance);
	XN_IS_STATUS_OK(nRetVal);

	// create handle
	XnInternalNodeData* pNodeData;
	XN_VALIDATE_CALLOC(pNodeData, XnInternalNodeData, 1);

	pNodeData->pTypeHierarchy = XN_NEW(XnBitSet);
	if (pNodeData->pTypeHierarchy == NULL)
	{
		return xnFreeProductionNodeImpl(pNodeData, XN_STATUS_ALLOC_FAILED);
	}

	*pNodeData->pTypeHierarchy = pModuleInstance->pLoaded->pInterface->HierarchyType;
	pNodeData->pNodeInfo = pTree;
	pNodeData->nRefCount = 1;
	pNodeData->pModuleInstance = pModuleInstance;

	// reference context
	nRetVal = xnContextAddRef(pContext);
	if (nRetVal != XN_STATUS_OK)
	{
		return xnFreeProductionNodeImpl(pNodeData, nRetVal);
	}
	pNodeData->pContext = pContext;

	nRetVal = xnOSCreateCriticalSection(&pNodeData->hLock);
	if (nRetVal != XN_STATUS_OK)
	{
		return xnFreeProductionNodeImpl(pNodeData, nRetVal);
	}

	pNodeData->pNeededNodesDataHash = XN_NEW(XnNeededNodesDataHash);

	if (pNodeData->pNeededNodesDataHash == NULL)
	{
		return xnFreeProductionNodeImpl(pNodeData, XN_STATUS_ALLOC_FAILED);
	}

	pNodeData->pRegistrationCookiesHash = XN_NEW(XnModuleStateCookieHash);

	if (pNodeData->pRegistrationCookiesHash == NULL)
	{
		return xnFreeProductionNodeImpl(pNodeData, XN_STATUS_ALLOC_FAILED);
	}

	nRetVal = xnFPSInit(&pNodeData->genFPS, XN_NODE_FPS_CALC_SAMPLES);
	if (nRetVal != XN_STATUS_OK)
	{
		return xnFreeProductionNodeImpl(pNodeData, nRetVal);
	}
	
	nRetVal = xnFPSInit(&pNodeData->readFPS, XN_NODE_FPS_CALC_SAMPLES);
	if (nRetVal != XN_STATUS_OK)
	{
		return xnFreeProductionNodeImpl(pNodeData, nRetVal);
	}

	// create meta data object
	nRetVal = xnCreateMetaData(pNodeData);
	if (nRetVal != XN_STATUS_OK)
	{
		return xnFreeProductionNodeImpl(pNodeData, nRetVal);
	}

	// create private data
	nRetVal = xnCreatePrivateData(pNodeData);
	if (nRetVal != XN_STATUS_OK)
	{
		return xnFreeProductionNodeImpl(pNodeData, nRetVal);
	}

	if (pNodeData->pModuleInstance->pLoaded->pInterface->HierarchyType.IsSet(XN_NODE_TYPE_GENERATOR))
	{
		// and try to set its mirror state
		if (pContext->bGlobalMirrorSet && xnIsCapabilitySupported(pNodeData, XN_CAPABILITY_MIRROR))
		{
			nRetVal = xnSetMirror(pNodeData, pContext->bGlobalMirror);
			if (nRetVal != XN_STATUS_OK)
			{
				return xnFreeProductionNodeImpl(pNodeData, nRetVal);
			}
		}

		// register for its new data event (for WaitAnyUpdateAll)
		nRetVal = xnRegisterToNewDataAvailable(pNodeData, xnGeneratorHasNewData, pContext, &pNodeData->hNewDataCallback);
		if (nRetVal != XN_STATUS_OK)
		{
			return xnFreeProductionNodeImpl(pNodeData, nRetVal);
		}
	}

	// register to lock changes
	if (xnIsCapabilitySupported(pNodeData, XN_CAPABILITY_LOCK_AWARE))
	{
		nRetVal = xnRegisterToLockChangeImpl(pNodeData, xnNodeLockChanged, pNodeData, &pNodeData->LockData.hLockChangedCallback);
		if (nRetVal != XN_STATUS_OK)
		{
			return xnFreeProductionNodeImpl(pNodeData, nRetVal);
		}
	}

	// register to error state changes
	if (xnIsCapabilitySupported(pNodeData, XN_CAPABILITY_ERROR_STATE))
	{
		nRetVal = xnRegisterToNodeErrorStateChange(pNodeData, xnNodeErrorStateChanged, NULL, &pNodeData->hErrorStateCallback);
		if (nRetVal != XN_STATUS_OK)
		{
			return xnFreeProductionNodeImpl(pNodeData, nRetVal);
		}
	}

	// register to frame sync changes
	if (xnIsCapabilitySupported(pNodeData, XN_CAPABILITY_FRAME_SYNC))
	{
		nRetVal = xnRegisterToFrameSyncChange(pNodeData, xnNodeFrameSyncChanged, NULL, &pNodeData->hFrameSyncCallback);
		if (nRetVal != XN_STATUS_OK)
		{
			return xnFreeProductionNodeImpl(pNodeData, nRetVal);
		}

		// check current
		xnNodeFrameSyncChanged(pNodeData, NULL);
	}

	// add it to the context
	nRetVal = pContext->nodesMap.Set(pTree->strInstanceName, pNodeData);
	if (nRetVal != XN_STATUS_OK)
	{
		return xnFreeProductionNodeImpl(pNodeData, nRetVal);
	}

	// if this is a player node, store it in the context (so we'll know to play it...)
	if (pNodeData->pModuleInstance->pLoaded->pInterface->HierarchyType.IsSet(XN_NODE_TYPE_PLAYER))
	{
		pContext->hPlayerNode = pNodeData;
	}

	// increase info ref count (context now holds it)
	++pTree->nRefCount;

	xnDumpRefCount(pContext, pNodeData, 1, "Create");

	// update handle in node info
	pTree->hNode = pNodeData;

	// raise event
	nRetVal = pContext->nodeCreationEvent.Raise(pContext, pNodeData);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLoggerWarning(g_logger, "Failed raising the 'Node Creation' event: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
	}

	// return value
	*phNode = pNodeData;

	return (XN_STATUS_OK);
}

static XnStatus xnCreateProductionTreeImpl(XnContext* pContext, XnNodeInfo* pTree, XnNodeHandle* phNode)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnArray<XnNodeHandle> createdNodes;

	*phNode = NULL;
	
	// check stop condition
	if (pTree->hNode != NULL)
	{
		// tree already exists. only increase ref count
		xnProductionNodeAddRef(pTree->hNode);
		*phNode = pTree->hNode;
		return (XN_STATUS_OK);
	}

	// we need to create it. start by checking its needed ones
	for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pTree->pNeededTrees);
		xnNodeInfoListIteratorIsValid(it);
		it = xnNodeInfoListGetNext(it))
	{
		XnNodeInfo* pChild = xnNodeInfoListGetCurrent(it);
		XnNodeHandle hNeeded;
		nRetVal = xnCreateProductionTreeImpl(pContext, pChild, &hNeeded);
		if (nRetVal != XN_STATUS_OK)
		{
			break;
		}
		else
		{
			// store created node
			nRetVal = createdNodes.AddLast(hNeeded);
			if (nRetVal != XN_STATUS_OK)
			{
				xnProductionNodeRelease(hNeeded);
				break;
			}
		}
	}

	// now create root
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = xnCreateProductionNodeImpl(pContext, pTree, phNode);
	}

	// If something went wrong, release all created objects
	if (nRetVal != XN_STATUS_OK)
	{
		for (XnUInt32 i = 0; i < createdNodes.GetSize(); ++i)
		{
			xnProductionNodeRelease(createdNodes[i]);
		}
	}

	return (nRetVal);
}

static XnStatus xnFreeProductionNodeImpl(XnNodeHandle hNode, XnStatus nRetVal /*= XN_STATUS_OK*/)
{
	if (hNode != NULL)	
	{
        if (hNode->pPrivateData != NULL)
        {
		    XN_DELETE(hNode->pPrivateData);
            hNode->pPrivateData = NULL;
        }

		if (hNode->pTypeHierarchy->IsSet(XN_NODE_TYPE_DEPTH))
		{
			xnFreeDepthMetaData(hNode->pMetaData.Depth);
		}
		else if (hNode->pTypeHierarchy->IsSet(XN_NODE_TYPE_IMAGE))
		{
			xnFreeImageMetaData(hNode->pMetaData.Image);
		}
		else if (hNode->pTypeHierarchy->IsSet(XN_NODE_TYPE_IR))
		{
			xnFreeIRMetaData(hNode->pMetaData.IR);
		}
		else if (hNode->pTypeHierarchy->IsSet(XN_NODE_TYPE_AUDIO))
		{
			xnFreeAudioMetaData(hNode->pMetaData.Audio);
		}
		else if (hNode->pTypeHierarchy->IsSet(XN_NODE_TYPE_SCENE))
		{
			xnFreeSceneMetaData(hNode->pMetaData.Scene);
		}
		else if (hNode->pTypeHierarchy->IsSet(XN_NODE_TYPE_PLAYER))
		{
			// remove it from the context
			hNode->pContext->hPlayerNode = NULL;

			// TODO: We might want to check if another player exists instead (so playback can continue with that player...)
		}

		// free all registration cookies that were not unregistered
		for (XnModuleStateCookieHash::ConstIterator it = hNode->pRegistrationCookiesHash->Begin(); it != hNode->pRegistrationCookiesHash->End(); ++it)
		{
			xnOSFree(it->Key());
		}
		XN_DELETE(hNode->pRegistrationCookiesHash);
		XN_DELETE(hNode->pNeededNodesDataHash);

		if (hNode->hLock != NULL)
		{
			xnOSCloseCriticalSection(&hNode->hLock);
		}

		xnFPSFree(&hNode->genFPS);
		xnFPSFree(&hNode->readFPS);

		if (hNode->pContext != NULL)
		{
			xnContextRelease(hNode->pContext);
		}

		if (hNode->pTypeHierarchy != NULL)
		{
			XN_DELETE(hNode->pTypeHierarchy);
		}

		xnOSFree(hNode);
	}
	return nRetVal;
}

void xnDestroyProductionNodeImpl(XnNodeHandle hNode)
{
	// keep the context
	XnContext* pContext = hNode->pContext;
	// keep its name
	const XnChar* strNodeName = xnOSStrDup(hNode->pNodeInfo->strInstanceName);

	xnLoggerInfo(g_logger, "Destroying node '%s'", hNode->pNodeInfo->strInstanceName);

	if (hNode->pPrivateData != NULL)
	{
		//Notify the object's private data that the node that's holding it is about to be destroyed.
		hNode->pPrivateData->BeforeNodeDestroy(); 
	}

	// unregister from events
	if (hNode->hNewDataCallback != NULL)
	{
		xnUnregisterFromNewDataAvailable(hNode, hNode->hNewDataCallback);
	}

	if (hNode->LockData.hLockChangedCallback != NULL)
	{
		xnUnregisterFromLockChangeImpl(hNode, hNode->LockData.hLockChangedCallback);
	}

	if (hNode->hErrorStateCallback != NULL)
	{
		xnUnregisterFromNodeErrorStateChange(hNode, hNode->hErrorStateCallback);
	}

	if (hNode->hFrameSyncCallback != NULL)
	{
		xnUnregisterFromFrameSyncChange(hNode, hNode->hFrameSyncCallback);
	}

	// remove it from map
	hNode->pContext->nodesMap.Remove(hNode->pNodeInfo->strInstanceName);
	// destroy module node
	hNode->pContext->moduleLoader.DestroyModuleInstance(hNode->pModuleInstance);

	// dec ref from all needed nodes
	for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(hNode->pNodeInfo->pNeededTrees);
		xnNodeInfoListIteratorIsValid(it);
		it = xnNodeInfoListGetNext(it))
	{
		XnNodeInfo* pNeededInfo = xnNodeInfoListGetCurrent(it);
		xnProductionNodeRelease(pNeededInfo->hNode);
	}

	// NULL handle in info object
	hNode->pNodeInfo->hNode = NULL;

	xnDumpRefCount(pContext, hNode, 0, "Destroy");


	// dec ref of info object (it was removed from context)
	xnNodeInfoFree(hNode->pNodeInfo);

	// raise event (do this before freeing the production node. It might cause context to be destroyed as well...)
	pContext->nodeDestructionEvent.Raise(pContext, strNodeName);

	// free memory
	xnFreeProductionNodeImpl(hNode);

	xnOSFree(strNodeName);
}

XN_C_API XnStatus xnProductionNodeAddRef(XnNodeHandle hNode)
{
	XN_VALIDATE_INPUT_PTR(hNode);

	// preform this in a lock
	XnAutoCSLocker lock(hNode->hLock);

	// now increase the ref count of the node itself
	++hNode->nRefCount;

	xnDumpRefCount(hNode->pContext, hNode, hNode->nRefCount, NULL);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnRefProductionNode(XnNodeHandle hNode)
{
	return xnProductionNodeAddRef(hNode);
}

XN_C_API void xnProductionNodeRelease(XnNodeHandle hNode)
{
	if (hNode == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}

	// preform this in a lock
	XnAutoCSLocker lock(hNode->hLock);

	// perform some checks
	if (hNode->nRefCount == 0)
	{
		XN_ASSERT(FALSE);
		return;
	}

	// and dec ref
	--hNode->nRefCount;

	xnDumpRefCount(hNode->pContext, hNode, hNode->nRefCount, NULL);

	// release it if needed
	if (hNode->nRefCount == 0)
	{
		// release lock (the destruction destroys the lock as well)
		lock.Unlock();

		// destroy it
		XnNodeInfo* pInfo = hNode->pNodeInfo;
		XnUInt32 nInfoRefCount = pInfo->nRefCount;
		xnDestroyProductionNodeImpl(hNode);

		if (nInfoRefCount > 1)
		{
			// this means the info object hasn't been destroyed yet. Update its instance
			pInfo->hNode = NULL;
		}
	}
}

XN_C_API void xnUnrefProductionNode(XnNodeHandle hNode)
{
	xnProductionNodeRelease(hNode);
}

XN_C_API XnStatus xnCreateProductionTree(XnContext* pContext, XnNodeInfo* pTree, XnNodeHandle* phNode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate input params
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_INPUT_PTR(pTree);
	XN_VALIDATE_OUTPUT_PTR(phNode);

	// create missing instances
	nRetVal = xnCreateProductionTreeImpl(pContext, pTree, phNode);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnCreateAnyProductionTree(XnContext* pContext, XnProductionNodeType type, XnNodeQuery* pQuery, XnNodeHandle* phNode, XnEnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// validate input params
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_OUTPUT_PTR(phNode);

	// first enumerate
	XnNodeInfoList* pList;
	nRetVal = xnEnumerateProductionTrees(pContext, type, pQuery, &pList, pErrors);
	XN_IS_STATUS_OK(nRetVal);

	// we know list has at least one option (otherwise, enumerate would fail). Take the first one
	XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pList);
	XnNodeInfo* pNodeInfo = xnNodeInfoListGetCurrent(it);

	// and create it
	nRetVal = xnCreateProductionTree(pContext, pNodeInfo, phNode);

	// free list anyway
	xnNodeInfoListFree(pList);

	// and check error code
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

static XnStatus xnCreateMockNodeImpl(XnContext* pContext, XnProductionNodeType type, const XnChar* strName, XnNodeHandle* phNode)
{
	// create a description for this node
	XnProductionNodeDescription description;
	strcpy(description.strVendor, XN_VENDOR_OPEN_NI);
	strcpy(description.strName, XN_MOCK_NODE_NAME);
	description.Type = type;
	xnGetVersion(&description.Version);

	// create the node info
	XnNodeInfo* pNodeInfo;
	XnStatus nRetVal = xnNodeInfoAllocate(&description, NULL, NULL, &pNodeInfo);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnNodeInfoSetInstanceName(pNodeInfo, strName);
	if (nRetVal != XN_STATUS_OK)
	{
		xnNodeInfoFree(pNodeInfo);
		return nRetVal;
	}

	// now create the node (if no mock is registered, this will fail)
	nRetVal = xnCreateProductionTree(pContext, pNodeInfo, phNode);
	if (nRetVal != XN_STATUS_OK)
	{
		xnNodeInfoFree(pNodeInfo);
		return nRetVal;
	}

	xnNodeInfoFree(pNodeInfo);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnCreateMockNode(XnContext* pContext, XnProductionNodeType type, const XnChar* strName, XnNodeHandle* phNode)
{
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_OUTPUT_PTR(phNode);
	XnStatus nRetVal = XN_STATUS_OK;
	XnChar strNameImpl[XN_MAX_NAME_LENGTH] = "";

	if (strName == NULL)
	{
		//Generate name for mock node
		xnFindValidNameForType(pContext, type, strNameImpl);
		strName = strNameImpl;
	}

	nRetVal = xnCreateMockNodeImpl(pContext, type, strName, phNode);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XN_C_API XnStatus xnCreateMockNodeBasedOn(XnContext* pContext, 
										  XnNodeHandle hOriginalNode, 
										  const XnChar* strName, 
										  XnNodeHandle* phMockNode)
{
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_INPUT_PTR(hOriginalNode);
	//strName may be NULL
	XN_VALIDATE_OUTPUT_PTR(phMockNode);
	XnChar strMockNodeName[XN_MAX_NAME_LENGTH];
	XnStatus nRetVal = XN_STATUS_OK;
	
	if (strName == NULL)
	{
		XnUInt32 nCharsWritten = 0;
		nRetVal = xnOSStrFormat(strMockNodeName, sizeof(strMockNodeName), &nCharsWritten, "%s_%s", xnGetNodeName(hOriginalNode), XN_MOCK_NODE_NAME);
		XN_IS_STATUS_OK_ASSERT(nRetVal);
		strName = strMockNodeName;
	}

	XnNodeHandle hMockNode = NULL;
	XnProductionNodeType type = hOriginalNode->pNodeInfo->Description.Type;
	nRetVal = xnCreateMockNodeImpl(pContext, type, strName, &hMockNode);
	XN_IS_STATUS_OK(nRetVal);
	
	//Copy state from original node to mock node
	xn::NodeWatcher* pNodeWatcher = NULL;
	xn::ProductionNode originalNode(hOriginalNode);
	nRetVal = xn::CreateNodeWatcher(originalNode, type, hMockNode, xn::MockNotifier::GetInstance(), pNodeWatcher);
	if (nRetVal != XN_STATUS_OK)
	{
		xnProductionNodeRelease(hMockNode);
		return nRetVal;
	}

	nRetVal = pNodeWatcher->NotifyState();
	XN_DELETE(pNodeWatcher);
	if (nRetVal != XN_STATUS_OK)
	{
		xnProductionNodeRelease(hMockNode);
		return nRetVal;
	}

	*phMockNode = hMockNode;

	return XN_STATUS_OK;
}

XN_C_API XnNodeInfo* xnGetNodeInfo(XnNodeHandle hNode)
{
	XN_ASSERT(hNode != NULL);
	XN_VALIDATE_PTR(hNode, NULL);
	return hNode->pNodeInfo;
}

XN_C_API const XnChar* xnGetNodeName(XnNodeHandle hNode)
{
	XN_ASSERT(hNode != NULL);
	XN_VALIDATE_PTR(hNode, NULL);
	XN_ASSERT(hNode->pNodeInfo != NULL);
	XN_VALIDATE_PTR(hNode->pNodeInfo, NULL);
	return hNode->pNodeInfo->strInstanceName;
}

XN_C_API XnBool xnIsTypeGenerator(XnProductionNodeType type)
{
	return xnIsTypeDerivedFrom(type, XN_NODE_TYPE_GENERATOR);
}

XN_C_API XnBool xnIsTypeDerivedFrom(XnProductionNodeType type, XnProductionNodeType base)
{
	XnBool bResult = FALSE;
	if (XN_STATUS_OK == TypeManager::GetInstance().IsTypeDerivedFrom(type, base, &bResult))
	{
		return bResult;
	}
	else
	{
		return FALSE;
	}
}

XN_C_API XnContext* xnGetContextFromNodeHandle(XnNodeHandle hNode)
{
	XnContext* pContext = xnGetRefContextFromNodeHandle(hNode);
	xnContextRelease(pContext);
	return pContext;
}

XN_C_API XnContext* xnGetRefContextFromNodeHandle(XnNodeHandle hNode)
{
	XN_ASSERT(hNode != NULL);
	XN_VALIDATE_PTR(hNode, NULL);

	XnStatus nRetVal = xnContextAddRef(hNode->pContext);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_ASSERT(FALSE);
	}

	return hNode->pContext;
}

inline void xnResetNewDataFlag(XnContext* pContext)
{
	XN_ASSERT(pContext != NULL);

	for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
	{
		XnNodeHandle hNode = it->Value();
		// update node
		hNode->bIsNewData = FALSE;
		// update meta data object
		if (hNode->pbMetaDataIsNewFlag != NULL)
		{
			*hNode->pbMetaDataIsNewFlag = FALSE;
		}
	}
}

void xnUpdateOutputMetaData(XnNodeHandle hNode, XnOutputMetaData* pMetaData)
{
	XN_ASSERT(hNode != NULL);
	XN_ASSERT(pMetaData != NULL);
	pMetaData->nTimestamp = xnGetTimestamp(hNode);
	pMetaData->nFrameID = xnGetFrameID(hNode);
	pMetaData->nDataSize = xnGetDataSize(hNode);
	pMetaData->bIsNew = xnIsDataNew(hNode);
}

void xnUpdateMapMetaData(XnNodeHandle hNode, XnMapMetaData* pMetaData)
{
	xnUpdateOutputMetaData(hNode, pMetaData->pOutput);

	XnMapOutputMode Mode;
	xnGetMapOutputMode(hNode, &Mode);
	pMetaData->FullRes.X = Mode.nXRes;
	pMetaData->FullRes.Y = Mode.nYRes;
	pMetaData->nFPS = Mode.nFPS;

	XnCropping Cropping;
	if (xnIsCapabilitySupported(hNode, XN_CAPABILITY_CROPPING) &&
		xnGetCropping(hNode, &Cropping) == XN_STATUS_OK &&
		Cropping.bEnabled)
	{
		pMetaData->Offset.X = Cropping.nXOffset;
		pMetaData->Offset.Y = Cropping.nYOffset;
		pMetaData->Res.X = Cropping.nXSize;
		pMetaData->Res.Y = Cropping.nYSize;
	}
	else
	{
		pMetaData->Offset.X = 0;
		pMetaData->Offset.Y = 0;
		pMetaData->Res.X = pMetaData->FullRes.X;
		pMetaData->Res.Y = pMetaData->FullRes.Y;
	}
}

void xnUpdateDepthMetaData(XnNodeHandle hNode)
{
	XnDepthMetaData* pMetaData = hNode->pMetaData.Depth;
	xnUpdateMapMetaData(hNode, pMetaData->pMap);
	pMetaData->pMap->PixelFormat = XN_PIXEL_FORMAT_GRAYSCALE_16_BIT;
	pMetaData->pData = xnGetDepthMap(hNode);
	pMetaData->nZRes = xnGetDeviceMaxDepth(hNode) + 1;
}

void xnUpdateImageMetaData(XnNodeHandle hNode)
{
	XnImageMetaData* pMetaData = hNode->pMetaData.Image;
	xnUpdateMapMetaData(hNode, pMetaData->pMap);
	pMetaData->pMap->PixelFormat = xnGetPixelFormat(hNode);
	pMetaData->pData = xnGetImageMap(hNode);
}

void xnUpdateIRMetaData(XnNodeHandle hNode)
{
	XnIRMetaData* pMetaData = hNode->pMetaData.IR;
	xnUpdateMapMetaData(hNode, pMetaData->pMap);
	pMetaData->pMap->PixelFormat = XN_PIXEL_FORMAT_GRAYSCALE_16_BIT;
	pMetaData->pData = xnGetIRMap(hNode);
}

void xnUpdateAudioMetaData(XnNodeHandle hNode)
{
	XnAudioMetaData* pMetaData = hNode->pMetaData.Audio;
	xnUpdateOutputMetaData(hNode, pMetaData->pOutput);
	xnGetWaveOutputMode(hNode, &pMetaData->Wave);
	pMetaData->pData = xnGetAudioBuffer(hNode);
}

void xnUpdateSceneMetaData(XnNodeHandle hNode)
{
	XnSceneMetaData* pMetaData = hNode->pMetaData.Scene;
	xnUpdateMapMetaData(hNode, pMetaData->pMap);
	pMetaData->pMap->PixelFormat = XN_PIXEL_FORMAT_GRAYSCALE_16_BIT;
	pMetaData->pData = xnGetLabelMap(hNode);
}

void xnUpdateMetaData(XnNodeHandle hNode)
{
	XN_ASSERT(hNode != NULL);
	XN_ASSERT(hNode->pNodeInfo != NULL);

	if (hNode->pTypeHierarchy->IsSet(XN_NODE_TYPE_DEPTH))
	{
		xnUpdateDepthMetaData(hNode);
	}
	else if (hNode->pTypeHierarchy->IsSet(XN_NODE_TYPE_IMAGE))
	{
		xnUpdateImageMetaData(hNode);
	}
	else if (hNode->pTypeHierarchy->IsSet(XN_NODE_TYPE_IR))
	{
		xnUpdateIRMetaData(hNode);
	}
	else if (hNode->pTypeHierarchy->IsSet(XN_NODE_TYPE_AUDIO))
	{
		xnUpdateAudioMetaData(hNode);
	}
	else if (hNode->pTypeHierarchy->IsSet(XN_NODE_TYPE_SCENE))
	{
		xnUpdateSceneMetaData(hNode);
	}
}

void xnUpdateMetaDataBeforeFirstRead(XnNodeHandle hNode)
{
	if (!hNode->bWasDataRead)
	{
		xnUpdateMetaData(hNode);
	}
}

static XnStatus xnUpdateTreeImpl(const XnNodeInfo* pNode)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// check if it was already updated
	if (!pNode->hNode->bWasVisited)
	{
		// not updated. start with input nodes
		for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pNode->pNeededTrees);
			xnNodeInfoListIteratorIsValid(it);
			it = xnNodeInfoListGetNext(it))
		{
			XnNodeInfo* pChildInfo = xnNodeInfoListGetCurrent(it);
			nRetVal = xnUpdateTreeImpl(pChildInfo);
			XN_IS_STATUS_OK(nRetVal);
		}

		// and now update root (only if it's a generator)
		if (pNode->hNode->pModuleInstance->pLoaded->pInterface->HierarchyType.IsSet(XN_NODE_TYPE_GENERATOR))
		{
			// when wait is not requested, we only update nodes that have new data
			if (xnIsNewDataAvailable(pNode->hNode, NULL))
			{
				nRetVal = xnUpdateDataImpl(pNode->hNode);
				XN_IS_STATUS_OK(nRetVal);
			}
		}
		// If it's a recorder, record
		else if (pNode->hNode->pModuleInstance->pLoaded->pInterface->HierarchyType.IsSet(XN_NODE_TYPE_RECORDER))
		{
			nRetVal = xnRecord(pNode->hNode);
			XN_IS_STATUS_OK(nRetVal);
		}

		pNode->hNode->bWasVisited = TRUE;
	}

	return (XN_STATUS_OK);
}

// Updates the entire graph (if pSubGraph is NULL), or just the sub graph starting from the pSubGraph node
static XnStatus xnUpdateGraph(XnContext* pContext, const XnNodeInfo* pSubGraph)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// mark all current nodes' data as "old"
	xnResetNewDataFlag(pContext);

	// mark all nodes as not-visited
	xnResetVisitState(pContext);

	// now start the update (each updated node will be marked as visited)
	if (pSubGraph != NULL)
	{
		nRetVal = xnUpdateTreeImpl(pSubGraph);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
		{
			XnNodeInfo* pNodeInfo = it->Value()->pNodeInfo;
			nRetVal = xnUpdateTreeImpl(pNodeInfo);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	return (XN_STATUS_OK);
}

XnBool xnDidNodeAdvanced(XnNodeHandle hNode)
{
	XnUInt64 nTimestamp;
	if (!xnIsNewDataAvailableImpl(hNode, &nTimestamp))
	{
		return (FALSE);
	}

	// node has new data
	if (hNode->hFrameSyncedWith != NULL)
	{
		// check that other node also have new data
		XnUInt64 nOtherTimestamp;
		if (!xnIsNewDataAvailableImpl(hNode->hFrameSyncedWith, &nOtherTimestamp))
		{
			return (FALSE);
		}

		// check for frame sync 
		XnUInt64 nNow;
		xnOSGetHighResTimeStamp(&nNow);
		xnDumpFileWriteString(hNode->pContext->pDumpDataFlow, "%llu,FrameSyncCheck,%s,%llu\n", nNow, hNode->pNodeInfo->strInstanceName, nTimestamp);
		xnDumpFileWriteString(hNode->pContext->pDumpDataFlow, "%llu,FrameSyncCheck,%s,%llu\n", nNow, hNode->hFrameSyncedWith->pNodeInfo->strInstanceName, nOtherTimestamp);
		XnUInt64 nDiff = (nTimestamp >= nOtherTimestamp) ? (nTimestamp - nOtherTimestamp) : (nOtherTimestamp - nTimestamp);
		return (nDiff <= XN_FRAME_SYNC_THRESHOLD);
	}
	else
	{
		return (TRUE);
	}
}

XnBool XN_CALLBACK_TYPE xnDidAllNodesAdvanced(void* pConditionData)
{
	XnContext* pContext = (XnContext*)pConditionData;

	for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
	{
		XnInternalNodeData* pData = it->Value();
		if (pData->pModuleInstance->pLoaded->pInterface->HierarchyType.IsSet(XN_NODE_TYPE_GENERATOR) &&
			!xnDidNodeAdvanced(pData))
		{
			return (FALSE);
		}
	}

	return (TRUE);
}

static XnStatus xnPlayRecording(XnContext* pContext, XnBool bAsynch = FALSE)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// check if we have players in this context
	// TODO: handle the case in which we have more than one player
	XnNodeHandle hPlayer = NULL;
	nRetVal = xnFindExistingRefNodeByType(pContext, XN_NODE_TYPE_PLAYER, &hPlayer);
	if (nRetVal == XN_STATUS_NO_MATCH)
	{
		return (XN_STATUS_OK);
	}
	else
	{
		XN_IS_STATUS_OK(nRetVal)
	}

	if (xnIsPlayerAtEOF(hPlayer))
	{
		return XN_STATUS_EOF;
	}
	else
	{
		if (bAsynch)
		{
			xn::PlayerImpl *pPlayerImpl = dynamic_cast<xn::PlayerImpl*>(hPlayer->pPrivateData);
			XN_VALIDATE_PTR(pPlayerImpl, XN_STATUS_ERROR);
			pPlayerImpl->TriggerPlayback();
		}
		else
		{
			nRetVal = xnPlayerReadNext(hPlayer);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	return (XN_STATUS_OK);
}

XnStatus xnWaitForCondition(XnContext* pContext, XnConditionFunc pConditionFunc, void* pConditionData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnMarkFPSFrame(pContext, &pContext->readFPS);

	// check if we have players in this context
	// TODO: handle the case in which we have more than one player
	if (pContext->hPlayerNode != NULL)
	{
		// play until condition is met
		while (!pConditionFunc(pConditionData))
		{
			if (xnIsPlayerAtEOF(pContext->hPlayerNode))
			{
				return XN_STATUS_EOF;
			}
			else
			{
				nRetVal = xnPlayerReadNext(pContext->hPlayerNode);
				XN_IS_STATUS_OK(nRetVal);
			}
		}
	}
	else
	{
		// no players, just wait for the event
		nRetVal = xnOSWaitForCondition(pContext->hNewDataEvent, XN_NODE_WAIT_FOR_DATA_TIMEOUT, pConditionFunc, pConditionData);
		if (nRetVal == XN_STATUS_OS_EVENT_TIMEOUT)
		{
			return (XN_STATUS_WAIT_DATA_TIMEOUT);
		}
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnWaitAndUpdateAll(XnContext* pContext)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pContext);

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);
	xnDumpFileWriteString(pContext->pDumpDataFlow, "%llu,WaitAndUpdateAll,Application,%s,\n", nNow);

	// wait on new data event
	nRetVal = xnWaitForCondition(pContext, xnDidAllNodesAdvanced, pContext);
	XN_IS_STATUS_OK(nRetVal);

	// now update entire tree
	nRetVal = xnUpdateGraph(pContext, NULL);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnBool XN_CALLBACK_TYPE xnDidNodeAdvanced(void* pConditionData)
{
	return xnDidNodeAdvanced((XnNodeHandle)pConditionData);
}

XN_C_API XnStatus xnWaitOneUpdateAll(XnContext* pContext, XnNodeHandle hNode)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_INPUT_PTR(hNode);

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);
	xnDumpFileWriteString(hNode->pContext->pDumpDataFlow, "%llu,WaitOneUpdateAll,Application,%s,\n", nNow, hNode->pNodeInfo->strInstanceName);

	// wait on new data event
	nRetVal = xnWaitForCondition(pContext, xnDidNodeAdvanced, hNode);
	XN_IS_STATUS_OK(nRetVal);

	// now update entire tree
	nRetVal = xnUpdateGraph(pContext, NULL);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnWaitNoneUpdateAll(XnContext* pContext)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pContext);

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);
	xnDumpFileWriteString(pContext->pDumpDataFlow, "%llu,WaitNoneUpdateAll,Application,\n", nNow);

	nRetVal = xnPlayRecording(pContext, TRUE);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnUpdateGraph(pContext, NULL);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnBool XN_CALLBACK_TYPE xnDidAnyNodeAdvanced(void* pConditionData)
{
	XnContext* pContext = (XnContext*)pConditionData;

	for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
	{
		XnInternalNodeData* pNode = it->Value();
		if (xnDidNodeAdvanced(pNode))
		{
			return TRUE;
		}
	}

	return (FALSE);
}

XN_C_API XnStatus xnWaitAnyUpdateAll(XnContext* pContext)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pContext);

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);
	xnDumpFileWriteString(pContext->pDumpDataFlow, "%llu,WaitAnyUpdateAll,Application,\n", nNow);
	
	// wait on new data event
	nRetVal = xnWaitForCondition(pContext, xnDidAnyNodeAdvanced, pContext);
	XN_IS_STATUS_OK(nRetVal);

	// we have new data, now just update all
	nRetVal = xnUpdateGraph(pContext, NULL);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnWaitAndUpdateData(XnNodeHandle hInstance)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(hInstance);

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);
	xnDumpFileWriteString(hInstance->pContext->pDumpDataFlow, "%llu,WaitAndUpdateData,Application,\n", nNow);

	// wait on new data event
	nRetVal = xnWaitForCondition(hInstance->pContext, xnDidNodeAdvanced, hInstance);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnUpdateGraph(hInstance->pContext, hInstance->pNodeInfo);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

static XnStatus xnStartGeneratingTreeImpl(XnNodeInfo* pNode)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// first start with dependencies
	for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pNode->pNeededTrees);
		xnNodeInfoListIteratorIsValid(it);
		it = xnNodeInfoListGetNext(it))
	{
		XnNodeInfo* pNeeded = xnNodeInfoListGetCurrent(it);
		nRetVal = xnStartGeneratingTreeImpl(pNeeded);
		XN_IS_STATUS_OK(nRetVal);
	}

	// now start root (if this is a generator)
	if (pNode->hNode->pModuleInstance->pLoaded->pInterface->HierarchyType.IsSet(XN_NODE_TYPE_GENERATOR) &&
		!xnIsGenerating(pNode->hNode))
	{
		nRetVal = xnStartGeneratingImpl(pNode->hNode);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnStartGeneratingAll(XnContext* pContext)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pContext);
		
	for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
	{
		XnInternalNodeData* pData = it->Value();
		nRetVal = xnStartGeneratingTreeImpl(pData->pNodeInfo);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnStopGeneratingAll(XnContext* pContext)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pContext);

	for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
	{
		// if this is a generator, stop it
		if (it->Value()->pModuleInstance->pLoaded->pInterface->HierarchyType.IsSet(XN_NODE_TYPE_GENERATOR))
		{
			XnInternalNodeData* pData = it->Value();
			nRetVal = xnStopGenerating(pData);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnSetGlobalMirror(XnContext* pContext, XnBool bMirror)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pContext);

	for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
	{
		// if it supports mirror capability, set it
		XnInternalNodeData* pData = it->Value();
		if (xnIsCapabilitySupported(pData, XN_CAPABILITY_MIRROR))
		{
			nRetVal = xnSetMirror(pData, bMirror);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	pContext->bGlobalMirrorSet = TRUE;
	pContext->bGlobalMirror = bMirror;
	
	return (XN_STATUS_OK);
}

XN_C_API XnBool xnGetGlobalMirror(XnContext* pContext)
{
	return pContext->bGlobalMirror;
}

XN_C_API XnStatus xnGetGlobalErrorState(XnContext* pContext)
{
	return pContext->globalErrorState;
}

XN_C_API XnStatus xnRegisterToGlobalErrorStateChange(XnContext* pContext, XnErrorStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_INPUT_PTR(handler);
	XN_VALIDATE_OUTPUT_PTR(phCallback);

	nRetVal = pContext->globalErrorChangeEvent.Register(handler, pCookie, *phCallback);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API void xnUnregisterFromGlobalErrorStateChange(XnContext* pContext, XnCallbackHandle hCallback)
{
	XN_ASSERT(pContext != NULL);
	XN_ASSERT(hCallback != NULL);

	pContext->globalErrorChangeEvent.Unregister(hCallback);
}

XN_C_API XnStatus XN_C_DECL xnRegisterToNodeCreation(XnContext* pContext, XnNodeCreationHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_INPUT_PTR(handler);
	XN_VALIDATE_OUTPUT_PTR(phCallback);

	nRetVal = pContext->nodeCreationEvent.Register(handler, pCookie, *phCallback);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API void XN_C_DECL xnUnregisterFromNodeCreation(XnContext* pContext, XnCallbackHandle hCallback)
{
	XN_ASSERT(pContext != NULL);
	XN_ASSERT(hCallback != NULL);

	pContext->nodeCreationEvent.Unregister(hCallback);
}

XN_C_API XnStatus XN_C_DECL xnRegisterToNodeDestruction(XnContext* pContext, XnNodeDestructionHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_INPUT_PTR(handler);
	XN_VALIDATE_OUTPUT_PTR(phCallback);

	nRetVal = pContext->nodeDestructionEvent.Register(handler, pCookie, *phCallback);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API void XN_C_DECL xnUnregisterFromNodeDestruction(XnContext* pContext, XnCallbackHandle hCallback)
{
	XN_ASSERT(pContext != NULL);
	XN_ASSERT(hCallback != NULL);

	pContext->nodeDestructionEvent.Unregister(hCallback);
}

XN_C_API XnStatus xnGetNodeHandleByName(XnContext* pContext, const XnChar* strInstanceName, XnNodeHandle* phNode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnGetRefNodeHandleByName(pContext, strInstanceName, phNode);
	XN_IS_STATUS_OK(nRetVal);

	// backwards compatibility
	xnProductionNodeRelease(*phNode);
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnGetRefNodeHandleByName(XnContext* pContext, const XnChar* strInstanceName, XnNodeHandle* phNode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_INPUT_PTR(strInstanceName);
	XN_VALIDATE_OUTPUT_PTR(phNode);

	*phNode = NULL;

	XnNodeHandle hNode;
	nRetVal = pContext->nodesMap.Get(strInstanceName, hNode);
	if (nRetVal == XN_STATUS_NO_MATCH)
	{
		return XN_STATUS_BAD_NODE_NAME;
	}
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnProductionNodeAddRef(hNode);
	XN_IS_STATUS_OK(nRetVal);

	*phNode = hNode;

	return (XN_STATUS_OK);
}

static XnStatus xnEnumerateExistingNodesImpl(XnContext* pContext, XnNodeInfoList** ppList, XnProductionNodeType* pType)
{
	XnStatus nRetVal = XN_STATUS_OK;

	*ppList = NULL;

	XnNodeInfoList* pResult;
	nRetVal = xnNodeInfoListAllocate(&pResult);
	XN_IS_STATUS_OK(nRetVal);

	for (XnNodesMap::Iterator it = pContext->nodesMap.Begin(); it != pContext->nodesMap.End(); ++it)
	{
		XnNodeInfo* pCurrInfo = it->Value()->pNodeInfo;
		if (pType == NULL ||
			*pType == pCurrInfo->Description.Type)
		{
			// add it to the list
			nRetVal = xnNodeInfoListAddNode(pResult, pCurrInfo);
			if (nRetVal != XN_STATUS_OK)
			{
				xnNodeInfoListFree(pResult);
				return (nRetVal);
			}
		}
	}

	*ppList = pResult;

	return (XN_STATUS_OK);
}


XN_C_API XnStatus xnEnumerateExistingNodes(XnContext* pContext, XnNodeInfoList** ppList)
{
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_OUTPUT_PTR(ppList);

	return xnEnumerateExistingNodesImpl(pContext, ppList, NULL);
}

XN_C_API XnStatus xnEnumerateExistingNodesByType(XnContext* pContext, XnProductionNodeType type, XnNodeInfoList** ppList)
{
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_OUTPUT_PTR(ppList);

	return xnEnumerateExistingNodesImpl(pContext, ppList, &type);
}

XN_C_API XnStatus xnFindExistingRefNodeByType(XnContext* pContext, XnProductionNodeType type, XnNodeHandle* phNode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_OUTPUT_PTR(phNode);

	XnNodeInfoList* pList;
	nRetVal = xnEnumerateExistingNodesByType(pContext, type, &pList);
	XN_IS_STATUS_OK(nRetVal);

	// take first
	XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pList);

	// make sure it's valid
	if (!xnNodeInfoListIteratorIsValid(it))
	{
		xnNodeInfoListFree(pList);
		return XN_STATUS_NO_MATCH;
	}

	XnNodeInfo* pNodeInfo = xnNodeInfoListGetCurrent(it);
	*phNode = xnNodeInfoGetRefHandle(pNodeInfo);
	xnNodeInfoListFree(pList);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnFindExistingNodeByType(XnContext* pContext, XnProductionNodeType type, XnNodeHandle* phNode)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = xnFindExistingRefNodeByType(pContext, type, phNode);
	XN_IS_STATUS_OK(nRetVal);

	xnProductionNodeRelease(*phNode);
	
	return (XN_STATUS_OK);
}

//---------------------------------------------------------------------------
// State Changes registration translation
//---------------------------------------------------------------------------

typedef struct XnModuleStateCookie
{
	XnNodeHandle hNode;
	XnStateChangedHandler userHandler;;
	void* pUserCookie;
	XnCallbackHandle hModuleCallback;
} XnModuleStateCookie;

typedef XnStatus (XN_CALLBACK_TYPE* XnModuleRegisterStateChangeFuncPtr)(XnModuleNodeHandle hNode, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
typedef void (XN_CALLBACK_TYPE* XnModuleUnregisterStateChangeFuncPtr)(XnModuleNodeHandle hNode, XnCallbackHandle hCallback);

void XN_CALLBACK_TYPE xnModuleStateChanged(void* pCookie)
{
	XnModuleStateCookie* pStateCookie = (XnModuleStateCookie*)pCookie;
	pStateCookie->userHandler(pStateCookie->hNode, pStateCookie->pUserCookie);
}

XnStatus xnRegisterToModuleStateChange(XnModuleRegisterStateChangeFuncPtr funcPtr, XnModuleNodeHandle hModuleNode, XnNodeHandle hNode, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnModuleStateCookie* pModuleCookie = NULL;
	XN_VALIDATE_CALLOC(pModuleCookie, XnModuleStateCookie, 1);
	pModuleCookie->hNode = hNode;
	pModuleCookie->userHandler = handler;
	pModuleCookie->pUserCookie = pCookie;

	nRetVal = funcPtr(hModuleNode, xnModuleStateChanged, pModuleCookie, &pModuleCookie->hModuleCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pModuleCookie);
		return (nRetVal);
	}

	// keep the cookie, so we can free it when node is destroyed
	hNode->pRegistrationCookiesHash->Set(pModuleCookie, pModuleCookie);

	*phCallback = pModuleCookie;
	return (XN_STATUS_OK);
}

void xnUnregisterFromModuleStateChange(XnModuleUnregisterStateChangeFuncPtr funcPtr, XnModuleNodeHandle hModuleNode, XnCallbackHandle hCallback)
{
	XnModuleStateCookie* pModuleCookie = (XnModuleStateCookie*)hCallback;
	pModuleCookie->hNode->pRegistrationCookiesHash->Remove(pModuleCookie);
	funcPtr(hModuleNode, pModuleCookie->hModuleCallback);
	xnOSFree(pModuleCookie);
}

//---------------------------------------------------------------------------
// Lock Aware
//---------------------------------------------------------------------------
static XnStatus xnSetLockStateImpl(XnNodeHandle hInstance, XnBool bLocked)
{
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->LockAware.SetLockState);
	return pInterface->LockAware.SetLockState(hModuleNode, bLocked);
}

XnBool xnGetLockStateImpl(XnNodeHandle hInstance)
{
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->LockAware.GetLockState, FALSE);
	return pInterface->LockAware.GetLockState(hModuleNode);
}

static XnStatus xnRegisterToLockChangeImpl(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->LockAware.RegisterToLockChange);
	return xnRegisterToModuleStateChange(pInterface->LockAware.RegisterToLockChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}

void xnUnregisterFromLockChangeImpl(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->LockAware.UnregisterFromLockChange, );
	xnUnregisterFromModuleStateChange(pInterface->LockAware.UnregisterFromLockChange, hModuleNode, hCallback);
}

//---------------------------------------------------------------------------
// Error State Capability
//---------------------------------------------------------------------------
XnStatus xnGetNodeErrorStateImpl(XnNodeHandle hInstance)
{
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->ErrorState.GetErrorState, XN_STATUS_OK);
	return pInterface->ErrorState.GetErrorState(hModuleNode);
}

XN_C_API XnStatus xnGetNodeErrorState(XnNodeHandle hInstance)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	if (!xnIsCapabilitySupported(hInstance, XN_CAPABILITY_ERROR_STATE))
	{
		return (XN_STATUS_OK);
	}
	else
	{
		return xnGetNodeErrorStateImpl(hInstance);
	}
}

XN_C_API XnStatus xnRegisterToNodeErrorStateChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->ErrorState.RegisterToErrorStateChange);
	return xnRegisterToModuleStateChange(pInterface->ErrorState.RegisterToErrorStateChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromNodeErrorStateChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->ErrorState.UnregisterFromErrorStateChange, );
	xnUnregisterFromModuleStateChange(pInterface->ErrorState.UnregisterFromErrorStateChange, hModuleNode, hCallback);
}

//---------------------------------------------------------------------------
// General Int Capability
//---------------------------------------------------------------------------
XN_C_API XnStatus xnGetGeneralIntRange(XnNodeHandle hInstance, const XnChar* strCap, XnInt32* pnMin, XnInt32* pnMax, XnInt32* pnStep, XnInt32* pnDefault, XnBool* pbIsAutoSupported)
{
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->GeneralInt.GetRange);
	return pInterface->GeneralInt.GetRange(hModuleNode, strCap, pnMin, pnMax, pnStep, pnDefault, pbIsAutoSupported);
}

XN_C_API XnStatus xnGetGeneralIntValue(XnNodeHandle hInstance, const XnChar* strCap, XnInt32* pnValue)
{
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->GeneralInt.Get);
	return pInterface->GeneralInt.Get(hModuleNode, strCap, pnValue);
}

XN_C_API XnStatus xnSetGeneralIntValue(XnNodeHandle hInstance, const XnChar* strCap, XnInt32 nValue)
{
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->GeneralInt.Set);
	return pInterface->GeneralInt.Set(hModuleNode, strCap, nValue);
}

XN_C_API XnStatus xnRegisterToGeneralIntValueChange(XnNodeHandle hInstance, const XnChar* strCap, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->GeneralInt.RegisterToValueChange);

	XnStatus nRetVal = XN_STATUS_OK;

	XnModuleStateCookie* pModuleCookie = NULL;
	XN_VALIDATE_CALLOC(pModuleCookie, XnModuleStateCookie, 1);
	pModuleCookie->hNode = hInstance;
	pModuleCookie->userHandler = handler;
	pModuleCookie->pUserCookie = pCookie;

	nRetVal = pInterface->GeneralInt.RegisterToValueChange(hModuleNode, strCap, xnModuleStateChanged, pModuleCookie, &pModuleCookie->hModuleCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pModuleCookie);
		return (nRetVal);
	}

	// keep the cookie, so we can free it when node is destroyed
	hInstance->pRegistrationCookiesHash->Set(pModuleCookie, pModuleCookie);

	*phCallback = pModuleCookie;
	return (XN_STATUS_OK);
}

XN_C_API void xnUnregisterFromGeneralIntValueChange(XnNodeHandle hInstance, const XnChar* strCap, XnCallbackHandle hCallback)
{
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->GeneralInt.UnregisterFromValueChange, );

	XnModuleStateCookie* pModuleCookie = (XnModuleStateCookie*)hCallback;
	pModuleCookie->hNode->pRegistrationCookiesHash->Remove(pModuleCookie);
	pInterface->GeneralInt.UnregisterFromValueChange(hModuleNode, strCap, pModuleCookie->hModuleCallback);
	xnOSFree(pModuleCookie);
}

//---------------------------------------------------------------------------
// Production Nodes
//---------------------------------------------------------------------------

inline XnBool xnAreChangesAllowed(XnNodeHandle hInstance)
{
	// changes are allowed if the node is not locked
	// - OR - 
	// it is locked, but the locker started a change, and the change comes from the same thread.
	XN_THREAD_ID nCurrThread = 0;
	return (hInstance->LockData.nCurrentLock == 0 ||
		(xnOSGetCurrentThreadID(&nCurrThread) == XN_STATUS_OK && hInstance->LockData.nCurrentThread == nCurrThread));
}

XN_C_API XnBool xnIsCapabilitySupported(XnNodeHandle hInstance, const XnChar* strCapabilityName)
{
	if ((hInstance == NULL) || (strCapabilityName == NULL))
	{
		XN_ASSERT(FALSE);
		return FALSE;
	}

	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->ProductionNode.IsCapabilitySupported(hModuleNode, strCapabilityName);
}

XN_C_API XnStatus xnSetIntProperty(XnNodeHandle hInstance, const XnChar* strName, XnUInt64 nValue)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->ProductionNode.SetIntProperty);
	return pInterface->ProductionNode.SetIntProperty(hModuleNode, strName, nValue);
}

XN_C_API XnStatus xnSetRealProperty(XnNodeHandle hInstance, const XnChar* strName, XnDouble dValue)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->ProductionNode.SetRealProperty);
	return pInterface->ProductionNode.SetRealProperty(hModuleNode, strName, dValue);
}

XN_C_API XnStatus xnSetStringProperty(XnNodeHandle hInstance, const XnChar* strName, const XnChar* strValue)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->ProductionNode.SetStringProperty);
	return pInterface->ProductionNode.SetStringProperty(hModuleNode, strName, strValue);
}

XN_C_API XnStatus xnSetGeneralProperty(XnNodeHandle hInstance, const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->ProductionNode.SetGeneralProperty);
	return pInterface->ProductionNode.SetGeneralProperty(hModuleNode, strName, nBufferSize, pBuffer);
}

XN_C_API XnStatus xnGetIntProperty(XnNodeHandle hInstance, const XnChar* strName, XnUInt64* pnValue)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->ProductionNode.GetIntProperty);
	return pInterface->ProductionNode.GetIntProperty(hModuleNode, strName, pnValue);
}

XN_C_API XnStatus xnGetRealProperty(XnNodeHandle hInstance, const XnChar* strName, XnDouble* pdValue)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->ProductionNode.GetRealProperty);
	return pInterface->ProductionNode.GetRealProperty(hModuleNode, strName, pdValue);
}

XN_C_API XnStatus xnGetStringProperty(XnNodeHandle hInstance, const XnChar* strName, XnChar* csValue, XnUInt32 nBufSize)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->ProductionNode.GetStringProperty);
	return pInterface->ProductionNode.GetStringProperty(hModuleNode, strName, csValue, nBufSize);
}

XN_C_API XnStatus xnGetGeneralProperty(XnNodeHandle hInstance, const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XnProductionNodeInterfaceContainer* pInterface = hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->ProductionNode.GetGeneralProperty);
	return pInterface->ProductionNode.GetGeneralProperty(hModuleNode, strName, nBufferSize, pBuffer);
}

XN_C_API XnStatus xnLockNodeForChanges(XnNodeHandle hInstance, XnLockHandle* phLock)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(hInstance);

	if (hInstance->LockData.nCurrentLock != 0)
	{
		return (XN_STATUS_NODE_IS_LOCKED);
	}

	// lock it
	if (xnIsCapabilitySupported(hInstance, XN_CAPABILITY_LOCK_AWARE))
	{
		nRetVal = xnSetLockStateImpl(hInstance, TRUE);
		XN_IS_STATUS_OK(nRetVal);
	}

	// give it a unique lock handle
	XnUInt32 nLockID = ++hInstance->pContext->nLastLockID;

	hInstance->LockData.nCurrentLock = nLockID;

	// return to user
	*phLock = nLockID;
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUnlockNodeForChanges(XnNodeHandle hInstance, XnLockHandle hLock)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate
	XN_VALIDATE_INPUT_PTR(hInstance);
	if (hInstance->LockData.nCurrentLock != hLock)
	{
		return (XN_STATUS_BAD_PARAM);
	}

	if (xnIsCapabilitySupported(hInstance, XN_CAPABILITY_LOCK_AWARE))
	{
		nRetVal = xnSetLockStateImpl(hInstance, FALSE);
		XN_IS_STATUS_OK(nRetVal);
	}

	// and remove lock
	hInstance->LockData.nCurrentLock = 0;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLockedNodeStartChanges(XnNodeHandle hInstance, XnLockHandle hLock)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(hInstance);
	if (hLock == 0)
	{
		return (XN_STATUS_INVALID_OPERATION);
	}

	if (hInstance->LockData.nCurrentLock != hLock)
	{
		return (XN_STATUS_NODE_IS_LOCKED);
	}

	nRetVal = xnOSGetCurrentThreadID(&hInstance->LockData.nCurrentThread);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLockedNodeEndChanges(XnNodeHandle hInstance, XnLockHandle hLock)
{
	if (hInstance->LockData.nCurrentLock != hLock)
	{
		return (XN_STATUS_BAD_PARAM);
	}

	hInstance->LockData.nCurrentThread = 0;
	return (XN_STATUS_OK);
}

static XnBool isNodeNeededBy(XnNodeInfo* pNodeInfo, XnNodeInfo* pNeededNodeInfo)
{
	XnNodeInfoList* pNeededNodes = xnNodeInfoGetNeededNodes(pNodeInfo);

	for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pNeededNodes);
		xnNodeInfoListIteratorIsValid(it);
		it = xnNodeInfoListGetNext(it))
	{
		XnNodeInfo* pNeeded = xnNodeInfoListGetCurrent(it);

		if (pNeeded == pNeededNodeInfo || isNodeNeededBy(pNeeded, pNeededNodeInfo))
		{
			return TRUE;
		}
	}

	return FALSE;
}

XN_C_API XnStatus xnAddNeededNode(XnNodeHandle hInstance, XnNodeHandle hNeededNode)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(hInstance);
	XN_VALIDATE_INPUT_PTR(hNeededNode);

	if (hInstance == hNeededNode)
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	// make sure this does not create a loop dependency
	if (isNodeNeededBy(hNeededNode->pNodeInfo, hInstance->pNodeInfo))
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	// increase ref of needed node
	nRetVal = xnProductionNodeAddRef(hNeededNode);
	XN_IS_STATUS_OK(nRetVal);

	XnNodeInfoList* pNeededNodesList = hInstance->pNodeInfo->pNeededTrees;
	nRetVal = xnNodeInfoListAddNode(pNeededNodesList, hNeededNode->pNodeInfo);
	if (nRetVal != XN_STATUS_OK)
	{
		xnProductionNodeRelease(hNeededNode);
		return (nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnRemoveNeededNode(XnNodeHandle hInstance, XnNodeHandle hNeededNode)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(hInstance);
	XN_VALIDATE_INPUT_PTR(hNeededNode);

	// find it in the list
	XnNodeInfoList* pNeededNodes = xnNodeInfoGetNeededNodes(hInstance->pNodeInfo);

	for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pNeededNodes);
		xnNodeInfoListIteratorIsValid(it);
		it = xnNodeInfoListGetNext(it))
	{
		XnNodeInfo* pNeeded = xnNodeInfoListGetCurrent(it);
		if (pNeeded->hNode == hNeededNode)
		{
			// remove it
			nRetVal = xnNodeInfoListRemove(pNeededNodes, it);
			XN_IS_STATUS_OK(nRetVal);

			// and dec ref it
			xnProductionNodeRelease(hNeededNode);

			return (XN_STATUS_OK);
		}
	}

	// if we got here, it wasn't found
	return (XN_STATUS_INVALID_OPERATION);
}

//---------------------------------------------------------------------------
// Device
//---------------------------------------------------------------------------

XN_C_API XnStatus xnCreateDevice(
	XnContext* pContext,
	XnNodeHandle* phDevice,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors)
{
	return xnCreateAnyProductionTree(pContext, XN_NODE_TYPE_DEVICE, pQuery, phDevice, pErrors);
}

//---------------------------------------------------------------------------
// Device Identification Capability
//---------------------------------------------------------------------------
XN_C_API XnStatus xnGetDeviceName(XnNodeHandle hInstance, XnChar* strBuffer, XnUInt32* pnBufferSize)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_DEVICE);
	XnDeviceInterfaceContainer* pInterface = (XnDeviceInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Identification.GetDeviceName);
	return pInterface->Identification.GetDeviceName(hModuleNode, strBuffer, pnBufferSize);
}

XN_C_API XnStatus xnGetVendorSpecificData(XnNodeHandle hInstance, XnChar* strBuffer, XnUInt32* pnBufferSize)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_DEVICE);
	XnDeviceInterfaceContainer* pInterface = (XnDeviceInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Identification.GetVendorSpecificData);
	return pInterface->Identification.GetVendorSpecificData(hModuleNode, strBuffer, pnBufferSize);
}

XN_C_API XnStatus xnGetSerialNumber(XnNodeHandle hInstance, XnChar* strBuffer, XnUInt32* pnBufferSize)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_DEVICE);
	XnDeviceInterfaceContainer* pInterface = (XnDeviceInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Identification.GetSerialNumber);
	return pInterface->Identification.GetSerialNumber(hModuleNode, strBuffer, pnBufferSize);
}

//---------------------------------------------------------------------------
// Generators
//---------------------------------------------------------------------------

static XnStatus xnStartGeneratingImpl(XnNodeHandle hInstance)
{
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Generator.StartGenerating(hModuleNode);
}

XN_C_API XnStatus xnStartGenerating(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	return xnStartGeneratingTreeImpl(hInstance->pNodeInfo);
}

XN_C_API XnBool xnIsGenerating(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, FALSE);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Generator.IsGenerating(hModuleNode);
}

XN_C_API XnStatus xnStopGenerating(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	pInterface->Generator.StopGenerating(hModuleNode);
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnRegisterToGenerationRunningChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return xnRegisterToModuleStateChange(pInterface->Generator.RegisterToGenerationRunningChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromGenerationRunningChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR,);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	xnUnregisterFromModuleStateChange(pInterface->Generator.UnregisterFromGenerationRunningChange, hModuleNode, hCallback);
}

XN_C_API XnStatus xnRegisterToNewDataAvailable(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return xnRegisterToModuleStateChange(pInterface->Generator.RegisterToNewDataAvailable, hModuleNode, hInstance, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromNewDataAvailable(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR,);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	xnUnregisterFromModuleStateChange(pInterface->Generator.UnregisterFromNewDataAvailable, hModuleNode, hCallback);
}

XnBool xnIsNewDataAvailableImpl(XnNodeHandle hInstance, XnUInt64* pnTimestamp)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, FALSE);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Generator.IsNewDataAvailable(hModuleNode, pnTimestamp);
}

XN_C_API XnBool xnIsNewDataAvailable(XnNodeHandle hInstance, XnUInt64* pnTimestamp)
{
	XnUInt64 nTemp;
	if (pnTimestamp == NULL)
	{
		pnTimestamp = &nTemp;
	}

	return xnIsNewDataAvailableImpl(hInstance, pnTimestamp);
}

static XnStatus xnUpdateDataImpl(XnNodeHandle hInstance)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	nRetVal = pInterface->Generator.UpdateData(hModuleNode);
	XN_IS_STATUS_OK(nRetVal);

	hInstance->bWasDataRead = TRUE;
	hInstance->bIsNewData = TRUE;
	xnMarkFPSFrame(hInstance->pContext, &hInstance->readFPS);
	xnUpdateMetaData(hInstance);

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);
	xnDumpFileWriteString(hInstance->pContext->pDumpDataFlow, "%llu,Update,%s,%llu\n", nNow, hInstance->pNodeInfo->strInstanceName, xnGetTimestamp(hInstance));

	return XN_STATUS_OK;
}

XN_C_API XnBool xnIsDataNew(XnNodeHandle hInstance)
{
	return hInstance->bIsNewData;
}

XN_C_API const void* xnGetData(XnNodeHandle hInstance)
{
	XN_VALIDATE_PTR(hInstance, NULL);
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, NULL);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Generator.GetData(hModuleNode);
}

XN_C_API XnUInt32 xnGetDataSize(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, (XnUInt32)-1);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Generator.GetDataSize(hModuleNode);
}

XN_C_API XnUInt64 xnGetTimestamp(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, (XnUInt64)-1);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Generator.GetTimestamp(hModuleNode);
}

XN_C_API XnUInt32 xnGetFrameID(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, (XnUInt32)-1);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Generator.GetFrameID(hModuleNode);
}

//---------------------------------------------------------------------------
// Recorder
//---------------------------------------------------------------------------
XN_C_API XnStatus xnCreateRecorder(XnContext* pContext, const XnChar* strFormatName, XnNodeHandle* phRecorder)
{
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_OUTPUT_PTR(phRecorder);

	XnStatus nRetVal = XN_STATUS_OK;
	XnNodeHandle hRecorder = NULL;
	const XnChar* strFormat = NULL;
	XnNodeInfoList* pList = NULL;

	// search for all recorders
	nRetVal = xnEnumerateProductionTrees(pContext, XN_NODE_TYPE_RECORDER, NULL, &pList, NULL);
	XN_IS_STATUS_OK(nRetVal);

	if (strFormatName == NULL)
	{
		//Default format is ONI
		strFormatName = XN_FORMAT_NAME_ONI;
	}


	// now search the list for the required type
	for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pList);
		xnNodeInfoListIteratorIsValid(it);
		it = xnNodeInfoListGetNext(it))
	{
		XnNodeInfo* pInfo = xnNodeInfoListGetCurrent(it);

		// only new ones
		if (pInfo->hNode == NULL)
		{
			nRetVal = xnCreateProductionTree(pContext, pInfo, &hRecorder);
			if (nRetVal != XN_STATUS_OK)
			{
				xnLoggerWarning(g_logger, "Failed to create recorder %s of vendor %s to check for its type: %s", pInfo->Description.strName, pInfo->Description.strVendor, xnGetStatusString(nRetVal));
				continue;
			}

			strFormat = xnGetRecorderFormat(hRecorder);
			if (xnOSStrCaseCmp(strFormat, strFormatName) == 0)
			{
				// found it
				break;
			}

			// type doesn't match, free it
			xnProductionNodeRelease(hRecorder);
			hRecorder = NULL;
		}
	}

	// free the list
	xnNodeInfoListFree(pList);

	if (hRecorder == NULL)
	{
		return XN_STATUS_BAD_PARAM;
	}

	*phRecorder = hRecorder;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnSetRecorderDestination(XnNodeHandle hRecorder, XnRecordMedium destType, const XnChar *strDest)
{
	XN_VALIDATE_INPUT_PTR(hRecorder);
	XN_VALIDATE_INTERFACE_TYPE(hRecorder, XN_NODE_TYPE_RECORDER);
	XN_VALIDATE_CHANGES_ALLOWED(hRecorder);
	//Get recorder object
	xn::RecorderImpl *pRecorderImpl = dynamic_cast<xn::RecorderImpl*>(hRecorder->pPrivateData);
	XN_VALIDATE_PTR(pRecorderImpl, XN_STATUS_ERROR);
	XnStatus nRetVal = pRecorderImpl->SetDestination(destType, strDest);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;	
}

XN_C_API XnStatus XN_C_DECL xnGetRecorderDestination(XnNodeHandle hRecorder, XnRecordMedium* pDestType, XnChar* strDest, XnUInt32 nBufSize)
{
	XN_VALIDATE_INPUT_PTR(hRecorder);
	XN_VALIDATE_INTERFACE_TYPE(hRecorder, XN_NODE_TYPE_RECORDER);
	XN_VALIDATE_CHANGES_ALLOWED(hRecorder);
	XN_VALIDATE_OUTPUT_PTR(pDestType);
	XN_VALIDATE_OUTPUT_PTR(strDest);
	//Get recorder object
	xn::RecorderImpl *pRecorderImpl = dynamic_cast<xn::RecorderImpl*>(hRecorder->pPrivateData);
	XN_VALIDATE_PTR(pRecorderImpl, XN_STATUS_ERROR);
	XnStatus nRetVal = pRecorderImpl->GetDestination(*pDestType, strDest, nBufSize);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;	
}

XN_C_API XnStatus xnAddNodeToRecording(XnNodeHandle hRecorder, XnNodeHandle hNode, XnCodecID compression)
{
	XN_VALIDATE_INPUT_PTR(hRecorder);
	XN_VALIDATE_INTERFACE_TYPE(hRecorder, XN_NODE_TYPE_RECORDER);
	XN_VALIDATE_INPUT_PTR(hNode);
	XN_VALIDATE_CHANGES_ALLOWED(hRecorder);
	XnStatus nRetVal = XN_STATUS_OK;

	//Get recorder object
	xn::RecorderImpl *pRecorderImpl = dynamic_cast<xn::RecorderImpl*>(hRecorder->pPrivateData);
	XN_VALIDATE_PTR(pRecorderImpl, XN_STATUS_ERROR);
	//Add node to recorder
	xn::ProductionNode node(hNode);
	nRetVal = pRecorderImpl->AddNode(node, compression);
	XN_IS_STATUS_OK(nRetVal);

	//Add node to recorder's dependency tree
	nRetVal = xnAddNeededNode(hRecorder, hNode);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnRemoveNodeFromRecording(XnNodeHandle hRecorder, XnNodeHandle hNode)
{
	XN_VALIDATE_INPUT_PTR(hRecorder);
	XN_VALIDATE_INTERFACE_TYPE(hRecorder, XN_NODE_TYPE_RECORDER);
	XN_VALIDATE_INPUT_PTR(hNode);
	XN_VALIDATE_CHANGES_ALLOWED(hRecorder);
	XnStatus nRetVal = XN_STATUS_OK;

	//Find node in recorder's dependencies
	XnNodeInfoList *pRecorderNeededTrees = hRecorder->pNodeInfo->pNeededTrees;
	XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pRecorderNeededTrees);
	while (xnNodeInfoListIteratorIsValid(it) && (xnNodeInfoListGetCurrent(it)->hNode != hNode))
	{
		 it = xnNodeInfoListGetNext(it);
	}

	if (!xnNodeInfoListIteratorIsValid(it))
	{
		//node not found in recorder dependency list
		return XN_STATUS_NO_MATCH;
	}

	// Remove node from recording
	xn::RecorderImpl *pRecorderImpl = dynamic_cast<xn::RecorderImpl*>(hRecorder->pPrivateData);
	XN_VALIDATE_PTR(pRecorderImpl, XN_STATUS_ERROR);

	xn::ProductionNode node(hNode);
	nRetVal = pRecorderImpl->RemoveNode(node);
	XN_IS_STATUS_OK(nRetVal);

	//Remove node from recorder's dependencies
	nRetVal = xnRemoveNeededNode(hRecorder, hNode);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnRecord(XnNodeHandle hInstance)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_RECORDER);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	//Get recorder object
	xn::RecorderImpl *pRecorderImpl = dynamic_cast<xn::RecorderImpl*>(hInstance->pPrivateData);
	XN_VALIDATE_PTR(pRecorderImpl, XN_STATUS_ERROR);
	XnStatus nRetVal = pRecorderImpl->Record();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XN_C_API const XnChar* xnGetRecorderFormat(XnNodeHandle /*hInstance*/)
{
	//TODO: Call module's function
	return "oni";
}

//---------------------------------------------------------------------------
// Player
//---------------------------------------------------------------------------
XN_C_API XnStatus xnCreatePlayer(XnContext* pContext, const XnChar* strFormatName, XnNodeHandle* phPlayer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// search for all players
	XnNodeInfoList* pList;
	nRetVal = xnEnumerateProductionTrees(pContext, XN_NODE_TYPE_PLAYER, NULL, &pList, NULL);
	XN_IS_STATUS_OK(nRetVal);

	XnNodeHandle hPlayer = NULL;

	// now search the list for the required type
	for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pList);
		xnNodeInfoListIteratorIsValid(it);
		it = xnNodeInfoListGetNext(it))
	{
		XnNodeInfo* pInfo = xnNodeInfoListGetCurrent(it);

		// only new ones
		if (pInfo->hNode == NULL)
		{
			nRetVal = xnCreateProductionTree(pContext, pInfo, &hPlayer);
			if (nRetVal != XN_STATUS_OK)
			{
				xnLoggerWarning(g_logger, "Failed to create player %s of vendor %s to check for its type: %s", pInfo->Description.strName, pInfo->Description.strVendor, xnGetStatusString(nRetVal));
				continue;
			}

			const XnChar* strFormat = xnGetPlayerSupportedFormat(hPlayer);

			if (xnOSStrCaseCmp(strFormat, strFormatName) == 0)
			{
				// found it
				break;
			}

			// type doesn't match, free it
			xnProductionNodeRelease(hPlayer);
			hPlayer = NULL;
		}
	}

	// free the list
	xnNodeInfoListFree(pList);

	if (hPlayer == NULL)
	{
		return XN_STATUS_BAD_FILE_EXT;
	}

	*phPlayer = hPlayer;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnSetPlayerRepeat(XnNodeHandle hPlayer, XnBool bRepeat)
{
	XN_VALIDATE_INPUT_PTR(hPlayer);
	XN_VALIDATE_INTERFACE_TYPE(hPlayer, XN_NODE_TYPE_PLAYER);
	XnPlayerInterfaceContainer* pInterface = (XnPlayerInterfaceContainer*)hPlayer->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hPlayer->pModuleInstance->hNode;
	return pInterface->Player.SetRepeat(hModuleNode, bRepeat);
}

XN_C_API XnStatus xnSetPlayerSource(XnNodeHandle hPlayer, XnRecordMedium sourceType, const XnChar* strSource)
{
	XN_VALIDATE_INPUT_PTR(hPlayer);
	XN_VALIDATE_INPUT_PTR(strSource);
	XN_VALIDATE_INTERFACE_TYPE(hPlayer, XN_NODE_TYPE_PLAYER);
	XN_VALIDATE_CHANGES_ALLOWED(hPlayer);
	//Get player impl object
	xn::PlayerImpl *pPlayerImpl = dynamic_cast<xn::PlayerImpl*>(hPlayer->pPrivateData);
	XN_VALIDATE_PTR(pPlayerImpl, XN_STATUS_ERROR);
	XnStatus nRetVal = pPlayerImpl->SetSource(sourceType, strSource);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;	
}

XN_C_API XnStatus xnPlayerReadNext(XnNodeHandle hPlayer)
{
	XN_VALIDATE_INPUT_PTR(hPlayer);
	XN_VALIDATE_INTERFACE_TYPE(hPlayer, XN_NODE_TYPE_PLAYER);

	xn::PlayerImpl *pPlayerImpl = dynamic_cast<xn::PlayerImpl*>(hPlayer->pPrivateData);
	XN_VALIDATE_PTR(pPlayerImpl, XN_STATUS_ERROR);
	XnStatus nRetVal = pPlayerImpl->ReadNext();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;	
}

XN_C_API XnStatus xnGetPlayerSource(XnNodeHandle hPlayer, XnRecordMedium* pSourceType, XnChar* strSource, XnUInt32 nBufSize)
{
	XN_VALIDATE_INPUT_PTR(hPlayer);
	XN_VALIDATE_OUTPUT_PTR(pSourceType);
	XN_VALIDATE_OUTPUT_PTR(strSource);
	XN_VALIDATE_INTERFACE_TYPE(hPlayer, XN_NODE_TYPE_PLAYER);
	XN_VALIDATE_CHANGES_ALLOWED(hPlayer);
	//Get player impl object
	xn::PlayerImpl *pPlayerImpl = dynamic_cast<xn::PlayerImpl*>(hPlayer->pPrivateData);
	XN_VALIDATE_PTR(pPlayerImpl, XN_STATUS_ERROR);
	XnStatus nRetVal = pPlayerImpl->GetSource(*pSourceType, strSource, nBufSize);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;	
}

XN_C_API XnStatus xnSeekPlayerToTimeStamp(XnNodeHandle hPlayer, 
										  XnInt64 nTimeOffset, 
										  XnPlayerSeekOrigin origin)
{
	XN_VALIDATE_INPUT_PTR(hPlayer);
	XN_VALIDATE_INTERFACE_TYPE(hPlayer, XN_NODE_TYPE_PLAYER);
	XN_VALIDATE_CHANGES_ALLOWED(hPlayer);
	//Get player impl object
	xn::PlayerImpl *pPlayerImpl = dynamic_cast<xn::PlayerImpl*>(hPlayer->pPrivateData);
	XN_VALIDATE_PTR(pPlayerImpl, XN_STATUS_ERROR);
	return pPlayerImpl->SeekToTimestamp(nTimeOffset, origin);
}

XN_C_API XnStatus xnSeekPlayerToFrame(XnNodeHandle hPlayer, 
									  const XnChar* strNodeName, 
									  XnInt32 nFrameOffset, 
									  XnPlayerSeekOrigin origin)
{
	XN_VALIDATE_INPUT_PTR(hPlayer);
	XN_VALIDATE_INTERFACE_TYPE(hPlayer, XN_NODE_TYPE_PLAYER);
	XN_VALIDATE_CHANGES_ALLOWED(hPlayer);
	//Get player impl object
	xn::PlayerImpl *pPlayerImpl = dynamic_cast<xn::PlayerImpl*>(hPlayer->pPrivateData);
	XN_VALIDATE_PTR(pPlayerImpl, XN_STATUS_ERROR);
	return pPlayerImpl->SeekToFrame(strNodeName, nFrameOffset, origin);
}

XN_C_API XnStatus xnTellPlayerTimestamp(XnNodeHandle hPlayer, XnUInt64* pnTimestamp)
{
	XN_VALIDATE_INPUT_PTR(hPlayer);
	XN_VALIDATE_INTERFACE_TYPE(hPlayer, XN_NODE_TYPE_PLAYER);
	XnPlayerInterfaceContainer* pInterface = (XnPlayerInterfaceContainer*)hPlayer->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hPlayer->pModuleInstance->hNode;
	return pInterface->Player.TellTimestamp(hModuleNode, pnTimestamp);
}

XN_C_API XnStatus xnTellPlayerFrame(XnNodeHandle hPlayer, const XnChar* strNodeName, XnUInt32* pnFrame)
{
	XN_VALIDATE_INPUT_PTR(hPlayer);
	XN_VALIDATE_OUTPUT_PTR(pnFrame);
	XN_VALIDATE_INTERFACE_TYPE(hPlayer, XN_NODE_TYPE_PLAYER);
	XnPlayerInterfaceContainer* pInterface = (XnPlayerInterfaceContainer*)hPlayer->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hPlayer->pModuleInstance->hNode;
	return pInterface->Player.TellFrame(hModuleNode, strNodeName, pnFrame);
}

XN_C_API XnStatus xnGetPlayerNumFrames(XnNodeHandle hPlayer, const XnChar* strNodeName, XnUInt32* pnFrames)
{
	XN_VALIDATE_INPUT_PTR(hPlayer);
	XN_VALIDATE_OUTPUT_PTR(pnFrames);
	XN_VALIDATE_INTERFACE_TYPE_RET(hPlayer, XN_NODE_TYPE_PLAYER, TRUE);
	XnPlayerInterfaceContainer* pInterface = (XnPlayerInterfaceContainer*)hPlayer->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hPlayer->pModuleInstance->hNode;
	return pInterface->Player.GetNumFrames(hModuleNode, strNodeName, pnFrames);
}

XN_C_API const XnChar* xnGetPlayerSupportedFormat(XnNodeHandle hPlayer)
{
	XN_VALIDATE_PTR(hPlayer, NULL);
	XN_VALIDATE_INTERFACE_TYPE_RET(hPlayer, XN_NODE_TYPE_PLAYER, NULL);
	XnPlayerInterfaceContainer* pInterface = (XnPlayerInterfaceContainer*)hPlayer->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hPlayer->pModuleInstance->hNode;
	return pInterface->Player.GetSupportedFormat(hModuleNode);
}

XN_C_API XnStatus xnEnumeratePlayerNodes(XnNodeHandle hPlayer, XnNodeInfoList** ppList)
{
	XN_VALIDATE_INPUT_PTR(hPlayer);
	XN_VALIDATE_INTERFACE_TYPE(hPlayer, XN_NODE_TYPE_PLAYER);
	XN_VALIDATE_OUTPUT_PTR(ppList);
	//Get player impl object
	xn::PlayerImpl *pPlayerImpl = dynamic_cast<xn::PlayerImpl*>(hPlayer->pPrivateData);
	XN_VALIDATE_PTR(pPlayerImpl, XN_STATUS_ERROR);
	return pPlayerImpl->EnumerateNodes(ppList);
}

XN_C_API XnBool xnIsPlayerAtEOF(XnNodeHandle hPlayer)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hPlayer, XN_NODE_TYPE_PLAYER, TRUE);
	XnPlayerInterfaceContainer* pInterface = (XnPlayerInterfaceContainer*)hPlayer->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hPlayer->pModuleInstance->hNode;
	return pInterface->Player.IsEOF(hModuleNode);
}

XN_C_API XnStatus xnRegisterToEndOfFileReached(XnNodeHandle hPlayer, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INPUT_PTR(hPlayer);
	XN_VALIDATE_INPUT_PTR(handler);
	XN_VALIDATE_INTERFACE_TYPE(hPlayer, XN_NODE_TYPE_PLAYER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnPlayerInterfaceContainer* pInterface = (XnPlayerInterfaceContainer*)hPlayer->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hPlayer->pModuleInstance->hNode;
	return xnRegisterToModuleStateChange(pInterface->Player.RegisterToEndOfFileReached, hModuleNode, hPlayer, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromEndOfFileReached(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	if (hCallback == NULL)
	{
		return;
	}
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_PLAYER, );
	XnPlayerInterfaceContainer* pInterface = (XnPlayerInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	xnUnregisterFromModuleStateChange(pInterface->Player.UnregisterFromEndOfFileReached, hModuleNode, hCallback);
}

XN_C_API XnStatus xnSetPlaybackSpeed(XnNodeHandle hInstance, XnDouble dSpeed)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_PLAYER);
	//Get player impl object
	xn::PlayerImpl *pPlayerImpl = dynamic_cast<xn::PlayerImpl*>(hInstance->pPrivateData);
	XN_VALIDATE_PTR(pPlayerImpl, XN_STATUS_ERROR);
	return pPlayerImpl->SetPlaybackSpeed(dSpeed);
}

XN_C_API XnDouble xnGetPlaybackSpeed(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_PLAYER, -1.0);
	//Get player impl object
	xn::PlayerImpl *pPlayerImpl = dynamic_cast<xn::PlayerImpl*>(hInstance->pPrivateData);
	XN_VALIDATE_PTR(pPlayerImpl, -1.0);
	return pPlayerImpl->GetPlaybackSpeed();
}

//---------------------------------------------------------------------------
// Mirror Capability
//---------------------------------------------------------------------------

XN_C_API XnStatus xnSetMirror(XnNodeHandle hInstance, XnBool bMirror)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Mirror.SetMirror);
	return pInterface->Mirror.SetMirror(hModuleNode, bMirror);
}

XN_C_API XnBool xnIsMirrored(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, -1);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Mirror.IsMirrored);
	return pInterface->Mirror.IsMirrored(hModuleNode);
}

XN_C_API XnStatus xnRegisterToMirrorChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Mirror.RegisterToMirrorChange);
	return xnRegisterToModuleStateChange(pInterface->Mirror.RegisterToMirrorChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromMirrorChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, );
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Mirror.UnregisterFromMirrorChange, );
	xnUnregisterFromModuleStateChange(pInterface->Mirror.UnregisterFromMirrorChange, hModuleNode, hCallback);
}

//---------------------------------------------------------------------------
// Alternative View Point Capability
//---------------------------------------------------------------------------
XN_C_API XnBool xnIsViewPointSupported(XnNodeHandle hInstance, XnNodeHandle hOther)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, FALSE);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->AlternativeViewPoint.IsViewPointSupported, FALSE);
	return pInterface->AlternativeViewPoint.IsViewPointSupported(hModuleNode, hOther);
}

XN_C_API XnStatus xnSetViewPoint(XnNodeHandle hInstance, XnNodeHandle hOther)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->AlternativeViewPoint.SetViewPoint);
	return pInterface->AlternativeViewPoint.SetViewPoint(hModuleNode, hOther);
}

XN_C_API XnStatus xnResetViewPoint(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->AlternativeViewPoint.ResetViewPoint);
	return pInterface->AlternativeViewPoint.ResetViewPoint(hModuleNode);
}

XN_C_API XnBool xnIsViewPointAs(XnNodeHandle hInstance, XnNodeHandle hOther)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, FALSE);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->AlternativeViewPoint.IsViewPointAs, FALSE);
	return pInterface->AlternativeViewPoint.IsViewPointAs(hModuleNode, hOther);
}

XN_C_API XnStatus xnRegisterToViewPointChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->AlternativeViewPoint.RegisterToViewPointChange);
	return xnRegisterToModuleStateChange(pInterface->AlternativeViewPoint.RegisterToViewPointChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromViewPointChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, );
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->AlternativeViewPoint.UnregisterFromViewPointChange, );
	xnUnregisterFromModuleStateChange(pInterface->AlternativeViewPoint.UnregisterFromViewPointChange, hModuleNode, hCallback);
}

XN_C_API XnStatus XN_C_DECL xnGetPixelCoordinatesInViewPoint(XnNodeHandle hInstance, XnNodeHandle hOther, XnUInt32 x, XnUInt32 y, XnUInt32* pAltX, XnUInt32* pAltY)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->AlternativeViewPoint.GetPixelCoordinatesInViewPoint);
	return pInterface->AlternativeViewPoint.GetPixelCoordinatesInViewPoint(hModuleNode, hOther, x, y, pAltX, pAltY);
}

//---------------------------------------------------------------------------
// Frame Sync Capability
//---------------------------------------------------------------------------
XN_C_API XnBool xnCanFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, FALSE);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->FrameSync.CanFrameSyncWith, FALSE);
	return pInterface->FrameSync.CanFrameSyncWith(hModuleNode, hOther);
}

XN_C_API XnStatus xnFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XN_VALIDATE_INPUT_PTR(hOther);
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);

	// check if this is already the case
	if (hInstance->hFrameSyncedWith == hOther)
	{
		return (XN_STATUS_OK);
	}

	// TODO: support more than one node. Right now, every node can only be synched to one other node.
	if (hInstance->hFrameSyncedWith != NULL || hOther->hFrameSyncedWith != NULL)
	{
		XN_RETURN_WITH_WARNING_LOG(g_logger, XN_STATUS_NOT_IMPLEMENTED, "Currently, a node can be frame synched to one node only.");
	}

	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->FrameSync.FrameSyncWith);
	XnStatus nRetVal = pInterface->FrameSync.FrameSyncWith(hModuleNode, hOther);
	XN_IS_STATUS_OK(nRetVal);

	// store this
	hInstance->hFrameSyncedWith = hOther;
	hOther->hFrameSyncedWith = hInstance;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnStopFrameSyncWith(XnNodeHandle hInstance, XnNodeHandle hOther)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XN_VALIDATE_INPUT_PTR(hOther);
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);

	if (hInstance->hFrameSyncedWith != hOther)
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->FrameSync.StopFrameSyncWith);
	XnStatus nRetVal = pInterface->FrameSync.StopFrameSyncWith(hModuleNode, hOther);
	XN_IS_STATUS_OK(nRetVal);

	// store this
	hOther->hFrameSyncedWith = NULL;
	hInstance->hFrameSyncedWith = NULL;

	return (XN_STATUS_OK);
}

XnBool xnIsFrameSyncedWithImpl(XnNodeHandle hInstance, XnNodeHandle hOther)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, FALSE);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->FrameSync.IsFrameSyncedWith, FALSE);
	return pInterface->FrameSync.IsFrameSyncedWith(hModuleNode, hOther);
}

XN_C_API XnBool xnIsFrameSyncedWith(XnNodeHandle hInstance, XnNodeHandle hOther)
{
	return (hInstance->hFrameSyncedWith == hOther);
}

XN_C_API XnStatus xnRegisterToFrameSyncChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->FrameSync.RegisterToFrameSyncChange);
	return xnRegisterToModuleStateChange(pInterface->FrameSync.RegisterToFrameSyncChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromFrameSyncChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GENERATOR, );
	XnGeneratorInterfaceContainer* pInterface = (XnGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->FrameSync.UnregisterFromFrameSyncChange, );
	xnUnregisterFromModuleStateChange(pInterface->FrameSync.UnregisterFromFrameSyncChange, hModuleNode, hCallback);
}

//---------------------------------------------------------------------------
// Map Generators
//---------------------------------------------------------------------------

XN_C_API XnUInt32 xnGetSupportedMapOutputModesCount(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_MAP_GENERATOR, 0);
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Map.GetSupportedMapOutputModesCount(hModuleNode);
}

XN_C_API XnStatus xnGetSupportedMapOutputModes(XnNodeHandle hInstance, XnMapOutputMode* aModes, XnUInt32* pnCount)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_MAP_GENERATOR);
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Map.GetSupportedMapOutputModes(hModuleNode, aModes, pnCount);
}

XN_C_API XnStatus xnSetMapOutputMode(XnNodeHandle hInstance, const XnMapOutputMode* pOutputMode)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_MAP_GENERATOR);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Map.SetMapOutputMode(hModuleNode, pOutputMode);
}

XN_C_API XnStatus xnGetMapOutputMode(XnNodeHandle hInstance, XnMapOutputMode* pOutputMode)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_MAP_GENERATOR);
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Map.GetMapOutputMode(hModuleNode, pOutputMode);
}

XN_C_API XnStatus xnRegisterToMapOutputModeChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_MAP_GENERATOR);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return xnRegisterToModuleStateChange(pInterface->Map.RegisterToMapOutputModeChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromMapOutputModeChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_MAP_GENERATOR,);
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	xnUnregisterFromModuleStateChange(pInterface->Map.UnregisterFromMapOutputModeChange, hModuleNode, hCallback);
}

XN_C_API XnUInt32 xnGetBytesPerPixel(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_MAP_GENERATOR, XnUInt32(-1));
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	if ((void*)pInterface->Map.GetBytesPerPixel == XN_SPECIAL_BC_BEHAVIOR)
	{
		// special BC behavior: old image nodes did not implement the GetBytesPerPixel
		// so we need to implement it for them
		return xnGetBytesPerPixelForPixelFormat(xnGetPixelFormat(hInstance));
	}
	else
	{
		return pInterface->Map.GetBytesPerPixel(hModuleNode);
	}
}

//---------------------------------------------------------------------------
// Cropping Capability
//---------------------------------------------------------------------------

XN_C_API XnStatus xnSetCropping(XnNodeHandle hInstance, const XnCropping* pCropping)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_MAP_GENERATOR);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Cropping.SetCropping);
	return pInterface->Cropping.SetCropping(hModuleNode, pCropping);
}

XN_C_API XnStatus xnGetCropping(XnNodeHandle hInstance, XnCropping* pCropping)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_MAP_GENERATOR);
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Cropping.GetCropping);
	return pInterface->Cropping.GetCropping(hModuleNode, pCropping);
}

XN_C_API XnStatus xnRegisterToCroppingChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_MAP_GENERATOR);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Cropping.RegisterToCroppingChange);
	return xnRegisterToModuleStateChange(pInterface->Cropping.RegisterToCroppingChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromCroppingChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_MAP_GENERATOR, );
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Cropping.UnregisterFromCroppingChange,);
	xnUnregisterFromModuleStateChange(pInterface->Cropping.UnregisterFromCroppingChange, hModuleNode, hCallback);
}

//---------------------------------------------------------------------------
// Anti Flicker Capability
//---------------------------------------------------------------------------

XN_C_API XnStatus xnSetPowerLineFrequency(XnNodeHandle hInstance, XnPowerLineFrequency nFrequency)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_MAP_GENERATOR);
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->AntiFlicker.SetPowerLineFrequency);
	return pInterface->AntiFlicker.SetPowerLineFrequency(hModuleNode, nFrequency);
}

XN_C_API XnPowerLineFrequency xnGetPowerLineFrequency(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_MAP_GENERATOR, XN_POWER_LINE_FREQUENCY_OFF);
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->AntiFlicker.GetPowerLineFrequency, XN_POWER_LINE_FREQUENCY_OFF);
	return pInterface->AntiFlicker.GetPowerLineFrequency(hModuleNode);
}

XN_C_API XnStatus xnRegisterToPowerLineFrequencyChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_MAP_GENERATOR);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->AntiFlicker.RegisterToPowerLineFrequencyChange);
	return xnRegisterToModuleStateChange(pInterface->AntiFlicker.RegisterToPowerLineFrequencyChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromPowerLineFrequencyChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_MAP_GENERATOR, );
	XnMapGeneratorInterfaceContainer* pInterface = (XnMapGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->AntiFlicker.UnregisterFromPowerLineFrequencyChange,);
	xnUnregisterFromModuleStateChange(pInterface->AntiFlicker.UnregisterFromPowerLineFrequencyChange, hModuleNode, hCallback);
}

//---------------------------------------------------------------------------
// Depth Generators
//---------------------------------------------------------------------------

xn::DepthPrivateData::DepthPrivateData() : 
	m_fRealWorldXtoZ(0), 
	m_fRealWorldYtoZ(0),
	m_hNode(NULL),
	m_hFOVCallbackHandle(NULL)
{
}

xn::DepthPrivateData::~DepthPrivateData()
{
	XN_ASSERT(m_hFOVCallbackHandle == NULL);
}

void xn::DepthPrivateData::BeforeNodeDestroy()
{
	if (m_hFOVCallbackHandle != NULL)
	{
		xnUnregisterFromDepthFieldOfViewChange(m_hNode, m_hFOVCallbackHandle);
		m_hFOVCallbackHandle = NULL;
	}
}

XnStatus xn::DepthPrivateData::Init(XnNodeHandle hDepth)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = xnRegisterToDepthFieldOfViewChange(hDepth, &xn::DepthPrivateData::FieldOfViewChangedCallback, this, &m_hFOVCallbackHandle);
	XN_IS_STATUS_OK(nRetVal);

	m_hNode = hDepth;

	OnFieldOfViewChanged();

	return (XN_STATUS_OK);
}

void xn::DepthPrivateData::OnFieldOfViewChanged()
{
	XnFieldOfView FOV;
	xnGetDepthFieldOfView(m_hNode, &FOV);

	m_fRealWorldXtoZ = tan(FOV.fHFOV/2)*2;
	m_fRealWorldYtoZ = tan(FOV.fVFOV/2)*2;
}

void XN_CALLBACK_TYPE xn::DepthPrivateData::FieldOfViewChangedCallback(XnNodeHandle /*hNode*/, void* pCookie)
{
	xn::DepthPrivateData* pThis = (xn::DepthPrivateData*)pCookie;
	pThis->OnFieldOfViewChanged();
}



xn::PosePrivateData::PosePrivateData()
{
    m_usersPoseDataPerPose = NULL;
    m_numPoses = 0;

    m_userCallbacksHandle = NULL;
    m_poseDetectHandle = NULL;
    m_outOfPoseHandle = NULL;
    m_inProgressHandle = NULL;
    m_hUserGenerator = NULL;
}

xn::PosePrivateData::~PosePrivateData()
{
    XN_ASSERT(m_userCallbacksHandle == NULL);
    XN_ASSERT(m_poseDetectHandle == NULL);
    XN_ASSERT(m_outOfPoseHandle == NULL);
    XN_ASSERT(m_inProgressHandle == NULL);
    XN_ASSERT(m_usersPoseDataPerPose == NULL);
    XN_ASSERT(m_hUserGenerator == NULL);
}

void xn::PosePrivateData::BeforeNodeDestroy()
{
    if (m_usersPoseDataPerPose != NULL)
    {
        XN_DELETE_ARR(m_usersPoseDataPerPose);
        m_usersPoseDataPerPose = NULL;
    }
    m_numPoses = 0;
    if (m_userCallbacksHandle != NULL)
    {
        xnUnregisterUserCallbacks(m_hUserGenerator, m_userCallbacksHandle);
        m_userCallbacksHandle = NULL;
    }
    if (m_poseDetectHandle != NULL)
    {
        xnUnregisterFromPoseDetected(m_hUserGenerator, m_poseDetectHandle);
        m_poseDetectHandle=NULL;
    }
    if (m_outOfPoseHandle != NULL)
    {
        xnUnregisterFromOutOfPose(m_hUserGenerator, m_outOfPoseHandle);
        m_outOfPoseHandle=NULL;
    }
    if (m_inProgressHandle != NULL)
    {
        xnUnregisterFromPoseDetectionInProgress(m_hUserGenerator, m_inProgressHandle);
        m_inProgressHandle = NULL;
    }
    m_hUserGenerator = NULL;
}

XnStatus xn::PosePrivateData::Init(XnNodeHandle hUserGenerator)
{
    m_hUserGenerator = hUserGenerator;
    XnStatus nRetVal = xnRegisterUserCallbacks(m_hUserGenerator, XnNewUserCallback, XnLostUserCallback, this, &m_userCallbacksHandle);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnRegisterToPoseDetected(m_hUserGenerator, XnPoseDetectedCallback, this, &m_poseDetectHandle);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnRegisterToOutOfPose(m_hUserGenerator, XnOutOfPoseDetectedCallback, this, &m_outOfPoseHandle);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnRegisterToPoseDetectionInProgress(hUserGenerator, XnPoseInProgressCallback, this, &m_inProgressHandle);
	XN_IS_STATUS_OK(nRetVal);

    m_numPoses = xnGetNumberOfPoses(hUserGenerator);
    if (m_numPoses < 1)
    {
        return XN_STATUS_OK; // no poses so nothing much to do
    }

    XnChar** arr = XN_NEW_ARR(XnChar*, m_numPoses);
    for (XnUInt32 i = 0; i < m_numPoses; i++)
    {
        arr[i] = XN_NEW_ARR(XnChar, XN_MAX_NAME_LENGTH);
    }
    XnUInt32 numTotal = m_numPoses;
    nRetVal = xnGetAllAvailablePoses(m_hUserGenerator, arr, XN_MAX_NAME_LENGTH, &m_numPoses);
    if(nRetVal == XN_STATUS_OK)
    {
        m_usersPoseDataPerPose = XN_NEW_ARR(UsersPoseData, m_numPoses);
        for (XnUInt32 i = 0; i < m_numPoses; i++)
        {
            XnUInt32 len = (XnUInt32)strlen(arr[i])+1;
            m_usersPoseDataPerPose[i].m_poseName = XN_NEW_ARR(XnChar, len);
            xnOSStrCopy(m_usersPoseDataPerPose[i].m_poseName, arr[i], len);
        }
    }

    for (XnUInt32 i = 0; i < numTotal; i++)
    {
        XN_DELETE_ARR(arr[i]);
    }

    XN_DELETE_ARR(arr);
    return nRetVal;
}
XnStatus xn::PosePrivateData::GetPoseStatus(XnUserID userID, const XnChar* poseName, XnUInt64& poseTime, XnPoseDetectionStatus& eStatus, XnPoseDetectionState& eState)
{
    for (XnUInt32 i = 0; i < m_numPoses; i++)
    {
        if (xnOSStrCmp(poseName, m_usersPoseDataPerPose[i].m_poseName) == 0)
        {
            PoseData tmpData;
            XnStatus nRetVal = m_usersPoseDataPerPose[i].m_usersHash.Get(userID, tmpData);
			XN_IS_STATUS_OK(nRetVal);

			if (tmpData.m_lastState != XN_POSE_DETECTION_STATE_IN_POSE)
            {
                poseTime = 0;
            }
            else
            {
                poseTime = tmpData.m_lastTimeInPose;
            }
            eStatus = tmpData.m_lastStatus;
            eState = tmpData.m_lastState;
            return XN_STATUS_OK;
        }
    }
    return XN_STATUS_NO_MATCH;
}

void XN_CALLBACK_TYPE xn::PosePrivateData::XnNewUserCallback(XnNodeHandle /*hNode*/, XnUserID nUserId, void* pCookie)
{
    xn::PosePrivateData* dataPtr = (xn::PosePrivateData*)pCookie;
    for (XnUInt32 i = 0; i < dataPtr->m_numPoses; i++)
    {
        PoseData tmpData;
        dataPtr->m_usersPoseDataPerPose[i].m_usersHash.Set(nUserId, tmpData);
    }
}

void XN_CALLBACK_TYPE xn::PosePrivateData::XnLostUserCallback(XnNodeHandle /*hNode*/, XnUserID nUserId, void* pCookie)
{
    xn::PosePrivateData* dataPtr = (xn::PosePrivateData*)pCookie;
    for (XnUInt32 i = 0; i < dataPtr->m_numPoses; i++)
    {
        dataPtr->m_usersPoseDataPerPose[i].m_usersHash.Remove(nUserId);
    }
}

void XN_CALLBACK_TYPE xn::PosePrivateData::XnPoseInProgressCallback(XnNodeHandle /*hNode*/, const XnChar* strPose, XnUserID nUserId, XnPoseDetectionStatus ePoseError, void* pCookie)
{
    xn::PosePrivateData* dataPtr = (xn::PosePrivateData*)pCookie;
    for (XnUInt32 i = 0; i < dataPtr->m_numPoses; i++)
    {
        if (xnOSStrCmp(dataPtr->m_usersPoseDataPerPose[i].m_poseName, strPose) == 0)
        {
            PoseData tmpData;
            if (dataPtr->m_usersPoseDataPerPose[i].m_usersHash.Get(nUserId, tmpData) != XN_STATUS_OK)
            {
                tmpData.Reset();
            }
            tmpData.m_lastStatus = ePoseError;
            dataPtr->m_usersPoseDataPerPose[i].m_usersHash.Set(nUserId, tmpData);
            break; // we already found the pose.
        }
    }
}

void XN_CALLBACK_TYPE xn::PosePrivateData::XnPoseDetectedCallback(XnNodeHandle /*hNode*/, const XnChar* strPose, XnUserID nUserId, void* pCookie)
{
    xn::PosePrivateData* dataPtr = (xn::PosePrivateData*)pCookie;
    for (XnUInt32 i = 0; i < dataPtr->m_numPoses; i++)
    {
        if (xnOSStrCmp(dataPtr->m_usersPoseDataPerPose[i].m_poseName, strPose) == 0)
        {
            PoseData tmpData;
            if (dataPtr->m_usersPoseDataPerPose[i].m_usersHash.Get(nUserId, tmpData) == XN_STATUS_OK)
            {
                if (tmpData.m_lastState == XN_POSE_DETECTION_STATE_IN_POSE)
                {
                    return; // we don't have anything to fix.
                }
            }
            tmpData.m_lastState = XN_POSE_DETECTION_STATE_IN_POSE;
            tmpData.m_lastStatus = XN_POSE_DETECTION_STATUS_OK;
            tmpData.m_lastTimeInPose = xnGetTimestamp(dataPtr->m_hUserGenerator);
            dataPtr->m_usersPoseDataPerPose[i].m_usersHash.Set(nUserId, tmpData);
            break; // we already found the pose.
        }
    }
}
void XN_CALLBACK_TYPE xn::PosePrivateData::XnOutOfPoseDetectedCallback(XnNodeHandle /*hNode*/, const XnChar* strPose, XnUserID nUserId, void* pCookie)
{
    xn::PosePrivateData* dataPtr = (xn::PosePrivateData*)pCookie;
    for (XnUInt32 i = 0; i < dataPtr->m_numPoses; i++)
    {
        if (xnOSStrCmp(dataPtr->m_usersPoseDataPerPose[i].m_poseName, strPose) == 0)
        {
            PoseData tmpData;
            if (dataPtr->m_usersPoseDataPerPose[i].m_usersHash.Get(nUserId, tmpData) == XN_STATUS_OK)
            {
                tmpData.Reset();
            }
            tmpData.m_lastState = XN_POSE_DETECTION_STATE_OUT_OF_POSE;
            tmpData.m_lastTimeInPose = 0;
            dataPtr->m_usersPoseDataPerPose[i].m_usersHash.Set(nUserId, tmpData);
            break; // we already found the pose.
        }
    }
}

XN_C_API XnStatus xnCreateDepthGenerator(
	XnContext* pContext,
	XnNodeHandle* phDepthGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors
	)
{
	return xnCreateAnyProductionTree(pContext, XN_NODE_TYPE_DEPTH, pQuery, phDepthGenerator, pErrors);
}

XN_C_API XnDepthPixel xnGetDeviceMaxDepth(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_DEPTH, (XnDepthPixel)-1);
	XnDepthGeneratorInterfaceContainer* pInterface = (XnDepthGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Depth.GetDeviceMaxDepth(hModuleNode);
}

XN_C_API XnStatus xnGetDepthFieldOfView(XnNodeHandle hInstance, XnFieldOfView* pFOV)
{
	XN_VALIDATE_INPUT_PTR(hInstance);
	XN_VALIDATE_OUTPUT_PTR(pFOV);
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_DEPTH);
	XnDepthGeneratorInterfaceContainer* pInterface = (XnDepthGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	pInterface->Depth.GetFieldOfView(hModuleNode, pFOV);
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnRegisterToDepthFieldOfViewChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_DEPTH);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnDepthGeneratorInterfaceContainer* pInterface = (XnDepthGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Depth.RegisterToFieldOfViewChange);
	return xnRegisterToModuleStateChange(pInterface->Depth.RegisterToFieldOfViewChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromDepthFieldOfViewChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_DEPTH, );
	XnDepthGeneratorInterfaceContainer* pInterface = (XnDepthGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Depth.UnregisterFromFieldOfViewChange, );
	xnUnregisterFromModuleStateChange(pInterface->Depth.UnregisterFromFieldOfViewChange, hModuleNode, hCallback);
}

XN_C_API XnStatus xnConvertProjectiveToRealWorld(XnNodeHandle hInstance, XnUInt32 nCount, const XnPoint3D* aProjective, XnPoint3D* aRealWorld)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_DEPTH);

	/**
	 * X_RW = (X_proj / X_res - 1/2) * Z * x_to_z
	 */

	XnMapOutputMode outputMode;
	nRetVal = xnGetMapOutputMode(hInstance, &outputMode);
	XN_IS_STATUS_OK(nRetVal);

	xn::DepthPrivateData* pDepthPrivate = (xn::DepthPrivateData*)hInstance->pPrivateData;
	XnDouble fXToZ = pDepthPrivate->GetRealWorldXtoZ();
	XnDouble fYToZ = pDepthPrivate->GetRealWorldYtoZ();

	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		XnDouble fNormalizedX = (aProjective[i].X / outputMode.nXRes - 0.5);
		aRealWorld[i].X = (XnFloat)(fNormalizedX * aProjective[i].Z * fXToZ);

		XnDouble fNormalizedY = (0.5 - aProjective[i].Y / outputMode.nYRes);
		aRealWorld[i].Y = (XnFloat)(fNormalizedY * aProjective[i].Z * fYToZ);

		aRealWorld[i].Z = aProjective[i].Z;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnConvertRealWorldToProjective(XnNodeHandle hInstance, XnUInt32 nCount, const XnPoint3D* aRealWorld, XnPoint3D* aProjective)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_DEPTH);

	/**
	 * X_proj = X_res * (X_RW / (z*x_to_z) + 1/2)
	 *		
	 *		= X_res / x_to_z * X_RW / z + X_res/2     (more efficient)
	 */

	XnMapOutputMode outputMode;
	nRetVal = xnGetMapOutputMode(hInstance, &outputMode);
	XN_IS_STATUS_OK(nRetVal);

	xn::DepthPrivateData* pDepthPrivate = (xn::DepthPrivateData*)hInstance->pPrivateData;
	XnDouble fXToZ = pDepthPrivate->GetRealWorldXtoZ();
	XnDouble fYToZ = pDepthPrivate->GetRealWorldYtoZ();

	XnDouble fCoeffX = outputMode.nXRes / fXToZ;
	XnDouble fCoeffY = outputMode.nYRes / fYToZ;

	// we can assume resolution is even (so integer div is sufficient)
	XnUInt32 nHalfXres = outputMode.nXRes / 2;
	XnUInt32 nHalfYres = outputMode.nYRes / 2;

	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		aProjective[i].X = (XnFloat)fCoeffX * aRealWorld[i].X / aRealWorld[i].Z + nHalfXres;
		aProjective[i].Y = nHalfYres - (XnFloat)fCoeffY * aRealWorld[i].Y / aRealWorld[i].Z;
		aProjective[i].Z = aRealWorld[i].Z;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnDepthPixel* xnGetDepthMap(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_DEPTH, NULL);
	XnDepthGeneratorInterfaceContainer* pInterface = (XnDepthGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Depth.GetDepthMap(hModuleNode);
}

XN_C_API void xnGetDepthMetaData(XnNodeHandle hInstance, XnDepthMetaData* pMetaData)
{
	xnUpdateMetaDataBeforeFirstRead(hInstance);
	xnCopyDepthMetaData(pMetaData, hInstance->pMetaData.Depth);
}

//---------------------------------------------------------------------------
// User Position Capability
//---------------------------------------------------------------------------

XN_C_API XnUInt32 xnGetSupportedUserPositionsCount(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_DEPTH, 0);
	XnDepthGeneratorInterfaceContainer* pInterface = (XnDepthGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->UserPosition.GetSupportedUserPositionsCount, 0);
	return pInterface->UserPosition.GetSupportedUserPositionsCount(hModuleNode);
}

XN_C_API XnStatus xnSetUserPosition(XnNodeHandle hInstance, XnUInt32 nIndex, const XnBoundingBox3D* pPosition)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_DEPTH);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnDepthGeneratorInterfaceContainer* pInterface = (XnDepthGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->UserPosition.SetUserPosition);
	return pInterface->UserPosition.SetUserPosition(hModuleNode, nIndex, pPosition);
}

XN_C_API XnStatus xnGetUserPosition(XnNodeHandle hInstance, XnUInt32 nIndex, XnBoundingBox3D* pPosition)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_DEPTH);
	XnDepthGeneratorInterfaceContainer* pInterface = (XnDepthGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->UserPosition.GetUserPosition);
	return pInterface->UserPosition.GetUserPosition(hModuleNode, nIndex, pPosition);
}

XN_C_API XnStatus xnRegisterToUserPositionChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_DEPTH);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnDepthGeneratorInterfaceContainer* pInterface = (XnDepthGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->UserPosition.RegisterToUserPositionChange);
	return xnRegisterToModuleStateChange(pInterface->UserPosition.RegisterToUserPositionChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromUserPositionChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_DEPTH, );
	XnDepthGeneratorInterfaceContainer* pInterface = (XnDepthGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->UserPosition.UnregisterFromUserPositionChange, );
	xnUnregisterFromModuleStateChange(pInterface->UserPosition.UnregisterFromUserPositionChange, hModuleNode, hCallback);
}

//---------------------------------------------------------------------------
// Image Generators
//---------------------------------------------------------------------------
XN_C_API XnStatus xnCreateImageGenerator(
	XnContext* pContext,
	XnNodeHandle* phImageGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors)
{
	return xnCreateAnyProductionTree(pContext, XN_NODE_TYPE_IMAGE, pQuery, phImageGenerator, pErrors);
}

XN_C_API XnUInt8* xnGetImageMap(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_IMAGE, NULL);
	XnImageGeneratorInterfaceContainer* pInterface = (XnImageGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Image.GetImageMap(hModuleNode);
}

XN_C_API XnRGB24Pixel* xnGetRGB24ImageMap(XnNodeHandle hInstance)
{
	return (XnRGB24Pixel*)xnGetImageMap(hInstance);
}

XN_C_API XnYUV422DoublePixel* xnGetYUV422ImageMap(XnNodeHandle hInstance)
{
	return (XnYUV422DoublePixel*)xnGetImageMap(hInstance);
}

XN_C_API XnGrayscale8Pixel* xnGetGrayscale8ImageMap(XnNodeHandle hInstance)
{
	return (XnGrayscale8Pixel*)xnGetImageMap(hInstance);
}

XN_C_API XnGrayscale16Pixel* xnGetGrayscale16ImageMap(XnNodeHandle hInstance)
{
	return (XnGrayscale16Pixel*)xnGetImageMap(hInstance);
}

XN_C_API XnBool xnIsPixelFormatSupported(XnNodeHandle hInstance, XnPixelFormat Format)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_IMAGE, FALSE);
	XnImageGeneratorInterfaceContainer* pInterface = (XnImageGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Image.IsPixelFormatSupported(hModuleNode, Format);
}

XN_C_API XnStatus xnSetPixelFormat(XnNodeHandle hInstance, XnPixelFormat Format)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_IMAGE);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnImageGeneratorInterfaceContainer* pInterface = (XnImageGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Image.SetPixelFormat(hModuleNode, Format);
}

XN_C_API XnPixelFormat xnGetPixelFormat(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_IMAGE, (XnPixelFormat)-1);
	XnImageGeneratorInterfaceContainer* pInterface = (XnImageGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Image.GetPixelFormat(hModuleNode);
}

XN_C_API XnStatus xnRegisterToPixelFormatChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_IMAGE);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnImageGeneratorInterfaceContainer* pInterface = (XnImageGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return xnRegisterToModuleStateChange(pInterface->Image.RegisterToPixelFormatChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}

XN_C_API void xnUnregisterFromPixelFormatChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_IMAGE, );
	XnImageGeneratorInterfaceContainer* pInterface = (XnImageGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	xnUnregisterFromModuleStateChange(pInterface->Image.UnregisterFromPixelFormatChange, hModuleNode, hCallback);
}

XN_C_API void xnGetImageMetaData(XnNodeHandle hInstance, XnImageMetaData* pMetaData)
{
	xnUpdateMetaDataBeforeFirstRead(hInstance);
	xnCopyImageMetaData(pMetaData, hInstance->pMetaData.Image);
}

//---------------------------------------------------------------------------
// IR Generators
//---------------------------------------------------------------------------
XN_C_API XnStatus xnCreateIRGenerator(
	XnContext* pContext,
	XnNodeHandle* phIRGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors)
{
	return xnCreateAnyProductionTree(pContext, XN_NODE_TYPE_IR, pQuery, phIRGenerator, pErrors);
}

XN_C_API XnIRPixel* xnGetIRMap(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_IR, NULL);
	XnIRGeneratorInterfaceContainer* pInterface = (XnIRGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->IR.GetIRMap(hModuleNode);
}

XN_C_API void xnGetIRMetaData(XnNodeHandle hInstance, XnIRMetaData* pMetaData)
{
	xnUpdateMetaDataBeforeFirstRead(hInstance);
	xnCopyIRMetaData(pMetaData, hInstance->pMetaData.IR);
}

//---------------------------------------------------------------------------
// Gesture Generators
//---------------------------------------------------------------------------
XN_C_API XnStatus xnCreateGestureGenerator(
	XnContext* pContext,
	XnNodeHandle* phGestureGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors)
{
	return xnCreateAnyProductionTree(pContext, XN_NODE_TYPE_GESTURE, pQuery, phGestureGenerator, pErrors);
}

XN_C_API XnStatus xnAddGesture(XnNodeHandle hInstance, const XnChar* strGesture, XnBoundingBox3D* pArea)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GESTURE);
	XN_VALIDATE_INPUT_PTR(strGesture);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Gesture.AddGesture(hModuleNode, strGesture, pArea);
}

XN_C_API XnStatus xnRemoveGesture(XnNodeHandle hInstance, const XnChar* strGesture)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GESTURE);
	XN_VALIDATE_INPUT_PTR(strGesture);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Gesture.RemoveGesture(hModuleNode, strGesture);
}

XN_C_API XnStatus xnGetActiveGestures(XnNodeHandle hInstance, XnChar** pstrGestures, XnUInt16* pnGestures)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GESTURE);
	XN_VALIDATE_OUTPUT_PTR(pstrGestures);
	XN_VALIDATE_OUTPUT_PTR(pnGestures);
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Gesture.GetActiveGestures(hModuleNode, pstrGestures, pnGestures);
}

XN_C_API XnStatus xnGetAllActiveGestures(XnNodeHandle hInstance, XnChar** pstrGestures, XnUInt32 nNameLength, XnUInt16* pnGestures)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GESTURE);
	XN_VALIDATE_OUTPUT_PTR(pstrGestures);
	XN_VALIDATE_OUTPUT_PTR(pnGestures);
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Gesture.GetAllActiveGestures);
	return pInterface->Gesture.GetAllActiveGestures(hModuleNode, pstrGestures, nNameLength, pnGestures);
}

XN_C_API XnStatus xnEnumerateGestures(XnNodeHandle hInstance, XnChar** pstrGestures, XnUInt16* pnGestures)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GESTURE);
	XN_VALIDATE_OUTPUT_PTR(pstrGestures);
	XN_VALIDATE_OUTPUT_PTR(pnGestures);
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Gesture.EnumerateGestures(hModuleNode, pstrGestures, pnGestures);
}



XN_C_API XnUInt16 xnGetNumberOfAvailableGestures(XnNodeHandle hInstance)
{
    XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GESTURE,0);
    XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
    XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
    XN_VALIDATE_FUNC_PTR_RET(pInterface->Gesture.EnumerateAllGestures,0);
    XnUInt16 nGesturesAllocated = XN_BASE_NUMBER_OF_GESTURES;
    XnUInt16 nGesturesFound = 0;
    XnUInt16 nLastNumGesturesAllocated = 0;
    XnChar** pstrArr = NULL;
    XnChar** pstrNewarr = NULL;
    while(nGesturesFound == nLastNumGesturesAllocated)
    {
        pstrNewarr = XN_NEW_ARR(XnChar *, nGesturesAllocated);
        for(XnUInt32 i = 0; i < nLastNumGesturesAllocated; i++)
        {
            pstrNewarr[i] = pstrArr[i];
        }
        for(XnUInt32 i = nLastNumGesturesAllocated; i < nGesturesAllocated; i++)
        {
            pstrNewarr[i] = XN_NEW_ARR(XnChar, XN_MAX_NAME_LENGTH);
        }
        if(pstrArr != NULL)
        {
            XN_DELETE_ARR(pstrArr);
        }
        pstrArr = pstrNewarr;
        nLastNumGesturesAllocated = nGesturesAllocated;
        nGesturesFound = nGesturesAllocated;
        if(pInterface->Gesture.EnumerateAllGestures(hModuleNode, pstrNewarr, XN_MAX_NAME_LENGTH, &nGesturesFound)!=XN_STATUS_OK)
        {
            nGesturesFound = 0;
            break;
        }
        nGesturesAllocated*=2;
    }
    for(XnUInt32 i = 0; i < nLastNumGesturesAllocated; i++)
    {
        XN_DELETE_ARR(pstrNewarr[i]);
    }
    XN_DELETE_ARR(pstrNewarr);
    return nGesturesFound;
}


XN_C_API XnStatus xnEnumerateAllGestures(XnNodeHandle hInstance, XnChar** pstrGestures, XnUInt32 nNameLength, XnUInt16* pnGestures)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GESTURE);
	XN_VALIDATE_OUTPUT_PTR(pstrGestures);
	XN_VALIDATE_OUTPUT_PTR(pnGestures);
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Gesture.EnumerateAllGestures);
	return pInterface->Gesture.EnumerateAllGestures(hModuleNode, pstrGestures, nNameLength, pnGestures);
}
XN_C_API XnBool xnIsGestureAvailable(XnNodeHandle hInstance, const XnChar* strGesture)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GESTURE, FALSE);
	XN_VALIDATE_INPUT_PTR(strGesture);
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Gesture.IsGestureAvailable(hModuleNode, strGesture);
}
XN_C_API XnBool xnIsGestureProgressSupported(XnNodeHandle hInstance, const XnChar* strGesture)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GESTURE, FALSE);
	XN_VALIDATE_INPUT_PTR(strGesture);
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Gesture.IsGestureProgressSupported(hModuleNode, strGesture);
}

typedef struct GestureCookie
{
	XnGestureRecognized recognizedHandler;
	XnGestureProgress progressHandler;
	void* pUserCookie;
	XnCallbackHandle hCallback;
	XnNodeHandle hNode;
} GestureCookie;

static void XN_CALLBACK_TYPE xnGestureRecognizedCallback(const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie)
{
	GestureCookie* pGestureCookie = (GestureCookie*)pCookie;
	if (pGestureCookie->recognizedHandler != NULL)
	{
		pGestureCookie->recognizedHandler(pGestureCookie->hNode, strGesture, pIDPosition, pEndPosition, pGestureCookie->pUserCookie);
	}
}

static void XN_CALLBACK_TYPE xnGestureProgressCallback(const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie)
{
	GestureCookie* pGestureCookie = (GestureCookie*)pCookie;
	if (pGestureCookie->progressHandler != NULL)
	{
		pGestureCookie->progressHandler(pGestureCookie->hNode, strGesture, pPosition, fProgress, pGestureCookie->pUserCookie);
	}
}

XN_C_API XnStatus xnRegisterGestureCallbacks(XnNodeHandle hInstance, XnGestureRecognized RecognizedCB, XnGestureProgress ProgressCB, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GESTURE);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	GestureCookie* pRegCookie;
	XN_VALIDATE_ALLOC(pRegCookie, GestureCookie);
	pRegCookie->recognizedHandler = RecognizedCB;
	pRegCookie->progressHandler = ProgressCB;
	pRegCookie->hNode = hInstance;
	pRegCookie->pUserCookie = pCookie;

	XnStatus nRetVal = pInterface->Gesture.RegisterGestureCallbacks(hModuleNode, xnGestureRecognizedCallback, xnGestureProgressCallback, pRegCookie, &pRegCookie->hCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pRegCookie);
		return (nRetVal);
	}

	*phCallback = pRegCookie;

	return XN_STATUS_OK;
}
XN_C_API void xnUnregisterGestureCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GESTURE, );
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	GestureCookie* pRegCookie = (GestureCookie*)hCallback;
	pInterface->Gesture.UnregisterGestureCallbacks(hModuleNode, pRegCookie->hCallback);
	xnOSFree(pRegCookie);
}
XN_C_API XnStatus xnRegisterToGestureChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GESTURE);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return xnRegisterToModuleStateChange(pInterface->Gesture.RegisterToGestureChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}
XN_C_API void xnUnregisterFromGestureChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GESTURE, );
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	xnUnregisterFromModuleStateChange(pInterface->Gesture.UnregisterFromGestureChange, hModuleNode, hCallback);
}

typedef struct GestureIntermediateStageCompletedCookie
{
	XnGestureIntermediateStageCompleted handler;
	void* pUserCookie;
	XnCallbackHandle hCallback;
	XnNodeHandle hNode;
} GestureIntermediateStageCompletedCookie;

static void XN_CALLBACK_TYPE xnModuleGestureIntermediateStageCompleted(const XnChar* strGesture, const XnPoint3D* pIDPosition, void* pCookie)
{
	GestureIntermediateStageCompletedCookie* pGestureCookie = (GestureIntermediateStageCompletedCookie*)pCookie;
	if (pGestureCookie->handler != NULL)
	{
		pGestureCookie->handler(pGestureCookie->hNode, strGesture, pIDPosition, pGestureCookie->pUserCookie);
	}
}

static void XN_CALLBACK_TYPE xnModuleGestureStateViaProgress(const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat /*fProgress*/, void* pCookie)
{
	GestureIntermediateStageCompletedCookie* pGestureCookie = (GestureIntermediateStageCompletedCookie*)pCookie;
	if (pGestureCookie->handler != NULL)
	{
		pGestureCookie->handler(pGestureCookie->hNode, strGesture, pPosition, pGestureCookie->pUserCookie);
	}
}

XN_C_API XnStatus xnRegisterToGestureIntermediateStageCompleted(XnNodeHandle hInstance, XnGestureIntermediateStageCompleted handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GESTURE);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	GestureIntermediateStageCompletedCookie* pRegCookie;
	XN_VALIDATE_ALLOC(pRegCookie, GestureIntermediateStageCompletedCookie);
	pRegCookie->handler = handler;
	pRegCookie->hNode = hInstance;
	pRegCookie->pUserCookie = pCookie;

	XnStatus nRetVal = XN_STATUS_OK;
	if (pInterface->Gesture.RegisterToGestureIntermediateStageCompleted != NULL)
	{
		nRetVal = pInterface->Gesture.RegisterToGestureIntermediateStageCompleted(hModuleNode, xnModuleGestureIntermediateStageCompleted, pRegCookie, &pRegCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		nRetVal = pInterface->Gesture.RegisterGestureCallbacks(hModuleNode, NULL, xnModuleGestureStateViaProgress, pRegCookie, &pRegCookie->hCallback);
	}
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pRegCookie);
		return (nRetVal);
	}

	*phCallback = pRegCookie;

	return XN_STATUS_OK;
}
XN_C_API void xnUnregisterFromGestureIntermediateStageCompleted(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GESTURE, );
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	GestureIntermediateStageCompletedCookie* pRegCookie = (GestureIntermediateStageCompletedCookie*)hCallback;
	if (pInterface->Gesture.UnregisterFromGestureIntermediateStageCompleted != NULL)
	{
		pInterface->Gesture.UnregisterFromGestureIntermediateStageCompleted(hModuleNode, pRegCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		pInterface->Gesture.UnregisterGestureCallbacks(hModuleNode, pRegCookie->hCallback);
	}
	xnOSFree(pRegCookie);
}

typedef struct GestureReadyForNextIntermediateStageCookie
{
	XnGestureReadyForNextIntermediateStage handler;
	void* pUserCookie;
	XnCallbackHandle hCallback;
	XnNodeHandle hNode;
} GestureReadyForNextIntermediateStageCookie;

static void XN_CALLBACK_TYPE xnModuleGestureReadyForNextIntermediateStage(const XnChar* strGesture, const XnPoint3D* pIDPosition, void* pCookie)
{
	GestureReadyForNextIntermediateStageCookie* pGestureCookie = (GestureReadyForNextIntermediateStageCookie*)pCookie;
	if (pGestureCookie->handler != NULL)
	{
		pGestureCookie->handler(pGestureCookie->hNode, strGesture, pIDPosition, pGestureCookie->pUserCookie);
	}
}

XN_C_API XnStatus xnRegisterToGestureReadyForNextIntermediateStage(XnNodeHandle hInstance, XnGestureReadyForNextIntermediateStage handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GESTURE);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	GestureReadyForNextIntermediateStageCookie* pRegCookie;
	XN_VALIDATE_ALLOC(pRegCookie, GestureReadyForNextIntermediateStageCookie);
	pRegCookie->handler = handler;
	pRegCookie->hNode = hInstance;
	pRegCookie->pUserCookie = pCookie;

	XnStatus nRetVal = XN_STATUS_OK;
	if (pInterface->Gesture.RegisterToGestureReadyForNextIntermediateStage != NULL)
	{
		nRetVal = pInterface->Gesture.RegisterToGestureReadyForNextIntermediateStage(hModuleNode, xnModuleGestureReadyForNextIntermediateStage, pRegCookie, &pRegCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		nRetVal = pInterface->Gesture.RegisterGestureCallbacks(hModuleNode, NULL, xnModuleGestureStateViaProgress, pRegCookie, &pRegCookie->hCallback);
	}
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pRegCookie);
		return (nRetVal);
	}

	*phCallback = pRegCookie;

	return XN_STATUS_OK;
}
XN_C_API void xnUnregisterFromGestureReadyForNextIntermediateStage(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_GESTURE, );
	XnGestureGeneratorInterfaceContainer* pInterface = (XnGestureGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	GestureReadyForNextIntermediateStageCookie* pRegCookie = (GestureReadyForNextIntermediateStageCookie*)hCallback;
	if (pInterface->Gesture.UnregisterFromGestureReadyForNextIntermediateStage != NULL)
	{
		pInterface->Gesture.UnregisterFromGestureReadyForNextIntermediateStage(hModuleNode, pRegCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		pInterface->Gesture.UnregisterGestureCallbacks(hModuleNode, pRegCookie->hCallback);
	}
	xnOSFree(pRegCookie);
}

//---------------------------------------------------------------------------
// Scene Analyzer
//---------------------------------------------------------------------------
XN_C_API XnStatus xnCreateSceneAnalyzer(
	XnContext* pContext,
	XnNodeHandle* phSceneAnalyzer,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors)
{
	return xnCreateAnyProductionTree(pContext, XN_NODE_TYPE_SCENE, pQuery, phSceneAnalyzer, pErrors);
}

XN_C_API const XnLabel* xnGetLabelMap(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_SCENE, NULL);
	XnSceneAnalyzerInterfaceContainer* pInterface = (XnSceneAnalyzerInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Scene.GetLabelMap(hModuleNode);
}
XN_C_API XnStatus xnGetFloor(XnNodeHandle hInstance, XnPlane3D* pPlane)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_SCENE);
	XN_VALIDATE_OUTPUT_PTR(pPlane);
	XnSceneAnalyzerInterfaceContainer* pInterface = (XnSceneAnalyzerInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Scene.GetFloor(hModuleNode, pPlane);
}

XN_C_API void xnGetSceneMetaData(XnNodeHandle hInstance, XnSceneMetaData* pMetaData)
{
	xnUpdateMetaDataBeforeFirstRead(hInstance);
	xnCopySceneMetaData(pMetaData, hInstance->pMetaData.Scene);
}

//---------------------------------------------------------------------------
// User Generators
//---------------------------------------------------------------------------
XN_C_API XnStatus xnCreateUserGenerator(
	XnContext* pContext,
	XnNodeHandle* phUserGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors)
{
	return xnCreateAnyProductionTree(pContext, XN_NODE_TYPE_USER, pQuery, phUserGenerator, pErrors);
}

XN_C_API XnUInt16 xnGetNumberOfUsers(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, 0);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->User.GetNumberOfUsers(hModuleNode);
}
XN_C_API XnStatus xnGetUsers(XnNodeHandle hInstance, XnUserID* pUsers, XnUInt16* pnUsers)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(pnUsers);
	XN_VALIDATE_OUTPUT_PTR(pUsers);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->User.GetUsers(hModuleNode, pUsers, pnUsers);
}
XN_C_API XnStatus xnGetUserCoM(XnNodeHandle hInstance, XnUserID user, XnPoint3D* pCoM)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(pCoM);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->User.GetCoM(hModuleNode, user, pCoM);
}
XN_C_API XnStatus xnGetUserPixels(XnNodeHandle hInstance, XnUserID user, XnSceneMetaData* pScene)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(pScene);
	XnUserGeneratorInterfaceContainer* pInteface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInteface->User.GetUserPixels(hModuleNode, user, pScene);
}

typedef struct UserCookie
{
	XnUserHandler newHandler;
	XnUserHandler lostHandler;
	void* pUserCookie;
	XnNodeHandle hNode;
	XnCallbackHandle hCallback;
} UserCookie;

static void XN_CALLBACK_TYPE xnNewUserCallback(XnUserID user, void* pCookie)
{
	UserCookie* pUserCookie = (UserCookie*)pCookie;
	if (pUserCookie->newHandler != NULL)
	{
		pUserCookie->newHandler(pUserCookie->hNode, user, pUserCookie->pUserCookie);
	}
}

static void XN_CALLBACK_TYPE xnLostUserCallback(XnUserID user, void* pCookie)
{
	UserCookie* pUserCookie = (UserCookie*)pCookie;
	if (pUserCookie->lostHandler != NULL)
	{
		pUserCookie->lostHandler(pUserCookie->hNode, user, pUserCookie->pUserCookie);
	}
}

XN_C_API XnStatus xnRegisterUserCallbacks(XnNodeHandle hInstance, XnUserHandler NewUserCB, XnUserHandler LostUserCB, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	UserCookie* pUserCookie;
	XN_VALIDATE_ALLOC(pUserCookie, UserCookie);
	pUserCookie->newHandler = NewUserCB;
	pUserCookie->lostHandler = LostUserCB;
	pUserCookie->hNode = hInstance;
	pUserCookie->pUserCookie = pCookie;

	XnStatus nRetVal = pInterface->User.RegisterUserCallbacks(hModuleNode, xnNewUserCallback, xnLostUserCallback, pUserCookie, &pUserCookie->hCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pUserCookie);
		return (nRetVal);
	}

	*phCallback = pUserCookie;

	return (XN_STATUS_OK);
}

XN_C_API void xnUnregisterUserCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, );
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	UserCookie* pUserCookie = (UserCookie*)hCallback;
	pInterface->User.UnregisterUserCallbacks(hModuleNode, pUserCookie->hCallback);
	xnOSFree(pUserCookie);
}

typedef struct UserSingleCookie
{
	XnUserHandler handler;
	void* pUserCookie;
	XnCallbackHandle hCallback;
	XnNodeHandle hNode;
} UserSingleCookie;

static void XN_CALLBACK_TYPE xnModuleUserSingle(XnUserID user, void* pCookie)
{
	UserSingleCookie* pHandCookie = (UserSingleCookie*)pCookie;
	if (pHandCookie->handler != NULL)
	{
		pHandCookie->handler(pHandCookie->hNode, user, pHandCookie->pUserCookie);
	}
}

XN_C_API XnStatus xnRegisterToUserExit(XnNodeHandle hInstance, XnUserHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	UserSingleCookie* pRegCookie;
	XN_VALIDATE_ALLOC(pRegCookie, UserSingleCookie);
	pRegCookie->handler = handler;
	pRegCookie->hNode = hInstance;
	pRegCookie->pUserCookie = pCookie;

	XnStatus nRetVal = XN_STATUS_OK;
	if (pInterface->User.RegisterToUserExit != NULL)
	{
		nRetVal = pInterface->User.RegisterToUserExit(hModuleNode, xnModuleUserSingle, pRegCookie, &pRegCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		nRetVal = pInterface->User.RegisterUserCallbacks(hModuleNode, NULL, xnModuleUserSingle, pRegCookie, &pRegCookie->hCallback);
	}
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pRegCookie);
		return (nRetVal);
	}

	*phCallback = pRegCookie;

	return XN_STATUS_OK;
}
XN_C_API void xnUnregisterFromUserExit(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, );
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	UserSingleCookie* pRegCookie = (UserSingleCookie*)hCallback;
	if (pInterface->User.UnregisterFromUserExit != NULL)
	{
		pInterface->User.UnregisterFromUserExit(hModuleNode, pRegCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		pInterface->User.UnregisterUserCallbacks(hModuleNode, pRegCookie->hCallback);
	}
	xnOSFree(pRegCookie);
}

XN_C_API XnStatus xnRegisterToUserReEnter(XnNodeHandle hInstance, XnUserHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	// Versions before 1.3.2 didn't have this API. Fabricate it.
	if (pInterface->User.RegisterToUserReEnter == NULL)
	{

	}

	UserSingleCookie* pRegCookie;
	XN_VALIDATE_ALLOC(pRegCookie, UserSingleCookie);
	pRegCookie->handler = handler;
	pRegCookie->hNode = hInstance;
	pRegCookie->pUserCookie = pCookie;

	XnStatus nRetVal = pInterface->User.RegisterToUserReEnter(hModuleNode, xnModuleUserSingle, pRegCookie, &pRegCookie->hCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pRegCookie);
		return (nRetVal);
	}

	*phCallback = pRegCookie;

	return XN_STATUS_OK;
}
XN_C_API void xnUnregisterFromUserReEnter(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, );
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	// Versions before 1.3.2 didn't have this API. Fabricate it.
	if (pInterface->User.UnregisterFromUserReEnter == NULL)
	{

	}

	UserSingleCookie* pRegCookie = (UserSingleCookie*)hCallback;
	pInterface->User.UnregisterFromUserReEnter(hModuleNode, pRegCookie->hCallback);
	xnOSFree(pRegCookie);
}
//---------------------------------------------------------------------------
//  Hands Generator
//---------------------------------------------------------------------------
XN_C_API XnStatus xnCreateHandsGenerator(
	XnContext* pContext,
	XnNodeHandle* phHandsGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors)
{
	return xnCreateAnyProductionTree(pContext, XN_NODE_TYPE_HANDS, pQuery, phHandsGenerator, pErrors);
}

typedef struct HandCookie
{
	XnHandCreate createHandler;
	XnHandUpdate updateHandler;
	XnHandDestroy destroyHandler;
	XnNodeHandle hNode;
	void* pUserCookie;
	XnCallbackHandle hCallback;
} HandCookie;

static void XN_CALLBACK_TYPE xnHandCreateCB(XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
{
	HandCookie* pHandCookie = (HandCookie*)pCookie;
	if (pHandCookie->createHandler != NULL)
	{
		pHandCookie->createHandler(pHandCookie->hNode, user, pPosition, fTime, pHandCookie->pUserCookie);
	}
}
static void XN_CALLBACK_TYPE xnHandUpdateCB(XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
{
	HandCookie* pHandCookie = (HandCookie*)pCookie;
	if (pHandCookie->updateHandler != NULL)
	{
		pHandCookie->updateHandler(pHandCookie->hNode, user, pPosition, fTime, pHandCookie->pUserCookie);
	}
}
static void XN_CALLBACK_TYPE xnHandDestroyCB(XnUserID user, XnFloat fTime, void* pCookie)
{
	HandCookie* pHandCookie = (HandCookie*)pCookie;
	if (pHandCookie->destroyHandler != NULL)
	{
		pHandCookie->destroyHandler(pHandCookie->hNode, user, fTime, pHandCookie->pUserCookie);
	}
}

XN_C_API XnStatus xnRegisterHandCallbacks(XnNodeHandle hInstance, XnHandCreate CreateCB, XnHandUpdate UpdateCB, XnHandDestroy DestroyCB, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_HANDS);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnHandsGeneratorInterfaceContainer* pInterface = (XnHandsGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	HandCookie* pHandCookie;
	XN_VALIDATE_ALLOC(pHandCookie, HandCookie);
	pHandCookie->createHandler = CreateCB;
	pHandCookie->updateHandler = UpdateCB;
	pHandCookie->destroyHandler = DestroyCB;
	pHandCookie->hNode = hInstance;
	pHandCookie->pUserCookie = pCookie;

	XnStatus nRetVal = pInterface->Hands.RegisterHandCallbacks(hModuleNode, xnHandCreateCB, xnHandUpdateCB, xnHandDestroyCB, pHandCookie, &pHandCookie->hCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pHandCookie);
		return (nRetVal);
	}

	*phCallback = pHandCookie;

	return (XN_STATUS_OK);
}
XN_C_API void xnUnregisterHandCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_HANDS, );
	XnHandsGeneratorInterfaceContainer* pInterface = (XnHandsGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	HandCookie* pHandCookie = (HandCookie*)hCallback;
	pInterface->Hands.UnregisterHandCallbacks(hModuleNode, pHandCookie->hCallback);
	xnOSFree(pHandCookie);
}

XN_C_API XnStatus xnStopTracking(XnNodeHandle hInstance, XnUserID user)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_HANDS);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnHandsGeneratorInterfaceContainer* pInterface = (XnHandsGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Hands.StopTracking(hModuleNode, user);
}
XN_C_API XnStatus xnStopTrackingAll(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_HANDS);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnHandsGeneratorInterfaceContainer* pInterface = (XnHandsGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Hands.StopTrackingAll(hModuleNode);
}
XN_C_API XnStatus xnStartTracking(XnNodeHandle hInstance, const XnPoint3D* pPosition)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_HANDS);
	XN_VALIDATE_INPUT_PTR(pPosition);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnHandsGeneratorInterfaceContainer* pInterface = (XnHandsGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Hands.StartTracking(hModuleNode, pPosition);
}

XN_C_API XnStatus xnSetTrackingSmoothing(XnNodeHandle hInstance, XnFloat fSmoothingFactor)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_HANDS);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnHandsGeneratorInterfaceContainer* pInterface = (XnHandsGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Hands.SetSmoothing(hModuleNode, fSmoothingFactor);
}
//---------------------------------------------------------------------------
// Hand Touching FOV Edge Capability
//---------------------------------------------------------------------------

typedef struct HandTouchingFOVEdgeCookie
{
	XnHandTouchingFOVEdge handler;
	void* pUserCookie;
	XnCallbackHandle hCallback;
	XnNodeHandle hNode;
} HandTouchingFOVEdgeCookie;

static void XN_CALLBACK_TYPE xnModuleHandTouchingFOVEdge(XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, XnDirection eDir, void* pCookie)
{
	HandTouchingFOVEdgeCookie* pHandCookie = (HandTouchingFOVEdgeCookie*)pCookie;
	if (pHandCookie->handler != NULL)
	{
		pHandCookie->handler(pHandCookie->hNode, user, pPosition, fTime, eDir, pHandCookie->pUserCookie);
	}
}

XN_C_API XnStatus xnRegisterToHandTouchingFOVEdge(XnNodeHandle hInstance, XnHandTouchingFOVEdge handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_HANDS);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnHandsGeneratorInterfaceContainer* pInterface = (XnHandsGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->HandTouchingFOVEdge.RegisterToHandTouchingFOVEdge);

	HandTouchingFOVEdgeCookie* pRegCookie;
	XN_VALIDATE_ALLOC(pRegCookie, HandTouchingFOVEdgeCookie);
	pRegCookie->handler = handler;
	pRegCookie->hNode = hInstance;
	pRegCookie->pUserCookie = pCookie;

	XnStatus nRetVal = pInterface->HandTouchingFOVEdge.RegisterToHandTouchingFOVEdge(hModuleNode, xnModuleHandTouchingFOVEdge, pRegCookie, &pRegCookie->hCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pRegCookie);
		return (nRetVal);
	}

	*phCallback = pRegCookie;

	return XN_STATUS_OK;
}
XN_C_API void xnUnregisterFromHandTouchingFOVEdge(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_HANDS, );
	XnHandsGeneratorInterfaceContainer* pInterface = (XnHandsGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->HandTouchingFOVEdge.UnregisterFromHandTouchingFOVEdge, );

	HandTouchingFOVEdgeCookie* pRegCookie = (HandTouchingFOVEdgeCookie*)hCallback;
	pInterface->HandTouchingFOVEdge.UnregisterFromHandTouchingFOVEdge(hModuleNode, pRegCookie->hCallback);
	xnOSFree(pRegCookie);
}

//---------------------------------------------------------------------------
// Skeleton Capability
//---------------------------------------------------------------------------

XN_C_API XnBool xnIsJointAvailable(XnNodeHandle hInstance, XnSkeletonJoint eJoint)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, FALSE);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Skeleton.IsJointAvailable, FALSE);
	return pInterface->Skeleton.IsJointAvailable(hModuleNode, eJoint);
}
XN_C_API XnBool xnIsProfileAvailable(XnNodeHandle hInstance, XnSkeletonProfile eProfile)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, FALSE);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Skeleton.IsProfileAvailable, FALSE);
	return pInterface->Skeleton.IsProfileAvailable(hModuleNode, eProfile);
}
XN_C_API XnStatus xnSetSkeletonProfile(XnNodeHandle hInstance, XnSkeletonProfile eProfile)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.SetSkeletonProfile);
	return pInterface->Skeleton.SetSkeletonProfile(hModuleNode, eProfile);
}
XN_C_API XnStatus xnSetJointActive(XnNodeHandle hInstance, XnSkeletonJoint eJoint, XnBool bState)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.SetJointActive);
	return pInterface->Skeleton.SetJointActive(hModuleNode, eJoint, bState);
}
XN_C_API XnBool xnIsJointActive(XnNodeHandle hInstance, XnSkeletonJoint eJoint)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, FALSE);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Skeleton.IsJointActive, FALSE);
	return pInterface->Skeleton.IsJointActive(hModuleNode, eJoint);
}
XN_C_API XnStatus xnRegisterToJointConfigurationChange(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.RegisterToJointConfigurationChange);
	return xnRegisterToModuleStateChange(pInterface->Skeleton.RegisterToJointConfigurationChange, hModuleNode, hInstance, handler, pCookie, phCallback);
}
XN_C_API void xnUnregisterFromJointConfigurationChange(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, );
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Skeleton.UnregisterFromJointConfigurationChange, );
	xnUnregisterFromModuleStateChange(pInterface->Skeleton.UnregisterFromJointConfigurationChange, hModuleNode, hCallback);
}
XN_C_API XnStatus xnEnumerateActiveJoints(XnNodeHandle hInstance, XnSkeletonJoint* pJoints, XnUInt16* pnJoints)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(pJoints);
	XN_VALIDATE_OUTPUT_PTR(pnJoints);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.EnumerateActiveJoints);
	return pInterface->Skeleton.EnumerateActiveJoints(hModuleNode, pJoints, pnJoints);
}

XN_C_API XnStatus xnGetSkeletonJoint(XnNodeHandle hInstance, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointTransformation* pJoint)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(pJoint);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.GetSkeletonJoint);
	return pInterface->Skeleton.GetSkeletonJoint(hModuleNode, user, eJoint, pJoint);
}
XN_C_API XnStatus xnGetSkeletonJointPosition(XnNodeHandle hInstance, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointPosition* pJoint)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(pJoint);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.GetSkeletonJointPosition);
	return pInterface->Skeleton.GetSkeletonJointPosition(hModuleNode, user, eJoint, pJoint);
}
XN_C_API XnStatus xnGetSkeletonJointOrientation(XnNodeHandle hInstance, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointOrientation* pJoint)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(pJoint);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.GetSkeletonJointOrientation);
	return pInterface->Skeleton.GetSkeletonJointOrientation(hModuleNode, user, eJoint, pJoint);
}
XN_C_API XnBool xnIsSkeletonTracking(XnNodeHandle hInstance, XnUserID user)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, FALSE);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Skeleton.IsTracking, FALSE);
	return pInterface->Skeleton.IsTracking(hModuleNode, user);
}
XN_C_API XnBool xnIsSkeletonCalibrated(XnNodeHandle hInstance, XnUserID user)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, FALSE);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Skeleton.IsCalibrated, FALSE);
	return pInterface->Skeleton.IsCalibrated(hModuleNode, user);
}
XN_C_API XnBool xnIsSkeletonCalibrating(XnNodeHandle hInstance, XnUserID user)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, FALSE);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Skeleton.IsCalibrating, FALSE);
	return pInterface->Skeleton.IsCalibrating(hModuleNode, user);
}
XN_C_API XnStatus xnRequestSkeletonCalibration(XnNodeHandle hInstance, XnUserID user, XnBool bForce)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.RequestCalibration);
	return pInterface->Skeleton.RequestCalibration(hModuleNode, user, bForce);
}

XN_C_API XnStatus xnAbortSkeletonCalibration(XnNodeHandle hInstance, XnUserID user)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.AbortCalibration);
	return pInterface->Skeleton.AbortCalibration(hModuleNode, user);
}

XN_C_API XnStatus xnSaveSkeletonCalibrationDataToFile(XnNodeHandle hInstance, XnUserID user, const XnChar* strFileName)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.SaveCalibrationDataToFile);
	return pInterface->Skeleton.SaveCalibrationDataToFile(hModuleNode, user, strFileName);
}
XN_C_API XnStatus xnLoadSkeletonCalibrationDataFromFile(XnNodeHandle hInstance, XnUserID user, const XnChar* strFileName)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.LoadCalibrationDataFromFile);
	return pInterface->Skeleton.LoadCalibrationDataFromFile(hModuleNode, user, strFileName);
}
XN_C_API XnStatus xnSaveSkeletonCalibrationData(XnNodeHandle hInstance, XnUserID user, XnUInt32 nSlot)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.SaveCalibrationData);
	return pInterface->Skeleton.SaveCalibrationData(hModuleNode, user, nSlot);
}
XN_C_API XnStatus xnLoadSkeletonCalibrationData(XnNodeHandle hInstance, XnUserID user, XnUInt32 nSlot)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.LoadCalibrationData);
	return pInterface->Skeleton.LoadCalibrationData(hModuleNode, user, nSlot);
}
XN_C_API XnStatus xnClearSkeletonCalibrationData(XnNodeHandle hInstance, XnUInt32 nSlot)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.ClearCalibrationData);
	return pInterface->Skeleton.ClearCalibrationData(hModuleNode, nSlot);
}
XN_C_API XnBool xnIsSkeletonCalibrationData(XnNodeHandle hInstance, XnUInt32 nSlot)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, FALSE);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Skeleton.IsCalibrationData, FALSE);
	return pInterface->Skeleton.IsCalibrationData(hModuleNode, nSlot);
}

XN_C_API XnStatus xnStartSkeletonTracking(XnNodeHandle hInstance, XnUserID user)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.StartTracking);
	return pInterface->Skeleton.StartTracking(hModuleNode, user);
}
XN_C_API XnStatus xnStopSkeletonTracking(XnNodeHandle hInstance, XnUserID user)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.StopTracking);
	return pInterface->Skeleton.StopTracking(hModuleNode, user);
}
XN_C_API XnStatus xnResetSkeleton(XnNodeHandle hInstance, XnUserID user)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.Reset);
	return pInterface->Skeleton.Reset(hModuleNode, user);
}
XN_C_API XnBool xnNeedPoseForSkeletonCalibration(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, FALSE);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Skeleton.NeedPoseForCalibration, FALSE);
	return pInterface->Skeleton.NeedPoseForCalibration(hModuleNode);
}
XN_C_API XnStatus xnGetSkeletonCalibrationPose(XnNodeHandle hInstance, XnChar* strPose)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(strPose);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.GetCalibrationPose);
	return pInterface->Skeleton.GetCalibrationPose(hModuleNode, strPose);
}
XN_C_API XnStatus xnSetSkeletonSmoothing(XnNodeHandle hInstance, XnFloat fSmoothingFactor)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.SetSmoothing);
	return pInterface->Skeleton.SetSmoothing(hModuleNode, fSmoothingFactor);
}

typedef struct SkeletonCookie
{
	XnCalibrationStart startHandler;
	XnCalibrationEnd endHandler;
	void* pUserCookie;
	XnNodeHandle hNode;
	XnCallbackHandle hCallback;
} SkeletonCookie;

static void XN_CALLBACK_TYPE xnCalibrationStartBundleCallback(XnUserID user, void* pCookie)
{
	SkeletonCookie* pRegCookie = (SkeletonCookie*)pCookie;
	if (pRegCookie->startHandler != NULL)
	{
		pRegCookie->startHandler(pRegCookie->hNode, user, pRegCookie->pUserCookie);
	}
}

static void XN_CALLBACK_TYPE xnCalibrationEndBundleCallback(XnUserID user, XnBool bSuccess, void* pCookie)
{
	SkeletonCookie* pRegCookie = (SkeletonCookie*)pCookie;
	if (pRegCookie->endHandler != NULL)
	{
		pRegCookie->endHandler(pRegCookie->hNode, user, bSuccess, pRegCookie->pUserCookie);
	}
}

XN_C_API XnStatus XN_API_DEPRECATED("Please use RegisterToCalibrationStart/Complete") xnRegisterCalibrationCallbacks(XnNodeHandle hInstance, XnCalibrationStart CalibrationStartCB, XnCalibrationEnd CalibrationEndCB, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->Skeleton.RegisterCalibrationCallbacks);

	SkeletonCookie* pSkeletonCookie;
	XN_VALIDATE_ALLOC(pSkeletonCookie, SkeletonCookie);
	pSkeletonCookie->startHandler = CalibrationStartCB;
	pSkeletonCookie->endHandler = CalibrationEndCB;
	pSkeletonCookie->hNode = hInstance;
	pSkeletonCookie->pUserCookie = pCookie;

	XnStatus nRetVal = pInterface->Skeleton.RegisterCalibrationCallbacks(hModuleNode, xnCalibrationStartBundleCallback, xnCalibrationEndBundleCallback, pSkeletonCookie, &pSkeletonCookie->hCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pSkeletonCookie);
		return (nRetVal);
	}

	*phCallback = pSkeletonCookie;

	return (XN_STATUS_OK);
}

XN_C_API void XN_API_DEPRECATED("Please use UnregisterFromCalibrationStart/Complete") xnUnregisterCalibrationCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, );
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->Skeleton.UnregisterCalibrationCallbacks, );

	SkeletonCookie* pSkeletonCookie = (SkeletonCookie*)hCallback;
	pInterface->Skeleton.UnregisterCalibrationCallbacks(hModuleNode, pSkeletonCookie->hCallback);
	xnOSFree(pSkeletonCookie);
}

typedef struct CalibrationStartCookie
{
	XnCalibrationStart handler;
	void* pUserCookie;
	XnNodeHandle hNode;
	XnCallbackHandle hCallback;
} CalibrationStartCookie;

static void XN_CALLBACK_TYPE xnCalibrationStartCallback(XnUserID user, void* pCookie)
{
	CalibrationStartCookie* pRegCookie = (CalibrationStartCookie*)pCookie;
	if (pRegCookie->handler != NULL)
	{
		pRegCookie->handler(pRegCookie->hNode, user, pRegCookie->pUserCookie);
	}
}

XN_C_API XnStatus xnRegisterToCalibrationStart(XnNodeHandle hInstance, XnCalibrationStart handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	CalibrationStartCookie* pCalibrationCookie;
	XN_VALIDATE_ALLOC(pCalibrationCookie, CalibrationStartCookie);
	pCalibrationCookie->handler = handler;
	pCalibrationCookie->hNode = hInstance;
	pCalibrationCookie->pUserCookie = pCookie;

	XnStatus nRetVal = XN_STATUS_OK;

	if (pInterface->Skeleton.RegisterToCalibrationStart != NULL)
	{
		nRetVal = pInterface->Skeleton.RegisterToCalibrationStart(hModuleNode, xnCalibrationStartCallback, pCalibrationCookie, &pCalibrationCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		nRetVal = pInterface->Skeleton.RegisterCalibrationCallbacks(hModuleNode, xnCalibrationStartCallback, NULL, pCalibrationCookie, &pCalibrationCookie->hCallback);
	}

	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pCalibrationCookie);
		return (nRetVal);
	}

	*phCallback = pCalibrationCookie;

	return (XN_STATUS_OK);
}

XN_C_API void xnUnregisterFromCalibrationStart(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, );
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	
	CalibrationStartCookie* pCalibrationCookie = (CalibrationStartCookie*)hCallback;

	if (pInterface->Skeleton.UnregisterFromCalibrationStart != NULL)
	{
		pInterface->Skeleton.UnregisterFromCalibrationStart(hModuleNode, pCalibrationCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		pInterface->Skeleton.UnregisterCalibrationCallbacks(hModuleNode, pCalibrationCookie->hCallback);
	}
	xnOSFree(pCalibrationCookie);
}

typedef struct CalibrationInProgressCookie
{
	XnCalibrationInProgress handler;
	void* pUserCookie;
	XnCallbackHandle hCallback;
	XnNodeHandle hNode;
} CalibrationInProgressCookie;

static void XN_CALLBACK_TYPE xnModuleCalibrationInProgress(XnUserID user, XnCalibrationStatus calibrationError, void* pCookie)
{
	CalibrationInProgressCookie* pCalibrationCookie = (CalibrationInProgressCookie*)pCookie;
	if (pCalibrationCookie->handler != NULL)
	{
		pCalibrationCookie->handler(pCalibrationCookie->hNode, user, calibrationError, pCalibrationCookie->pUserCookie);
	}
}

static void XN_CALLBACK_TYPE xnModuleCalibrationInProgressViaStart(XnUserID user, void* pCookie)
{
	CalibrationInProgressCookie* pCalibrationCookie = (CalibrationInProgressCookie*)pCookie;
	if (pCalibrationCookie->handler != NULL)
	{
		pCalibrationCookie->handler(pCalibrationCookie->hNode, user, XN_CALIBRATION_STATUS_OK, pCalibrationCookie->pUserCookie);
	}
}

XN_C_API XnStatus xnRegisterToCalibrationInProgress(XnNodeHandle hInstance, XnCalibrationInProgress handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	CalibrationInProgressCookie* pRegCookie;
	XN_VALIDATE_ALLOC(pRegCookie, CalibrationInProgressCookie);
	pRegCookie->handler = handler;
	pRegCookie->hNode = hInstance;
	pRegCookie->pUserCookie = pCookie;

	XnStatus nRetVal = XN_STATUS_OK;
	if (pInterface->Skeleton.RegisterToCalibrationInProgress != NULL)
	{
		pInterface->Skeleton.RegisterToCalibrationInProgress(hModuleNode, xnModuleCalibrationInProgress, pRegCookie, &pRegCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		pInterface->Skeleton.RegisterCalibrationCallbacks(hModuleNode, xnModuleCalibrationInProgressViaStart, NULL, pRegCookie, &pRegCookie->hCallback);
	}
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pRegCookie);
		return (nRetVal);
	}

	*phCallback = pRegCookie;

	return XN_STATUS_OK;
}
XN_C_API void xnUnregisterFromCalibrationInProgress(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, );
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	// Versions before 1.3.2 didn't have this API. Fabricate it.
	if (pInterface->Skeleton.UnregisterFromCalibrationInProgress == NULL)
	{

	}

	CalibrationInProgressCookie* pRegCookie = (CalibrationInProgressCookie*)hCallback;
	pInterface->Skeleton.UnregisterFromCalibrationInProgress(hModuleNode, pRegCookie->hCallback);
	xnOSFree(pRegCookie);
}

typedef struct CalibrationCompleteCookie
{
	XnCalibrationComplete handler;
	void* pUserCookie;
	XnCallbackHandle hCallback;
	XnNodeHandle hNode;
} CalibrationCompleteCookie;

static void XN_CALLBACK_TYPE xnModuleCalibrationComplete(XnUserID user, XnCalibrationStatus calibrationError, void* pCookie)
{
	CalibrationCompleteCookie* pHandCookie = (CalibrationCompleteCookie*)pCookie;
	if (pHandCookie->handler != NULL)
	{
		pHandCookie->handler(pHandCookie->hNode, user, calibrationError, pHandCookie->pUserCookie);
	}
}

static void XN_CALLBACK_TYPE xnModuleCalibrationCompleteViaEnd(XnUserID user, XnBool bSuccess, void* pCookie)
{
	CalibrationCompleteCookie* pHandCookie = (CalibrationCompleteCookie*)pCookie;
	if (pHandCookie->handler != NULL)
	{
		pHandCookie->handler(pHandCookie->hNode, user, bSuccess?XN_CALIBRATION_STATUS_OK:XN_CALIBRATION_STATUS_POSE, pHandCookie->pUserCookie);
	}
}

XN_C_API XnStatus xnRegisterToCalibrationComplete(XnNodeHandle hInstance, XnCalibrationComplete handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	CalibrationCompleteCookie* pRegCookie;
	XN_VALIDATE_ALLOC(pRegCookie, CalibrationCompleteCookie);
	pRegCookie->handler = handler;
	pRegCookie->hNode = hInstance;
	pRegCookie->pUserCookie = pCookie;

	XnStatus nRetVal = XN_STATUS_OK;

	if (pInterface->Skeleton.RegisterToCalibrationComplete != NULL)
	{
		nRetVal = pInterface->Skeleton.RegisterToCalibrationComplete(hModuleNode, xnModuleCalibrationComplete, pRegCookie, &pRegCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		nRetVal = pInterface->Skeleton.RegisterCalibrationCallbacks(hModuleNode, NULL, xnModuleCalibrationCompleteViaEnd, pRegCookie, &pRegCookie->hCallback);
	}

	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pRegCookie);
		return (nRetVal);
	}

	*phCallback = pRegCookie;

	return XN_STATUS_OK;
}
XN_C_API void xnUnregisterFromCalibrationComplete(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, );
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	CalibrationCompleteCookie* pRegCookie = (CalibrationCompleteCookie*)hCallback;
	if (pInterface->Skeleton.UnregisterFromCalibrationComplete != NULL)
	{
		pInterface->Skeleton.UnregisterFromCalibrationComplete(hModuleNode, pRegCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		pInterface->Skeleton.UnregisterCalibrationCallbacks(hModuleNode, pRegCookie->hCallback);
	}

	xnOSFree(pRegCookie);
}
//---------------------------------------------------------------------------
// Pose Detection Capability
//---------------------------------------------------------------------------
XN_C_API XnUInt32 xnGetNumberOfPoses(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, 0);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->PoseDetection.GetNumberOfPoses, 0);
	return pInterface->PoseDetection.GetNumberOfPoses(hModuleNode);
}
XN_C_API XnStatus xnGetAvailablePoses(XnNodeHandle hInstance, XnChar** pstrPoses, XnUInt32* pnPoses)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(pstrPoses);
	XN_VALIDATE_OUTPUT_PTR(pnPoses);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->PoseDetection.GetAvailablePoses);
	return pInterface->PoseDetection.GetAvailablePoses(hModuleNode, pstrPoses, pnPoses);
}
XN_C_API XnStatus xnGetAllAvailablePoses(XnNodeHandle hInstance, XnChar** pstrPoses, XnUInt32 nNameLength, XnUInt32* pnPoses)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(pstrPoses);
	XN_VALIDATE_OUTPUT_PTR(pnPoses);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->PoseDetection.GetAllAvailablePoses);
	return pInterface->PoseDetection.GetAllAvailablePoses(hModuleNode, pstrPoses, nNameLength, pnPoses);
}


XN_C_API XnStatus xnGetPoseStatus(XnNodeHandle hInstance, XnUserID userID, const XnChar* poseName, XnUInt64* poseTime, XnPoseDetectionStatus* eStatus, XnPoseDetectionState* eState)
{
    XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
    XN_VALIDATE_OUTPUT_PTR(poseName);
    XN_VALIDATE_OUTPUT_PTR(poseTime);
    XN_VALIDATE_OUTPUT_PTR(eStatus);
    XN_VALIDATE_OUTPUT_PTR(eState);
    xn::PosePrivateData *pPosePrivateData = dynamic_cast<xn::PosePrivateData*>(hInstance->pPrivateData);
    XN_VALIDATE_PTR(pPosePrivateData, XN_STATUS_ERROR);

    return pPosePrivateData->GetPoseStatus(userID, poseName, *poseTime, *eStatus, *eState);
}

XN_C_API XnBool xnIsPoseSupported(XnNodeHandle hInstance, const XnChar* strPose)
{
    XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, FALSE);
    XN_VALIDATE_PTR(strPose, FALSE);

    XnUInt32 numPoses = xnGetNumberOfPoses(hInstance);
    if(numPoses == 0)
    {
        return FALSE;
    }
    XnUInt32 len = (XnUInt32)strlen(strPose) + 1;
    XnChar** pstrList = XN_NEW_ARR(XnChar*, numPoses);
    for (XnUInt32 i = 0; i < numPoses; i++)
    {
        pstrList[i] = XN_NEW_ARR(XnChar, len);
    }
    XnUInt32 origNumPoses = numPoses;
    XnStatus tmpRes = xnGetAllAvailablePoses(hInstance, pstrList, len, &numPoses);
    if (tmpRes != XN_STATUS_OK)
    {
        numPoses = 0;
    }
    XnBool bRes = FALSE;
    for (XnUInt32 i = 0; i < origNumPoses; i++)
    {
        if (i < numPoses && xnOSStrCmp(strPose, pstrList[i]) == 0)
        {
            bRes = TRUE;
            numPoses = 0;
        }
        XN_DELETE_ARR(pstrList[i]);
    }
    XN_DELETE_ARR(pstrList);
    return bRes;
}

XN_C_API XnStatus xnStartPoseDetection(XnNodeHandle hInstance, const XnChar* strPose, XnUserID user)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_INPUT_PTR(strPose);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->PoseDetection.StartPoseDetection);
	return pInterface->PoseDetection.StartPoseDetection(hModuleNode, strPose, user);
}
XN_C_API XnStatus xnStopPoseDetection(XnNodeHandle hInstance, XnUserID user)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->PoseDetection.StopPoseDetection);
	return pInterface->PoseDetection.StopPoseDetection(hModuleNode, user);
}
XN_C_API XnStatus xnStopSinglePoseDetection(XnNodeHandle hInstance, XnUserID user, const XnChar* strPose)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	if (pInterface->PoseDetection.StopSinglePoseDetection == NULL)
	{
		return pInterface->PoseDetection.StopPoseDetection(hModuleNode, user);
	}
	return pInterface->PoseDetection.StopSinglePoseDetection(hModuleNode, user, strPose);
}

typedef struct PoseCookie
{
	XnPoseDetectionCallback startHandler;
	XnPoseDetectionCallback endHandler;
	void* pPoseCookie;
	XnNodeHandle hNode;
	XnCallbackHandle hCallback;
} PoseCookie;

static void XN_CALLBACK_TYPE xnPoseDetectionStartCallback(const XnChar* strPose, XnUserID user, void* pCookie)
{
	PoseCookie* pRegCookie = (PoseCookie*)pCookie;
	if (pRegCookie->startHandler != NULL)
	{
		pRegCookie->startHandler(pRegCookie->hNode, strPose, user, pRegCookie->pPoseCookie);
	}
}

static void XN_CALLBACK_TYPE xnPoseDetectionEndCallback(const XnChar* strPose, XnUserID user, void* pCookie)
{
	PoseCookie* pRegCookie = (PoseCookie*)pCookie;
	if (pRegCookie->endHandler != NULL)
	{
		pRegCookie->endHandler(pRegCookie->hNode, strPose, user, pRegCookie->pPoseCookie);
	}
}

XN_C_API XnStatus XN_API_DEPRECATED("Please use xnRegisterToPoseDetected/xnRegisterToOutOfPose instead") xnRegisterToPoseCallbacks(XnNodeHandle hInstance, XnPoseDetectionCallback PoseDetectionStartCB, XnPoseDetectionCallback PoseDetectionEndCB, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->PoseDetection.RegisterToPoseCallbacks);

	PoseCookie* pPoseCookie;
	XN_VALIDATE_ALLOC(pPoseCookie, PoseCookie);
	pPoseCookie->startHandler = PoseDetectionStartCB;
	pPoseCookie->endHandler = PoseDetectionEndCB;
	pPoseCookie->hNode = hInstance;
	pPoseCookie->pPoseCookie = pCookie;

	XnStatus nRetVal = pInterface->PoseDetection.RegisterToPoseCallbacks(hModuleNode, xnPoseDetectionStartCallback, xnPoseDetectionEndCallback, pPoseCookie, &pPoseCookie->hCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pPoseCookie);
		return (nRetVal);
	}

	*phCallback = pPoseCookie;

	return (XN_STATUS_OK);
}

XN_C_API void XN_API_DEPRECATED("Please use xnUnregisterFromPoseDetected/xnUnregisterFromOutOfPose instead") xnUnregisterFromPoseCallbacks(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, );
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->PoseDetection.UnregisterFromPoseCallbacks, );

	PoseCookie* pPoseCookie = (PoseCookie*)hCallback;
	pInterface->PoseDetection.UnregisterFromPoseCallbacks(hModuleNode, pPoseCookie->hCallback);
	xnOSFree(pPoseCookie);
}

typedef struct PoseDetectionCookie
{
	XnPoseDetectionCallback handler;
	void* pPoseCookie;
	XnNodeHandle hNode;
	XnCallbackHandle hCallback;
} PoseDetectionCookie;

static void XN_CALLBACK_TYPE xnPoseDetectionCallback(const XnChar* strPose, XnUserID user, void* pCookie)
{
	PoseDetectionCookie* pRegCookie = (PoseDetectionCookie*)pCookie;
	if (pRegCookie->handler != NULL)
	{
		pRegCookie->handler(pRegCookie->hNode, strPose, user, pRegCookie->pPoseCookie);
	}
}

XN_C_API XnStatus xnRegisterToPoseDetected(XnNodeHandle hInstance, XnPoseDetectionCallback handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	PoseDetectionCookie* pPoseCookie;
	XN_VALIDATE_ALLOC(pPoseCookie, PoseDetectionCookie);
	pPoseCookie->handler = handler;
	pPoseCookie->hNode = hInstance;
	pPoseCookie->pPoseCookie = pCookie;

	XnStatus nRetVal = XN_STATUS_OK;
	
	if (pInterface->PoseDetection.RegisterToPoseDetected != NULL)
	{
		nRetVal = pInterface->PoseDetection.RegisterToPoseDetected(hModuleNode, xnPoseDetectionCallback, pPoseCookie, &pPoseCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		nRetVal = pInterface->PoseDetection.RegisterToPoseCallbacks(hModuleNode, xnPoseDetectionCallback, NULL, pPoseCookie, &pPoseCookie->hCallback);
	}
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pPoseCookie);
		return (nRetVal);
	}

	*phCallback = pPoseCookie;

	return (XN_STATUS_OK);
}

XN_C_API void xnUnregisterFromPoseDetected(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, );
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	PoseDetectionCookie* pPoseCookie = (PoseDetectionCookie*)hCallback;

	if (pInterface->PoseDetection.UnregisterFromPoseDetected != NULL)
	{
		pInterface->PoseDetection.UnregisterFromPoseDetected(hModuleNode, pPoseCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		pInterface->PoseDetection.UnregisterFromPoseCallbacks(hModuleNode, pPoseCookie->hCallback);
	}

	xnOSFree(pPoseCookie);
}

XN_C_API XnStatus xnRegisterToOutOfPose(XnNodeHandle hInstance, XnPoseDetectionCallback handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	PoseDetectionCookie* pPoseCookie;
	XN_VALIDATE_ALLOC(pPoseCookie, PoseDetectionCookie);
	pPoseCookie->handler = handler;
	pPoseCookie->hNode = hInstance;
	pPoseCookie->pPoseCookie = pCookie;

	XnStatus nRetVal = XN_STATUS_OK;

	if (pInterface->PoseDetection.RegisterToOutOfPose != NULL)
	{
		nRetVal = pInterface->PoseDetection.RegisterToOutOfPose(hModuleNode, xnPoseDetectionCallback, pPoseCookie, &pPoseCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		nRetVal = pInterface->PoseDetection.RegisterToPoseCallbacks(hModuleNode, NULL, xnPoseDetectionCallback, pPoseCookie, &pPoseCookie->hCallback);
	}
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pPoseCookie);
		return (nRetVal);
	}

	*phCallback = pPoseCookie;

	return (XN_STATUS_OK);
}

XN_C_API void xnUnregisterFromOutOfPose(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, );
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	PoseDetectionCookie* pPoseCookie = (PoseDetectionCookie*)hCallback;
	if (pInterface->PoseDetection.UnregisterFromOutOfPose != NULL)
	{
		pInterface->PoseDetection.UnregisterFromOutOfPose(hModuleNode, pPoseCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		pInterface->PoseDetection.UnregisterFromPoseCallbacks(hModuleNode, pPoseCookie->hCallback);
	}
	xnOSFree(pPoseCookie);
}


typedef struct PoseDetectionInProgressCookie
{
	XnPoseDetectionInProgress handler;
	void* pUserCookie;
	XnCallbackHandle hCallback;
	XnNodeHandle hNode;
} PoseDetectionInProgressCookie;

static void XN_CALLBACK_TYPE xnModulePoseDetectionInProgress(const XnChar* strPose, XnUserID user, XnPoseDetectionStatus poseDetectionError, void* pCookie)
{
	PoseDetectionInProgressCookie* pHandCookie = (PoseDetectionInProgressCookie*)pCookie;
	if (pHandCookie->handler != NULL)
	{
		pHandCookie->handler(pHandCookie->hNode, strPose, user, poseDetectionError, pHandCookie->pUserCookie);
	}
}

static void XN_CALLBACK_TYPE xnModulePoseDetectionInProgressViaStart(const XnChar* strPose, XnUserID user, void* pCookie)
{
	PoseDetectionInProgressCookie* pHandCookie = (PoseDetectionInProgressCookie*)pCookie;
	if (pHandCookie->handler != NULL)
	{
		pHandCookie->handler(pHandCookie->hNode, strPose, user, XN_POSE_DETECTION_STATUS_OK, pHandCookie->pUserCookie);
	}
}

XN_C_API XnStatus xnRegisterToPoseDetectionInProgress(XnNodeHandle hInstance, XnPoseDetectionInProgress handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_USER);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	PoseDetectionInProgressCookie* pRegCookie;
	XN_VALIDATE_ALLOC(pRegCookie, PoseDetectionInProgressCookie);
	pRegCookie->handler = handler;
	pRegCookie->hNode = hInstance;
	pRegCookie->pUserCookie = pCookie;

	XnStatus nRetVal = XN_STATUS_OK;
	if (pInterface->PoseDetection.RegisterToPoseDetectionInProgress != NULL)
	{
		nRetVal = pInterface->PoseDetection.RegisterToPoseDetectionInProgress(hModuleNode, xnModulePoseDetectionInProgress, pRegCookie, &pRegCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		nRetVal = pInterface->PoseDetection.RegisterToPoseCallbacks(hModuleNode, xnModulePoseDetectionInProgressViaStart, NULL, pRegCookie, &pRegCookie->hCallback);
	}
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pRegCookie);
		return (nRetVal);
	}

	*phCallback = pRegCookie;

	return XN_STATUS_OK;
}
XN_C_API void xnUnregisterFromPoseDetectionInProgress(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_USER, );
	XnUserGeneratorInterfaceContainer* pInterface = (XnUserGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;

	PoseDetectionInProgressCookie* pRegCookie = (PoseDetectionInProgressCookie*)hCallback;
	if (pInterface->PoseDetection.UnregisterFromPoseDetectionInProgress != NULL)
	{
		pInterface->PoseDetection.UnregisterFromPoseDetectionInProgress(hModuleNode, pRegCookie->hCallback);
	}
	else
	{
		// Versions before 1.3.2 didn't have this API. Fabricate it.
		pInterface->PoseDetection.UnregisterFromPoseCallbacks(hModuleNode, pRegCookie->hCallback);
	}
	xnOSFree(pRegCookie);
}

//---------------------------------------------------------------------------
// Audio Generator
//---------------------------------------------------------------------------

XN_C_API XnStatus xnCreateAudioGenerator(
	XnContext* pContext,
	XnNodeHandle* phAudioGenerator,
	XnNodeQuery* pQuery, 
	XnEnumerationErrors* pErrors)
{
	return xnCreateAnyProductionTree(pContext, XN_NODE_TYPE_AUDIO, pQuery, phAudioGenerator, pErrors);
}

XN_C_API XnUChar* xnGetAudioBuffer(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_AUDIO, NULL);
	XnAudioGeneratorInterfaceContainer* pInterface = (XnAudioGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Audio.GetAudioBuffer(hModuleNode);
}
XN_C_API XnUInt32 xnGetSupportedWaveOutputModesCount(XnNodeHandle hInstance)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_AUDIO, 0);
	XnAudioGeneratorInterfaceContainer* pInterface = (XnAudioGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Audio.GetSupportedWaveOutputModesCount(hModuleNode);
}
XN_C_API XnStatus xnGetSupportedWaveOutputModes(XnNodeHandle hInstance, XnWaveOutputMode* aSupportedModes, XnUInt32* pnCount)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_AUDIO);
	XnAudioGeneratorInterfaceContainer* pInterface = (XnAudioGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Audio.GetSupportedWaveOutputModes(hModuleNode, aSupportedModes, pnCount);
}
XN_C_API XnStatus xnSetWaveOutputMode(XnNodeHandle hInstance, const XnWaveOutputMode* pOutputMode)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_AUDIO);
	XN_VALIDATE_CHANGES_ALLOWED(hInstance);
	XnAudioGeneratorInterfaceContainer* pInterface = (XnAudioGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Audio.SetWaveOutputMode(hModuleNode, pOutputMode);
}
XN_C_API XnStatus xnGetWaveOutputMode(XnNodeHandle hInstance, XnWaveOutputMode* pOutputMode)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_AUDIO);
	XnAudioGeneratorInterfaceContainer* pInterface = (XnAudioGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return pInterface->Audio.GetWaveOutputMode(hModuleNode, pOutputMode);
}
XN_C_API XnStatus xnRegisterToWaveOutputModeChanges(XnNodeHandle hInstance, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_AUDIO);
	XN_VALIDATE_OUTPUT_PTR(phCallback);
	XnAudioGeneratorInterfaceContainer* pInterface = (XnAudioGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	return xnRegisterToModuleStateChange(pInterface->Audio.RegisterToWaveOutputModeChanges, hModuleNode, hInstance, handler, pCookie, phCallback);
}
XN_C_API void xnUnregisterFromWaveOutputModeChanges(XnNodeHandle hInstance, XnCallbackHandle hCallback)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hInstance, XN_NODE_TYPE_AUDIO,);
	XnAudioGeneratorInterfaceContainer* pInterface = (XnAudioGeneratorInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	xnUnregisterFromModuleStateChange(pInterface->Audio.UnregisterFromWaveOutputModeChanges, hModuleNode, hCallback);
}

XN_C_API void xnGetAudioMetaData(XnNodeHandle hInstance, XnAudioMetaData* pMetaData)
{
	xnUpdateMetaDataBeforeFirstRead(hInstance);
	xnCopyAudioMetaData(pMetaData, hInstance->pMetaData.Audio);
}

//---------------------------------------------------------------------------
// Codecs
//---------------------------------------------------------------------------
XN_C_API XnStatus xnCreateCodec(XnContext* pContext, XnCodecID codecID, XnNodeHandle hInitializerNode, XnNodeHandle* phCodec)
{
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_OUTPUT_PTR(phCodec);
	if (codecID == XN_CODEC_NULL)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_BAD_PARAM;
	}
	XnStatus nRetVal = XN_STATUS_OK;

	// search for all codecs
	XnNodeInfoList* pList;
	nRetVal = xnEnumerateProductionTrees(pContext, XN_NODE_TYPE_CODEC, NULL, &pList, NULL);
	XN_IS_STATUS_OK_LOG_ERROR("Enumerate production trees", nRetVal);

	XnNodeHandle hCodec = NULL;

	// now search the list for the required type
	for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pList);
		xnNodeInfoListIteratorIsValid(it);
		it = xnNodeInfoListGetNext(it))
	{
		XnNodeInfo* pInfo = xnNodeInfoListGetCurrent(it);

		// only new ones
		if (pInfo->hNode == NULL)
		{
			nRetVal = xnCreateProductionTree(pContext, pInfo, &hCodec);
			if (nRetVal != XN_STATUS_OK)
			{
				xnLoggerWarning(g_logger, "Failed to create codec %s of vendor %s to check for its type: %s", pInfo->Description.strName, pInfo->Description.strVendor, xnGetStatusString(nRetVal));
				continue;
			}

			XnCodecID type = xnGetCodecID(hCodec);

			if (type == codecID)
			{
				// found it
				break;
			}

			// type doesn't match, free it
			xnProductionNodeRelease(hCodec);
			hCodec = NULL;
		}
	}

	// free the list
	xnNodeInfoListFree(pList);

	if (hCodec == NULL)
	{
		xnLogError(XN_MASK_OPEN_NI, "Unsupported codec '%.4s'", &codecID);
		return XN_STATUS_UNSUPPORTED_CODEC;
	}

	// OK, we have a codec. Initialize it with the node
	nRetVal = xnInitCodec(hCodec, hInitializerNode);
	if (nRetVal != XN_STATUS_OK)
	{
		xnProductionNodeRelease(hCodec);
		XN_RETURN_WITH_ERROR_LOG(g_logger, nRetVal, "Failed to init codec using given node: %s", xnGetStatusString(nRetVal));
	}

	*phCodec = hCodec;

	return (XN_STATUS_OK);
}

XN_C_API XnCodecID xnGetCodecID(XnNodeHandle hCodec)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hCodec, XN_NODE_TYPE_CODEC, XN_CODEC_NULL);
	XnCodecInterfaceContainer* pInterface = (XnCodecInterfaceContainer*)hCodec->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hCodec->pModuleInstance->hNode;
	return pInterface->Codec.GetCodecID(hModuleNode);
}

XnStatus xnInitCodec(XnNodeHandle hCodec, XnNodeHandle hInitializerNode)
{
	XN_VALIDATE_INTERFACE_TYPE(hCodec, XN_NODE_TYPE_CODEC);
	XnCodecInterfaceContainer* pInterface = (XnCodecInterfaceContainer*)hCodec->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hCodec->pModuleInstance->hNode;
	return pInterface->Codec.Init(hModuleNode, hInitializerNode);
}

XN_C_API XnStatus xnEncodeData(XnNodeHandle hCodec, const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten)
{
	XN_VALIDATE_INTERFACE_TYPE(hCodec, XN_NODE_TYPE_CODEC);
	XnCodecInterfaceContainer* pInterface = (XnCodecInterfaceContainer*)hCodec->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hCodec->pModuleInstance->hNode;
	return pInterface->Codec.CompressData(hModuleNode, pSrc, nSrcSize, pDst, nDstSize, pnBytesWritten);
}

XN_C_API XnStatus xnDecodeData(XnNodeHandle hCodec, const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten)
{
	XN_VALIDATE_INTERFACE_TYPE(hCodec, XN_NODE_TYPE_CODEC);
	XnCodecInterfaceContainer* pInterface = (XnCodecInterfaceContainer*)hCodec->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hCodec->pModuleInstance->hNode;
	return pInterface->Codec.DecompressData(hModuleNode, pSrc, nSrcSize, pDst, nDstSize, pnBytesWritten);
}

//---------------------------------------------------------------------------
// ExtendedSerialization
//---------------------------------------------------------------------------
XnStatus xnNotifyExState(XnNodeHandle hInstance, XnNodeNotifications* pNotifications, void* pCookie)
{
	XnProductionNodeInterfaceContainer* pInterface = (XnProductionNodeInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR(pInterface->ExtendedSerialization.InitNotifications);
	return pInterface->ExtendedSerialization.InitNotifications(hModuleNode, pNotifications, pCookie);
}

void xnUnregisterExNotifications(XnNodeHandle hInstance)
{
	XnProductionNodeInterfaceContainer* pInterface = (XnProductionNodeInterfaceContainer*)hInstance->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hInstance->pModuleInstance->hNode;
	XN_VALIDATE_FUNC_PTR_RET(pInterface->ExtendedSerialization.StopNotifications, );
	pInterface->ExtendedSerialization.StopNotifications(hModuleNode);
}

//---------------------------------------------------------------------------
// Mocks
//---------------------------------------------------------------------------
XnStatus xnMockSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const void* pData)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// set next data
	nRetVal = xnSetGeneralProperty(hInstance, XN_PROP_NEWDATA, nDataSize, pData);
	XN_IS_STATUS_OK(nRetVal);

	// Set frame id
	nRetVal = xnSetIntProperty(hInstance, XN_PROP_FRAME_ID, nFrameID);
	XN_IS_STATUS_OK(nRetVal);

	// Set timestamp
	nRetVal = xnSetIntProperty(hInstance, XN_PROP_TIMESTAMP, nTimestamp);
	XN_IS_STATUS_OK(nRetVal);

	// Update mock node, so that data will be replaced
	nRetVal = xnUpdateDataImpl(hInstance);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnMockDepthSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnDepthPixel* pData)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_DEPTH);
	return xnMockSetData(hInstance, nFrameID, nTimestamp, nDataSize, pData);
}

XN_C_API XnStatus xnMockImageSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnUInt8* pData)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_IMAGE);
	return xnMockSetData(hInstance, nFrameID, nTimestamp, nDataSize, pData);
}

XN_C_API XnStatus xnMockIRSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnIRPixel* pData)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_IR);
	return xnMockSetData(hInstance, nFrameID, nTimestamp, nDataSize, pData);
}

XN_C_API XnStatus xnMockAudioSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnUInt8* pData)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_AUDIO);
	return xnMockSetData(hInstance, nFrameID, nTimestamp, nDataSize, pData);
}

XN_C_API XnStatus xnMockRawSetData(XnNodeHandle hInstance, XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const void* pData)
{
	XN_VALIDATE_INTERFACE_TYPE(hInstance, XN_NODE_TYPE_GENERATOR);
	return xnMockSetData(hInstance, nFrameID, nTimestamp, nDataSize, pData);
}

XN_C_API XnStatus xnCreateScriptNode(XnContext* pContext, const XnChar* strFormat, XnNodeHandle* phScript)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// for now we only support OpenNI XML format
	if (strcmp(strFormat, XN_SCRIPT_FORMAT_XML) != 0)
	{
		return XN_STATUS_NOT_IMPLEMENTED;
	}

	XnNodeInfo* pInfo;
	XnProductionNodeDescription desc;
	GetOpenNIScriptNodeDescription(&desc);

	nRetVal = xnNodeInfoAllocate(&desc, NULL, NULL, &pInfo);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnCreateProductionTree(pContext, pInfo, phScript);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API const XnChar* xnScriptNodeGetSupportedFormat(XnNodeHandle hScript)
{
	XN_VALIDATE_INTERFACE_TYPE_RET(hScript, XN_NODE_TYPE_SCRIPT, NULL);
	XnScriptNodeInterfaceContainer* pInterface = (XnScriptNodeInterfaceContainer*)hScript->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hScript->pModuleInstance->hNode;
	return pInterface->Script.GetSupportedFormat(hModuleNode);
}

XN_C_API XnStatus xnLoadScriptFromFile(XnNodeHandle hScript, const XnChar* strFileName)
{
	XN_VALIDATE_INTERFACE_TYPE(hScript, XN_NODE_TYPE_SCRIPT);
	XN_VALIDATE_INPUT_PTR(strFileName);
	XnScriptNodeInterfaceContainer* pInterface = (XnScriptNodeInterfaceContainer*)hScript->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hScript->pModuleInstance->hNode;
	return pInterface->Script.LoadScriptFromFile(hModuleNode, strFileName);
}

XN_C_API XnStatus xnLoadScriptFromString(XnNodeHandle hScript, const XnChar* strScript)
{
	XN_VALIDATE_INTERFACE_TYPE(hScript, XN_NODE_TYPE_SCRIPT);
	XN_VALIDATE_INPUT_PTR(strScript);
	XnScriptNodeInterfaceContainer* pInterface = (XnScriptNodeInterfaceContainer*)hScript->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hScript->pModuleInstance->hNode;
	return pInterface->Script.LoadScriptFromString(hModuleNode, strScript);
}

static XnStatus xnScriptNodeRunImpl(XnNodeHandle hScript, XnNodeInfoList* pCreatedNodes, XnEnumerationErrors* pErrors)
{
	XN_VALIDATE_INTERFACE_TYPE(hScript, XN_NODE_TYPE_SCRIPT);
	XN_VALIDATE_INPUT_PTR(pCreatedNodes);
	XN_VALIDATE_INPUT_PTR(pErrors);
	XnScriptNodeInterfaceContainer* pInterface = (XnScriptNodeInterfaceContainer*)hScript->pModuleInstance->pLoaded->pInterface;
	XnModuleNodeHandle hModuleNode = hScript->pModuleInstance->hNode;
	return pInterface->Script.Run(hModuleNode, pCreatedNodes, pErrors);
}

XN_C_API XnStatus xnScriptNodeRun(XnNodeHandle hScript, XnEnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INTERFACE_TYPE(hScript, XN_NODE_TYPE_SCRIPT);

	XnNodeInfoList* pCreatedNodes = NULL;
	nRetVal = xnNodeInfoListAllocate(&pCreatedNodes);
	XN_IS_STATUS_OK(nRetVal);

	XnEnumerationErrors* pActualErrors = pErrors;
	if (pActualErrors == NULL)
	{
		nRetVal = xnEnumerationErrorsAllocate(&pActualErrors);
		if (nRetVal != XN_STATUS_OK)
		{
			xnNodeInfoListFree(pCreatedNodes);
			return (nRetVal);
		}
	}

	// run the script
	nRetVal = xnScriptNodeRunImpl(hScript, pCreatedNodes, pActualErrors);

	if (nRetVal == XN_STATUS_OK)
	{
		// add all created nodes to the list of dependencies
		for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pCreatedNodes);
			xnNodeInfoListIteratorIsValid(it);
			it = xnNodeInfoListGetNext(it))
		{
			XnNodeInfo* pInfo = xnNodeInfoListGetCurrent(it);
			if (pInfo->hNode == NULL)
			{
				nRetVal = XN_STATUS_ERROR;
				break;
			}

			nRetVal = xnAddNeededNode(hScript, pInfo->hNode);
			if (nRetVal != XN_STATUS_OK)
			{
				break;
			}
		}
	}

	// release all nodes (either we had an error, or they are already added to the list of needed nodes
	for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pCreatedNodes);
		xnNodeInfoListIteratorIsValid(it);
		it = xnNodeInfoListGetNext(it))
	{
		XnNodeInfo* pInfo = xnNodeInfoListGetCurrent(it);
		if (pInfo->hNode != NULL)
		{
			xnProductionNodeRelease(pInfo->hNode);
		}
	}

	// free the list
	xnNodeInfoListFree(pCreatedNodes);

	// free errors object (if needed)
	if (pErrors == NULL)
	{
		xnEnumerationErrorsFree(pErrors);
	}

	return (nRetVal);
}

//---------------------------------------------------------------------------
// General
//---------------------------------------------------------------------------
#if (XN_PLATFORM == XN_PLATFORM_WIN32) && (_M_X64)
	#define XN_OPEN_NI_INSTALL_PATH_ENV "OPEN_NI_INSTALL_PATH64"
#else
	#define XN_OPEN_NI_INSTALL_PATH_ENV "OPEN_NI_INSTALL_PATH"
#endif

#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	#define XN_OPEN_NI_FILES_LOCATION "\\Data\\"
#elif (CE4100)
	#define XN_OPEN_NI_FILES_LOCATION "/usr/etc/ni/"
#elif (XN_PLATFORM == XN_PLATFORM_LINUX_X86 || XN_PLATFORM == XN_PLATFORM_LINUX_ARM || XN_PLATFORM == XN_PLATFORM_MACOSX)
	#define XN_OPEN_NI_FILES_LOCATION "/var/lib/ni/"
#elif (XN_PLATFORM == XN_PLATFORM_ANDROID_ARM)
	/* Resolved dynamically in Android */
#else
	#error "Unsupported platform!"
#endif

XnStatus xnGetOpenNIConfFilesPath(XnChar* strDest, XnUInt32 nBufSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
#if XN_PLATFORM == XN_PLATFORM_ANDROID_ARM
	

	xnOSGetApplicationFilesDir(strDest, nBufSize);
#else
	nRetVal = xnOSGetEnvironmentVariable(XN_OPEN_NI_INSTALL_PATH_ENV, strDest, nBufSize);
	if (nRetVal == XN_STATUS_OS_ENV_VAR_NOT_FOUND)
	{
		#if (XN_PLATFORM == XN_PLATFORM_WIN32)
			// we don't allow environment variable not to be defined on Windows.
			return nRetVal;
		#else
			// use root FS
			strDest[0] = '\0';
		#endif
	}
	else
	{
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = xnOSStrAppend(strDest, XN_OPEN_NI_FILES_LOCATION, nBufSize);
	XN_IS_STATUS_OK(nRetVal);
#endif
	
	return (XN_STATUS_OK);
}
