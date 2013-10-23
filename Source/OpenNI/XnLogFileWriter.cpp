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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnLogFileWriter.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnLogFileWriter::XnLogFileWriter() : 
	m_fLogFile(XN_INVALID_FILE_HANDLE),
	m_bWriteLineInfo(TRUE)
{
	m_strCurrFileName[0] = '\0';
}

XnLogFileWriter::~XnLogFileWriter()
{
	CloseFile();
}

void XnLogFileWriter::WriteEntry(const XnLogEntry* pEntry)
{
	// make sure file is open
	if (m_fLogFile == XN_INVALID_FILE_HANDLE)
	{
		return;
	}

	// write timestamp and severity
	const XnUInt32 nMaxMessageSize = 2047;
	XnChar strBuffer[nMaxMessageSize + 1];

	XnUInt32 nMessageLen = 0;
	XnUInt32 nChars = 0;
	xnOSStrFormat(strBuffer + nMessageLen, nMaxMessageSize - nMessageLen, &nChars, "%9llu\t%-10s\t%s\t", pEntry->nTimestamp, pEntry->strSeverity, pEntry->strMask);
	nMessageLen += nChars;

	if (m_bWriteLineInfo)
	{
		// write line info
		XnChar fileName[XN_FILE_MAX_PATH];
		XnStatus nRetVal = xnOSGetFileName(pEntry->strFile, fileName, sizeof(fileName));
		if (nRetVal == XN_STATUS_OK)
		{
			xnOSStrFormat(strBuffer + nMessageLen, nMaxMessageSize - nMessageLen, &nChars, "%s\t%d\t", fileName, pEntry->nLine);
			nMessageLen += nChars;
		}
	}

	xnOSStrFormat(strBuffer + nMessageLen, nMaxMessageSize - nMessageLen, &nChars, "%s\n", pEntry->strMessage);
	nMessageLen += nChars;

	xnOSWriteFile(m_fLogFile, strBuffer, nMessageLen);
}

void XnLogFileWriter::WriteUnformatted(const XnChar* strMessage)
{
	xnOSWriteFile(m_fLogFile, strMessage, xnOSStrLen(strMessage) + 1);
}

XnStatus XnLogFileWriter::OpenFile()
{
	if (m_fLogFile == XN_INVALID_FILE_HANDLE)
	{
		// open file
		XnStatus nRetVal = xnLogCreateNewFile("log", TRUE, m_strCurrFileName, XN_FILE_MAX_PATH, &m_fLogFile);
		if (nRetVal != XN_STATUS_OK)
		{
			// we don't have much to do if files can't be open. Logs will not be written to file
			printf("Couldn't create log file! Logs will not be written (error: %s)\n", xnGetStatusString(nRetVal));
			m_fLogFile = XN_INVALID_FILE_HANDLE;
			return nRetVal;
		}
	}

	return XN_STATUS_OK;
}

void XnLogFileWriter::CloseFile()
{
	if (m_fLogFile != XN_INVALID_FILE_HANDLE)
	{
		xnOSCloseFile(&m_fLogFile);
		m_fLogFile = XN_INVALID_FILE_HANDLE;
	}
}

void XnLogFileWriter::SetLineInfo(XnBool bLineInfo)
{
	m_bWriteLineInfo = bLineInfo;
}

void XnLogFileWriter::OnRegister()
{
	OpenFile();
}

void XnLogFileWriter::OnUnregister()
{
	CloseFile();
}