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
#include <XnCppWrapper.h>
#include <XnLog.h>
#include "XnRecorderImpl.h"
#include "XnNodeWatcher.h"
#include "XnModuleLoader.h"
#include <XnModuleInterface.h>
#include "XnInternalTypes.h"
#include "XnPropNames.h"
#include <XnCodecIDs.h>
#include "XnTypeManager.h"

namespace xn 
{

XnRecorderOutputStreamInterface RecorderImpl::s_fileOutputStream = 
{
	&RecorderImpl::OpenFile,
	&RecorderImpl::WriteFile,
	&RecorderImpl::SeekFile,
	&RecorderImpl::TellFile,
	&RecorderImpl::CloseFile,
	&RecorderImpl::SeekFile64,
	&RecorderImpl::TellFile64
};

RecorderImpl::RecorderImpl() : 
	m_destType(XN_RECORD_MEDIUM_FILE),
	m_bIsFileOpen(FALSE),
	m_hOutFile(XN_INVALID_FILE_HANDLE),
	m_hRecorder(NULL)
{
	xnOSMemSet(m_strFileName, 0, sizeof(m_strFileName));
}

RecorderImpl::~RecorderImpl()
{
	Destroy();
}

XnStatus RecorderImpl::Init(XnNodeHandle hRecorder)
{
	XN_VALIDATE_PTR(hRecorder, XN_STATUS_ERROR);
	XnModuleInstance* pModuleInstance = hRecorder->pModuleInstance;
	XN_VALIDATE_PTR(pModuleInstance, XN_STATUS_ERROR);
	XnModuleNodeHandle hModule = pModuleInstance->hNode;
	XN_VALIDATE_PTR(hModule, XN_STATUS_ERROR);
	XN_VALIDATE_PTR(pModuleInstance->pLoaded, XN_STATUS_ERROR);
	XN_VALIDATE_PTR(pModuleInstance->pLoaded->pInterface, XN_STATUS_ERROR);
	
	m_hRecorder = hRecorder;
	
	return XN_STATUS_OK;
}

void RecorderImpl::BeforeNodeDestroy()
{
	//Do nothing here - we only destroy in the destructor.
}

void RecorderImpl::Destroy()
{
	for (NodeWatchersMap::Iterator it = m_nodeWatchersMap.Begin(); it != m_nodeWatchersMap.End(); ++it)
	{
		XN_DELETE(it->Value());
	}
	m_nodeWatchersMap.Clear();
	CloseFileImpl();	
}

XnStatus RecorderImpl::AddNode(ProductionNode &node, XnCodecID compression)
{
	if (!node.IsValid())
	{
		return XN_STATUS_BAD_PARAM;		
	}

	NodeWatchersMap::ConstIterator it = m_nodeWatchersMap.Find(node.GetHandle());
	if (it != m_nodeWatchersMap.End())
	{
		return XN_STATUS_NODE_ALREADY_RECORDED;
	}

	if (compression == XN_CODEC_NULL)
	{
		compression = GetDefaultCodecID(node);
	}

	// take the predefined type (if we'll store extension types, we won't be able to play them later on)
	XnProductionNodeType type = TypeManager::GetInstance().GetPredefinedBaseType(node.GetInfo().GetDescription().Type);

	NodeWatcher* pNodeWatcher = NULL;
	XnStatus nRetVal = CreateNodeWatcher(node, type, ModuleHandle(), Notifications(), pNodeWatcher);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = pNodeWatcher->Register();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pNodeWatcher);
		return nRetVal;
	}

	nRetVal = NotifyNodeAdded(node.GetHandle(), type, compression);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pNodeWatcher);
		return nRetVal;
	}

	nRetVal = pNodeWatcher->NotifyState();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pNodeWatcher);
		return nRetVal;
	}

	nRetVal = m_nodeWatchersMap.Set(node.GetHandle(), pNodeWatcher);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pNodeWatcher);
		return nRetVal;
	}
	
	return XN_STATUS_OK;
}

XnStatus RecorderImpl::RemoveNode(ProductionNode &node)
{
	if (!node.IsValid())
	{
		return XN_STATUS_BAD_PARAM;		
	}

	XnStatus nRetVal = NotifyNodeRemoved(node.GetHandle());
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = RemoveNodeImpl(node);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}


XnStatus RecorderImpl::AddRawNode(const XnChar* strNodeName)
{
	if (IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Attempted to add a raw node by name of '%s' but there is already a raw node by that name", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_INVALID_OPERATION;
	}

	XnNodeHandle hNode = NULL;
	NodeWatcher* pNodeWatcher = NULL;
	if ((xnGetRefNodeHandleByName(m_hRecorder->pContext, strNodeName, &hNode) == XN_STATUS_OK) &&
		(m_nodeWatchersMap.Get(hNode, pNodeWatcher) == XN_STATUS_OK))
	{
		//There's a node by that name and we're already watching it
		xnLogWarning(XN_MASK_OPEN_NI, "Attempted to add a raw node by name of '%s' but there is already another node by that name that is being recorded", strNodeName);
		XN_ASSERT(FALSE);
		xnProductionNodeRelease(hNode);
		return XN_STATUS_INVALID_OPERATION;
	}

	XnStatus nRetVal = Notifications().OnNodeAdded(ModuleHandle(), 
		strNodeName, (XnProductionNodeType)0, XN_CODEC_UNCOMPRESSED);
	XN_IS_STATUS_OK(nRetVal);

	RawNodeInfo rawNodeInfo;
	nRetVal = m_rawNodesMap.Set(strNodeName, rawNodeInfo);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}


XnStatus RecorderImpl::RemoveRawNode(const XnChar* strNodeName)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Tried to remove non-existing raw node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}

	return Notifications().OnNodeRemoved(ModuleHandle(), strNodeName);
}


XnStatus RecorderImpl::SetRawNodeIntProp(const XnChar* strNodeName, const XnChar* strPropName, XnUInt64 nVal)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Tried to set property of non-existing node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}

	return Notifications().OnNodeIntPropChanged(ModuleHandle(), strNodeName, strPropName, nVal);
}

XnStatus RecorderImpl::SetRawNodeRealProp(const XnChar* strNodeName, const XnChar* strPropName, XnDouble dVal)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Tried to set property of non-existing node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}

	return Notifications().OnNodeRealPropChanged(ModuleHandle(), strNodeName, strPropName, dVal);
}

XnStatus RecorderImpl::SetRawNodeStringProp(const XnChar* strNodeName, const XnChar* strPropName, const XnChar* strVal)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Tried to set property of non-existing node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}

	return Notifications().OnNodeStringPropChanged(ModuleHandle(), strNodeName, strPropName, strVal);
}

XnStatus RecorderImpl::SetRawNodeGeneralProp(const XnChar* strNodeName, const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Tried to set property of non-existing node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}

	return Notifications().OnNodeGeneralPropChanged(ModuleHandle(), strNodeName, strPropName, nBufferSize, pBuffer);
}

XnStatus RecorderImpl::NotifyRawNodeStateReady(const XnChar* strNodeName)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "There is no node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}
	
	return Notifications().OnNodeStateReady(ModuleHandle(), strNodeName);
}

XnStatus RecorderImpl::SetRawNodeNewData(const XnChar* strNodeName, XnUInt64 nTimeStamp, XnUInt32 nFrame, const void* pData, XnUInt32 nSize)
{
	if (!IsRawNode(strNodeName))
	{
		xnLogWarning(XN_MASK_OPEN_NI, "There is no node by the name of '%s'", strNodeName);
		XN_ASSERT(FALSE);
		return XN_STATUS_NO_MATCH;
	}

	return Notifications().OnNodeNewData(ModuleHandle(), strNodeName, nTimeStamp, nFrame, pData, nSize);
}

XnStatus RecorderImpl::RemoveNodeImpl(ProductionNode &node)
{
	XnNodeHandle hNode = node.GetHandle();

	NodeWatchersMap::ConstIterator it = m_nodeWatchersMap.Find(hNode);
	if (it == m_nodeWatchersMap.End())
	{
		return XN_STATUS_NO_MATCH;
	}

	XN_DELETE(it->Value());

	XnStatus nRetVal = m_nodeWatchersMap.Remove(it);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnNodeNotifications& RecorderImpl::Notifications()
{
	return ((XnRecorderInterfaceContainer*)
		(m_hRecorder->pModuleInstance->pLoaded->pInterface))->nodeNotifications;
}

XnModuleRecorderInterface& RecorderImpl::ModuleRecorder()
{
	return ((XnRecorderInterfaceContainer*)
		m_hRecorder->pModuleInstance->pLoaded->pInterface)->recorder;
}

XnModuleNodeHandle RecorderImpl::ModuleHandle()
{
	return m_hRecorder->pModuleInstance->hNode;
}

XnStatus RecorderImpl::NotifyNodeAdded(XnNodeHandle hNode, XnProductionNodeType type, XnCodecID compression)
{
	return Notifications().OnNodeAdded(ModuleHandle(), xnGetNodeName(hNode), type, compression);
}

XnStatus RecorderImpl::NotifyNodeRemoved(XnNodeHandle hNode)
{
	return Notifications().OnNodeRemoved(ModuleHandle(), xnGetNodeName(hNode));
}

XnStatus RecorderImpl::SetDestination(XnRecordMedium destType, const XnChar* strDest)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (destType)
	{
		//Right now only file destination is supported
		case XN_RECORD_MEDIUM_FILE:
		{
			if (m_bIsFileOpen)
			{
				XN_LOG_WARNING_RETURN(XN_STATUS_INVALID_OPERATION, XN_MASK_OPEN_NI, "Recorder destination is already set!");
			}

			m_destType = destType;
			nRetVal = xnOSStrCopy(m_strFileName, strDest, sizeof(m_strFileName));
			XN_IS_STATUS_OK(nRetVal);
			nRetVal = ModuleRecorder().SetOutputStream(ModuleHandle(), this, &s_fileOutputStream);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
		default:
			XN_ASSERT(FALSE);
			return XN_STATUS_BAD_PARAM;
	}

	return XN_STATUS_OK;
}

XnStatus RecorderImpl::GetDestination(XnRecordMedium& destType, XnChar* strDest, XnUInt32 nBufSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	switch (m_destType)
	{
		case XN_RECORD_MEDIUM_FILE:
			destType = m_destType;
			nRetVal = xnOSStrCopy(strDest, m_strFileName, nBufSize);
			XN_IS_STATUS_OK(nRetVal);
			break;

		default:
			XN_ASSERT(FALSE);
			return XN_STATUS_ERROR;
	}

	return XN_STATUS_OK;
}

typedef struct 
{
	NodeWatcher* pWatcher;
	XnUInt64 nTimestamp;
} WatcherData;

static void SortWatchersByTimestamp(WatcherData aWatchers[], XnUInt32 nCount)
{
	// use bubble sort
	XnUInt32 n = nCount;
	XnBool bSwapped;
	WatcherData temp;

	if (nCount == 0)
		return;

	do
	{
		bSwapped = FALSE;
		for (XnUInt32 i = 0; i < n - 1; ++i)
		{
			if (aWatchers[i].nTimestamp > aWatchers[i+1].nTimestamp)
			{
				// swap
				temp = aWatchers[i];
				aWatchers[i] = aWatchers[i+1];
				aWatchers[i+1] = temp;

				bSwapped = TRUE;
			}
		}

		n -= 1;

	} while (bSwapped);
}

XnStatus RecorderImpl::Record()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// We need to sort the watchers according to their timestamp, so that data buffers will be sorted in the
	// file.
	const XnUInt32 MAX_SUPPORTED_NODES = 200;
	WatcherData watchers[MAX_SUPPORTED_NODES];
	XnUInt32 nCount = 0;

	for (NodeWatchersMap::Iterator it = m_nodeWatchersMap.Begin(); it != m_nodeWatchersMap.End(); ++it)
	{
		watchers[nCount].pWatcher = it->Value();
		watchers[nCount].nTimestamp = it->Value()->GetTimestamp();
		++nCount;

		if (nCount > MAX_SUPPORTED_NODES)
		{
			XN_LOG_ERROR_RETURN(XN_STATUS_ERROR, XN_MASK_OPEN_NI, "OpenNI recorder does not support more than %u nodes.", MAX_SUPPORTED_NODES);
		}
	}

	SortWatchersByTimestamp(watchers, nCount);

	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		nRetVal = watchers[i].pWatcher->Watch();
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
}

XnStatus RecorderImpl::OpenFile(void* pCookie)
{
	RecorderImpl* pThis = (RecorderImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	return pThis->OpenFileImpl();
}

XnStatus RecorderImpl::WriteFile(void* pCookie, const XnChar* strNodeName, const void* pData, XnUInt32 nSize)
{
	RecorderImpl* pThis = (RecorderImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	return pThis->WriteFileImpl(strNodeName, pData, nSize);
}


XnStatus XN_CALLBACK_TYPE RecorderImpl::SeekFile(void* pCookie, XnOSSeekType seekType, const XnInt32 nOffset)
{
	RecorderImpl* pThis = (RecorderImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	return pThis->SeekFileImpl(seekType, nOffset);
}

XnStatus XN_CALLBACK_TYPE RecorderImpl::SeekFile64(void* pCookie, XnOSSeekType seekType, const XnInt64 nOffset)
{
	RecorderImpl* pThis = (RecorderImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	return pThis->SeekFile64Impl(seekType, nOffset);
}

XnUInt32 XN_CALLBACK_TYPE RecorderImpl::TellFile(void* pCookie)
{
	RecorderImpl* pThis = (RecorderImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	return pThis->TellFileImpl();
}

XnUInt64 XN_CALLBACK_TYPE RecorderImpl::TellFile64(void* pCookie)
{
	RecorderImpl* pThis = (RecorderImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	return pThis->TellFile64Impl();
}

void RecorderImpl::CloseFile(void* pCookie)
{
	RecorderImpl* pThis = (RecorderImpl*)pCookie;
	if (pThis == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}
	pThis->CloseFileImpl();
}

XnStatus RecorderImpl::OpenFileImpl()
{
	if (m_bIsFileOpen)
	{
		//Already open
		return XN_STATUS_OK;
	}

	XnStatus nRetVal = xnOSOpenFile(m_strFileName, XN_OS_FILE_WRITE | XN_OS_FILE_TRUNCATE, &m_hOutFile);

	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to open file '%s' for writing", m_strFileName);
		return XN_STATUS_OS_FILE_OPEN_FAILED;
	}
	m_bIsFileOpen = TRUE;

	return XN_STATUS_OK;	
}

XnStatus RecorderImpl::WriteFileImpl(const XnChar* /*strNodeName*/, 
									 const void* pData, 
									 XnUInt32 nSize)
{
	//strNodeName may be NULL
	XN_IS_BOOL_OK_RET(m_bIsFileOpen, XN_STATUS_ERROR);

	return xnOSWriteFile(m_hOutFile, pData, nSize);
}


XnStatus RecorderImpl::SeekFileImpl(XnOSSeekType seekType, const XnInt32 nOffset)
{
	XN_IS_BOOL_OK_RET(m_bIsFileOpen, XN_STATUS_ERROR);
	return xnOSSeekFile64(m_hOutFile, seekType, nOffset);
}

XnStatus RecorderImpl::SeekFile64Impl(XnOSSeekType seekType, const XnInt64 nOffset)
{
	XN_IS_BOOL_OK_RET(m_bIsFileOpen, XN_STATUS_ERROR);
	return xnOSSeekFile64(m_hOutFile, seekType, nOffset);
}

XnUInt32 RecorderImpl::TellFileImpl()
{
	XN_IS_BOOL_OK_RET(m_bIsFileOpen, XN_STATUS_ERROR);
	XnUInt64 pos;
	XnStatus nRetVal = xnOSTellFile64(m_hOutFile, &pos);
	XN_IS_STATUS_OK_RET(nRetVal, (XnUInt32) -1);
	// Enforce uint32 limitation
	if (pos >> 32)
		return (XnUInt32) -1;

	return (XnUInt32)pos;
}

XnUInt64 RecorderImpl::TellFile64Impl()
{
	XN_IS_BOOL_OK_RET(m_bIsFileOpen, XN_STATUS_ERROR);
	XnUInt64 pos;
	XnStatus nRetVal = xnOSTellFile64(m_hOutFile, &pos);
	XN_IS_STATUS_OK_RET(nRetVal, (XnUInt64) -1);

	return pos;
}

void RecorderImpl::CloseFileImpl()
{
	if (m_bIsFileOpen)
	{
		xnOSCloseFile(&m_hOutFile);
		m_bIsFileOpen = FALSE;
	}
}

XnCodecID RecorderImpl::GetDefaultCodecID(ProductionNode& node)
{
	XN_ASSERT(node.IsValid());
	XnProductionNodeType type = node.GetInfo().GetDescription().Type;

	if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_DEPTH))
	{
		return XN_CODEC_16Z_EMB_TABLES;
	}
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_IMAGE))
	{
		ImageGenerator gen(node);
		XnPixelFormat format = gen.GetPixelFormat();
		switch (format)
		{
			case XN_PIXEL_FORMAT_RGB24:
				return XN_CODEC_JPEG;
			case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
				return XN_CODEC_8Z;
			default:
				return XN_CODEC_UNCOMPRESSED;
		}
	}
	else
	{
		return XN_CODEC_UNCOMPRESSED;
	}
}

XnBool RecorderImpl::IsRawNode(const XnChar* strNodeName)
{
	RawNodeInfo* pRawNodeInfo = NULL;
	return (m_rawNodesMap.Get(strNodeName, pRawNodeInfo) == XN_STATUS_OK);
}

} //namespace xn
