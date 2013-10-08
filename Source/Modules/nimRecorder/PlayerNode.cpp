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
#include "PlayerNode.h"
#include "DataRecords.h"
#include <XnPropNames.h>
#include <XnCodecIDs.h>
#include <XnLog.h>
#include <math.h>


#ifdef PLAYER_NODE_LOG_RECORDS
	template <typename T>
	inline static void DEBUG_LOG_RECORD(T record, const XnChar* strRecordName)
	{
		XnChar s[1024];
		XnUInt32 nCharsWritten = 0;
		XnStatus nRetVal = record.AsString(s, sizeof(s), nCharsWritten);
		XN_ASSERT(nRetVal == XN_STATUS_OK);
		xnLogVerbose(XN_MASK_OPEN_NI, "--PLAYER--> %s: %s", strRecordName, s);
	}
#else
	#ifdef __WIN32
		#define DEBUG_LOG_RECORD __noop
	#else
		#define DEBUG_LOG_RECORD(x,y)
	#endif
#endif

//---------------------------------------------------------------------------
// Backward Compatibility Issues
//---------------------------------------------------------------------------
#define XN_PROP_REAL_WORLD_TRANSLATION_DATA "xnRealWorldTranslationData" //general

typedef struct XnRealWorldTranslationData
{
	XnDouble dZeroPlaneDistance;
	XnDouble dPixelSizeAtZeroPlane;
	XnDouble dSourceToDepthPixelRatio;
} XnRealWorldTranslationData;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

//DATA_MAX_SIZE is set to support a resolution of 1600x1200 with 24 bits per pixel
const XnUInt64 PlayerNode::DATA_MAX_SIZE = 1600 * 1200 * 3; 
const XnUInt64 PlayerNode::RECORD_MAX_SIZE = 
	NewDataRecordHeader::MAX_SIZE +
	PlayerNode::DATA_MAX_SIZE; //Maximum data size

const XnVersion PlayerNode::OLDEST_SUPPORTED_FILE_FORMAT_VERSION = {1, 0, 0, 4};
const XnVersion PlayerNode::FIRST_FILESIZE64BIT_FILE_FORMAT_VERSION = {1, 0, 1, 0};

PlayerNode::PlayerNode(xn::Context &context, const XnChar* strName) :
	m_bOpen(FALSE),
	m_pStreamCookie(NULL),
	m_pInputStream(NULL),
	m_pNotificationsCookie(NULL),
	m_pNodeNotifications(NULL),
	m_pRecordBuffer(NULL),
	m_bRepeat(TRUE),
	m_context(context),
	m_bDataBegun(FALSE),
	m_nTimeStamp(0),
	m_nGlobalMaxTimeStamp(0),
	m_pNodeInfoMap(NULL),
	m_nMaxNodes(0),
	m_bEOF(FALSE),
	m_aSeekTempArray(NULL),
	m_hSelf(NULL),
	m_bIs32bitFileFormat(FALSE),
	m_pUncompressedData(NULL)
{
	xnOSMemSet(&m_fileVersion, 0, sizeof(m_fileVersion));
	xnOSStrCopy(m_strName, strName, sizeof(m_strName));
}

PlayerNode::~PlayerNode()
{
	Destroy();
}

XnStatus PlayerNode::Init()
{
	m_pRecordBuffer = XN_NEW_ARR(XnUInt8, RECORD_MAX_SIZE);
	XN_VALIDATE_ALLOC_PTR(m_pRecordBuffer);
	m_pUncompressedData = XN_NEW_ARR(XnUInt8, DATA_MAX_SIZE);
	XN_VALIDATE_ALLOC_PTR(m_pUncompressedData);
	return XN_STATUS_OK;
}

XnStatus PlayerNode::Destroy()
{
	CloseStream();
	//Don't verify return value - proceed anyway

	if (m_pNodeInfoMap != NULL)
	{
		for (XnUInt32 i = 0; i < m_nMaxNodes; i++)
		{
			RemovePlayerNodeInfo(i);
		}

		XN_DELETE_ARR(m_pNodeInfoMap);
		m_pNodeInfoMap = NULL;
	}

	if (m_aSeekTempArray != NULL)
	{
		xnOSFree(m_aSeekTempArray);
		m_aSeekTempArray = NULL;
	}

	XN_DELETE_ARR(m_pRecordBuffer);
	m_pRecordBuffer = NULL;
	XN_DELETE_ARR(m_pUncompressedData);
	m_pUncompressedData = NULL;

	return XN_STATUS_OK;
}

XnStatus PlayerNode::SetInputStream(void *pStreamCookie, XnPlayerInputStreamInterface *pStream)
{
	XN_VALIDATE_INPUT_PTR(pStream);
	m_pStreamCookie = pStreamCookie;
	m_pInputStream = pStream;
	XnStatus nRetVal = OpenStream();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus PlayerNode::SetNodeNotifications(void *pNotificationsCookie, XnNodeNotifications *pNodeNotifications)
{
	XN_VALIDATE_INPUT_PTR(pNodeNotifications);
	m_pNotificationsCookie = pNotificationsCookie;
	m_pNodeNotifications = pNodeNotifications;
	return XN_STATUS_OK;
}

XnStatus PlayerNode::SetRepeat(XnBool bRepeat)
{
	m_bRepeat = bRepeat;
	return XN_STATUS_OK;
}

XnStatus PlayerNode::SeekToTimeStamp(XnInt64 /*nTimeOffset*/, XnPlayerSeekOrigin /*origin*/)
{
	/*
	switch (origin)
	{
		case XN_PLAYER_SEEK_SET:
			return SeekToTimeStampAbsolute((XnUInt64)nTimeOffset);
		case XN_PLAYER_SEEK_CUR:
			return SeekToTimeStampRelative(nTimeOffset);
		case XN_PLAYER_SEEK_END:
			return SeekToTimeStampAbsolute(m_nGlobalMaxTimeStamp);
		default:
			XN_ASSERT(FALSE);
			XN_LOG_ERROR_RETURN(XN_STATUS_BAD_PARAM, XN_MASK_OPEN_NI, "Invalid seek origin: %u", origin);
	}*/
	return XN_STATUS_NOT_IMPLEMENTED;
}

XnStatus PlayerNode::SeekToFrame(const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnUInt32 nNodeID = GetPlayerNodeIDByName(strNodeName);
	if (nNodeID == INVALID_NODE_ID)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_BAD_NODE_NAME, XN_MASK_OPEN_NI, "Bad node name '%s'", strNodeName);
	}

	PlayerNodeInfo* pPlayerNodeInfo = &m_pNodeInfoMap[nNodeID];

	XnInt64 nOriginFrame = 0;
	switch (origin)
	{
		case XN_PLAYER_SEEK_SET:
		{
			nOriginFrame = 0;
			break;
		}
		case XN_PLAYER_SEEK_CUR:
		{
			nOriginFrame = pPlayerNodeInfo->nCurFrame;
			break;
		}
		case XN_PLAYER_SEEK_END:
		{
			nOriginFrame = pPlayerNodeInfo->nFrames;
			break;
		}
		default:
		{
			XN_ASSERT(FALSE);
			XN_LOG_ERROR_RETURN(XN_STATUS_BAD_PARAM, XN_MASK_OPEN_NI, "Invalid seek origin: %u", origin);
		}
	}
	XnUInt32 nDestFrame = (XnUInt32)XN_MIN(XN_MAX(1, nOriginFrame + nFrameOffset), pPlayerNodeInfo->nFrames);
	nRetVal = SeekToFrameAbsolute(nNodeID, nDestFrame);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus PlayerNode::UndoRecord(PlayerNode::RecordUndoInfo& undoInfo, XnUInt64 nDestPos, XnBool& bUndone)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnUInt64 nOriginalPos = TellStream();
	bUndone = FALSE;
	Record record(m_pRecordBuffer, RECORD_MAX_SIZE, m_bIs32bitFileFormat);
	while ((undoInfo.nRecordPos > nDestPos) && (undoInfo.nUndoRecordPos != 0))
	{
		nRetVal = SeekStream(XN_OS_SEEK_SET, undoInfo.nUndoRecordPos);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = ReadRecordHeader(record);
		XN_IS_STATUS_OK(nRetVal);
		undoInfo.nRecordPos = undoInfo.nUndoRecordPos;
		undoInfo.nUndoRecordPos = record.GetUndoRecordPos();
	}

	if (undoInfo.nRecordPos <= nDestPos)
	{
		/*We found a record that can undo the record originally pointed to by undoInfo.nDestRecordPos, 
		  so now we handle it. */
		nRetVal = ReadRecordFields(record);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = HandleRecord(record, FALSE);
		XN_IS_STATUS_OK(nRetVal);
		bUndone = TRUE;
	}
	else
	{
		nRetVal = SeekStream(XN_OS_SEEK_SET, nOriginalPos);
		XN_IS_STATUS_OK(nRetVal);
	}
	return XN_STATUS_OK;
}

DataIndexEntry* PlayerNode::FindFrameForSeekPosition(XnUInt32 nNodeID, XnUInt64 nSeekPos)
{
	XN_ASSERT((nNodeID != INVALID_NODE_ID) && (nNodeID < m_nMaxNodes));
	PlayerNodeInfo* pPlayerNodeInfo = &m_pNodeInfoMap[nNodeID];
	
	// perform binary search. We're looking for the highest timestamp BEFORE searched timestamp
	int first = 1;
	int last = pPlayerNodeInfo->nFrames;
	int mid;
	XnUInt64 nMidPos;

	while (first <= last)
	{
		mid = (first + last) / 2;
		nMidPos = pPlayerNodeInfo->pDataIndex[mid].nSeekPos;

		if (nMidPos > nSeekPos)
		{
			last = mid - 1;
		}
		else if (nMidPos < nSeekPos)
		{
			first = mid + 1;
		}
		else // equals
		{
			break;
		}
	}

	return &pPlayerNodeInfo->pDataIndex[first-1];
}

DataIndexEntry** PlayerNode::GetSeekLocationsFromDataIndex(XnUInt32 nNodeID, XnUInt32 nDestFrame)
{
	PlayerNodeInfo* pPlayerNodeInfo = &m_pNodeInfoMap[nNodeID];
	if (pPlayerNodeInfo->pDataIndex == NULL)
	{
		xnLogVerbose(XN_MASK_OPEN_NI, "Slow seek being used (recording doesn't have seek tables)");
		return NULL;
	}

	DataIndexEntry* pCurrentFrame = &pPlayerNodeInfo->pDataIndex[pPlayerNodeInfo->nCurFrame];
	DataIndexEntry* pDestFrame = &pPlayerNodeInfo->pDataIndex[nDestFrame];

	if (pCurrentFrame->nConfigurationID != pDestFrame->nConfigurationID)
	{
		// can't use fast seek. We'll have to do it the old fashion way
		xnLogVerbose(XN_MASK_OPEN_NI, "Seeking from %u to %u: Slow seek being used (configuration was changed between source and destination frames)", pPlayerNodeInfo->nCurFrame, nDestFrame);
		return NULL;
	}

	m_aSeekTempArray[nNodeID] = pDestFrame;

	// find corresponding frames of other nodes
	for (XnUInt32 i = 0; i < m_nMaxNodes; ++i)
	{
		if (m_pNodeInfoMap[i].bIsGenerator && i != nNodeID)
		{
			m_aSeekTempArray[i] = FindFrameForSeekPosition(i, pDestFrame->nSeekPos);
			if (m_aSeekTempArray[i] != NULL && m_aSeekTempArray[i]->nConfigurationID != pCurrentFrame->nConfigurationID)
			{
				xnLogVerbose(XN_MASK_OPEN_NI, "Seeking from %u to %u: Slow seek being used (configuration was changed between source and destination frames or other nodes)", pPlayerNodeInfo->nCurFrame, nDestFrame);
				return NULL;
			}
		}
	}

	// if we got here, all nodes are in the same configuration ID, and we can perform fast seek
	return m_aSeekTempArray;
}

XnStatus PlayerNode::SeekToFrameAbsolute(XnUInt32 nNodeID, XnUInt32 nDestFrame)
{
	XN_ASSERT((nNodeID != INVALID_NODE_ID) && (nNodeID < m_nMaxNodes));
	PlayerNodeInfo* pPlayerNodeInfo = &m_pNodeInfoMap[nNodeID];
	XN_ASSERT((nDestFrame > 0) && (nDestFrame <= pPlayerNodeInfo->nFrames));
	XN_VALIDATE_INPUT_PTR(m_pNodeNotifications);

	XnStatus nRetVal = XN_STATUS_OK;

	if (nDestFrame == pPlayerNodeInfo->nCurFrame)
	{
		//Just go back to position of current frame
		nRetVal = SeekStream(XN_OS_SEEK_SET, pPlayerNodeInfo->nLastDataPos);
		XN_IS_STATUS_OK(nRetVal);
		// and re-read it
		nRetVal = ReadNext();
		XN_IS_STATUS_OK(nRetVal);

		return XN_STATUS_OK;
	}

	// not same frame. Find seek locations of each stream
	DataIndexEntry** pDataIndex = GetSeekLocationsFromDataIndex(nNodeID, nDestFrame);
	if (pDataIndex != NULL)
	{
		XnUInt64 nLastPos = 0;

		// move each node to its relevant data
		for (XnUInt32 i = 0; i < m_nMaxNodes; i++)
		{
			if (m_aSeekTempArray[i] != NULL)
			{
				// read data
				nRetVal = SeekStream(XN_OS_SEEK_SET, m_aSeekTempArray[i]->nSeekPos);
				XN_IS_STATUS_OK(nRetVal);
				nRetVal = ReadNext();
				XN_IS_STATUS_OK(nRetVal);

				// check for latest position. This will be directly after the frame we seeked to.
				XnUInt64 nPos = TellStream();
				if (nPos > nLastPos)
				{
					nLastPos = nPos;
				}
			}
		}

		// now seek to directly after last position
		SeekStream(XN_OS_SEEK_SET, nLastPos);
	}
	else
	{
		// perform old seek (no data indexes)
		XnUInt64 nStartPos = TellStream();
		XnUInt32 nNextFrame = pPlayerNodeInfo->nCurFrame + 1;
		XnStatus nRetVal = XN_STATUS_OK;

		if (nDestFrame < nNextFrame)
		{
			//Seek backwards
			XnUInt64 nDestRecordPos = pPlayerNodeInfo->newDataUndoInfo.nRecordPos;
			XnUInt64 nUndoRecordPos = pPlayerNodeInfo->newDataUndoInfo.nUndoRecordPos;
			NewDataRecordHeader record(m_pRecordBuffer, RECORD_MAX_SIZE, m_bIs32bitFileFormat);
			
			/*Scan back through the frames' undo positions until we get to a frame number that is smaller or equal
			  to nDestFrame. We put the position of the frame we find in nDestRecordPos. */
			do
			{
				if (nUndoRecordPos == 0)
				{
					/* The last frame we encountered doesn't have an undo frame. But this data frame can't be the first,
					   so the file is corrupt */
					XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Undo frame not found for frame in position %u", nDestRecordPos);
				}
				nRetVal = SeekStream(XN_OS_SEEK_SET, nUndoRecordPos);
				XN_IS_STATUS_OK(nRetVal);
				nDestRecordPos = nUndoRecordPos;
				record.ResetRead();
				nRetVal = ReadRecordHeader(record);
				XN_IS_STATUS_OK(nRetVal);
				if (record.GetType() != RECORD_NEW_DATA)
				{
					XN_ASSERT(FALSE);
					XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Unexpected record type: %u", record.GetType());
				}

				if (record.GetNodeID() != nNodeID)
				{
					XN_ASSERT(FALSE);
					XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Unexpected node id: %u", record.GetNodeID());
				}

				nRetVal = ReadRecordFields(record);
				XN_IS_STATUS_OK(nRetVal);
				nRetVal = record.Decode();
				XN_IS_STATUS_OK(nRetVal);
				nUndoRecordPos = record.GetUndoRecordPos();
			} while (record.GetFrameNumber() > nDestFrame);

			//Now handle the frame
			nRetVal = HandleNewDataRecord(record, FALSE);
			XnBool bUndone = FALSE;

			for (XnUInt32 i = 0; i < m_nMaxNodes; ++i)
			{
				//Rollback all properties to match the state the stream was in at position nDestRecordPos
				PlayerNodeInfo &pni = m_pNodeInfoMap[i];
				for (RecordUndoInfoMap::Iterator it = pni.recordUndoInfoMap.Begin(); 
					 it != pni.recordUndoInfoMap.End(); ++it)
				{
					if ((it->Value().nRecordPos > nDestRecordPos) && (it->Value().nRecordPos < nStartPos))
					{
						//This property was set between nDestRecordPos and our start position, so we need to undo it.
						nRetVal = UndoRecord(it->Value(), nDestRecordPos, bUndone);
						XN_IS_STATUS_OK(nRetVal);
					}
				}

				if ((i != nNodeID) && pni.bIsGenerator)
				{
					//Undo all other generator nodes' data
					RecordUndoInfo &undoInfo = pni.newDataUndoInfo;
					if ((undoInfo.nRecordPos > nDestRecordPos) && (undoInfo.nRecordPos < nStartPos))
					{
						nRetVal = UndoRecord(undoInfo, nDestRecordPos, bUndone);
						XN_IS_STATUS_OK(nRetVal);
						if (!bUndone)
						{
							//We couldn't find a record that can undo this data record
							pni.nLastDataPos = 0;
							pni.newDataUndoInfo.Reset();
						}
					}
				}
			}

			/*Now, for each node, go to the position of the last encountered data record, and process that record
			  (including its payload).*/
			/*TODO: Optimization: remember each node's last data pos, and later, see if it changes. Only process data
			  frames of nodes whose last data pos actually changed.*/

			nRetVal = ProcessEachNodeLastData(nNodeID);
			XN_IS_STATUS_OK(nRetVal);
		}
		else //(nDestFrame >= nNextFrame)
		{
			//Skip all frames until we get to our frame number, but handle any properties we run into.
			while (pPlayerNodeInfo->nCurFrame < nDestFrame)
			{
				nRetVal = ProcessRecord(FALSE);
				XN_IS_STATUS_OK(nRetVal);
			}

			/*Now, for each node, go to the position of the last encountered data record, and process that record
			  (including its payload).*/
			/*TODO: Optimization: remember each node's last data pos, and later, see if it changes. Only process data
			  frames of nodes whose last data pos actually changed.*/
			nRetVal = ProcessEachNodeLastData(nNodeID);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	return XN_STATUS_OK;
}

XnStatus PlayerNode::ProcessEachNodeLastData(XnUInt32 nIDToProcessLast)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnUInt32 nItNodeID = 0; //Node ID handled in each iteration.
	
	for (XnUInt32 i = 0; i < m_nMaxNodes; i++)
	{
		/*We switch positions between nIDToProcessLast and the last position, to make sure that nIDToProcessLast is 
		  handled last. This way the position at the end of our seek operation is right after the record we read
		  for nIDToProcessLast.*/
		if (i == nIDToProcessLast)
		{
			nItNodeID = m_nMaxNodes - 1;
		}
		else if (i == m_nMaxNodes - 1)
		{
			nItNodeID = nIDToProcessLast;
		}
		else
		{
			nItNodeID = i;
		}
		PlayerNodeInfo &pni = m_pNodeInfoMap[nItNodeID];
		if (pni.bIsGenerator)
		{
			if (!pni.bValid)
			{
				xnLogError(XN_MASK_OPEN_NI, "Node with ID %u is not valid", nItNodeID);
				XN_ASSERT(FALSE);
				return XN_STATUS_CORRUPT_FILE;
			}

			if (pni.nLastDataPos == 0)
			{
				/*This means we had to undo this node's data, but found no data frame before our main node's
			      data frame. In this case we push a 0 frame.*/
				memset(m_pRecordBuffer, 0, RECORD_MAX_SIZE);
				nRetVal = m_pNodeNotifications->OnNodeNewData(m_pNotificationsCookie, pni.strName, 0, 0, m_pRecordBuffer, RECORD_MAX_SIZE);
				XN_IS_STATUS_OK(nRetVal);
			}
			else
			{
				nRetVal = SeekStream(XN_OS_SEEK_SET, pni.nLastDataPos);
				XN_IS_STATUS_OK(nRetVal);
				nRetVal = ProcessRecord(TRUE);
				XN_IS_STATUS_OK(nRetVal);
			}
		}
	}
	//Now our position is right after the last data of the node with id nIDToProcessLast

	return XN_STATUS_OK;
}

XnStatus PlayerNode::TellTimestamp(XnUInt64& nTimestamp)
{
	nTimestamp = m_nTimeStamp;
	return (XN_STATUS_OK);
}

XnStatus PlayerNode::TellFrame(const XnChar* strNodeName, XnUInt32& nFrameNumber)
{
	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfoByName(strNodeName);
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_BAD_NODE_NAME);
	if (!pPlayerNodeInfo->bValid)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_BAD_NODE_NAME;
	}

	nFrameNumber = pPlayerNodeInfo->nCurFrame;
	return XN_STATUS_OK;
}

XnUInt32 PlayerNode::GetNumFrames(const XnChar* strNodeName, XnUInt32& nFrames)
{
	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfoByName(strNodeName);
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_BAD_NODE_NAME);
	if (!pPlayerNodeInfo->bValid)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_BAD_NODE_NAME;
	}

	nFrames = pPlayerNodeInfo->nFrames;
	return XN_STATUS_OK;
}

const XnChar* PlayerNode::GetSupportedFormat()
{
	return XN_FORMAT_NAME_ONI;
}

XnBool PlayerNode::IsEOF()
{
	return m_bEOF;
}

XnStatus PlayerNode::RegisterToEndOfFileReached(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_eofReachedEvent.Register(handler, pCookie, hCallback);
}

void PlayerNode::UnregisterFromEndOfFileReached(XnCallbackHandle hCallback)
{
	m_eofReachedEvent.Unregister(hCallback);
}

XnStatus PlayerNode::ReadNext()
{
	return ProcessRecord(TRUE);
}

XnStatus PlayerNode::ProcessRecord(XnBool bProcessPayload)
{
	//Read a record and handle it
	Record record(m_pRecordBuffer, RECORD_MAX_SIZE, m_bIs32bitFileFormat);
	XnStatus nRetVal = ReadRecord(record);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = HandleRecord(record, bProcessPayload);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus PlayerNode::OpenStream()
{
	XN_VALIDATE_INPUT_PTR(m_pInputStream);
	XnStatus nRetVal = m_pInputStream->Open(m_pStreamCookie);
	XN_IS_STATUS_OK(nRetVal);
	RecordingHeader header;
	XnUInt32 nBytesRead = 0;
	
	nRetVal = m_pInputStream->Read(m_pStreamCookie, &header, sizeof(header), &nBytesRead);
	XN_IS_STATUS_OK(nRetVal);
	if (nBytesRead < sizeof(header))
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Not enough bytes read");
	}

	/* Check header */
	if (xnOSMemCmp(header.headerMagic, DEFAULT_RECORDING_HEADER.headerMagic, sizeof(header.headerMagic)) != 0)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Invalid header magic");
	}

	if ((xnVersionCompare(&header.version, &OLDEST_SUPPORTED_FILE_FORMAT_VERSION) < 0) || //File format is too old
		(xnVersionCompare(&header.version, &DEFAULT_RECORDING_HEADER.version) > 0)) //File format is too new
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_UNSUPPORTED_VERSION, XN_MASK_OPEN_NI, "Unsupported file format version: %u.%u.%u.%u", header.version.nMajor, header.version.nMinor, header.version.nMaintenance, header.version.nBuild);
	}

	/* Do we need to parse an old 32bit-filesize file? */
	if (xnVersionCompare(&header.version, &FIRST_FILESIZE64BIT_FILE_FORMAT_VERSION) >= 0)
	{
		m_bIs32bitFileFormat = FALSE;
	} else {
		m_bIs32bitFileFormat = TRUE;
	}

	m_fileVersion = header.version;
	m_nGlobalMaxTimeStamp = header.nGlobalMaxTimeStamp;
	m_nMaxNodes = header.nMaxNodeID + 1;
	XN_ASSERT(m_nMaxNodes > 0);
	XN_DELETE_ARR(m_pNodeInfoMap);
	xnOSFree(m_aSeekTempArray);
	m_pNodeInfoMap = XN_NEW_ARR(PlayerNodeInfo, m_nMaxNodes);
	XN_VALIDATE_ALLOC_PTR(m_pNodeInfoMap);
	XN_VALIDATE_CALLOC(m_aSeekTempArray, DataIndexEntry*, m_nMaxNodes);
	
	m_bOpen = TRUE;
	nRetVal = ProcessUntilFirstData();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE_ARR(m_pNodeInfoMap);
		m_pNodeInfoMap = NULL;
		xnOSFree(m_aSeekTempArray);
		m_aSeekTempArray = NULL;
		return nRetVal;
	}

	return XN_STATUS_OK;
}

XnStatus PlayerNode::Read(void *pData, XnUInt32 nSize, XnUInt32 &nBytesRead)
{
	XN_VALIDATE_INPUT_PTR(m_pInputStream);
	if (!m_bOpen)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_INVALID_OPERATION, XN_MASK_OPEN_NI, "Stream was not opened");
	}

	return m_pInputStream->Read(m_pStreamCookie, pData, nSize, &nBytesRead);
} 

XnStatus PlayerNode::ReadRecordHeader(Record &record)
{
	XnUInt32 nBytesRead = 0;
	XnStatus nRetVal = Read(record.GetData(), record.HEADER_SIZE, nBytesRead);
	XN_IS_STATUS_OK(nRetVal);

	if (nBytesRead != record.HEADER_SIZE)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Incorrect number of bytes read");
	}

	if (!record.IsHeaderValid())
	{
		XN_ASSERT(FALSE);
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Invalid record header");
	}
	return XN_STATUS_OK;
}

XnStatus PlayerNode::ReadRecordFields(Record &record)
{
	XnUInt32 nBytesToRead = record.GetSize() - record.HEADER_SIZE;
	XnUInt32 nBytesRead = 0;
	XnStatus nRetVal = Read(record.GetData() + record.HEADER_SIZE, nBytesToRead, nBytesRead);
	XN_IS_STATUS_OK(nRetVal);
	if (nBytesRead < nBytesToRead)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Incorrect number of bytes read");
	}
	return XN_STATUS_OK;
}

XnStatus PlayerNode::ReadRecord(Record &record)
{
	XnStatus nRetVal = ReadRecordHeader(record);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = ReadRecordFields(record);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus PlayerNode::SeekStream(XnOSSeekType seekType, XnInt64 nOffset)
{
	XN_VALIDATE_INPUT_PTR(m_pInputStream);
	return m_pInputStream->Seek64(m_pStreamCookie, seekType, nOffset);
}

XnUInt64 PlayerNode::TellStream()
{
	XN_VALIDATE_PTR(m_pInputStream, (XnUInt64)-1);
	return m_pInputStream->Tell64(m_pStreamCookie);
}

XnStatus PlayerNode::CloseStream()
{
	if (m_bOpen)
	{
		XN_VALIDATE_INPUT_PTR(m_pInputStream);
		m_pInputStream->Close(m_pStreamCookie);
		m_pInputStream = NULL;
		m_pStreamCookie = NULL;
		m_bOpen = FALSE;
	}
	return XN_STATUS_OK;
}

XnStatus PlayerNode::HandleRecord(Record &record, XnBool bHandlePayload)
{
	XN_ASSERT(record.IsHeaderValid());
	switch (record.GetType())
	{
		case RECORD_NODE_ADDED:
			return HandleNodeAddedRecord(record);
		case RECORD_INT_PROPERTY:
			return HandleIntPropRecord(record);
		case RECORD_REAL_PROPERTY:
			return HandleRealPropRecord(record);
		case RECORD_STRING_PROPERTY:
			return HandleStringPropRecord(record);
		case RECORD_GENERAL_PROPERTY:
			return HandleGeneralPropRecord(record);
		case RECORD_NODE_REMOVED:
			return HandleNodeRemovedRecord(record);
		case RECORD_NODE_STATE_READY:
			return HandleNodeStateReadyRecord(record);
		case RECORD_NODE_DATA_BEGIN:
			return HandleNodeDataBeginRecord(record);
		case RECORD_NEW_DATA:
			return HandleNewDataRecord(record, bHandlePayload);
		case RECORD_SEEK_TABLE:
			// never process this record (it is processed only during node added)
			return HandleDataIndexRecord(record, FALSE);
		case RECORD_END:
			return HandleEndRecord(record);

		// BC stuff
		case RECORD_NODE_ADDED_1_0_0_5:
			return HandleNodeAdded_1_0_0_5_Record(record);
		case RECORD_NODE_ADDED_1_0_0_4:
			return HandleNodeAdded_1_0_0_4_Record(record);

		default:
			XN_ASSERT(FALSE);
			XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Unrecognized record type: %u", record.GetType());
	}		
}

PlayerNode::PlayerNodeInfo* PlayerNode::GetPlayerNodeInfo(XnUInt32 nNodeID)
{
	if (nNodeID >= m_nMaxNodes)
	{
		xnLogWarning(XN_MASK_OPEN_NI, "Got node ID %u, bigger than said max of %u", nNodeID, m_nMaxNodes);
		XN_ASSERT(FALSE);
		return NULL;
	}

	return &m_pNodeInfoMap[nNodeID];
}

XnStatus PlayerNode::RemovePlayerNodeInfo(XnUInt32 nNodeID)
{
	XnStatus nRetVal = XN_STATUS_OK;

	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfo(nNodeID);
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_CORRUPT_FILE);
	if (pPlayerNodeInfo->bValid)
	{
		if (m_pNodeNotifications != NULL)
		{
			nRetVal = m_pNodeNotifications->OnNodeRemoved(m_pNotificationsCookie, pPlayerNodeInfo->strName);
			if (nRetVal != XN_STATUS_OK)
			{
				return nRetVal;
			}
		}

		if (pPlayerNodeInfo->codec.IsValid())
		{
			xnRemoveNeededNode(GetSelfNodeHandle(), pPlayerNodeInfo->codec);
			pPlayerNodeInfo->codec.Release();
		}
		pPlayerNodeInfo->Reset(); //Now it's not valid anymore
	}

	return XN_STATUS_OK;
}

XnStatus PlayerNode::HandleNodeAddedImpl(XnUInt32 nNodeID, XnProductionNodeType type, const XnChar* strName, XnCodecID compression, XnUInt32 nNumberOfFrames, XnUInt64 /*nMinTimestamp*/, XnUInt64 nMaxTimestamp)
{
	XN_VALIDATE_INPUT_PTR(m_pNodeNotifications);

	XnStatus nRetVal = XN_STATUS_OK;

	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfo(nNodeID);
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_CORRUPT_FILE);

	//Notify node was added
	nRetVal = m_pNodeNotifications->OnNodeAdded(m_pNotificationsCookie, strName, type, compression);
	XN_IS_STATUS_OK(nRetVal);

	pPlayerNodeInfo->compression = compression;
	nRetVal = xnOSStrCopy(pPlayerNodeInfo->strName, strName, sizeof(pPlayerNodeInfo->strName));
	XN_IS_STATUS_OK(nRetVal);

	if (xnIsTypeGenerator(type))
	{
		pPlayerNodeInfo->bIsGenerator = TRUE;
		pPlayerNodeInfo->nFrames = nNumberOfFrames;
		pPlayerNodeInfo->nMaxTimeStamp = nMaxTimestamp;
	}

	//Mark this player node as valid
	pPlayerNodeInfo->bValid = TRUE;

	//Loop until this node's state is ready.
	//TODO: Check for eof
	while (!pPlayerNodeInfo->bStateReady)
	{
		nRetVal = ProcessRecord(TRUE);
		if (nRetVal != XN_STATUS_OK)
		{
			pPlayerNodeInfo->bValid = FALSE;
			return nRetVal;
		}
	}

	return (XN_STATUS_OK);
}

XnStatus PlayerNode::HandleNodeAdded_1_0_0_4_Record(NodeAdded_1_0_0_4_Record record)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = record.Decode();
	XN_IS_STATUS_OK(nRetVal);

	DEBUG_LOG_RECORD(record, "NodeAdded1_0_0_4");

	/** BC issue **/
	// NOTE: ONI files up to version 1.0.0.4 didn't had a different NodeAdded record. It did
	// not contain seek data (number of frames and min/max timestamp). Instead, this data was
	// in the DataBegin record. So we need to also find this record, and read these props from it.

	XnUInt32 nNodeID = record.GetNodeID();
	XnChar strName[XN_MAX_NAME_LENGTH];
	nRetVal = xnOSStrCopy(strName, record.GetNodeName(), XN_MAX_NAME_LENGTH);
	XN_IS_STATUS_OK(nRetVal);
	XnProductionNodeType type = record.GetNodeType();
	XnCodecID compression = record.GetCompression();
	XnUInt32 nNumFrames = 0;
	XnUInt64 nMinTimestamp = 0;
	XnUInt64 nMaxTimestamp = 0;

	if (xnIsTypeGenerator(type))
	{
		// we need to look for the DataBegin record to have number of frames, etc.
		XnUInt64 nStartPos = TellStream();

		// NOTE: this overwrites the current NodeAdded record buffer!!!
		nRetVal = SeekToRecordByType(nNodeID, RECORD_NODE_DATA_BEGIN);
		if (nRetVal == XN_STATUS_OK)
		{
			NodeDataBeginRecord dataBeginRecord(m_pRecordBuffer, RECORD_MAX_SIZE, m_bIs32bitFileFormat);
			nRetVal = ReadRecord(dataBeginRecord);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = dataBeginRecord.Decode();
			XN_IS_STATUS_OK(nRetVal);

			nNumFrames = dataBeginRecord.GetNumFrames();
			nMaxTimestamp = dataBeginRecord.GetMaxTimeStamp();

			// also find data record for min timestamp
			nRetVal = SeekToRecordByType(record.GetNodeID(), RECORD_NEW_DATA);
			if (nRetVal == XN_STATUS_OK)
			{
				NewDataRecordHeader newDataRecord(m_pRecordBuffer, RECORD_MAX_SIZE, m_bIs32bitFileFormat);
				nRetVal = ReadRecord(newDataRecord);
				XN_IS_STATUS_OK(nRetVal);

				nRetVal = newDataRecord.Decode();
				XN_IS_STATUS_OK(nRetVal);

				nMinTimestamp = newDataRecord.GetTimeStamp();
			}

			// get back to start position
			nRetVal = SeekStream(XN_OS_SEEK_SET, nStartPos);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	nRetVal = HandleNodeAddedImpl(nNodeID, type, strName, compression, nNumFrames, nMinTimestamp, nMaxTimestamp);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus PlayerNode::HandleNodeAdded_1_0_0_5_Record(NodeAdded_1_0_0_5_Record record)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = record.Decode();
	XN_IS_STATUS_OK(nRetVal);

	DEBUG_LOG_RECORD(record, "NodeAdded1_0_0_5");

	nRetVal = HandleNodeAddedImpl(
		record.GetNodeID(), record.GetNodeType(), record.GetNodeName(), record.GetCompression(),
		record.GetNumberOfFrames(), record.GetMinTimestamp(), record.GetMaxTimestamp());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus PlayerNode::HandleNodeAddedRecord(NodeAddedRecord record)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = record.Decode();

	XN_IS_STATUS_OK(nRetVal);

	DEBUG_LOG_RECORD(record, "NodeAdded");

	nRetVal = HandleNodeAddedImpl(
		record.GetNodeID(), record.GetNodeType(), record.GetNodeName(), record.GetCompression(),
		record.GetNumberOfFrames(), record.GetMinTimestamp(), record.GetMaxTimestamp());
	XN_IS_STATUS_OK(nRetVal);

	// get seek table (if exists)
	if (record.GetNumberOfFrames() > 0 && record.GetSeekTablePosition() != 0)
	{
		XnUInt64 nCurrPos = TellStream();

		nRetVal = SeekStream(XN_OS_SEEK_SET, record.GetSeekTablePosition());
		XN_IS_STATUS_OK(nRetVal);

		DataIndexRecordHeader seekTableHeader(m_pRecordBuffer, RECORD_MAX_SIZE, m_bIs32bitFileFormat);
		nRetVal = ReadRecord(seekTableHeader);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = HandleDataIndexRecord(seekTableHeader, TRUE);
		XN_IS_STATUS_OK(nRetVal);

		// and seek back
		nRetVal = SeekStream(XN_OS_SEEK_SET, nCurrPos);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus PlayerNode::SeekToRecordByType(XnUInt32 nNodeID, RecordType type)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	Record record(m_pRecordBuffer, RECORD_MAX_SIZE, m_bIs32bitFileFormat);

	XnUInt64 nStartPos = TellStream();

	XnBool bFound = FALSE;
	XnUInt64 nPosBeforeRecord = 0;
	while (!bFound && nRetVal == XN_STATUS_OK)
	{
		nPosBeforeRecord = TellStream();

		nRetVal = ReadRecord(record);
		XN_IS_STATUS_OK(nRetVal);

		if ((record.GetType() == type) && (record.GetNodeID() == nNodeID))
		{
			bFound = TRUE;
		}
		else if (record.GetType() == RECORD_END)
		{
			nRetVal = XN_STATUS_NO_MATCH;
		}
		else
		{
			// if record has payload, skip it
			nRetVal = SkipRecordPayload(record);
		}
	}

	if (bFound)
	{
		// seek to before requested record
		nRetVal = SeekStream(XN_OS_SEEK_SET, nPosBeforeRecord);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// seek back to starting position
		SeekStream(XN_OS_SEEK_SET, nStartPos);
		return (nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus PlayerNode::HandleGeneralPropRecord(GeneralPropRecord record)
{
	XN_VALIDATE_INPUT_PTR(m_pNodeNotifications);
	XnStatus nRetVal = record.Decode();
	XN_IS_STATUS_OK(nRetVal);
	DEBUG_LOG_RECORD(record, "GeneralProp");

	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfo(record.GetNodeID());
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_CORRUPT_FILE);
	if (!pPlayerNodeInfo->bValid)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_CORRUPT_FILE;
	}

	// Fix backwards compatibility issues
	if (strcmp(record.GetPropName(), XN_PROP_REAL_WORLD_TRANSLATION_DATA) == 0)
	{
		// old recordings held the RealWorldTranslationData, but API has changed. Translate
		// it to Field Of View
		if (record.GetPropDataSize() != sizeof(XnRealWorldTranslationData))
		{
			return XN_STATUS_CORRUPT_FILE;
		}

		const XnRealWorldTranslationData* pTransData = (const XnRealWorldTranslationData*)record.GetPropData();

		// we also need resolution for the translation
		xn::DepthGenerator depthGen;
		nRetVal = m_context.GetProductionNodeByName(pPlayerNodeInfo->strName, depthGen);
		XN_IS_STATUS_OK(nRetVal);

		XnMapOutputMode outputMode;
		nRetVal = depthGen.GetMapOutputMode(outputMode);
		XN_IS_STATUS_OK(nRetVal);
		
		XnFieldOfView FOV;
		FOV.fHFOV = 2*atan(pTransData->dPixelSizeAtZeroPlane * pTransData->dSourceToDepthPixelRatio * outputMode.nXRes / 2 / pTransData->dZeroPlaneDistance);
		FOV.fVFOV = 2*atan(pTransData->dPixelSizeAtZeroPlane * pTransData->dSourceToDepthPixelRatio * outputMode.nYRes / 2 / pTransData->dZeroPlaneDistance);

		nRetVal = m_pNodeNotifications->OnNodeGeneralPropChanged(m_pNotificationsCookie,
			pPlayerNodeInfo->strName,
			XN_PROP_FIELD_OF_VIEW,
			sizeof(FOV),
			&FOV);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = m_pNodeNotifications->OnNodeGeneralPropChanged(m_pNotificationsCookie,
			pPlayerNodeInfo->strName, 
			record.GetPropName(),
			record.GetPropDataSize(),
			record.GetPropData());
		XN_IS_STATUS_OK(nRetVal);
	}
	
	nRetVal = SaveRecordUndoInfo(pPlayerNodeInfo, 
		record.GetPropName(), 
		TellStream() - record.GetSize(),
		record.GetUndoRecordPos());
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus PlayerNode::HandleIntPropRecord(IntPropRecord record)
{
	XN_VALIDATE_INPUT_PTR(m_pNodeNotifications);
	XnStatus nRetVal = record.Decode();
	XN_IS_STATUS_OK(nRetVal);
	DEBUG_LOG_RECORD(record, "IntProp");
	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfo(record.GetNodeID());
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_CORRUPT_FILE);
	if (!pPlayerNodeInfo->bValid)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_CORRUPT_FILE;
	}

	const XnChar* strPropName = record.GetPropName();
	XnUInt64 nValue = record.GetValue();

	// old files workaround: some old files recorded nodes as not generating though having frames.
	// make them generating.
	if (strcmp(strPropName, XN_PROP_IS_GENERATING) == 0 &&
		nValue == FALSE &&
		pPlayerNodeInfo->nFrames > 0)
	{
		nValue = TRUE;
	}

	nRetVal = m_pNodeNotifications->OnNodeIntPropChanged(m_pNotificationsCookie, 
		pPlayerNodeInfo->strName,
		strPropName,
		nValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = SaveRecordUndoInfo(pPlayerNodeInfo, 
		record.GetPropName(), 
		TellStream() - record.GetSize(),
		record.GetUndoRecordPos());
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus PlayerNode::HandleRealPropRecord(RealPropRecord record)
{
	XN_VALIDATE_INPUT_PTR(m_pNodeNotifications);
	XnStatus nRetVal = record.Decode();
	XN_IS_STATUS_OK(nRetVal);
	DEBUG_LOG_RECORD(record, "RealProp");

	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfo(record.GetNodeID());
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_CORRUPT_FILE);
	if (!pPlayerNodeInfo->bValid)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_CORRUPT_FILE;
	}

	nRetVal = m_pNodeNotifications->OnNodeRealPropChanged(m_pNotificationsCookie, 
		pPlayerNodeInfo->strName,
		record.GetPropName(),
		record.GetValue());
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = SaveRecordUndoInfo(pPlayerNodeInfo, 
		record.GetPropName(), 
		TellStream() - record.GetSize(),
		record.GetUndoRecordPos());
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus PlayerNode::HandleStringPropRecord(StringPropRecord record)
{
	XN_VALIDATE_INPUT_PTR(m_pNodeNotifications);
	XnStatus nRetVal = record.Decode();
	XN_IS_STATUS_OK(nRetVal);
	DEBUG_LOG_RECORD(record, "StringProp");
	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfo(record.GetNodeID());
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_CORRUPT_FILE);
	if (!pPlayerNodeInfo->bValid)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_CORRUPT_FILE;
	}

	nRetVal = m_pNodeNotifications->OnNodeStringPropChanged(m_pNotificationsCookie, 
		pPlayerNodeInfo->strName,
		record.GetPropName(),
		record.GetValue());
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = SaveRecordUndoInfo(pPlayerNodeInfo, 
		record.GetPropName(), 
		TellStream() - record.GetSize(),
		record.GetUndoRecordPos());
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus PlayerNode::HandleNodeRemovedRecord(NodeRemovedRecord record)
{
	XN_VALIDATE_INPUT_PTR(m_pNodeNotifications);
	XnStatus nRetVal = record.Decode();
	XN_IS_STATUS_OK(nRetVal);
	DEBUG_LOG_RECORD(record, "NodeRemoved");

	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfo(record.GetNodeID());
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_CORRUPT_FILE);
	if (!pPlayerNodeInfo->bValid)
	{
		XN_ASSERT(FALSE);
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Got a node removed record for non-existing node %u.", record.GetNodeID());
	}

	nRetVal = RemovePlayerNodeInfo(record.GetNodeID());
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus PlayerNode::HandleNodeStateReadyRecord(NodeStateReadyRecord record)
{
	XN_VALIDATE_INPUT_PTR(m_pNodeNotifications);
	XnStatus nRetVal = record.Decode();
	XN_IS_STATUS_OK(nRetVal);
	DEBUG_LOG_RECORD(record, "NodeStateReady");
	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfo(record.GetNodeID());
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_CORRUPT_FILE);
	if (!pPlayerNodeInfo->bValid)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_CORRUPT_FILE;
	}

	// after wrap-around, if node wasn't destroyed, no need to notify about state ready
	if (!pPlayerNodeInfo->bStateReady)
	{
		nRetVal = m_pNodeNotifications->OnNodeStateReady(m_pNotificationsCookie, pPlayerNodeInfo->strName);
		XN_IS_STATUS_OK(nRetVal);
	}

	if (pPlayerNodeInfo->bIsGenerator && 
		(pPlayerNodeInfo->compression != XN_CODEC_NULL) && 
		!pPlayerNodeInfo->codec.IsValid())
	{
		xn::ProductionNode node;
		/*at this point the node should have all its properties set so we can create the codec. A node 
		  with the name pPlayerNodeInfo->strName should have been created by now. If it wasn't,
		  GetProductionNodeByName() will fail. */
		nRetVal = m_context.GetProductionNodeByName(pPlayerNodeInfo->strName, node);
		XN_IS_STATUS_OK_LOG_ERROR("Get codec production node", nRetVal);

		nRetVal = m_context.CreateCodec(pPlayerNodeInfo->compression, node, pPlayerNodeInfo->codec);
		XN_IS_STATUS_OK_LOG_ERROR("Create codec", nRetVal);

		// we need to make the codec a needed node, so that if xnForceShutdown() is called, we will be
		// destroyed *before* it does (as we hold a reference to it).
		nRetVal = xnAddNeededNode(GetSelfNodeHandle(), pPlayerNodeInfo->codec);
		XN_IS_STATUS_OK_LOG_ERROR("Add needed node", nRetVal);
	}

	pPlayerNodeInfo->bStateReady = TRUE;
	return XN_STATUS_OK;
}

XnStatus PlayerNode::HandleNodeDataBeginRecord(NodeDataBeginRecord record)
{
	XN_VALIDATE_INPUT_PTR(m_pNodeNotifications);
	XnStatus nRetVal = record.Decode();
	XN_IS_STATUS_OK(nRetVal);
	DEBUG_LOG_RECORD(record, "NodeDataBegin");
	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfo(record.GetNodeID());
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_CORRUPT_FILE);
	if (!pPlayerNodeInfo->bValid)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_CORRUPT_FILE;
	}

	if (!pPlayerNodeInfo->bIsGenerator)
	{
		XN_ASSERT(FALSE);
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Got data for non-generator node '%s'", pPlayerNodeInfo->strName);
	}

	m_bDataBegun = TRUE;

	return XN_STATUS_OK;	
}

XnStatus PlayerNode::HandleNewDataRecord(NewDataRecordHeader record, XnBool bReadPayload)
{
	XN_VALIDATE_INPUT_PTR(m_pNodeNotifications);
	XnStatus nRetVal = record.Decode();
	XN_IS_STATUS_OK(nRetVal);
	DEBUG_LOG_RECORD(record, "NewData");

	XN_ASSERT(record.GetNodeID() != INVALID_NODE_ID);
	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfo(record.GetNodeID());
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_CORRUPT_FILE);
	if (!pPlayerNodeInfo->bValid)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_CORRUPT_FILE;
	}

	XnUInt32 nRecordTotalSize = record.GetSize() + record.GetPayloadSize();
	if (nRecordTotalSize > RECORD_MAX_SIZE)
	{
		XN_ASSERT(FALSE);
		XN_LOG_ERROR_RETURN(XN_STATUS_INTERNAL_BUFFER_TOO_SMALL, XN_MASK_OPEN_NI, "Record size %u is larger than player internal buffer", nRecordTotalSize);
	}
	
	pPlayerNodeInfo->nLastDataPos = TellStream() - record.GetSize();
	pPlayerNodeInfo->newDataUndoInfo.nRecordPos =  pPlayerNodeInfo->nLastDataPos;
	pPlayerNodeInfo->newDataUndoInfo.nUndoRecordPos = record.GetUndoRecordPos();
	if (record.GetFrameNumber() > pPlayerNodeInfo->nFrames)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_CORRUPT_FILE;
	}

	pPlayerNodeInfo->nCurFrame = record.GetFrameNumber();
	
	if (record.GetTimeStamp() > m_nGlobalMaxTimeStamp)	
	{
		XN_ASSERT(FALSE);
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Record timestamp for record in position %u is larger than reported max timestamp", pPlayerNodeInfo->nLastDataPos);
	}

	m_nTimeStamp = record.GetTimeStamp();

	if (bReadPayload)
	{
		//Now read the actual data
		XnUInt32 nBytesRead = 0;
		nRetVal = Read(record.GetPayload(), record.GetPayloadSize(), nBytesRead);
		XN_IS_STATUS_OK(nRetVal);
		if (nBytesRead < record.GetPayloadSize())
		{
			XN_ASSERT(FALSE);
			XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Not enough bytes read");
		}

		const XnUInt8* pCompressedData = record.GetPayload(); //The new (compressed) data is right at the end of the header
		XnUInt32 nCompressedDataSize = record.GetPayloadSize();
		const XnUInt8* pUncompressedData = NULL;
		XnUInt32 nUncompressedDataSize = 0;
		XnCodecID compression = pPlayerNodeInfo->codec.GetCodecID();
		if (compression == XN_CODEC_UNCOMPRESSED)
		{
			pUncompressedData = pCompressedData;
			nUncompressedDataSize = nCompressedDataSize;
		}
		else
		{
			//Decode data with codec
			nRetVal = pPlayerNodeInfo->codec.DecodeData(pCompressedData, nCompressedDataSize, 
				m_pUncompressedData, DATA_MAX_SIZE, &nUncompressedDataSize);
			XN_IS_STATUS_OK_ASSERT(nRetVal);
			pUncompressedData = m_pUncompressedData;
		}

		nRetVal = m_pNodeNotifications->OnNodeNewData(m_pNotificationsCookie, pPlayerNodeInfo->strName, 
			record.GetTimeStamp(), record.GetFrameNumber(), pUncompressedData, nUncompressedDataSize);
		XN_IS_STATUS_OK_ASSERT(nRetVal);
	}
	else
	{
		//Just skip the data
		nRetVal = SkipRecordPayload(record);
		XN_IS_STATUS_OK_ASSERT(nRetVal);
	}

	return XN_STATUS_OK;
}

XnStatus PlayerNode::HandleDataIndexRecord(DataIndexRecordHeader record, XnBool bReadPayload)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(m_pNodeNotifications);
	nRetVal = record.Decode();
	XN_IS_STATUS_OK_ASSERT(nRetVal);
	DEBUG_LOG_RECORD(record, "DataIndex");

	XN_ASSERT(record.GetNodeID() != INVALID_NODE_ID);
	PlayerNodeInfo* pPlayerNodeInfo = GetPlayerNodeInfo(record.GetNodeID());
	XN_VALIDATE_PTR(pPlayerNodeInfo, XN_STATUS_CORRUPT_FILE);

	XnUInt32 nRecordTotalSize = record.GetSize() + record.GetPayloadSize();
	if (nRecordTotalSize > RECORD_MAX_SIZE)
	{
		XN_ASSERT(FALSE);
		XN_LOG_ERROR_RETURN(XN_STATUS_INTERNAL_BUFFER_TOO_SMALL, XN_MASK_OPEN_NI, "Record size %u is larger than player internal buffer", nRecordTotalSize);
	}

	if (bReadPayload)
	{
		// make sure node exists
		if (!pPlayerNodeInfo->bValid)
		{
			XN_ASSERT(FALSE);
			return XN_STATUS_CORRUPT_FILE;
		}

		XnUInt32 DIESize;
		if (m_bIs32bitFileFormat) 	DIESize = sizeof(DataIndexEntry_old32);
		else						DIESize = sizeof(DataIndexEntry);

		if (record.GetPayloadSize() != (pPlayerNodeInfo->nFrames+1) * DIESize)
		{
			XN_ASSERT(FALSE);
			XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Seek table has %u entries, but node has %u frames!", record.GetPayloadSize() / DIESize, pPlayerNodeInfo->nFrames);
		}

		// allocate our data index
		pPlayerNodeInfo->pDataIndex = (DataIndexEntry*)xnOSCalloc(pPlayerNodeInfo->nFrames+1, sizeof(DataIndexEntry));
		XN_VALIDATE_ALLOC_PTR(pPlayerNodeInfo->pDataIndex);

		//Now read the actual data
		XnUInt32 nBytesRead = 0;

		if (m_bIs32bitFileFormat)
		{
			DataIndexEntry_old32 old32;
			XnUInt32 nRead = 0;
			for(XnUInt32 n = 0; n < pPlayerNodeInfo->nFrames+1; n++)
			{
				nRetVal = Read(&old32, sizeof(DataIndexEntry_old32), nRead);
				XN_IS_STATUS_OK(nRetVal); nBytesRead += nRead;
				DataIndexEntry::FillFromOld32Entry(&pPlayerNodeInfo->pDataIndex[n], &old32);
			}
		}
		else
		{
			nRetVal = Read(pPlayerNodeInfo->pDataIndex, record.GetPayloadSize(), nBytesRead);
			XN_IS_STATUS_OK(nRetVal);
		}

		if (nBytesRead < record.GetPayloadSize())
		{
			XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Not enough bytes read");
		}
	}
	else
	{
		//Just skip the data
		nRetVal = SkipRecordPayload(record);
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
}

XnStatus PlayerNode::HandleEndRecord(EndRecord record)
{
	XN_VALIDATE_INPUT_PTR(m_pNodeNotifications);
	XnStatus nRetVal = record.Decode();
	XN_IS_STATUS_OK(nRetVal);
	DEBUG_LOG_RECORD(record, "End");

	if (!m_bDataBegun)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "File does not contain any data!");
	}

	nRetVal = m_eofReachedEvent.Raise();
	XN_IS_STATUS_OK(nRetVal);

	if (m_bRepeat)
	{
		nRetVal = Rewind();
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		m_bEOF = TRUE;
		CloseStream();
	}

	return XN_STATUS_OK;	
}


XnStatus PlayerNode::Rewind()
{
	//skip recording header
	XnStatus nRetVal = SeekStream(XN_OS_SEEK_SET, sizeof(RecordingHeader));
	XN_IS_STATUS_OK(nRetVal);

	//Reset all node info's
	for (XnUInt32 i = 0; i < m_nMaxNodes; i++)
	{
		m_pNodeInfoMap[i].Reset();
	}

	m_bDataBegun = FALSE;
	m_nTimeStamp = 0;
	m_bEOF = FALSE;
	
	//Skip to first data
	nRetVal = ProcessUntilFirstData();
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus PlayerNode::ProcessUntilFirstData()
{
	XnStatus nRetVal = XN_STATUS_OK;
	//Loop until we get to the first 'data begin' marker in the file.

	while (!m_bDataBegun)
	{
		nRetVal = ProcessRecord(TRUE);	
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return XN_STATUS_OK;
}

XnStatus PlayerNode::SeekToTimeStampAbsolute(XnUInt64 nDestTimeStamp)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnUInt64 nRecordTimeStamp = 0LL;
	XnUInt64 nStartPos = TellStream(); //We'll revert to this in case nDestTimeStamp is beyond end of stream
	XN_IS_STATUS_OK(nRetVal);

	if (nDestTimeStamp < m_nTimeStamp)
	{
		nRetVal = Rewind();
	}
	else if (nDestTimeStamp == m_nTimeStamp)
	{
		//Nothing to do
		return XN_STATUS_OK;
	}
	else if (nDestTimeStamp > m_nGlobalMaxTimeStamp)
	{
		nDestTimeStamp = m_nGlobalMaxTimeStamp;
	}

	Record record(m_pRecordBuffer, RECORD_MAX_SIZE, m_bIs32bitFileFormat);
	XnBool bEnd = FALSE;
	XnUInt32 nBytesRead = 0;

	while ((nRecordTimeStamp < nDestTimeStamp) && !bEnd)
	{
		nRetVal = ReadRecordHeader(record);
		XN_IS_STATUS_OK(nRetVal);
		switch (record.GetType())
		{
			case RECORD_NEW_DATA:
			{
				//We already read Record::HEADER_SIZE, now read the rest of the new data record header
				nRetVal = Read(m_pRecordBuffer + record.HEADER_SIZE, 
					NewDataRecordHeader::MAX_SIZE - record.HEADER_SIZE, 
					nBytesRead);
				XN_IS_STATUS_OK(nRetVal);
				if (nBytesRead < NewDataRecordHeader::MAX_SIZE - record.HEADER_SIZE)
				{
					return XN_STATUS_CORRUPT_FILE;
				}
				NewDataRecordHeader newDataRecordHeader(record);
				nRetVal = newDataRecordHeader.Decode();
				XN_IS_STATUS_OK(nRetVal);
				//Save record time stamp
				nRecordTimeStamp = newDataRecordHeader.GetTimeStamp();
				
				if (nRecordTimeStamp >= nDestTimeStamp)
				{
					//We're done - move back to beginning of record
					nRetVal = SeekStream(XN_OS_SEEK_CUR, -XnInt32(nBytesRead));
					XN_IS_STATUS_OK(nRetVal);
				}
				else
				{
					//Skip to next record
					nRetVal = SeekStream(XN_OS_SEEK_CUR, 
						newDataRecordHeader.GetSize() - NewDataRecordHeader::MAX_SIZE);
					XN_IS_STATUS_OK(nRetVal);
				}
				break;
			}

			case RECORD_END:
			{
				bEnd = TRUE;
				break;
			}

			case RECORD_NODE_ADDED_1_0_0_4:
			case RECORD_NODE_ADDED_1_0_0_5:
			case RECORD_NODE_ADDED:
			case RECORD_INT_PROPERTY:	
			case RECORD_REAL_PROPERTY:	
			case RECORD_STRING_PROPERTY:
			case RECORD_GENERAL_PROPERTY:
			case RECORD_NODE_REMOVED:	
			case RECORD_NODE_DATA_BEGIN:
			case RECORD_NODE_STATE_READY:
			{
				//Read rest of record and handle it normally
				nRetVal = Read(m_pRecordBuffer + record.HEADER_SIZE, record.GetSize() - record.HEADER_SIZE, nBytesRead);
				XN_IS_STATUS_OK(nRetVal);
				Record record(m_pRecordBuffer, RECORD_MAX_SIZE, m_bIs32bitFileFormat);
				nRetVal = HandleRecord(record, TRUE);
				XN_IS_STATUS_OK(nRetVal);
				break;
			}
			default:
			{
				XN_ASSERT(FALSE);
				return XN_STATUS_CORRUPT_FILE;
			}

		} //switch
	} //while
	
	if (bEnd)
	{
		SeekStream(XN_OS_SEEK_SET, nStartPos);
		return XN_STATUS_ILLEGAL_POSITION;
	}

	return XN_STATUS_OK;
}//function

XnStatus PlayerNode::SeekToTimeStampRelative(XnInt64 nOffset)
{
	//TODO: Implement more efficiently
	return SeekToTimeStampAbsolute(m_nTimeStamp + nOffset);
}

XnUInt32 PlayerNode::GetPlayerNodeIDByName(const XnChar* strNodeName)
{
	for (XnUInt32 i = 0; i < m_nMaxNodes; i++)
	{
		if (xnOSStrCmp(strNodeName, m_pNodeInfoMap[i].strName) == 0)
		{
			return i;
		}
	}
	return INVALID_NODE_ID;
}

PlayerNode::PlayerNodeInfo* PlayerNode::GetPlayerNodeInfoByName(const XnChar* strNodeName)
{
	XnUInt32 nNodeID = GetPlayerNodeIDByName(strNodeName);
	return (nNodeID == INVALID_NODE_ID) ? NULL : &m_pNodeInfoMap[nNodeID];
}

XnStatus PlayerNode::SaveRecordUndoInfo(PlayerNodeInfo* pPlayerNodeInfo, 
										const XnChar* strPropName, 
										XnUInt64 nRecordPos,
										XnUInt64 nUndoRecordPos)
{
	RecordUndoInfo recordUndoInfo;
	recordUndoInfo.nRecordPos = nRecordPos;
	recordUndoInfo.nUndoRecordPos = nUndoRecordPos;
	XnStatus nRetVal = pPlayerNodeInfo->recordUndoInfoMap.Set(strPropName, recordUndoInfo);
	XN_IS_STATUS_OK(nRetVal);
	return XN_STATUS_OK;
}

XnStatus PlayerNode::GetRecordUndoInfo(PlayerNodeInfo* pPlayerNodeInfo, const XnChar* strPropName, XnUInt64& nRecordPos, XnUInt64& nUndoRecordPos)
{
	RecordUndoInfo *pRecordUndoInfo = NULL;
	XnStatus nRetVal = pPlayerNodeInfo->recordUndoInfoMap.Get(strPropName, pRecordUndoInfo);
	XN_IS_STATUS_OK(nRetVal);
	nRecordPos = pRecordUndoInfo->nRecordPos;
	nUndoRecordPos = pRecordUndoInfo->nUndoRecordPos;
	return XN_STATUS_OK;
}

XnStatus PlayerNode::SkipRecordPayload(Record record)
{
	return SeekStream(XN_OS_SEEK_CUR, record.GetPayloadSize());
}

XnNodeHandle PlayerNode::GetSelfNodeHandle()
{
	if (m_hSelf == NULL)
	{
		xn::Player thisPlayer;
		XnStatus nRetVal = m_context.GetProductionNodeByName(m_strName, thisPlayer);
		XN_ASSERT(nRetVal == XN_STATUS_OK);
		XN_REFERENCE_VARIABLE(nRetVal);

		// we keep just the handle, without a reference (otherwise, we keep a reference to ourselves, 
		// and we will never be destroyed)
		m_hSelf = thisPlayer;
	}

	return m_hSelf;
}

PlayerNode::PlayerNodeInfo::PlayerNodeInfo()
{
	pDataIndex = NULL;
	Reset();
}

PlayerNode::PlayerNodeInfo::~PlayerNodeInfo()
{
}

void PlayerNode::PlayerNodeInfo::Reset()
{
	xnOSMemSet(strName, 0, sizeof(strName));
	nLastDataPos = 0;
	compression = XN_CODEC_NULL;
	nFrames = 0; 
	nCurFrame = 0;
	nMaxTimeStamp = 0;
	bStateReady = FALSE;
	bIsGenerator = FALSE;
	recordUndoInfoMap.Clear();
	newDataUndoInfo.Reset();
	bValid = FALSE;
	xnOSFree(pDataIndex);
	pDataIndex = NULL;
}
