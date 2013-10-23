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
#include <XnOpenNI.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MAX_CAPABILITIES_COUNT		100
#define XN_MAX_NEEDED_NODES				100

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------



struct XnNodeQuery
{
	XnChar strVendor[XN_MAX_NAME_LENGTH];
	XnChar strName[XN_MAX_NAME_LENGTH];
	XnVersion MinVersion;
	XnVersion MaxVersion;
	const XnChar* astrSupportedCapabilities[XN_MAX_CAPABILITIES_COUNT];
	XnUInt32 nSupportedCapabilities;
	XnMapOutputMode aSupportedMapOutputModes[XN_MAX_CAPABILITIES_COUNT];
	XnUInt32 nSupportedMapOutputModes;
	XnUInt32 nMinUserPositions;
	XnBool bExistingNodeOnly;
	XnBool bNonExistingNodeOnly;
	const XnChar* astrNeededNodes[XN_MAX_NEEDED_NODES];
	XnUInt32 nNeededNodes;
	XnChar strCreationInfo[XN_MAX_CREATION_INFO_LENGTH];
};

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XN_C_API XnStatus xnNodeQueryAllocate(XnNodeQuery** ppQuery)
{
	XN_VALIDATE_OUTPUT_PTR(ppQuery);

	XN_VALIDATE_CALLOC(*ppQuery, XnNodeQuery, 1);
	(*ppQuery)->MaxVersion.nMajor = 0xFF;

	return (XN_STATUS_OK);
}

XN_C_API void xnNodeQueryFree(XnNodeQuery* pQuery)
{
	// free needed nodes list
	for (XnUInt32 i = 0; i < pQuery->nNeededNodes; ++i)
	{
		xnOSFree(pQuery->astrNeededNodes[i]);
	}

	// free capabilities list
	for (XnUInt32 i = 0; i < pQuery->nSupportedCapabilities; ++i)
	{
		xnOSFree(pQuery->astrSupportedCapabilities[i]);
	}

	xnOSFree(pQuery);
}

XN_C_API XnStatus xnNodeQuerySetVendor(XnNodeQuery* pQuery, const XnChar* strVendor)
{
	XN_VALIDATE_INPUT_PTR(pQuery);
	XN_VALIDATE_INPUT_PTR(strVendor);
	strncpy(pQuery->strVendor, strVendor, XN_MAX_NAME_LENGTH);
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeQuerySetName(XnNodeQuery* pQuery, const XnChar* strName)
{
	XN_VALIDATE_INPUT_PTR(pQuery);
	XN_VALIDATE_INPUT_PTR(strName);
	strncpy(pQuery->strName, strName, XN_MAX_NAME_LENGTH);
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeQuerySetMinVersion(XnNodeQuery* pQuery, const XnVersion* pMinVersion)
{
	XN_VALIDATE_INPUT_PTR(pQuery);
	XN_VALIDATE_INPUT_PTR(pMinVersion);
	xnOSMemCopy(&pQuery->MinVersion, pMinVersion, sizeof(XnVersion));
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeQuerySetMaxVersion(XnNodeQuery* pQuery, const XnVersion* pMaxVersion)
{
	XN_VALIDATE_INPUT_PTR(pQuery);
	XN_VALIDATE_INPUT_PTR(pMaxVersion);
	xnOSMemCopy(&pQuery->MaxVersion, pMaxVersion, sizeof(XnVersion));
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeQueryAddSupportedCapability(XnNodeQuery* pQuery, const XnChar* strNeededCapability)
{
	XN_VALIDATE_INPUT_PTR(pQuery);
	XN_VALIDATE_INPUT_PTR(strNeededCapability);
	pQuery->astrSupportedCapabilities[pQuery->nSupportedCapabilities++] = xnOSStrDup(strNeededCapability);
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeQueryAddSupportedMapOutputMode(XnNodeQuery* pQuery, const XnMapOutputMode* pMapOutputMode)
{
	XN_VALIDATE_INPUT_PTR(pQuery);
	XN_VALIDATE_INPUT_PTR(pMapOutputMode);
	xnOSMemCopy(&pQuery->aSupportedMapOutputModes[pQuery->nSupportedMapOutputModes++], pMapOutputMode, sizeof(XnMapOutputMode));
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeQuerySetSupportedMinUserPositions(XnNodeQuery* pQuery, const XnUInt32 nCount)
{
	XN_VALIDATE_INPUT_PTR(pQuery);
	pQuery->nMinUserPositions = nCount;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeQuerySetExistingNodeOnly(XnNodeQuery* pQuery, XnBool bExistingNode)
{
	XN_VALIDATE_INPUT_PTR(pQuery);
	pQuery->bExistingNodeOnly = bExistingNode;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XN_C_DECL xnNodeQuerySetNonExistingNodeOnly(XnNodeQuery* pQuery, XnBool bNonExistingNode)
{
	XN_VALIDATE_INPUT_PTR(pQuery);
	pQuery->bNonExistingNodeOnly = bNonExistingNode;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeQueryAddNeededNode(XnNodeQuery* pQuery, const XnChar* strInstanceName)
{
	XN_VALIDATE_INPUT_PTR(pQuery);
	XN_VALIDATE_INPUT_PTR(strInstanceName);
	pQuery->astrNeededNodes[pQuery->nNeededNodes++] = xnOSStrDup(strInstanceName);
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnNodeQuerySetCreationInfo(XnNodeQuery* pQuery, const XnChar* strCreationInfo)
{
	XN_VALIDATE_INPUT_PTR(pQuery);
	XN_VALIDATE_INPUT_PTR(strCreationInfo);
	strncpy(pQuery->strCreationInfo, strCreationInfo, sizeof(pQuery->strCreationInfo));
	return (XN_STATUS_OK);
}

static XnBool xnIsInstanceInTree(XnNodeInfo* pNodeInfo, const XnChar* strInstanceName)
{
	if (strcmp(xnNodeInfoGetInstanceName(pNodeInfo), strInstanceName) == 0)
	{
		return (TRUE);
	}

	XnNodeInfoList* pNeededNodes = xnNodeInfoGetNeededNodes(pNodeInfo);

	for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pNeededNodes);
		xnNodeInfoListIteratorIsValid(it);
		it = xnNodeInfoListGetNext(it))
	{
		XnNodeInfo* pChild = xnNodeInfoListGetCurrent(it);
		if (xnIsInstanceInTree(pChild, strInstanceName))
		{
			return (TRUE);
		}
	}

	return (FALSE);
}

static XnBool xnIsMapOutputModeSupported(XnNodeHandle hNode, const XnMapOutputMode* pQuery)
{
	XnUInt32 nModes = xnGetSupportedMapOutputModesCount(hNode);
	if (nModes == 0)
	{
		return (FALSE);
	}

	XnMapOutputMode* aModes = (XnMapOutputMode*)xnOSCalloc(nModes, sizeof(XnMapOutputMode));
	if (aModes == NULL)
	{
		return (FALSE);
	}

	if (XN_STATUS_OK != xnGetSupportedMapOutputModes(hNode, aModes, &nModes))
	{
		xnOSFree(aModes);
		return (FALSE);
	}

	for (XnUInt i = 0; i < nModes; ++i)
	{
		if (pQuery->nXRes != (XnUInt32)(-1) && pQuery->nXRes == aModes[i].nXRes &&
			pQuery->nYRes != (XnUInt32)(-1) && pQuery->nYRes == aModes[i].nYRes &&
			pQuery->nFPS != (XnUInt32)(-1) && pQuery->nFPS == aModes[i].nFPS)
		{
			xnOSFree(aModes);
			return (TRUE);
		}
	}

	xnOSFree(aModes);
	return (FALSE);
}

static XnBool xnIsInfoQueryMatch(const XnNodeQuery* pQuery, XnNodeInfo* pNodeInfo)
{
	const XnProductionNodeDescription* pDescription = xnNodeInfoGetDescription(pNodeInfo);
	
	// vendor
	if (pQuery->strVendor[0] != '\0' && strcmp(pQuery->strVendor, pDescription->strVendor) != 0)
	{
		return (FALSE);
	}

	// name
	if (pQuery->strName[0] != '\0' && strcmp(pQuery->strName, pDescription->strName) != 0)
	{
		return (FALSE);
	}

	// min version
	if (xnVersionCompare(&pQuery->MinVersion, &pDescription->Version) > 0)
	{
		return (FALSE);
	}

	// max version
	if (xnVersionCompare(&pQuery->MaxVersion, &pDescription->Version) < 0)
	{
		return (FALSE);
	}

	// check needed nodes
	for (XnUInt i = 0; i < pQuery->nNeededNodes; ++i)
	{
		if (!xnIsInstanceInTree(pNodeInfo, pQuery->astrNeededNodes[i]))
		{
			return (FALSE);
		}
	}

	// Creation info
	if (pQuery->strCreationInfo[0] != '\0' && strcmp(pQuery->strCreationInfo, xnNodeInfoGetCreationInfo(pNodeInfo)) != 0)
	{
		return (FALSE);
	}

	return (TRUE);
}

static XnBool xnIsNodeInstanceMatch(const XnNodeQuery* pQuery, XnNodeHandle hNode)
{
	for (XnUInt i = 0; i < pQuery->nSupportedCapabilities; ++i)
	{
		if (!xnIsCapabilitySupported(hNode, pQuery->astrSupportedCapabilities[i]))
		{
			return (FALSE);
		}
	}

	for (XnUInt i = 0; i < pQuery->nSupportedMapOutputModes; ++i)
	{
		if (!xnIsMapOutputModeSupported(hNode, &pQuery->aSupportedMapOutputModes[i]))
		{
			return (FALSE);
		}
	}

	if (pQuery->nMinUserPositions > 0 && xnGetSupportedUserPositionsCount(hNode) < pQuery->nMinUserPositions)
	{
		return (FALSE);
	}

	return (TRUE);
}

static XnBool xnIsNodeMatch(XnContext* pContext, const XnNodeQuery* pQuery, XnNodeInfo* pNodeInfo)
{
	// check existing node
	XnNodeHandle hNode = xnNodeInfoGetRefHandle(pNodeInfo);
	if (pQuery->bExistingNodeOnly && (hNode == NULL))
	{
		return (FALSE);
	}

	if (pQuery->bNonExistingNodeOnly && (hNode != NULL))
	{
		return (FALSE);
	}

	if (!xnIsInfoQueryMatch(pQuery, pNodeInfo))
	{
		if (hNode != NULL)
		{
			xnProductionNodeRelease(hNode);
		}

		return (FALSE);
	}

	// check if we need to create an instance, to check capabilities
	if (pQuery->nSupportedCapabilities > 0 ||
		pQuery->nSupportedMapOutputModes > 0 ||
		pQuery->nMinUserPositions > 0)
	{
		if (hNode == NULL)
		{
			const XnProductionNodeDescription* pDescription = xnNodeInfoGetDescription(pNodeInfo);
			xnLogVerbose(XN_MASK_OPEN_NI, "Creating node '%s' of type '%s' for querying...", pDescription->strName, xnProductionNodeTypeToString(pDescription->Type));
			XnStatus nRetVal = xnCreateProductionTree(pContext, pNodeInfo, &hNode);
			if (nRetVal != XN_STATUS_OK)
			{
				xnLogWarning(XN_MASK_OPEN_NI, "Failed to create node of type '%s' for querying: %s", xnProductionNodeTypeToString(pDescription->Type), xnGetStatusString(nRetVal));
				return (FALSE);
			}
		}
	}

	XnBool bResult = xnIsNodeInstanceMatch(pQuery, hNode);

	// in any case, we need to release the node. if we created it, this will cause it to be destroyed. If we just took
	// a reference to it, we need to release it.
	if (hNode != NULL)
	{
		xnProductionNodeRelease(hNode);
	}

	return (bResult);
}

XN_C_API XnStatus xnNodeQueryFilterList(XnContext* pContext, const XnNodeQuery* pQuery, XnNodeInfoList* pList)
{
	XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pList);

	while (xnNodeInfoListIteratorIsValid(it))
	{
		// keep current
		XnNodeInfoListIterator currIt = it;
		XnNodeInfo* pNodeInfo = xnNodeInfoListGetCurrent(currIt);

		// move to next (we might remove this one)
		it = xnNodeInfoListGetNext(it);

		if (!xnIsNodeMatch(pContext, pQuery, pNodeInfo))
		{
			xnNodeInfoListRemove(pList, currIt);
		}
	}

	return (XN_STATUS_OK);
}
