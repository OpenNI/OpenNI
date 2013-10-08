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
#ifndef __XN_PLAYER_IMPL_H__
#define __XN_PLAYER_IMPL_H__

#include "XnStatus.h"
#include "XnInternalTypes.h"
#include <XnModuleInterface.h>
#include <XnStringsHashT.h>
#include <XnTypes.h>
#include <XnOS.h>

namespace xn
{

class PlayerImpl : public NodePrivateData
{
public:
	PlayerImpl();
	virtual ~PlayerImpl();

	XnStatus Init(XnNodeHandle hPlayer);
	virtual void BeforeNodeDestroy();
	XnStatus SetSource(XnRecordMedium sourceType, const XnChar* strSource);
	XnStatus GetSource(XnRecordMedium &sourceType, XnChar* strSource, XnUInt32 nBufSize);
	void Destroy();
	XnStatus EnumerateNodes(XnNodeInfoList** ppList);
	XnStatus SetPlaybackSpeed(XnDouble dSpeed);
	XnDouble GetPlaybackSpeed();
	void TriggerPlayback();
	XnStatus ReadNext();
	XnStatus SeekToTimestamp(XnInt64 nTimeOffset, XnPlayerSeekOrigin origin);
	XnStatus SeekToFrame(const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin);

private:
	XnModulePlayerInterface& ModulePlayer();
	XnModuleNodeHandle ModuleHandle();
	void ResetTimeReference();

	static XnStatus XN_CALLBACK_TYPE OpenFile(void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ReadFile(void* pCookie, void *pBuffer, XnUInt32 nSize, XnUInt32 *pnBytesRead);
	static XnStatus XN_CALLBACK_TYPE SeekFile  (void* pCookie, XnOSSeekType seekType, const XnInt32 nOffset);
	static XnStatus XN_CALLBACK_TYPE SeekFile64(void* pCookie, XnOSSeekType seekType, const XnInt64 nOffset);
	static XnUInt32 XN_CALLBACK_TYPE TellFile  (void* pCookie);
	static XnUInt64 XN_CALLBACK_TYPE TellFile64(void* pCookie);
	static void XN_CALLBACK_TYPE CloseFile(void *pCookie);

	XnStatus OpenFileImpl();
	XnStatus ReadFileImpl(void *pData, XnUInt32 nSize, XnUInt32& nBytesRead);
	XnStatus SeekFileImpl  (XnOSSeekType seekType, XnInt32 nOffset);
	XnStatus SeekFile64Impl(XnOSSeekType seekType, XnInt64 nOffset);
	XnUInt32 TellFileImpl  ();
	XnUInt64 TellFile64Impl();
	void CloseFileImpl();

	//Node notifications
	static XnStatus XN_CALLBACK_TYPE OnNodeAdded(void* pCookie, const XnChar* strNodeName,
		XnProductionNodeType type, XnCodecID compression);
	static XnStatus XN_CALLBACK_TYPE OnNodeRemoved(void* pCookie, const XnChar* strNodeName);
	static XnStatus XN_CALLBACK_TYPE OnNodeIntPropChanged(void* pCookie, const XnChar* strNodeName, 
		const XnChar* strPropName, XnUInt64 nValue);
	static XnStatus XN_CALLBACK_TYPE OnNodeRealPropChanged(void* pCookie, const XnChar* strNodeName, 
		const XnChar* strPropName, XnDouble dValue);
	static XnStatus XN_CALLBACK_TYPE OnNodeStringPropChanged(void* pCookie, const XnChar* strNodeName, 
		const XnChar* strPropName, const XnChar* strValue);
	static XnStatus XN_CALLBACK_TYPE OnNodeGeneralPropChanged(void* pCookie, const XnChar* strNodeName, 
		const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer);
	static XnStatus XN_CALLBACK_TYPE OnNodeStateReady(void* pCookie, const XnChar* strNodeName);
	static XnStatus XN_CALLBACK_TYPE OnNodeNewData(void* pCookie, const XnChar* strNodeName, 
		XnUInt64 nTimeStamp, XnUInt32 nFrame, const void* pData, XnUInt32 nSize);

	XnStatus AddNode(const XnChar* strNodeName, XnProductionNodeType type, XnCodecID compression);
	XnStatus RemoveNode(const XnChar* strNodeName);
	XnStatus SetNodeIntProp(const XnChar* strNodeName, const XnChar* strPropName, XnUInt64 nValue);
	XnStatus SetNodeRealProp(const XnChar* strNodeName, const XnChar* strPropName, XnDouble dValue);
	XnStatus SetNodeStringProp(const XnChar* strNodeName, const XnChar* strPropName, const XnChar* strValue);
	XnStatus SetNodeGeneralProp(const XnChar* strNodeName, const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer);
	XnStatus SetNodeNewData(const XnChar* strNodeName, XnUInt64 nTimeStamp, XnUInt32 nFrame, const void* pData, XnUInt32 nSize);
	XnStatus SetNodeStateReady(const XnChar* strNodeName);

	void OnEndOfFileReached();
	static void XN_CALLBACK_TYPE EndOfFileReachedCallback(void* pCookie);

	void PlaybackThread();
	static XN_THREAD_PROC PlaybackThread(XN_THREAD_PARAM pThreadParam);

	typedef struct PlayedNodeInfo
	{
		XnNodeHandle hNode;
		XnLockHandle hLock;
	} PlayedNodeInfo;

	typedef XnStringsHashT<PlayedNodeInfo> PlayedNodesHash;

	static XnPlayerInputStreamInterface s_fileInputStream;
	static XnNodeNotifications s_nodeNotifications;

	XnNodeHandle m_hPlayer;
	XnBool m_bIsFileOpen;
	XN_FILE_HANDLE m_hInFile;
	XnChar m_strSource[XN_FILE_MAX_PATH];
	XnRecordMedium m_sourceType;
	PlayedNodesHash m_playedNodes;
	XnDouble m_dPlaybackSpeed;
	XnUInt64 m_nStartTimestamp;
	XnUInt64 m_nStartTime;
	XnBool m_bHasTimeReference;
	XN_THREAD_HANDLE m_hPlaybackThread;
	XN_EVENT_HANDLE m_hPlaybackEvent;
	XN_CRITICAL_SECTION_HANDLE m_hPlaybackLock;
	XnBool m_bPlaybackThreadShutdown;
};

}

#endif //__XN_PLAYER_IMPL_H__
