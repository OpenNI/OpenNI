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
#include "RecorderNode.h"
#include "DataRecords.h"
#include "XnPropNames.h"
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <XnCodecIDs.h>

const XnUInt32 RecorderNode::RECORD_MAX_SIZE = 20 * 1024;

/*PAYLOAD_DATA_SIZE is set to support a resolution of 1600x1200 with 24 bits per pixel with the worst case
  of compression.
*/
const XnUInt32 RecorderNode::PAYLOAD_DATA_SIZE = (XnUInt32)(1600 * 1200 * 3 * 1.2);

RecorderNode::RecorderNode(xn::Context &context) : 
	m_pStreamCookie(NULL),
	m_pOutputStream(NULL),
	m_bOpen(FALSE),
	m_pRecordBuffer(NULL),
	m_context(context),
	m_pPayloadData(NULL),
	m_nGlobalStartTimeStamp(XN_MAX_UINT64),
	m_nGlobalMaxTimeStamp(0),
	m_nNumNodes(0),
	m_nConfigurationID(0)
{
}

RecorderNode::~RecorderNode()
{
	Destroy();
}

XnStatus RecorderNode::Init()
{
	m_pRecordBuffer = XN_NEW_ARR(XnUInt8, RECORD_MAX_SIZE);
	XN_VALIDATE_ALLOC_PTR(m_pRecordBuffer);
	m_pPayloadData = XN_NEW_ARR(XnUInt8, PAYLOAD_DATA_SIZE);
	XN_VALIDATE_ALLOC_PTR(m_pPayloadData);
	return XN_STATUS_OK;
}

XnStatus RecorderNode::Destroy()
{
	CloseStream();
	//Don't verify return value - proceed anyway
	XN_DELETE_ARR(m_pRecordBuffer);
	m_pRecordBuffer = NULL;
	XN_DELETE_ARR(m_pPayloadData);
	m_pPayloadData = NULL;
	return XN_STATUS_OK;
}

XnStatus RecorderNode::SetOutputStream(void* pStreamCookie, XnRecorderOutputStreamInterface* pStream)
{
	m_pStreamCookie = pStreamCookie;
	m_pOutputStream = pStream;
	XnStatus nRetVal = OpenStream();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus RecorderNode::OnNodeAdded(const XnChar* strNodeName, XnProductionNodeType type, XnCodecID compression)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnUInt32 nNodeID = ++m_nNumNodes;

	m_nConfigurationID++;

	NodeAddedRecord nodeAddedRecord(m_pRecordBuffer, RECORD_MAX_SIZE, FALSE);
	nodeAddedRecord.SetNodeName(strNodeName);
	nodeAddedRecord.SetNodeType(type);
	nodeAddedRecord.SetCompression(compression);
	nodeAddedRecord.SetNodeID(nNodeID);
	nRetVal = nodeAddedRecord.Encode();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to encode Node Added nodeAddedRecord: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}

	XnUInt64 nNodeAddedPos = TellStream();
	nRetVal = WriteRecordToStream(strNodeName, nodeAddedRecord);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to write Node Added nodeAddedRecord to file: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}
	RecordedNodeInfo recordedNodeInfo;
	xn::ProductionNode node;
	recordedNodeInfo.nNodeID = nNodeID;
	recordedNodeInfo.type = type;
	recordedNodeInfo.compression = compression;
	recordedNodeInfo.nNodeAddedPos = nNodeAddedPos;

	if (xnIsTypeGenerator(type))
	{
		/* Create Codec for node */
		nRetVal = m_context.GetProductionNodeByName(strNodeName, node);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = m_context.CreateCodec(compression, node, recordedNodeInfo.codec);
		XN_IS_STATUS_OK(nRetVal);
	}

	/* Index recorded node info by name in hash */
	nRetVal = m_recordedNodesInfo.Set(strNodeName, recordedNodeInfo);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus RecorderNode::OnNodeRemoved(const XnChar* strNodeName)
{
	m_nConfigurationID++;

	XnStatus nRetVal = RemoveNode(strNodeName);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus RecorderNode::OnNodeIntPropChanged(const XnChar* strNodeName, const XnChar* strPropName, XnUInt64 nValue)
{
	m_nConfigurationID++;

	XnUInt64 nUndoRecordPos = 0;
	RecordedNodeInfo* pRecordedNodeInfo = NULL;
	XnStatus nRetVal = UpdateNodePropInfo(strNodeName, strPropName, pRecordedNodeInfo, nUndoRecordPos);
	XN_IS_STATUS_OK(nRetVal);

	IntPropRecord intPropRecord(m_pRecordBuffer, RECORD_MAX_SIZE, FALSE);
	intPropRecord.SetNodeID(pRecordedNodeInfo->nNodeID);
	intPropRecord.SetPropName(strPropName);
	intPropRecord.SetValue(nValue);
	intPropRecord.SetUndoRecordPos(nUndoRecordPos);

	nRetVal = intPropRecord.Encode();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to encode Int Property '%s': %s", strPropName, xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}

	nRetVal = WriteRecordToStream(strNodeName, intPropRecord);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to write Int Property '%s' to file: %s", strPropName, xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}
	return XN_STATUS_OK;
}

XnStatus RecorderNode::OnNodeRealPropChanged(const XnChar* strNodeName, const XnChar* strPropName, XnDouble dValue)
{
	m_nConfigurationID++;

	XnUInt64 nUndoRecordPos = 0;
	RecordedNodeInfo* pRecordedNodeInfo = NULL;
	XnStatus nRetVal = UpdateNodePropInfo(strNodeName, strPropName, pRecordedNodeInfo, nUndoRecordPos);
	XN_IS_STATUS_OK(nRetVal);

	RealPropRecord record(m_pRecordBuffer, RECORD_MAX_SIZE, FALSE);
	record.SetNodeID(pRecordedNodeInfo->nNodeID);
	record.SetPropName(strPropName);
	record.SetValue(dValue);
	record.SetUndoRecordPos(nUndoRecordPos);
	nRetVal = record.Encode();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to encode Real Property record: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}

	nRetVal = WriteRecordToStream(strNodeName, record);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to write Real Property record to file: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}
	return XN_STATUS_OK;
}

XnStatus RecorderNode::OnNodeStringPropChanged(const XnChar* strNodeName, const XnChar* strPropName, const XnChar* strValue)
{
	m_nConfigurationID++;

	XnUInt64 nUndoRecordPos = 0;
	RecordedNodeInfo* pRecordedNodeInfo = NULL;
	XnStatus nRetVal = UpdateNodePropInfo(strNodeName, strPropName, pRecordedNodeInfo, nUndoRecordPos);
	XN_IS_STATUS_OK(nRetVal);
	StringPropRecord record(m_pRecordBuffer, RECORD_MAX_SIZE, FALSE);
	record.SetNodeID(pRecordedNodeInfo->nNodeID);
	record.SetPropName(strPropName);
	record.SetValue(strValue);
	record.SetUndoRecordPos(nUndoRecordPos);
	nRetVal = record.Encode();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to encode String Property record: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}

	nRetVal = WriteRecordToStream(strNodeName, record);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to write String Property record to file: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}
	return XN_STATUS_OK;
}

XnStatus RecorderNode::OnNodeGeneralPropChanged(const XnChar* strNodeName, const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer)
{
	m_nConfigurationID++;

	RecordedNodeInfo* pRecordedNodeInfo = NULL;
	XnUInt64 nUndoRecordPos = 0;
	XnStatus nRetVal = UpdateNodePropInfo(strNodeName, strPropName, pRecordedNodeInfo, nUndoRecordPos);
	XN_IS_STATUS_OK(nRetVal);
	GeneralPropRecord record(m_pRecordBuffer, RECORD_MAX_SIZE, FALSE);
	record.SetNodeID(pRecordedNodeInfo->nNodeID);
	record.SetPropName(strPropName);
	record.SetPropData(pBuffer);
	record.SetPropDataSize(nBufferSize);
	record.SetUndoRecordPos(nUndoRecordPos);
	nRetVal = record.Encode();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to encode General Property record: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}

	nRetVal = WriteRecordToStream(strNodeName, record);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to write General Property to file: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}
	return XN_STATUS_OK;
}

XnStatus RecorderNode::OnNodeStateReady(const XnChar* strNodeName)
{
	m_nConfigurationID++;

	RecordedNodeInfo* pRecordedNodeInfo = GetRecordedNodeInfo(strNodeName);
	XN_VALIDATE_PTR(pRecordedNodeInfo, XN_STATUS_BAD_NODE_NAME);
	NodeStateReadyRecord record(m_pRecordBuffer, RECORD_MAX_SIZE, FALSE);
	record.SetNodeID(pRecordedNodeInfo->nNodeID);
	XnStatus nRetVal = record.Encode();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = WriteRecordToStream(strNodeName, record);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus RecorderNode::OnNodeNewData(const XnChar* strNodeName, XnUInt64 nTimeStamp, XnUInt32 /*nFrame*/, const void* pData, XnUInt32 nSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	//Find node info
	RecordedNodeInfo* pRecordedNodeInfo = GetRecordedNodeInfo(strNodeName);
	XN_VALIDATE_PTR(pRecordedNodeInfo, XN_STATUS_BAD_NODE_NAME);

	const void* pCompressedData = pData;
	XnUInt32 nCompressedSize = nSize;
	XnCodecID compression = pRecordedNodeInfo->compression;
	if (compression != XN_CODEC_UNCOMPRESSED)
	{
		if (!pRecordedNodeInfo->codec.IsValid())
		{
			xnLogWarning(XN_MASK_OPEN_NI, "Codec is not valid for node '%s'", strNodeName);
			XN_ASSERT(FALSE);
			return XN_STATUS_INVALID_OPERATION;
		}

		//Compress data
		nRetVal = pRecordedNodeInfo->codec.EncodeData(pData, nSize, m_pPayloadData, PAYLOAD_DATA_SIZE, &nCompressedSize);
		XN_IS_STATUS_OK(nRetVal);
		pCompressedData = m_pPayloadData;
	}

	// correct timestamp according to first data recorded
	if (m_nGlobalStartTimeStamp == XN_MAX_UINT64)
	{
		m_nGlobalStartTimeStamp = nTimeStamp;
	}

	if (nTimeStamp < m_nGlobalStartTimeStamp)
	{
		// can happen in the rare case where a node was added in the middle of recording, and this node has data
		// which is before the starting of the recording. In this case, we'll just ignore this data
		return XN_STATUS_OK;
	}
	else
	{
		nTimeStamp -= m_nGlobalStartTimeStamp;
	}

	if (!pRecordedNodeInfo->bGotData)
	{
		// write down the DataBegin record
		nRetVal = WriteNodeDataBegin(strNodeName);
		XN_IS_STATUS_OK(nRetVal);

		pRecordedNodeInfo->bGotData = TRUE;
		pRecordedNodeInfo->nMinTimeStamp = nTimeStamp;
	}

	pRecordedNodeInfo->nMaxTimeStamp = nTimeStamp;

	XnUInt64 nUndoRecordPos = 0;
	nRetVal = UpdateNodePropInfo(strNodeName, XN_PROP_NEWDATA, pRecordedNodeInfo, nUndoRecordPos);
	XN_IS_STATUS_OK(nRetVal);

	//Prepare data header
	NewDataRecordHeader recordHeader(m_pRecordBuffer, RECORD_MAX_SIZE, FALSE);
	recordHeader.SetNodeID(pRecordedNodeInfo->nNodeID);
	recordHeader.SetTimeStamp(nTimeStamp);
	recordHeader.SetFrameNumber(++pRecordedNodeInfo->nMaxFrameNum);
	recordHeader.SetPayloadSize(nCompressedSize);
	recordHeader.SetUndoRecordPos(nUndoRecordPos);
	nRetVal = recordHeader.Encode();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to encode New Data Header: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}

	DataIndexEntry dataIndexEntry;
	dataIndexEntry.nTimestamp = nTimeStamp;
	dataIndexEntry.nConfigurationID = m_nConfigurationID;
	dataIndexEntry.nSeekPos = TellStream();

	//First we write just the header
	nRetVal = WriteRecordToStream(strNodeName, recordHeader);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to write New Data Header to file: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}

	//Now write actual data
	nRetVal = WriteToStream(strNodeName, pCompressedData, nCompressedSize);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to write New Data to file: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}

	if (nTimeStamp > m_nGlobalMaxTimeStamp)
	{
		m_nGlobalMaxTimeStamp = nTimeStamp;
	}

	// write to seek table
	nRetVal = pRecordedNodeInfo->dataIndex.AddLast(dataIndexEntry);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus RecorderNode::OpenStream()
{
	XN_VALIDATE_INPUT_PTR(m_pOutputStream);
	XnStatus nRetVal = m_pOutputStream->Open(m_pStreamCookie);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = WriteHeader(INVALID_TIMESTAMP, INVALID_NODE_ID); //Write invalid values to mark the file is not finalized
	XN_IS_STATUS_OK(nRetVal);
	m_bOpen = TRUE;
	return XN_STATUS_OK;
}

XnStatus RecorderNode::WriteHeader(XnUInt64 nGlobalMaxTimeStamp, XnUInt32 nMaxNodeID)
{
	RecordingHeader recordingHeader = DEFAULT_RECORDING_HEADER;
	recordingHeader.nGlobalMaxTimeStamp = nGlobalMaxTimeStamp;
	recordingHeader.nMaxNodeID = nMaxNodeID;
	XnStatus nRetVal = WriteToStream(NULL, &recordingHeader, sizeof(recordingHeader));
	XN_IS_STATUS_OK(nRetVal);
	return nRetVal;
}

XnStatus RecorderNode::WriteToStream(const XnChar* strNodeName, const void* pData, XnUInt32 nSize)
{
	XN_VALIDATE_INPUT_PTR(m_pOutputStream);
	return m_pOutputStream->Write(m_pStreamCookie, strNodeName, pData, nSize);
}

XnStatus RecorderNode::WriteRecordToStream(const XnChar* strNodeName, Record &record)
{
	return WriteToStream(strNodeName, record.GetData(), record.GetSize());
}

XnStatus RecorderNode::SeekStream(XnOSSeekType seekType, XnUInt64 nOffset)
{
	XN_VALIDATE_INPUT_PTR(m_pOutputStream);
	XN_VALIDATE_INPUT_PTR(m_pOutputStream->Seek64);
	return m_pOutputStream->Seek64(m_pStreamCookie, seekType, nOffset);
}

XnUInt64 RecorderNode::TellStream()
{
	XN_VALIDATE_INPUT_PTR(m_pOutputStream);
	XN_VALIDATE_INPUT_PTR(m_pOutputStream->Tell64);
	return m_pOutputStream->Tell64(m_pStreamCookie);
}

XnStatus RecorderNode::FinalizeStream()
{
	XN_VALIDATE_INPUT_PTR(m_pOutputStream);
	EndRecord endRecord(m_pRecordBuffer, RECORD_MAX_SIZE, FALSE);
	XnStatus nRetVal = endRecord.Encode();
	XN_IS_STATUS_OK(nRetVal);
	/* Write End Record */
	nRetVal = WriteRecordToStream(NULL, endRecord);
	XN_IS_STATUS_OK(nRetVal);

	/* Remove all nodes in the map. When each node is removed, we write its number of frames and max time stamp */
	RecordedNodesInfo::ConstIterator it = m_recordedNodesInfo.Begin();
	while (it != m_recordedNodesInfo.End())
	{
		RecordedNodesInfo::ConstIterator curr = it;
		++it;
		nRetVal = RemoveNode(curr->Key());
		XN_IS_STATUS_OK(nRetVal);
	}

	/* Re-write header with correct max timestamp*/
	nRetVal = SeekStream(XN_OS_SEEK_SET, 0);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = WriteHeader(m_nGlobalMaxTimeStamp, m_nNumNodes);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus RecorderNode::CloseStream()
{
	XnStatus nRetVal = XN_STATUS_OK;
	if (m_bOpen)
	{
		XN_VALIDATE_INPUT_PTR(m_pOutputStream);
		nRetVal = FinalizeStream();
		XN_IS_STATUS_OK(nRetVal);
		m_pOutputStream->Close(m_pStreamCookie);
		m_bOpen = FALSE;
	}
	return XN_STATUS_OK;
}

XnStatus RecorderNode::WriteNodeDataBegin(const XnChar* strNodeName)
{
	RecordedNodeInfo* pRecordedNodeInfo = GetRecordedNodeInfo(strNodeName);
	XN_VALIDATE_PTR(pRecordedNodeInfo, XN_STATUS_BAD_NODE_NAME);
	NodeDataBeginRecord record(m_pRecordBuffer, RECORD_MAX_SIZE, FALSE);
	record.SetNodeID(pRecordedNodeInfo->nNodeID);
	XnStatus nRetVal = record.Encode();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = WriteRecordToStream(strNodeName, record);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus RecorderNode::UpdateNodeSeekInfo(const XnChar* strNodeName, const RecordedNodeInfo& recordedNodeInfo)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (recordedNodeInfo.bGotData)
	{
		XnUInt64 nSeekTablePos = 0;

		nSeekTablePos = TellStream();

		// write seek table
		DataIndexRecordHeader seekTableHeader(m_pRecordBuffer, RECORD_MAX_SIZE, FALSE);
		seekTableHeader.SetNodeID(recordedNodeInfo.nNodeID);
		seekTableHeader.SetPayloadSize((recordedNodeInfo.nMaxFrameNum+1) * sizeof(DataIndexEntry));
		nRetVal = seekTableHeader.Encode();
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = WriteRecordToStream(strNodeName, seekTableHeader);
		XN_IS_STATUS_OK(nRetVal);

		// write an empty entry for frame 0 (frames start with 1)
		DataIndexEntry* pPayload = (DataIndexEntry*)m_pPayloadData;
		xnOSMemSet(pPayload, 0, sizeof(DataIndexEntry));
		pPayload++;

		// now write the table itself
		for (DataIndexEntryList::ConstIterator it = recordedNodeInfo.dataIndex.Begin(); it != recordedNodeInfo.dataIndex.End(); ++it, ++pPayload)
		{
			*pPayload = *it;
		}
		XN_ASSERT((recordedNodeInfo.nMaxFrameNum+1) == XnUInt32(pPayload - (DataIndexEntry*)m_pPayloadData));

		nRetVal = WriteToStream(strNodeName, m_pPayloadData, (XnUInt32)((XnUInt8*)pPayload - m_pPayloadData));
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogWarning(XN_MASK_OPEN_NI, "Failed to write Seek Table to file: %s", xnGetStatusString(nRetVal));
			XN_ASSERT(FALSE);
			return nRetVal;
		}

		XnUInt64 nStartPos = TellStream();

		//Seek to position of node added record
		nRetVal = SeekStream(XN_OS_SEEK_SET, recordedNodeInfo.nNodeAddedPos);
		XN_IS_STATUS_OK(nRetVal);

		// re-write this record, this time with seek data
		NodeAddedRecord record(m_pRecordBuffer, RECORD_MAX_SIZE, FALSE);
		record.SetNodeID(recordedNodeInfo.nNodeID);
		record.SetNodeName(strNodeName);
		record.SetCompression(recordedNodeInfo.compression);
		record.SetNodeType(recordedNodeInfo.type);
		record.SetNumberOfFrames(recordedNodeInfo.nMaxFrameNum);
		record.SetMinTimestamp(recordedNodeInfo.nMinTimeStamp);
		record.SetMaxTimestamp(recordedNodeInfo.nMaxTimeStamp);
		record.SetSeekTablePosition(nSeekTablePos);

		nRetVal = record.Encode();
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = WriteRecordToStream(strNodeName, record);
		XN_IS_STATUS_OK(nRetVal);

		// and seek back
		nRetVal = SeekStream(XN_OS_SEEK_SET, nStartPos);
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
}


XnStatus RecorderNode::RemoveNode(const XnChar* strNodeName)
{
	RecordedNodeInfo recordedNodeInfo;
	XnChar strNodeNameCopy[XN_MAX_NAME_LENGTH];
	//We copy the node name cuz when we'll remove it from the hash it will be freed, but we need it later.
	XnStatus nRetVal = xnOSStrCopy(strNodeNameCopy, strNodeName, sizeof(strNodeNameCopy));
	XN_IS_STATUS_OK(nRetVal);

	RecordedNodesInfo::ConstIterator it = m_recordedNodesInfo.Find(strNodeName);
	if (it == m_recordedNodesInfo.End())
	{
		return XN_STATUS_NO_MATCH;
	}

	recordedNodeInfo = it->Value();

	nRetVal = m_recordedNodesInfo.Remove(it);
	XN_IS_STATUS_OK(nRetVal);

	NodeRemovedRecord record(m_pRecordBuffer, RECORD_MAX_SIZE, FALSE);
	record.SetNodeID(recordedNodeInfo.nNodeID);
	record.SetUndoRecordPos(recordedNodeInfo.nNodeAddedPos);

	nRetVal = record.Encode();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to encode Node Removed record: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}

	nRetVal = WriteRecordToStream(strNodeNameCopy, record);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Failed to write Node Removed record to file: %s", xnGetStatusString(nRetVal));
		XN_ASSERT(FALSE);
		return nRetVal;
	}

	nRetVal = UpdateNodeSeekInfo(strNodeNameCopy, recordedNodeInfo);
	XN_IS_STATUS_OK(nRetVal);

	recordedNodeInfo.codec.Release();
	return XN_STATUS_OK;
}


XnStatus RecorderNode::UpdateNodePropInfo(const XnChar* strNodeName, const XnChar* strPropName, 
										  RecordedNodeInfo*& pRecordedNodeInfo, XnUInt64& nUndoPos)
{
	XnStatus nRetVal = m_recordedNodesInfo.Get(strNodeName, pRecordedNodeInfo);
	XN_IS_STATUS_OK(nRetVal);
	RecordedNodePropInfoMap& propInfoMap = pRecordedNodeInfo->propInfoMap;
	RecordedNodePropInfo propInfo;
	propInfoMap.Get(strPropName, propInfo);
	nUndoPos = propInfo.nPos;
	propInfo.nPos = TellStream();
	nRetVal = propInfoMap.Set(strPropName, propInfo);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

RecorderNode::RecordedNodeInfo* RecorderNode::GetRecordedNodeInfo(const XnChar* strNodeName)
{
	RecordedNodeInfo* pRecordedNodeInfo = NULL;
	return (m_recordedNodesInfo.Get(strNodeName, pRecordedNodeInfo) == XN_STATUS_OK) ? pRecordedNodeInfo : NULL;
}

RecorderNode::RecordedNodeInfo::RecordedNodeInfo()
{
	Reset();
}

void RecorderNode::RecordedNodeInfo::Reset()
{
	nNodeID = 0;
	type = (XnProductionNodeType)-1;
	nMaxFrameNum = 0;
	nCurFrameNum = 0;
	nMinTimeStamp = 0;
	nMaxTimeStamp = 0; 
	nNodeAddedPos = 0;
	bGotData = FALSE;
	compression = XN_CODEC_NULL;
	propInfoMap.Clear();
	dataIndex.Clear();
}
