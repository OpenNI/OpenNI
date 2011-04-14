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
#ifndef __DATA_RECORDS_H__
#define __DATA_RECORDS_H__

#include <XnPlatform.h>
#include <XnStatus.h>
#include <XnTypes.h>

#pragma pack(push, 1)

#define HEADER_MAGIC_SIZE 4

enum RecordType
{
	RECORD_NODE_ADDED_1_0_0_4		= 0x02,
	RECORD_INT_PROPERTY				= 0x03,
	RECORD_REAL_PROPERTY			= 0x04,
	RECORD_STRING_PROPERTY			= 0x05,
	RECORD_GENERAL_PROPERTY			= 0x06,
	RECORD_NODE_REMOVED				= 0x07,
	RECORD_NODE_DATA_BEGIN			= 0x08,
	RECORD_NODE_STATE_READY			= 0x09,
	RECORD_NEW_DATA					= 0x0A,
	RECORD_END						= 0x0B,
	RECORD_NODE_ADDED_1_0_0_5		= 0x0C,
	RECORD_NODE_ADDED				= 0x0D,
	RECORD_SEEK_TABLE               = 0x0E,
};

enum {INVALID_NODE_ID = -1};
enum {INVALID_TIMESTAMP = -1};

struct RecordingHeader
{
	XnChar headerMagic[HEADER_MAGIC_SIZE];
	XnVersion version;
	XnUInt64 nGlobalMaxTimeStamp;
	XnUInt32 nMaxNodeID;
};

extern const RecordingHeader DEFAULT_RECORDING_HEADER;

class Record
{
public:
	Record(XnUInt8* pData, XnUInt32 nMaxSize);
	Record(const Record &other);
	XnUInt32 GetType() const;
	XnUInt32 GetNodeID() const;
	XnUInt32 GetSize() const; //GetSize() returns just the fields' size, not including the payload
	XnUInt32 GetPayloadSize() const;
	XnUInt32 GetUndoRecordPos() const;

	void SetNodeID(XnUInt32 nNodeID);
	void SetPayloadSize(XnUInt32 nPayloadSize);
	void SetUndoRecordPos(XnUInt32 nUndoRecordPos);

	XnUInt8* GetData(); //GetData() returns the entire encoded record
	const XnUInt8* GetData() const; //GetData() returns the entire encoded record
	void SetData(XnUInt8* pData, XnUInt32 nMaxSize);
	XnUInt8* GetPayload(); //GetPayload() returns just the payload part of the record (after the fields)
	const XnUInt8* GetPayload() const;
	const XnUInt8* GetReadPos() const;
	XnBool IsHeaderValid() const;
	void ResetRead();
	
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);

protected:
	const XnUInt8* GetWritePos() const;
	XnStatus StartWrite(XnUInt32 nRecordType);
	XnStatus Write(const void* pData, XnUInt32 nSize);
	XnStatus WriteString(const XnChar* str);
	XnStatus FinishWrite();
	
	XnStatus StartRead();

	//Copies the data to the specified output buffer
	XnStatus Read(void* pData, XnUInt32 nSize) const;
	
	//Gives a pointer to the string in the current position, and advances that position till after the string.
	XnStatus ReadString(const XnChar*& strDest) const;
	XnStatus FinishRead();

private:
	struct Header
	{
		XnUInt32 m_nMagic;
		XnUInt32 m_nRecordType;
		XnUInt32 m_nNodeID;
		XnUInt32 m_nFieldsSize;
		XnUInt32 m_nPayloadSize;
		XnUInt32 m_nUndoRecordPos;
	};

	union
	{
		Header* m_pHeader;
		XnUInt8* m_pData;
	};

	static const XnUInt32 MAGIC;
	mutable XnUInt32 m_nReadOffset;
	XnUInt32 m_nMaxSize;

public:
	enum {HEADER_SIZE = sizeof(Header)};
};

class NodeAdded_1_0_0_4_Record : public Record
{
public:
	NodeAdded_1_0_0_4_Record(XnUInt8* pData, XnUInt32 nMaxSize);
	NodeAdded_1_0_0_4_Record(const Record& record);
	void SetNodeName(const XnChar* strNodeName);
	void SetNodeType(XnProductionNodeType type);
	void SetCompression(XnCodecID compression);

	const XnChar* GetNodeName() const;
	XnProductionNodeType GetNodeType() const;
	XnCodecID GetCompression() const;

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);

protected:
	XnStatus EncodeImpl();
	XnStatus DecodeImpl();

private:
	const XnChar* m_strNodeName;
	XnProductionNodeType m_type;
	XnCodecID m_compression;
};

class NodeAdded_1_0_0_5_Record : public NodeAdded_1_0_0_4_Record
{
public:
	NodeAdded_1_0_0_5_Record(XnUInt8* pData, XnUInt32 nMaxSize);
	NodeAdded_1_0_0_5_Record(const Record& record);

	void SetNumberOfFrames(XnUInt32 nNumberOfFrames);
	void SetMinTimestamp(XnUInt64 nMinTimestamp);
	void SetMaxTimestamp(XnUInt64 nMaxTimestamp);

	XnUInt32 GetNumberOfFrames() const;
	XnUInt64 GetMinTimestamp() const;
	XnUInt64 GetMaxTimestamp() const;

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);

protected:
	XnStatus EncodeImpl();
	XnStatus DecodeImpl();

private:
	XnUInt32 m_nNumberOfFrames;
	XnUInt64 m_nMinTimestamp;
	XnUInt64 m_nMaxTimestamp;
};

class NodeAddedRecord : public NodeAdded_1_0_0_5_Record
{
public:
	NodeAddedRecord(XnUInt8* pData, XnUInt32 nMaxSize);
	NodeAddedRecord(const Record& record);

	void SetSeekTablePosition(XnUInt32 nPos);

	XnUInt32 GetSeekTablePosition();

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);

private:
	XnUInt32 m_nSeekTablePosition;
};

class NodeRemovedRecord : public Record
{
public:
	NodeRemovedRecord(XnUInt8* pData, XnUInt32 nMaxSize);
	NodeRemovedRecord(const Record& record);

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);
};

class GeneralPropRecord : public Record
{
public:
	GeneralPropRecord(XnUInt8* pData, XnUInt32 nMaxSize, XnUInt32 nPropRecordType = RECORD_GENERAL_PROPERTY);
	GeneralPropRecord(const Record& record);

	void SetPropName(const XnChar* strPropName);
	void SetPropDataSize(XnUInt32 nPropDataSize);
	void SetPropData(const void* pPropData);

	const XnChar* GetPropName() const;
	XnUInt32 GetPropDataSize() const;
	const void* GetPropData() const;

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);
private:
	XnUInt32 m_nPropRecordType;
	const XnChar* m_strPropName;
	XnUInt32 m_nPropDataSize;
	void* m_pPropData;
};

class IntPropRecord : public GeneralPropRecord
{
public:
	IntPropRecord(XnUInt8* pData, XnUInt32 nMaxSize);
	IntPropRecord(const Record& record);
	void SetValue(XnUInt64 nValue);
	XnUInt64 GetValue() const;
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);

private:
	XnUInt64 m_nValue;
};

class RealPropRecord : public GeneralPropRecord
{
public:
	RealPropRecord(XnUInt8* pData, XnUInt32 nMaxSize);
	RealPropRecord(const Record& record);
	void SetValue(XnDouble dValue);
	XnDouble GetValue() const;
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);

private:
	XnDouble m_dValue;
};

class StringPropRecord : public GeneralPropRecord
{
public:
	StringPropRecord(XnUInt8* pData, XnUInt32 nMaxSize);
	StringPropRecord(const Record& record);
	void SetValue(const XnChar* strValue);
	const XnChar* GetValue() const;
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);
};

struct SeekInfo
{
	XnUInt32 m_nFrames;
	XnUInt64 m_nMaxTimeStamp;
};

class NodeDataBeginRecord : public Record
{
public:
	NodeDataBeginRecord(XnUInt8* pData, XnUInt32 nMaxSize);
	NodeDataBeginRecord(const Record& record);

	XnUInt32 GetNumFrames() const;
	XnUInt64 GetMaxTimeStamp() const;

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);

private:
	SeekInfo m_seekInfo;
};

class NodeStateReadyRecord : public Record
{
public:
	NodeStateReadyRecord(XnUInt8* pData, XnUInt32 nMaxSize);
	NodeStateReadyRecord(const Record& record);
	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);
};

/*This class represents only the data record HEADER, not the data itself.
  This is because we want to be able to read/write the data directly without copying it.*/
class NewDataRecordHeader : public Record
{
public:
	enum {MAX_SIZE = Record::HEADER_SIZE + //Record header
	             (XN_MAX_NAME_LENGTH + 1) + //Max node name + terminating null
	             sizeof(XnUInt64) + //Data timestamp
				 sizeof(XnUInt32)}; //Frame number

	NewDataRecordHeader(XnUInt8* pData, XnUInt32 nMaxSize);
	NewDataRecordHeader(const Record& record);
	void SetTimeStamp(XnUInt64 nTimeStamp);
	void SetFrameNumber(XnUInt32 nFrameNumber);

	XnUInt64 GetTimeStamp() const;
	XnUInt32 GetFrameNumber() const;

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);

private:
	XnUInt64 m_nTimeStamp;
	XnUInt32 m_nFrameNumber;
};

typedef struct
{
	XnUInt64 nTimestamp;
	XnUInt32 nConfigurationID;
	XnUInt32 nSeekPos;
} DataIndexEntry;

class DataIndexRecordHeader : public Record
{
public:
	DataIndexRecordHeader(XnUInt8* pData, XnUInt32 nMaxSize);
	DataIndexRecordHeader(const Record& record);

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);
};

class EndRecord : public Record
{
public:
	EndRecord(XnUInt8* pData, XnUInt32 nMaxSize);
	EndRecord(const Record& record);
	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten);
};
#pragma pack(pop)

#endif //__DATA_RECORDS_H__