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
{}

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
	xnOSWriteFile(m_fLogFile, strMessage, strlen(strMessage) + 1);
}

XnStatus XnLogFileWriter::OpenFile()
{
	if (m_fLogFile == XN_INVALID_FILE_HANDLE)
	{
		// open file
		XnStatus nRetVal = xnLogCreateFile("log", &m_fLogFile);
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