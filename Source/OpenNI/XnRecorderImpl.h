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
#ifndef __XN_RECORDER_IMPL_H__
#define __XN_RECORDER_IMPL_H__

#include "XnStatus.h"
#include "XnInternalTypes.h"
#include <XnTypes.h>
#include <XnStringsHashT.h>

typedef struct XnModuleRecorderInterface XnModuleRecorderInterface;

namespace xn
{
	class ProductionNode;
	class NodeWatcher;

	class RecorderImpl : public NodePrivateData
	{
	public:
		RecorderImpl();
		virtual ~RecorderImpl();

		XnStatus Init(XnNodeHandle hRecorder);
		virtual void BeforeNodeDestroy();
		void Destroy();
		XnStatus AddNode(ProductionNode &node, XnCodecID compression);
		XnStatus RemoveNode(ProductionNode &node);
		XnStatus AddRawNode(const XnChar* strNodeName);
		XnStatus RemoveRawNode(const XnChar* strNodeName);
		XnStatus SetRawNodeIntProp(const XnChar* strNodeName, const XnChar* strPropName, XnUInt64 nVal);
		XnStatus SetRawNodeRealProp(const XnChar* strNodeName, const XnChar* strPropName, XnDouble dVal);
		XnStatus SetRawNodeStringProp(const XnChar* strNodeName, const XnChar* strPropName, const XnChar* strVal);
		XnStatus SetRawNodeGeneralProp(const XnChar* strNodeName, const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer);
		XnStatus NotifyRawNodeStateReady(const XnChar* strNodeName);
		XnStatus SetRawNodeNewData(const XnChar* strNodeName, XnUInt64 nTimeStamp, XnUInt32 nFrame, const void* pData, XnUInt32 nSize);

		XnStatus SetDestination(XnRecordMedium destType, const XnChar* strDest);
		XnStatus GetDestination(XnRecordMedium& destType, XnChar* strDest, XnUInt32 nBufSize);
		XnStatus Record();

	protected:
		XnStatus NotifyNodeAdded(XnNodeHandle hNode, XnProductionNodeType type, XnCodecID compression);
		XnStatus NotifyNodeRemoved(XnNodeHandle hNode);

		XnNodeNotifications& Notifications();
		XnModuleRecorderInterface& ModuleRecorder();
		XnModuleNodeHandle ModuleHandle();
		XnStatus RemoveNodeImpl(ProductionNode &node);

	private:
		typedef XnHashT<XnNodeHandle, NodeWatcher*> NodeWatchersMap;

		struct RawNodeInfo
		{
		};

		typedef XnStringsHashT<RawNodeInfo> RawNodesMap;

		static XnStatus XN_CALLBACK_TYPE OpenFile(void* pCookie);
		static XnStatus XN_CALLBACK_TYPE WriteFile(void* pCookie, const XnChar* strNodeName, 
			const void* pData, XnUInt32 nSize);
		static XnStatus XN_CALLBACK_TYPE SeekFile  (void* pCookie, XnOSSeekType seekType, const XnInt32 nOffset);
		static XnStatus XN_CALLBACK_TYPE SeekFile64(void* pCookie, XnOSSeekType seekType, const XnInt64 nOffset);
		static XnUInt32 XN_CALLBACK_TYPE TellFile(void* pCookie);
		static XnUInt64 XN_CALLBACK_TYPE TellFile64(void* pCookie);
		static void XN_CALLBACK_TYPE CloseFile(void* pCookie);

		XnStatus OpenFileImpl();
		XnStatus WriteFileImpl(const XnChar* strNodeName, const void* pData, XnUInt32 nSize);
		XnStatus SeekFileImpl  (XnOSSeekType seekType, const XnInt32 nOffset);
		XnStatus SeekFile64Impl(XnOSSeekType seekType, const XnInt64 nOffset);
		XnUInt32 TellFileImpl();
		XnUInt64 TellFile64Impl();
		void CloseFileImpl();

		XnCodecID GetDefaultCodecID(ProductionNode& node);
		
		XnBool IsRawNode(const XnChar* strNodeName);

		//XnRecorderOutputStreamInterface implementation that writes to an stdio file
		static XnRecorderOutputStreamInterface s_fileOutputStream;

		XnRecordMedium m_destType;
		XnChar m_strFileName[XN_FILE_MAX_PATH];
		XnBool m_bIsFileOpen;
		XN_FILE_HANDLE m_hOutFile;
		XnNodeHandle m_hRecorder;
		NodeWatchersMap m_nodeWatchersMap;
		RawNodesMap m_rawNodesMap;
	};
}


#endif //__XN_RECORDER_IMPL_H__
