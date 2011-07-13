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
#ifndef __XN_RECORDER_IMPL_H__
#define __XN_RECORDER_IMPL_H__

#include "XnStatus.h"
#include "XnInternalTypes.h"
#include <XnTypes.h>
#include <XnHash.h>

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
		static XnStatus XN_CALLBACK_TYPE OpenFile(void* pCookie);
		static XnStatus XN_CALLBACK_TYPE WriteFile(void* pCookie, const XnChar* strNodeName, 
			const void* pData, XnUInt32 nSize);
		static XnStatus XN_CALLBACK_TYPE SeekFile(void* pCookie, XnOSSeekType seekType, const XnUInt32 nOffset);
		static XnUInt32 XN_CALLBACK_TYPE TellFile(void* pCookie);
		static void XN_CALLBACK_TYPE CloseFile(void* pCookie);

		XnStatus OpenFileImpl();
		XnStatus WriteFileImpl(const XnChar* strNodeName, const void* pData, XnUInt32 nSize);
		XnStatus SeekFileImpl(XnOSSeekType seekType, const XnUInt32 nOffset);
		XnUInt32 TellFileImpl();
		void CloseFileImpl();

		XnCodecID GetDefaultCodecID(ProductionNode& node);
		
		XnBool IsRawNode(const XnChar* strNodeName);

		//XnRecorderOutputStreamInterface implementation that writes to an stdio file
		static XnRecorderOutputStreamInterface s_fileOutputStream;

		XnRecordMedium m_destType;
		XnChar m_strFileName[XN_FILE_MAX_PATH];
		FILE* m_pOutFile;
		XnNodeHandle m_hRecorder;
		XN_DECLARE_DEFAULT_HASH(XnNodeHandle, NodeWatcher*, NodeWatchersMap);
		NodeWatchersMap m_nodeWatchersMap;

		struct RawNodeInfo
		{
		};

		XN_DECLARE_STRINGS_HASH(RawNodeInfo, RawNodesMap);
		RawNodesMap m_rawNodesMap;
	};
}


#endif //__XN_RECORDER_IMPL_H__
