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
#ifndef __PLAYER_NODE_H__
#define __PLAYER_NODE_H__

#include <XnModuleCppInterface.h>
#include <XnCppWrapper.h>
#include <XnTypes.h>
#include <XnEvent.h>
#include <XnStringsHash.h>
#include "DataRecords.h"
#include <XnCodecIDs.h>

class PlayerNode : public xn::ModulePlayer
{
public:
	PlayerNode(xn::Context &context, const XnChar* strName);
	virtual ~PlayerNode();

	//public functions
	virtual XnStatus Init();
	virtual XnStatus Destroy();

	//xn::ModulePlayer implementation
	virtual XnStatus SetInputStream(void* pStreamCookie, XnPlayerInputStreamInterface* pStream);
	virtual XnStatus ReadNext();
	virtual XnStatus SetNodeNotifications(void* pNotificationsCookie, XnNodeNotifications* pNodeNotifications);
	virtual XnStatus SetRepeat(XnBool bRepeat);
	virtual XnStatus SeekToTimeStamp(XnInt64 nTimeOffset, XnPlayerSeekOrigin origin);

	virtual XnStatus SeekToFrame(const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin);
	virtual XnStatus TellTimestamp(XnUInt64& nTimestamp);
	virtual XnStatus TellFrame(const XnChar* strNodeName, XnUInt32& nFrameNumber);
	virtual XnUInt32 GetNumFrames(const XnChar* strNodeName, XnUInt32& nFrames);

	virtual const XnChar* GetSupportedFormat();
	virtual XnBool IsEOF();
	virtual XnStatus RegisterToEndOfFileReached(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromEndOfFileReached(XnCallbackHandle hCallback);

private:
	struct RecordUndoInfo
	{
		RecordUndoInfo() { Reset(); }
		void Reset() { nRecordPos = 0; nUndoRecordPos = 0; }
		XnUInt32 nRecordPos;
		XnUInt32 nUndoRecordPos;
	};

	XN_DECLARE_STRINGS_HASH(RecordUndoInfo, RecordUndoInfoMap);

	struct PlayerNodeInfo
	{
		PlayerNodeInfo();
		~PlayerNodeInfo();

		void Reset();

		XnBool bValid;
		XnChar strName[XN_MAX_NAME_LENGTH];
		XnUInt32 nLastDataPos;
		XnCodecID compression;
		XnUInt32 nFrames;
		XnUInt32 nCurFrame;
		XnUInt64 nMaxTimeStamp;
		XnBool bStateReady;
		XnBool bIsGenerator;
		xn::Codec codec;
		RecordUndoInfoMap recordUndoInfoMap;
		RecordUndoInfo newDataUndoInfo;
		DataIndexEntry* pDataIndex;
	};

	XnStatus ProcessRecord(XnBool bProcessPayload);
	XnStatus SeekToTimeStampAbsolute(XnUInt64 nDestTimeStamp);
	XnStatus SeekToTimeStampRelative(XnInt64 nOffset);
	XnStatus UndoRecord(PlayerNode::RecordUndoInfo& undoInfo, XnUInt32 nDestPos, XnBool& nUndone);
	XnStatus SeekToFrameAbsolute(XnUInt32 nNodeID, XnUInt32 nFrameNumber);
	XnStatus ProcessEachNodeLastData(XnUInt32 nIDToProcessLast);

	XnStatus OpenStream();
	XnStatus Read(void* pData, XnUInt32 nSize, XnUInt32& nBytesRead);
	XnStatus ReadRecordHeader(Record& record);
	XnStatus ReadRecordFields(Record& record);
	//ReadRecord reads just the fields of the record, not the payload.
	XnStatus ReadRecord(Record& record);
	XnStatus SeekStream(XnOSSeekType seekType, XnInt32 nOffset);
	XnUInt32 TellStream();
	XnStatus CloseStream();

	XnStatus HandleRecord(Record& record, XnBool bHandleRecord);
	XnStatus HandleNodeAddedImpl(XnUInt32 nNodeID, XnProductionNodeType type, const XnChar* strName, XnCodecID compression, XnUInt32 nNumberOfFrames, XnUInt64 nMinTimestamp, XnUInt64 nMaxTimestamp);
	XnStatus HandleNodeAddedRecord(NodeAddedRecord record);
	XnStatus HandleGeneralPropRecord(GeneralPropRecord record);
	XnStatus HandleIntPropRecord(IntPropRecord record);
	XnStatus HandleRealPropRecord(RealPropRecord record);
	XnStatus HandleStringPropRecord(StringPropRecord record);
	XnStatus HandleNodeRemovedRecord(NodeRemovedRecord record);
	XnStatus HandleNodeStateReadyRecord(NodeStateReadyRecord record);
	XnStatus HandleNodeDataBeginRecord(NodeDataBeginRecord record);
	XnStatus HandleNewDataRecord(NewDataRecordHeader record, XnBool bHandleRecord);
	XnStatus HandleDataIndexRecord(DataIndexRecordHeader record, XnBool bReadPayload);
	XnStatus HandleEndRecord(EndRecord record);
	XnStatus Rewind();
	XnStatus ProcessUntilFirstData();
	PlayerNodeInfo* GetPlayerNodeInfo(XnUInt32 nNodeID);
	XnStatus RemovePlayerNodeInfo(XnUInt32 nNodeID);
	XnUInt32 GetPlayerNodeIDByName(const XnChar* strNodeName);
	PlayerNodeInfo* GetPlayerNodeInfoByName(const XnChar* strNodeName);
	XnStatus SaveRecordUndoInfo(PlayerNodeInfo* pPlayerNodeInfo, const XnChar* strPropName, XnUInt32 nRecordPos, XnUInt32 nUndoRecordPos);
	XnStatus GetRecordUndoInfo(PlayerNodeInfo* pPlayerNodeInfo, const XnChar* strPropName, XnUInt32& nRecordPos, XnUInt32& nUndoRecordPos);
	XnStatus SkipRecordPayload(Record record);
	XnStatus SeekToRecordByType(XnUInt32 nNodeID, RecordType type);
	DataIndexEntry* FindTimestampInDataIndex(XnUInt32 nNodeID, XnUInt64 nTimestamp);
	DataIndexEntry** GetSeekLocationsFromDataIndex(XnUInt32 nNodeID, XnUInt32 nDestFrame);
	XnNodeHandle GetSelfNodeHandle();

	// BC functions
	XnStatus HandleNodeAdded_1_0_0_5_Record(NodeAdded_1_0_0_5_Record record);
	XnStatus HandleNodeAdded_1_0_0_4_Record(NodeAdded_1_0_0_4_Record record);

	static const XnUInt64 DATA_MAX_SIZE;
	static const XnUInt64 RECORD_MAX_SIZE;
	static const XnVersion OLDEST_SUPPORTED_FILE_FORMAT_VERSION;

	XnChar m_strName[XN_MAX_NAME_LENGTH];
	XnBool m_bOpen;
	XnUInt8* m_pRecordBuffer;
	XnUInt8* m_pUncompressedData;
	void* m_pStreamCookie;
	XnPlayerInputStreamInterface* m_pInputStream;
	void* m_pNotificationsCookie;
	XnNodeNotifications* m_pNodeNotifications;
	XnBool m_bRepeat;
	XnBool m_bDataBegun;
	XnBool m_bEOF;
	
	XnUInt64 m_nTimeStamp;
	XnUInt64 m_nGlobalMaxTimeStamp;

	XN_DECLARE_EVENT_0ARG(EndOfFileReachedEvent, IEndOfFileReachedEvent);
	EndOfFileReachedEvent m_eofReachedEvent;

	PlayerNodeInfo* m_pNodeInfoMap;
	XnUInt32 m_nMaxNodes;
	xn::Context m_context;
	XnNodeHandle m_hSelf;

	DataIndexEntry** m_aSeekTempArray;
};


#endif //__PLAYER_NODE_H__
