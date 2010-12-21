/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/


#include "XnPlayerImpl.h"
#include <cstdio>
#include <XnModuleInterface.h>
#include <XnLog.h>
#include <XnContext.h>
#include "XnModuleLoader.h"
#include "XnInternalTypes.h"
#include "XnPropNames.h"
#include <XnCppWrapper.h>

using namespace std;

namespace xn
{

XnPlayerInputStreamInterface PlayerImpl::s_fileInputStream = 
{
	&OpenFile,
	&ReadFile,
	&SeekFile,
	&TellFile,
	&CloseFile
};

XnNodeNotifications PlayerImpl::s_nodeNotifications =
{
	&OnNodeAdded,
	&OnNodeRemoved,
	&OnNodeIntPropChanged,
	&OnNodeRealPropChanged,
	&OnNodeStringPropChanged,
	&OnNodeGeneralPropChanged,
	&OnNodeStateReady,
	&OnNodeNewData
};

PlayerImpl::PlayerImpl() : 
	m_hPlayer(NULL), 
	m_pInFile(NULL),
	m_bHasTimeReference(FALSE),
	m_dPlaybackSpeed(1.0)
{
	xnOSMemSet(m_strSource, 0, sizeof(m_strSource));
}

PlayerImpl::~PlayerImpl()
{
	Destroy();
}

XnStatus PlayerImpl::Init(XnNodeHandle hPlayer)
{
	XN_VALIDATE_PTR(hPlayer, XN_STATUS_ERROR);
	XnModuleInstance* pModuleInstance = hPlayer->pModuleInstance;
	XN_VALIDATE_PTR(pModuleInstance, XN_STATUS_ERROR);
	XnModuleNodeHandle hModule = pModuleInstance->hNode;
	XN_VALIDATE_PTR(hModule, XN_STATUS_ERROR);
	XN_VALIDATE_PTR(pModuleInstance->pLoaded, XN_STATUS_ERROR);
	XN_VALIDATE_PTR(pModuleInstance->pLoaded->pInterface, XN_STATUS_ERROR);
	
	m_hPlayer = hPlayer;
	XnStatus nRetVal = ModulePlayer().SetNodeNotifications(ModuleHandle(), this, &s_nodeNotifications);
	XN_IS_STATUS_OK(nRetVal);

	XnCallbackHandle hDummy; // node will be destroyed anyway
	nRetVal = ModulePlayer().RegisterToEndOfFileReached(ModuleHandle(), EndOfFileReachedCallback, this, &hDummy);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

void PlayerImpl::BeforeNodeDestroy()
{
	//Do nothing here - we only destroy in the destructor.
}

XnStatus PlayerImpl::SetSource(XnRecordMedium sourceType, const XnChar* strSource)
{
	XnStatus nRetVal = XN_STATUS_OK;
	m_sourceType = sourceType;
	//Right now the only record medium we support is a file
	
	switch (m_sourceType)
	{
		case XN_RECORD_MEDIUM_FILE:
		{
			nRetVal = xnOSStrCopy(m_strSource, strSource, sizeof(m_strSource));
			XN_IS_STATUS_OK(nRetVal);
			nRetVal = ModulePlayer().SetInputStream(ModuleHandle(), this, &s_fileInputStream);
			XN_IS_STATUS_OK(nRetVal);
			break;
		}
		default:
			XN_ASSERT(FALSE);
			return XN_STATUS_BAD_PARAM;
	}
	return XN_STATUS_OK;
}

XnStatus PlayerImpl::GetSource(XnRecordMedium &sourceType, XnChar* strSource, XnUInt32 nBufSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	sourceType = m_sourceType;
	nRetVal = xnOSStrCopy(strSource, m_strSource, nBufSize);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

void PlayerImpl::Destroy()
{
	CloseFileImpl();
}

XnStatus PlayerImpl::EnumerateNodes(XnNodeInfoList** ppList)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = xnNodeInfoListAllocate(ppList);
	XN_IS_STATUS_OK(nRetVal);

	for (PlayedNodesHash::Iterator it = m_playedNodes.begin(); it != m_playedNodes.end(); ++it)
	{
		XnNodeInfo* pNodeInfo = xnGetNodeInfo(it.Value().hNode);

		nRetVal = xnNodeInfoListAddNode(*ppList, pNodeInfo);
		if (nRetVal != XN_STATUS_OK)
		{
			xnNodeInfoListFree(*ppList);
			return (nRetVal);
		}
	}

	return (XN_STATUS_OK);
}

XnStatus PlayerImpl::SetPlaybackSpeed(XnDouble dSpeed)
{
	if (dSpeed < 0)
	{
		return XN_STATUS_BAD_PARAM;
	}

	m_dPlaybackSpeed = dSpeed;

	return XN_STATUS_OK;
}

XnDouble PlayerImpl::GetPlaybackSpeed()
{
	return m_dPlaybackSpeed;
}

void PlayerImpl::ResetTimeReference()
{
	m_bHasTimeReference = FALSE;
}

XnModulePlayerInterface& PlayerImpl::ModulePlayer()
{
	return ((XnPlayerInterfaceContainer*)
		m_hPlayer->pModuleInstance->pLoaded->pInterface)->Player;
}

XnModuleNodeHandle PlayerImpl::ModuleHandle()
{
	return m_hPlayer->pModuleInstance->hNode;
}

XnStatus XN_CALLBACK_TYPE PlayerImpl::OpenFile(void* pCookie)
{
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	return pThis->OpenFileImpl();
}

XnStatus XN_CALLBACK_TYPE PlayerImpl::ReadFile(void* pCookie, void* pBuffer, XnUInt32 nSize, XnUInt32* pnBytesRead)
{
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	XnUInt32 nBytesRead = 0;
	return pThis->ReadFileImpl(pBuffer, nSize, (pnBytesRead != NULL) ? *pnBytesRead : nBytesRead);
}

XnStatus XN_CALLBACK_TYPE PlayerImpl::SeekFile(void* pCookie, XnOSSeekType seekType, const XnInt32 nOffset)
{
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	XN_VALIDATE_INPUT_PTR(pThis);
	return pThis->SeekFileImpl(seekType, nOffset);
}

XnUInt32 XN_CALLBACK_TYPE PlayerImpl::TellFile(void* pCookie)
{
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	XN_VALIDATE_PTR(pThis, (XnUInt32)-1);
	return pThis->TellFileImpl();
}

void XN_CALLBACK_TYPE PlayerImpl::CloseFile(void* pCookie)
{
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	if (pThis == NULL)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Got NULL cookie");
		return;
	}
	pThis->CloseFileImpl();
}

XnStatus PlayerImpl::OpenFileImpl()
{
	if (m_pInFile != NULL)
	{
		//Already open
		return XN_STATUS_OK;
	}
	
	m_pInFile = fopen(m_strSource, "rb");
	if (m_pInFile == NULL)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to open file '%s' for reading", m_strSource);
		return XN_STATUS_OS_FILE_NOT_FOUND;
	}

	return XN_STATUS_OK;
}

XnStatus PlayerImpl::ReadFileImpl(void* pData, XnUInt32 nSize, XnUInt32 &nBytesRead)
{
	XN_VALIDATE_PTR(m_pInFile, XN_STATUS_ERROR);
	nBytesRead = fread(pData, 1, nSize, m_pInFile);
	if (ferror(m_pInFile))
	{
		return XN_STATUS_OS_FILE_READ_FAILED;
	}
	//nBytesRead could be smaller than nSize at the end, but that's not an error
	return XN_STATUS_OK;
}

XnStatus PlayerImpl::SeekFileImpl(XnOSSeekType seekType, XnInt32 nOffset)
{
	XN_VALIDATE_PTR(m_pInFile, XN_STATUS_ERROR);
	long nOrigin = 0;
	switch (seekType)
	{
		case XN_OS_SEEK_CUR:
			nOrigin = SEEK_CUR;
			break;
		case XN_OS_SEEK_END:
			nOrigin = SEEK_END;
			break;
		case SEEK_SET:
			nOrigin = SEEK_SET;
			break;
		default:
			XN_ASSERT(FALSE);
			return XN_STATUS_BAD_PARAM;
	}
	
	if (fseek(m_pInFile, nOffset, nOrigin) != 0)
	{
		return XN_STATUS_ERROR;
	}

	return XN_STATUS_OK;	
}

XnUInt32 PlayerImpl::TellFileImpl()
{
	XN_VALIDATE_PTR(m_pInFile, (XnUInt32)-1);
	return ftell(m_pInFile);
}

void PlayerImpl::CloseFileImpl()
{
	if (m_pInFile != NULL)	
	{
		fclose(m_pInFile);
		m_pInFile = NULL;
	}
}

XnStatus XN_CALLBACK_TYPE PlayerImpl::OnNodeAdded(void* pCookie, const XnChar* strNodeName, XnProductionNodeType type, XnCodecID compression)
{
	XN_VALIDATE_INPUT_PTR(pCookie);
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	return pThis->AddNode(strNodeName, type, compression);
}

XnStatus XN_CALLBACK_TYPE PlayerImpl::OnNodeRemoved(void* pCookie, const XnChar* strNodeName)
{
	XN_VALIDATE_INPUT_PTR(pCookie);
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	return pThis->RemoveNode(strNodeName);
}

XnStatus XN_CALLBACK_TYPE PlayerImpl::OnNodeIntPropChanged(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, XnUInt64 nValue)
{
	XN_VALIDATE_INPUT_PTR(pCookie);
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	return pThis->SetNodeIntProp(strNodeName, strPropName, nValue);
}

XnStatus XN_CALLBACK_TYPE PlayerImpl::OnNodeRealPropChanged(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, XnDouble dValue)
{
	XN_VALIDATE_INPUT_PTR(pCookie);
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	return pThis->SetNodeRealProp(strNodeName, strPropName, dValue);
}

XnStatus XN_CALLBACK_TYPE PlayerImpl::OnNodeStringPropChanged(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, const XnChar* strValue)
{
	XN_VALIDATE_INPUT_PTR(pCookie);
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	return pThis->SetNodeStringProp(strNodeName, strPropName, strValue);
}

XnStatus XN_CALLBACK_TYPE PlayerImpl::OnNodeGeneralPropChanged(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer)
{
	XN_VALIDATE_INPUT_PTR(pCookie);
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	return pThis->SetNodeGeneralProp(strNodeName, strPropName, nBufferSize, pBuffer);
}

XnStatus XN_CALLBACK_TYPE PlayerImpl::OnNodeStateReady(void* pCookie, const XnChar* strNodeName)
{
	XN_VALIDATE_INPUT_PTR(pCookie);
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	return pThis->SetNodeStateReady(strNodeName);
}

XnStatus XN_CALLBACK_TYPE PlayerImpl::OnNodeNewData(void* pCookie, const XnChar* strNodeName, XnUInt64 nTimeStamp, XnUInt32 nFrame, const void* pData, XnUInt32 nSize)
{
	XN_VALIDATE_INPUT_PTR(pCookie);
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	return pThis->SetNodeNewData(strNodeName, nTimeStamp, nFrame, pData, nSize);
}

XnStatus PlayerImpl::AddNode(const XnChar* strNodeName, XnProductionNodeType type, XnCodecID compression)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	PlayedNodeInfo playedNodeInfo = {0};

	if (m_playedNodes.Get(strNodeName, playedNodeInfo) == XN_STATUS_OK)
	{
		// already in the list, just return OK.
		return (XN_STATUS_OK);
	}

	// check if we need to create it (maybe it's a rewind...)
	if (xnGetNodeHandleByName(m_hPlayer->pContext, strNodeName, &playedNodeInfo.hNode) != XN_STATUS_OK)
	{
		XnStatus nRetVal = xnCreateMockNode(m_hPlayer->pContext, type, strNodeName, &playedNodeInfo.hNode);
		XN_IS_STATUS_OK(nRetVal);
	}

	// lock it, so no one can change configuration (this is a file recording)
	nRetVal = xnLockNodeForChanges(playedNodeInfo.hNode, &playedNodeInfo.hLock);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_playedNodes.Set(strNodeName, playedNodeInfo);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus PlayerImpl::RemoveNode(const XnChar* strNodeName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	PlayedNodeInfo playedNodeInfo = {0};

	nRetVal = m_playedNodes.Get(strNodeName, playedNodeInfo);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnUnlockNodeForChanges(playedNodeInfo.hNode, playedNodeInfo.hLock);
	XN_IS_STATUS_OK(nRetVal);
	
	nRetVal = m_playedNodes.Remove(strNodeName);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus PlayerImpl::SetNodeIntProp(const XnChar* strNodeName, const XnChar* strPropName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	PlayedNodeInfo playedNode;
	nRetVal = m_playedNodes.Get(strNodeName, playedNode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnLockedNodeStartChanges(playedNode.hNode, playedNode.hLock);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetIntProperty(playedNode.hNode, strPropName, nValue);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
		return (nRetVal);
	}

	nRetVal = xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus PlayerImpl::SetNodeRealProp(const XnChar* strNodeName, const XnChar* strPropName, XnDouble dValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	PlayedNodeInfo playedNode;
	nRetVal = m_playedNodes.Get(strNodeName, playedNode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnLockedNodeStartChanges(playedNode.hNode, playedNode.hLock);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetRealProperty(playedNode.hNode, strPropName, dValue);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
		return (nRetVal);
	}

	nRetVal = xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus PlayerImpl::SetNodeStringProp(const XnChar* strNodeName, const XnChar* strPropName, const XnChar* strValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	PlayedNodeInfo playedNode;
	nRetVal = m_playedNodes.Get(strNodeName, playedNode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnLockedNodeStartChanges(playedNode.hNode, playedNode.hLock);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetStringProperty(playedNode.hNode, strPropName, strValue);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
		return (nRetVal);
	}

	nRetVal = xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus PlayerImpl::SetNodeGeneralProp(const XnChar* strNodeName, const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	PlayedNodeInfo playedNode;
	nRetVal = m_playedNodes.Get(strNodeName, playedNode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnLockedNodeStartChanges(playedNode.hNode, playedNode.hLock);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetGeneralProperty(playedNode.hNode, strPropName, nBufferSize, pBuffer);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
		return (nRetVal);
	}

	nRetVal = xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus PlayerImpl::SetNodeNewData(const XnChar* strNodeName, XnUInt64 nTimeStamp, XnUInt32 nFrame, const void* pData, XnUInt32 nSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);

	if (!m_bHasTimeReference)
	{
		m_nStartTimestamp = nTimeStamp;
		m_nStartTime = nNow;

		m_bHasTimeReference = TRUE;
	}

	if (m_dPlaybackSpeed != XN_PLAYBACK_SPEED_FASTEST)
	{
		// check this data timestamp compared to when we started
		XnInt64 nTimestampDiff = nTimeStamp - m_nStartTimestamp;
		XnInt64 nTimeDiff = nNow - m_nStartTime;

		// check if we need to wait some time
		XnInt64 nRequestedTimeDiff = (XnInt64)(nTimestampDiff / m_dPlaybackSpeed);
		if (nTimeDiff < nRequestedTimeDiff)
		{
			xnOSSleep(XnUInt32((nRequestedTimeDiff - nTimeDiff)/1000));
		}

		// update reference to current frame (this will handle cases in which application
		// stopped reading frames and continued after a while)
		m_nStartTimestamp = nTimeStamp;
		xnOSGetHighResTimeStamp(&m_nStartTime);
	}

	PlayedNodeInfo playedNode;
	nRetVal = m_playedNodes.Get(strNodeName, playedNode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnLockedNodeStartChanges(playedNode.hNode, playedNode.hLock);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetIntProperty(playedNode.hNode, XN_PROP_TIMESTAMP, nTimeStamp);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
		return (nRetVal);
	}
	nRetVal = xnSetIntProperty(playedNode.hNode, XN_PROP_FRAME_ID, nFrame);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
		return (nRetVal);
	}
	nRetVal = xnSetGeneralProperty(playedNode.hNode, XN_PROP_NEWDATA, nSize, pData);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
		return (nRetVal);
	}

	nRetVal = xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus PlayerImpl::SetNodeStateReady(const XnChar* strNodeName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	PlayedNodeInfo playedNode;
	nRetVal = m_playedNodes.Get(strNodeName, playedNode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnLockedNodeStartChanges(playedNode.hNode, playedNode.hLock);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetIntProperty(playedNode.hNode, XN_PROP_STATE_READY, 1);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLockedNodeEndChanges(playedNode.hNode, playedNode.hLock);
		return (nRetVal);
	}

	//TODO: When we move the codec handling to PlayerImpl this notification will be more useful for PlayerImpl.
	
	return (XN_STATUS_OK);
}

void PlayerImpl::OnEndOfFileReached()
{
	ResetTimeReference();
}

void PlayerImpl::EndOfFileReachedCallback(void* pCookie)
{
	PlayerImpl* pThis = (PlayerImpl*)pCookie;
	pThis->OnEndOfFileReached();
}

}
