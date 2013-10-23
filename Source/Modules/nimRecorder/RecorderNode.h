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
#ifndef __RECORDER_NODE_H__
#define __RECORDER_NODE_H__

#include <XnModuleCppInterface.h>
#include <XnStringsHashT.h>
#include <DataRecords.h>

class Record;

class RecorderNode : public xn::ModuleRecorder
{
public:
	RecorderNode(xn::Context &context);
	virtual ~RecorderNode();
	
	//public functions
	virtual XnStatus Init();
	virtual XnStatus Destroy();

	//xn::ModuleRecorder implementation
	virtual XnStatus SetOutputStream(void* pStreamToken, XnRecorderOutputStreamInterface* pStream);
	virtual XnStatus OnNodeAdded(const XnChar* strNodeName, XnProductionNodeType type, XnCodecID compression);
	virtual XnStatus OnNodeRemoved(const XnChar* strNodeName);
	virtual XnStatus OnNodeIntPropChanged(const XnChar* strNodeName, const XnChar* strPropName, XnUInt64 nValue);
	virtual XnStatus OnNodeRealPropChanged(const XnChar* strNodeName, const XnChar* strPropName, XnDouble dValue);
	virtual XnStatus OnNodeStringPropChanged(const XnChar* strNodeName, const XnChar* strPropName, const XnChar* strValue);
	virtual XnStatus OnNodeStateReady(const XnChar* strNodeName);
	virtual XnStatus OnNodeGeneralPropChanged(const XnChar* strNodeName, const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer);
	virtual XnStatus OnNodeNewData(const XnChar* strNodeName, XnUInt64 nTimeStamp, XnUInt32 nFrame, const void* pData, XnUInt32 nSize);

private:
	struct RecordedNodePropInfo
	{
		RecordedNodePropInfo() { Reset(); }
		void Reset()
		{
			nPos = 0;
		}

		XnUInt64 nPos;		//Position in stream of record that did this property change
	};

	typedef XnStringsHashT<RecordedNodePropInfo> RecordedNodePropInfoMap;
	typedef XnListT<DataIndexEntry> DataIndexEntryList;

	struct RecordedNodeInfo
	{
		RecordedNodeInfo();
		void Reset();

		XnUInt32 nNodeID;
		XnProductionNodeType type;
		XnUInt64 nNodeAddedPos;
		XnUInt32 nMaxFrameNum;
		XnUInt32 nCurFrameNum;
		XnUInt64 nMinTimeStamp;
		XnUInt64 nMaxTimeStamp;
		XnBool bGotData;
		XnCodecID compression;
		xn::Codec codec;
		RecordedNodePropInfoMap propInfoMap;
		DataIndexEntryList dataIndex;
	};

	typedef XnStringsHashT<RecordedNodeInfo> RecordedNodesInfo;

	XnStatus OpenStream();
	XnStatus WriteHeader(XnUInt64 nGlobalMaxTimeStamp, XnUInt32 nMaxNodeID);
	XnStatus WriteToStream(const XnChar* strNodeName, const void* pData, XnUInt32 nSize);
	XnStatus WriteRecordToStream(const XnChar* strNodeName, Record &record);
	XnStatus SeekStream(XnOSSeekType seekType, XnUInt64 nOffset);
	XnUInt64 TellStream();
	XnStatus FinalizeStream();
	XnStatus CloseStream();
	XnStatus WriteNodeDataBegin(const XnChar* strNodeName);
	XnStatus UpdateNodeSeekInfo(const XnChar* strNodeName, const RecordedNodeInfo& recordedNodeInfo);
	XnStatus RemoveNode(const XnChar* strNodeName);
	
	//UpdateNodePropInfo() returns, in nUndoPos, the position in the file you should read to undo the property update.
	XnStatus UpdateNodePropInfo(const XnChar* strNodeName, const XnChar* strPropName, RecordedNodeInfo*& pRecordedNodeInfo, XnUInt64& nUndoPos);
	RecordedNodeInfo* GetRecordedNodeInfo(const XnChar* strNodeName);

	static const XnUInt32 RECORD_MAX_SIZE;
	static const XnUInt32 PAYLOAD_DATA_SIZE;
	XnBool m_bOpen;
	XnUInt8* m_pRecordBuffer;
	XnUInt8* m_pPayloadData;
	void* m_pStreamCookie;
	XnRecorderOutputStreamInterface* m_pOutputStream;

	RecordedNodesInfo m_recordedNodesInfo;
	xn::Context m_context;
	XnUInt64 m_nGlobalStartTimeStamp;
	XnUInt64 m_nGlobalMaxTimeStamp;
	XnUInt32 m_nNumNodes;
	XnUInt32 m_nConfigurationID;
};

#endif //__RECORDER_NODE_H__
