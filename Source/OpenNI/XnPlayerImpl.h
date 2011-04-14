/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
#ifndef __XN_PLAYER_IMPL_H__
#define __XN_PLAYER_IMPL_H__

#include "XnStatus.h"
#include "XnInternalTypes.h"
#include <XnModuleInterface.h>
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
	void ResetTimeReference();

private:
	XnModulePlayerInterface& ModulePlayer();
	XnModuleNodeHandle ModuleHandle();

	static XnStatus XN_CALLBACK_TYPE OpenFile(void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ReadFile(void* pCookie, void *pBuffer, XnUInt32 nSize, XnUInt32 *pnBytesRead);
	static XnStatus XN_CALLBACK_TYPE SeekFile(void* pCookie, XnOSSeekType seekType, const XnInt32 nOffset);
	static XnUInt32 XN_CALLBACK_TYPE TellFile(void* pCookie);
	static void XN_CALLBACK_TYPE CloseFile(void *pCookie);

	XnStatus OpenFileImpl();
	XnStatus ReadFileImpl(void *pData, XnUInt32 nSize, XnUInt32& nBytesRead);
	XnStatus SeekFileImpl(XnOSSeekType seekType, XnInt32 nOffset);
	XnUInt32 TellFileImpl();
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

	typedef struct PlayedNodeInfo
	{
		XnNodeHandle hNode;
		XnLockHandle hLock;
	} PlayedNodeInfo;

	XN_DECLARE_STRINGS_HASH(PlayedNodeInfo, PlayedNodesHash);

	XnNodeHandle m_hPlayer;
	static XnPlayerInputStreamInterface s_fileInputStream;
	static XnNodeNotifications s_nodeNotifications;
	FILE* m_pInFile;
	XnChar m_strSource[XN_FILE_MAX_PATH];
	XnRecordMedium m_sourceType;
	PlayedNodesHash m_playedNodes;
	XnDouble m_dPlaybackSpeed;
	XnUInt64 m_nStartTimestamp;
	XnUInt64 m_nStartTime;
	XnBool m_bHasTimeReference;
};

}

#endif //__XN_PLAYER_IMPL_H__