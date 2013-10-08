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
#include "DataRecords.h"
#include <XnLog.h>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>

const RecordingHeader DEFAULT_RECORDING_HEADER = 
{
	{'N','I','1','0'}, //Magic
	{1, 0, 1, 0}, //Version
	0, //Global max timestamp
	0 //Max node id
};

const XnUInt32 Record::MAGIC = 0x0052494E; //It reads "NIR\0"

Record::Record(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) : 
	m_pData(pData),
	m_nMaxSize(nMaxSize),
	m_nReadOffset(0),
	m_bUseOld32Header(bUseOld32Header),
	HEADER_SIZE(bUseOld32Header ? HEADER_SIZE_old32 : HEADER_SIZE_current)
{
	XN_ASSERT(m_pData != NULL);
	XN_ASSERT(m_nMaxSize >= HEADER_SIZE);
	SetNodeID(INVALID_NODE_ID);
	SetPayloadSize(0);
	SetUndoRecordPos(0);
}

Record::Record(const Record &other) :
	m_pData(other.m_pData),
	m_nMaxSize(other.m_nMaxSize),
	m_nReadOffset(other.m_nReadOffset),
	m_bUseOld32Header(other.m_bUseOld32Header),
	HEADER_SIZE(other.HEADER_SIZE)
{
	//We don't set the header info here, cuz it was already set by the other record
}

RecordType Record::GetType() const
{
	return (RecordType)m_pHeader->m_nRecordType;
}

XnUInt32 Record::GetNodeID() const
{
	return m_pHeader->m_nNodeID;
}

XnUInt32 Record::GetSize() const
{
	return m_pHeader->m_nFieldsSize;
}

XnUInt32 Record::GetPayloadSize() const
{
	return m_pHeader->m_nPayloadSize;
}

XnUInt64 Record::GetUndoRecordPos() const
{
	if (m_bUseOld32Header)
		return ((Header_old32 *)m_pHeader)->m_nUndoRecordPos;
	else
		return m_pHeader->m_nUndoRecordPos;
}

void Record::SetNodeID(XnUInt32 nNodeID)
{
	m_pHeader->m_nNodeID = nNodeID;
}

void Record::SetPayloadSize(XnUInt32 nPayloadSize)
{
	m_pHeader->m_nPayloadSize = nPayloadSize;
}

void Record::SetUndoRecordPos(XnUInt64 nUndoRecordPos)
{
	m_pHeader->m_nUndoRecordPos = nUndoRecordPos;
}

XnUInt8* Record::GetData()
{
	return m_pData;
}

const XnUInt8* Record::GetData() const
{
	return m_pData;
}

void Record::SetData(XnUInt8* pData, XnUInt32 nMaxSize)
{
	m_pData = pData;
	m_nMaxSize = nMaxSize;
}

const XnUInt8* Record::GetWritePos() const
{
	return m_pData + m_pHeader->m_nFieldsSize;
}

XnStatus Record::StartWrite(XnUInt32 nRecordType)
{
	XN_VALIDATE_INPUT_PTR(m_pData);
	if (m_nMaxSize < HEADER_SIZE)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_INPUT_BUFFER_OVERFLOW, XN_MASK_OPEN_NI, "Record buffer too small");
	}
	m_pHeader->m_nMagic = MAGIC;
	m_pHeader->m_nRecordType = nRecordType;
	m_pHeader->m_nFieldsSize = HEADER_SIZE;
	return XN_STATUS_OK;
}

XnStatus Record::Write(const void* pData, XnUInt32 nSize)
{
	XN_VALIDATE_INPUT_PTR(pData);
	XnUInt32 nNewSize = m_pHeader->m_nFieldsSize + nSize;
	if (nNewSize > m_nMaxSize)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_INPUT_BUFFER_OVERFLOW, XN_MASK_OPEN_NI, "Record buffer too small");
	}

	xnOSMemCopy(m_pData + m_pHeader->m_nFieldsSize, pData, nSize);
	m_pHeader->m_nFieldsSize = nNewSize;

	return XN_STATUS_OK;
}

XnStatus Record::WriteString(const XnChar* str)
{
	XN_VALIDATE_INPUT_PTR(str);
	XnUInt32 nStrSize = (XnUInt32)strlen(str) + 1; //+1 for terminating '\0'
	XnStatus nRetVal = Write(&nStrSize, sizeof(nStrSize));
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Write(str, nStrSize); 
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus Record::FinishWrite()
{
	//Nothing to do here right now - we can add a tail if we like.
	return XN_STATUS_OK;
}

XnStatus Record::StartRead()
{
	m_nReadOffset = HEADER_SIZE;
	return XN_STATUS_OK;
}

XnStatus Record::Read(void* pDest, XnUInt32 nSize) const
{
	XN_VALIDATE_OUTPUT_PTR(pDest);
	if (m_nReadOffset + nSize > m_nMaxSize)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_INPUT_BUFFER_OVERFLOW, XN_MASK_OPEN_NI, "Record buffer too small");
	}

	xnOSMemCopy(pDest, m_pData + m_nReadOffset, nSize);
	m_nReadOffset += nSize;
	
	return XN_STATUS_OK;
}

XnStatus Record::ReadString(const XnChar* &strDest) const
{
	XnUInt32 nStrSize = 0;
	//Get size
	XnStatus nRetVal = Read(&nStrSize, sizeof(nStrSize));
	XN_IS_STATUS_OK(nRetVal);
	//Check size is ok
	if (m_nReadOffset + nStrSize > m_nMaxSize)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_INPUT_BUFFER_OVERFLOW, XN_MASK_OPEN_NI, "Record buffer too small");
	}
	//Point destination string to current position
	strDest = (const XnChar*)(m_pData + m_nReadOffset);
	//Skip string
	m_nReadOffset += nStrSize;

	return XN_STATUS_OK;
}

const XnUInt8* Record::GetReadPos() const
{
	return (m_pData + m_nReadOffset);
}

const XnUInt8* Record::GetPayload() const
{
	XN_ASSERT(m_pHeader->m_nPayloadSize != 0);
	return (m_pData + m_pHeader->m_nFieldsSize);
}

XnUInt8* Record::GetPayload()
{
	XN_ASSERT(m_pHeader->m_nPayloadSize != 0);
	return (m_pData + m_pHeader->m_nFieldsSize);
}

XnBool Record::IsHeaderValid() const
{
	if (m_pData == NULL)
	{
		return FALSE;
	}

	if (m_pHeader->m_nMagic != Record::MAGIC)
	{
		return FALSE;
	}

	if (m_pHeader->m_nFieldsSize < HEADER_SIZE)
	{
		return FALSE;
	}

	return TRUE;
}


void Record::ResetRead()
{
	m_nReadOffset = 0;
}

XnStatus Record::FinishRead()
{
	//Nothing to do here right now - if we add a tail we can read it here.
	return XN_STATUS_OK;
}

XnStatus Record::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	return xnOSStrFormat(strDest, nSize, &nCharsWritten, 
		"type=%u ID=%u fieldsSize=%u payloadSize=%u undoRecordPos=%u", 
		m_pHeader->m_nRecordType, m_pHeader->m_nNodeID, m_pHeader->m_nFieldsSize, 
		m_pHeader->m_nPayloadSize, m_pHeader->m_nFieldsSize);
}

/****************************/
/* NodeAdded_1_0_0_4_Record */
/****************************/
NodeAdded_1_0_0_4_Record::NodeAdded_1_0_0_4_Record(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) :
	Record(pData, nMaxSize, bUseOld32Header), m_strNodeName(NULL), m_type(XnProductionNodeType(0)), m_compression(XN_CODEC_NULL)
{
	xnOSMemSet(&m_compression, 0, sizeof(m_compression));
}

NodeAdded_1_0_0_4_Record::NodeAdded_1_0_0_4_Record(const Record& record) : 
	Record(record), m_strNodeName(NULL), m_type(XnProductionNodeType(0)), m_compression(XN_CODEC_NULL)
{

}

void NodeAdded_1_0_0_4_Record::SetNodeName(const XnChar* strNodeName)
{
	m_strNodeName = strNodeName;
}

void NodeAdded_1_0_0_4_Record::SetNodeType(XnProductionNodeType type)
{
	m_type = type;
}

void NodeAdded_1_0_0_4_Record::SetCompression(XnCodecID compression)
{
	m_compression = compression;
}

const XnChar* NodeAdded_1_0_0_4_Record::GetNodeName() const
{
	return m_strNodeName;
}

XnProductionNodeType NodeAdded_1_0_0_4_Record::GetNodeType() const
{
	return m_type;
}

XnCodecID NodeAdded_1_0_0_4_Record::GetCompression() const
{
	return m_compression;
}

XnStatus NodeAdded_1_0_0_4_Record::Encode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartWrite(RECORD_NODE_ADDED_1_0_0_4);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = EncodeImpl();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = FinishWrite();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus NodeAdded_1_0_0_4_Record::Decode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartRead();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = DecodeImpl();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = FinishRead();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus NodeAdded_1_0_0_4_Record::EncodeImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = WriteString(m_strNodeName);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Write(&m_type, sizeof(m_type));
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Write(&m_compression, sizeof(m_compression));
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus NodeAdded_1_0_0_4_Record::DecodeImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = ReadString(m_strNodeName);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Read(&m_type, sizeof(m_type));
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Read(&m_compression, sizeof(m_compression));
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus NodeAdded_1_0_0_4_Record::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = Record::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	nRetVal = xnOSStrFormat(strDest + nCharsWritten, nSize - nCharsWritten, &nTempCharsWritten, 
		" name='%s' nodeType=%u compression='%.4s'", m_strNodeName, m_type, &m_compression);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	
	return XN_STATUS_OK;
}
/****************************/
/* NodeAdded_1_0_0_5_Record */
/****************************/
NodeAdded_1_0_0_5_Record::NodeAdded_1_0_0_5_Record(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) :
	NodeAdded_1_0_0_4_Record(pData, nMaxSize, bUseOld32Header), m_nNumberOfFrames(0), m_nMinTimestamp(0), m_nMaxTimestamp(0)
{
}

NodeAdded_1_0_0_5_Record::NodeAdded_1_0_0_5_Record(const Record& record) : 
	NodeAdded_1_0_0_4_Record(record), m_nNumberOfFrames(0), m_nMinTimestamp(0), m_nMaxTimestamp(0)
{

}

void NodeAdded_1_0_0_5_Record::SetNumberOfFrames(XnUInt32 nNumberOfFrames)
{
	m_nNumberOfFrames = nNumberOfFrames;
}

void NodeAdded_1_0_0_5_Record::SetMinTimestamp(XnUInt64 nMinTimestamp)
{
	m_nMinTimestamp = nMinTimestamp;
}

void NodeAdded_1_0_0_5_Record::SetMaxTimestamp(XnUInt64 nMaxTimestamp)
{
	m_nMaxTimestamp = nMaxTimestamp;
}

XnUInt32 NodeAdded_1_0_0_5_Record::GetNumberOfFrames() const
{
	return m_nNumberOfFrames;
}

XnUInt64 NodeAdded_1_0_0_5_Record::GetMinTimestamp() const
{
	return m_nMinTimestamp;
}

XnUInt64 NodeAdded_1_0_0_5_Record::GetMaxTimestamp() const
{
	return m_nMaxTimestamp;
}

XnStatus NodeAdded_1_0_0_5_Record::Encode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartWrite(RECORD_NODE_ADDED_1_0_0_5);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = EncodeImpl();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = FinishWrite();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus NodeAdded_1_0_0_5_Record::Decode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartRead();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = DecodeImpl();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = FinishRead();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus NodeAdded_1_0_0_5_Record::EncodeImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = NodeAdded_1_0_0_4_Record::EncodeImpl();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Write(&m_nNumberOfFrames, sizeof(m_nNumberOfFrames));
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Write(&m_nMinTimestamp, sizeof(m_nMinTimestamp));
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Write(&m_nMaxTimestamp, sizeof(m_nMaxTimestamp));
	XN_IS_STATUS_OK(nRetVal);
	return (XN_STATUS_OK);
}

XnStatus NodeAdded_1_0_0_5_Record::DecodeImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = NodeAdded_1_0_0_4_Record::DecodeImpl();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Read(&m_nNumberOfFrames, sizeof(m_nNumberOfFrames));
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Read(&m_nMinTimestamp, sizeof(m_nMinTimestamp));
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Read(&m_nMaxTimestamp, sizeof(m_nMaxTimestamp));
	XN_IS_STATUS_OK(nRetVal);
	return (XN_STATUS_OK);
}

XnStatus NodeAdded_1_0_0_5_Record::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = NodeAdded_1_0_0_4_Record::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	nRetVal = xnOSStrFormat(strDest + nCharsWritten, nSize - nCharsWritten, &nTempCharsWritten, 
		" numFrames=%u minTS=%u maxTS=%s", m_nNumberOfFrames, m_nMinTimestamp, m_nMaxTimestamp);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	return XN_STATUS_OK;
}

/****************************/
/* NodeAddedRecord          */
/****************************/
NodeAddedRecord::NodeAddedRecord(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) :
	NodeAdded_1_0_0_5_Record(pData, nMaxSize, bUseOld32Header), m_nSeekTablePosition(0)
{
}

NodeAddedRecord::NodeAddedRecord(const Record& record) : 
	NodeAdded_1_0_0_5_Record(record), m_nSeekTablePosition(0)
{
}

void NodeAddedRecord::SetSeekTablePosition(XnUInt64 nPos)
{
	m_nSeekTablePosition = nPos;
}

XnUInt64 NodeAddedRecord::GetSeekTablePosition()
{
	return m_nSeekTablePosition;
}

XnStatus NodeAddedRecord::Encode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartWrite(RECORD_NODE_ADDED);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NodeAdded_1_0_0_5_Record::EncodeImpl();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Write(&m_nSeekTablePosition, sizeof(m_nSeekTablePosition));
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus NodeAddedRecord::Decode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartRead();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = NodeAdded_1_0_0_5_Record::DecodeImpl();
	XN_IS_STATUS_OK(nRetVal);
	if (m_bUseOld32Header)
		nRetVal = Read(&m_nSeekTablePosition, sizeof(XnUInt32));
	else
		nRetVal = Read(&m_nSeekTablePosition, sizeof(m_nSeekTablePosition));
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus NodeAddedRecord::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = NodeAdded_1_0_0_5_Record::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	nRetVal = xnOSStrFormat(strDest + nCharsWritten, nSize - nCharsWritten, &nTempCharsWritten, 
		" seekTablePos=%u", m_nSeekTablePosition);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	return XN_STATUS_OK;
}

/*********************/
/* NodeRemovedRecord */
/*********************/
NodeRemovedRecord::NodeRemovedRecord(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) :
	Record(pData, nMaxSize, bUseOld32Header)
{
}

NodeRemovedRecord::NodeRemovedRecord(const Record &record) : Record(record)
{
}

XnStatus NodeRemovedRecord::Encode()
{
	return StartWrite(RECORD_NODE_REMOVED);
}

XnStatus NodeRemovedRecord::Decode()
{
	return StartRead();
}

XnStatus NodeRemovedRecord::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = Record::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}
/*********************/
/* GeneralPropRecord */
/*********************/
GeneralPropRecord::GeneralPropRecord(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header, XnUInt32 nPropRecordType /*= RECORD_GENERAL_PROPERTY*/) : 
	Record(pData, nMaxSize, bUseOld32Header), 
	m_nPropRecordType(nPropRecordType),
	m_strPropName(NULL),
	m_nPropDataSize(0),
	m_pPropData(NULL)
{
}

GeneralPropRecord::GeneralPropRecord(const Record& record) : 
	Record(record),
	m_nPropRecordType(RECORD_GENERAL_PROPERTY),
	m_strPropName(NULL),
	m_nPropDataSize(0),
	m_pPropData(NULL)
{
}

void GeneralPropRecord::SetPropName(const XnChar* strPropName)
{
	m_strPropName = strPropName;
}

void GeneralPropRecord::SetPropDataSize(XnUInt32 nPropDataSize)
{
	m_nPropDataSize = nPropDataSize;	
}

void GeneralPropRecord::SetPropData(const void* pPropData)
{
	m_pPropData = const_cast<void*>(pPropData);
}

const XnChar* GeneralPropRecord::GetPropName() const
{
	return m_strPropName;
}

XnUInt32 GeneralPropRecord::GetPropDataSize() const
{
	return m_nPropDataSize;
}

const void* GeneralPropRecord::GetPropData() const
{
	return m_pPropData;
}

XnStatus GeneralPropRecord::Encode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartWrite(m_nPropRecordType);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = WriteString(m_strPropName);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Write(&m_nPropDataSize, sizeof(m_nPropDataSize));
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Write(m_pPropData, m_nPropDataSize);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus GeneralPropRecord::Decode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartRead();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = ReadString(m_strPropName);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Read(&m_nPropDataSize, sizeof(m_nPropDataSize));
	XN_IS_STATUS_OK(nRetVal);

	//The property data is not copied but just pointed to
	XnUInt8* pData = const_cast<XnUInt8*>(GetReadPos());

#if (XN_PLATFORM == XN_PLATFORM_LINUX_ARM || XN_PLATFORM == XN_PLATFORM_ARC || XN_PLATFORM == XN_PLATFORM_ANDROID_ARM)
	// under ARM we have some alignment issues. Move this buffer so it will be aligned.
	XnUInt32 nAlignFix = XN_DEFAULT_MEM_ALIGN - ((XnUInt32)pData % XN_DEFAULT_MEM_ALIGN);
	if (nAlignFix != 0)
	{
		xnOSMemMove(pData + nAlignFix, pData, m_nPropDataSize);
		pData += nAlignFix;
	}
#endif

	m_pPropData = pData;

	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus GeneralPropRecord::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = Record::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	nRetVal = xnOSStrFormat(strDest + nCharsWritten, nSize - nCharsWritten, &nTempCharsWritten, 
		" propName='%s' propDataSize=%u", m_strPropName, m_nPropDataSize);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;

	return XN_STATUS_OK;
}
/*****************/
/* IntPropRecord */
/*****************/
IntPropRecord::IntPropRecord(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) :
	GeneralPropRecord(pData, nMaxSize, bUseOld32Header, RECORD_INT_PROPERTY),
	m_nValue(0)
{
}

IntPropRecord::IntPropRecord(const Record &record) : 
	GeneralPropRecord(record),
	m_nValue(0)
{
}

void IntPropRecord::SetValue(XnUInt64 nValue)
{
	m_nValue = nValue;
	SetPropData(&m_nValue);
	SetPropDataSize(sizeof(m_nValue));
}

XnUInt64 IntPropRecord::GetValue() const
{
	XN_ASSERT(GetPropDataSize() == sizeof(XnUInt64));
	return *(XnUInt64*)GetPropData();
}

XnStatus IntPropRecord::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = GeneralPropRecord::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	nRetVal = xnOSStrFormat(strDest + nCharsWritten, nSize - nCharsWritten, &nTempCharsWritten, 
		" val=%llu", GetValue());
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	return XN_STATUS_OK;
}
/******************/
/* RealPropRecord */
/******************/
RealPropRecord::RealPropRecord(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) :
	GeneralPropRecord(pData, nMaxSize, bUseOld32Header, RECORD_REAL_PROPERTY),
	m_dValue(0)
{
}

RealPropRecord::RealPropRecord(const Record &record) : 
	GeneralPropRecord(record),
	m_dValue(0)
{
}

void RealPropRecord::SetValue(XnDouble dValue)
{
	m_dValue = dValue;
	SetPropData(&m_dValue);
	SetPropDataSize(sizeof(m_dValue));
}

XnDouble RealPropRecord::GetValue() const
{
	XN_ASSERT(GetPropDataSize() == sizeof(XnDouble));
	return *(XnDouble*)GetPropData();
}

XnStatus RealPropRecord::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = GeneralPropRecord::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	nRetVal = xnOSStrFormat(strDest + nCharsWritten, nSize - nCharsWritten, &nTempCharsWritten, 
		" val=%f", GetValue());
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	return XN_STATUS_OK;
}
/********************/
/* StringPropRecord */
/********************/
StringPropRecord::StringPropRecord(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) : 
	GeneralPropRecord(pData, nMaxSize, bUseOld32Header, RECORD_STRING_PROPERTY)
{
}

StringPropRecord::StringPropRecord(const Record &record) : GeneralPropRecord(record)
{
}

void StringPropRecord::SetValue(const XnChar* strValue)
{
	SetPropDataSize((XnUInt32)strlen(strValue)+1);
	SetPropData(const_cast<XnChar*>(strValue));
}

const XnChar* StringPropRecord::GetValue() const
{
	return (const XnChar*)GetPropData();
}

XnStatus StringPropRecord::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = GeneralPropRecord::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	nRetVal = xnOSStrFormat(strDest + nCharsWritten, nSize - nCharsWritten, &nTempCharsWritten, 
		" val='%s'", GetValue());
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	return XN_STATUS_OK;
}

/***********************/
/* NodeDataBeginRecord */
/***********************/
NodeDataBeginRecord::NodeDataBeginRecord(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) : 
	Record(pData, nMaxSize, bUseOld32Header)
{
	xnOSMemSet(&m_seekInfo, 0, sizeof(m_seekInfo));
}

NodeDataBeginRecord::NodeDataBeginRecord(const Record& record) : Record(record)
{
	xnOSMemSet(&m_seekInfo, 0, sizeof(m_seekInfo));
}

XnUInt32 NodeDataBeginRecord::GetNumFrames() const
{
	return m_seekInfo.m_nFrames;
}

XnUInt64 NodeDataBeginRecord::GetMaxTimeStamp() const
{
	return m_seekInfo.m_nMaxTimeStamp;
}

XnStatus NodeDataBeginRecord::Encode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartWrite(RECORD_NODE_DATA_BEGIN);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Write(&m_seekInfo, sizeof(m_seekInfo));
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus NodeDataBeginRecord::Decode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartRead();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Read(&m_seekInfo, sizeof(m_seekInfo));
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus NodeDataBeginRecord::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = Record::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	nRetVal = xnOSStrFormat(strDest + nCharsWritten, nSize - nCharsWritten, &nTempCharsWritten, 
		" frames=%u maxTS=%u", m_seekInfo.m_nFrames, m_seekInfo.m_nMaxTimeStamp);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	return XN_STATUS_OK;
}

/************************/
/* NodeStateReadyRecord */
/************************/
NodeStateReadyRecord::NodeStateReadyRecord(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) : 
	Record(pData, nMaxSize, bUseOld32Header)
{

}

NodeStateReadyRecord::NodeStateReadyRecord(const Record& record) : 
	Record(record)
{

}

XnStatus NodeStateReadyRecord::Encode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartWrite(RECORD_NODE_STATE_READY);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus NodeStateReadyRecord::Decode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartRead();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus NodeStateReadyRecord::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = Record::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

/***********************/
/* NewDataRecordHeader */
/***********************/
NewDataRecordHeader::NewDataRecordHeader(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) :
	Record(pData, nMaxSize, bUseOld32Header),
	m_nTimeStamp(0),
	m_nFrameNumber(0)
{
}

NewDataRecordHeader::NewDataRecordHeader(const Record &record) : 
	Record(record),
	m_nTimeStamp(0),
	m_nFrameNumber(0)
{
}

void NewDataRecordHeader::SetTimeStamp(XnUInt64 nTimeStamp)
{
	m_nTimeStamp = nTimeStamp;
}

void NewDataRecordHeader::SetFrameNumber(XnUInt32 nFrameNumber)
{
	m_nFrameNumber = nFrameNumber;
}

XnUInt64 NewDataRecordHeader::GetTimeStamp() const
{
	return m_nTimeStamp;
}

XnUInt32 NewDataRecordHeader::GetFrameNumber() const
{
	return m_nFrameNumber;
}


XnStatus NewDataRecordHeader::Encode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartWrite(RECORD_NEW_DATA);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Write(&m_nTimeStamp, sizeof(m_nTimeStamp));
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Write(&m_nFrameNumber, sizeof(m_nFrameNumber));
	XN_IS_STATUS_OK(nRetVal);
	//No call to FinishWrite() - this record is not done yet
	return XN_STATUS_OK;
}

XnStatus NewDataRecordHeader::Decode()
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = StartRead();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Read(&m_nTimeStamp, sizeof(m_nTimeStamp));
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = Read(&m_nFrameNumber, sizeof(m_nFrameNumber));
	XN_IS_STATUS_OK(nRetVal);
	//No call to FinishRead() - this record is not done yet
	return XN_STATUS_OK;
}

XnStatus NewDataRecordHeader::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = Record::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	nRetVal = xnOSStrFormat(strDest + nCharsWritten, nSize - nCharsWritten, &nTempCharsWritten, 
		" TS=%llu FN=%u", m_nTimeStamp, m_nFrameNumber);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	return XN_STATUS_OK;
}

/*******************/
/* DataIndexRecordHeader */
/*******************/
DataIndexRecordHeader::DataIndexRecordHeader(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) :
	Record(pData, nMaxSize, bUseOld32Header)
{
}

DataIndexRecordHeader::DataIndexRecordHeader(const Record& record) :
	Record(record)
{
}

XnStatus DataIndexRecordHeader::Encode()
{
	XnStatus nRetVal = StartWrite(RECORD_SEEK_TABLE);
	XN_IS_STATUS_OK(nRetVal);
	//No call to FinishWrite() - this record is not done yet
	return XN_STATUS_OK;
}

XnStatus DataIndexRecordHeader::Decode()
{
	XnStatus nRetVal = StartRead();
	XN_IS_STATUS_OK(nRetVal);
	//No call to FinishRead() - this record is not done yet
	return XN_STATUS_OK;
}

XnStatus DataIndexRecordHeader::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = Record::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);
	nCharsWritten += nTempCharsWritten;
	return XN_STATUS_OK;
}

/*************/
/* EndRecord */
/*************/
EndRecord::EndRecord(XnUInt8* pData, XnUInt32 nMaxSize, XnBool bUseOld32Header) : 
	Record(pData, nMaxSize, bUseOld32Header)
{
}

EndRecord::EndRecord(const Record& record) : Record(record)
{
}

XnStatus EndRecord::Encode()
{
	XnStatus nRetVal = StartWrite(RECORD_END);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = FinishWrite();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus EndRecord::Decode()
{
	XnStatus nRetVal = StartRead();
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = FinishRead();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus EndRecord::AsString(XnChar* strDest, XnUInt32 nSize, XnUInt32& nCharsWritten)
{
	XnUInt32 nTempCharsWritten = 0;
	nCharsWritten = 0;
	XnStatus nRetVal = Record::AsString(strDest, nSize, nTempCharsWritten);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}
