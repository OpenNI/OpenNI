/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include <XnStringsHash.h>
#include <XnVersion.h>
#include <stdarg.h>
#include "XnXml.h"

#define XN_LOG_IMPL
#include <XnLog.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_LOG_MASKS_DELIM	";"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnLoggerData
{
public:
	XnLoggerData() : m_fLogFile(XN_INVALID_FILE_HANDLE)
	{
		Reset();
	}

	void Reset()
	{
		CloseFile();
		m_csLogDir[0] = '\0';
		m_nLogFilteringType = XN_LOG_WRITE_NONE;
		m_bWriteLineInfo = TRUE;
		m_nDumpFilteringType = XN_LOG_WRITE_NONE;
	}

	void CloseFile()
	{
		m_bBannerPrinted = FALSE;
		if (m_fLogFile != XN_INVALID_FILE_HANDLE)
		{
			xnOSCloseFile(&m_fLogFile);
			m_fLogFile = XN_INVALID_FILE_HANDLE; 
		}
		m_csTime[0] = '\0';
	}

	XnStringsHash m_LogMasks;
	XN_FILE_HANDLE m_fLogFile;
	XnChar m_csLogDir[XN_FILE_MAX_PATH];
	XnChar m_csTime[25];
	XnLogFilteringType m_nLogFilteringType;
	XnLogSeverity m_nFilterSeverity;
	XnBool m_bWriteLineInfo;
	XnBool m_bWriteToConsole;
	XnBool m_bWriteToFile;

	XnLogFilteringType m_nDumpFilteringType;
	XnStringsHash m_DumpMasks;

	XnBool m_bBannerPrinted;
};

static XnLoggerData g_xnLoggerData;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

const XnChar* xnLogGetSeverityString(XnLogSeverity nSeverity);
void xnLogWriteImplV(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, va_list args);

void xnLogGetMasksString(XnChar* csString)
{
	switch (g_xnLoggerData.m_nLogFilteringType)
	{
	case XN_LOG_WRITE_NONE:
		xnOSStrCopy(csString, "NONE", XN_LOG_MASKS_STRING_LEN);
		return;
	case XN_LOG_WRITE_ALL:
		xnOSStrCopy(csString, "ALL", XN_LOG_MASKS_STRING_LEN);
		return;
	case XN_LOG_WRITE_MASKS:
		{
			csString[0] = '\0';

			for (XnStringsHash::Iterator it = g_xnLoggerData.m_LogMasks.begin(); it != g_xnLoggerData.m_LogMasks.end(); ++it)
			{
				xnOSStrAppend(csString, it.Key(), XN_LOG_MASKS_STRING_LEN);
				xnOSStrAppend(csString, ";", XN_LOG_MASKS_STRING_LEN);
			}

			return;
		}
	default:
		xnOSStrCopy(csString, "UNKNOWN", XN_LOG_MASKS_STRING_LEN);
		return;
	}
}

void xnLogWriteImpl(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, ...)
{
	va_list args;
	va_start(args, csFormat);
	xnLogWriteImplV(csLogMask, nSeverity, csFile, nLine, csFormat, args);
	va_end(args);
}

XnStatus xnLogCreateFile(const XnChar* csFileName, XN_FILE_HANDLE* phFile)
{
	// create the log directory (to make sure it exists)
	xnOSCreateDirectory(g_xnLoggerData.m_csLogDir);

	// now open the file
	return xnOSOpenFile(csFileName, XN_OS_FILE_WRITE | XN_OS_FILE_TRUNCATE, phFile);
}

void xnLogConfigurationChanged()
{
	// if new configuration requires a log file, and we don't have one opened
	if (g_xnLoggerData.m_nLogFilteringType != XN_LOG_WRITE_NONE && g_xnLoggerData.m_bWriteToFile && g_xnLoggerData.m_fLogFile == XN_INVALID_FILE_HANDLE)
	{
		XN_PROCESS_ID nProcID = 0;
		xnOSGetCurrentProcessID(&nProcID);
		XnChar strFileName[XN_FILE_MAX_PATH];
		sprintf(strFileName, "%s%s_%u.log", g_xnLoggerData.m_csLogDir, g_xnLoggerData.m_csTime, nProcID);
		XnStatus nRetVal = xnLogCreateFile(strFileName, &g_xnLoggerData.m_fLogFile);
		if (nRetVal != XN_STATUS_OK)
		{
			// we don't have much to do if files can't be open. Logs will not be written to file
			printf("Couldn't create log file %s! Logs will not be written (error: %s)\n", strFileName, xnGetStatusString(nRetVal));
			g_xnLoggerData.m_fLogFile = XN_INVALID_FILE_HANDLE;
			g_xnLoggerData.m_bWriteToFile = FALSE;
		}
	}

	if (!g_xnLoggerData.m_bBannerPrinted && xnLogIsEnabled(XN_MASK_LOG, XN_LOG_INFO))
	{
		xnLogInfo(XN_MASK_LOG, "OpenNI version is %s", XN_VERSION_STRING);
		g_xnLoggerData.m_bBannerPrinted = TRUE;
	}

	if (g_xnLoggerData.m_fLogFile != XN_INVALID_FILE_HANDLE)
	{
		XnChar csMasks[XN_LOG_MASKS_STRING_LEN];
		xnLogGetMasksString(csMasks);
		xnLogWriteImpl(XN_MASK_LOG, XN_LOG_INFO, __FILE__, __LINE__, "Log system initialized. Console: %d, File: %d, Severity: %s, Masks: %s",
			g_xnLoggerData.m_bWriteToConsole, g_xnLoggerData.m_bWriteToFile, xnLogGetSeverityString(g_xnLoggerData.m_nFilterSeverity), csMasks);
	}
}

XN_C_API XnStatus xnLogInitSystem()
{
	// make sure xnOS is initialized (we need timers)
	XnStatus nRetVal = xnOSInit();
	if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_OS_ALREADY_INIT)
	{
		return (nRetVal);
	}

	if (g_xnLoggerData.m_csLogDir[0] == '\0')
	{
		XnChar csCurDir[XN_FILE_MAX_PATH];
		xnOSGetCurrentDir(csCurDir, XN_FILE_MAX_PATH);
		XnUInt32 nBytesWritten;
		xnOSStrFormat(g_xnLoggerData.m_csLogDir, XN_FILE_MAX_PATH, &nBytesWritten, "%s%s%s%s", csCurDir, XN_FILE_DIR_SEP, XN_LOG_DIR_NAME, XN_FILE_DIR_SEP);
	}

	if (g_xnLoggerData.m_csTime[0] == '\0')
	{
		time_t currtime;
		time(&currtime);
		strftime(g_xnLoggerData.m_csTime, sizeof(g_xnLoggerData.m_csTime)-1, "%Y_%m_%d__%H_%M_%S", localtime(&currtime)); 
	}

	xnLogConfigurationChanged();

	return (XN_STATUS_OK);
}

void xnLogSetMasks(XnChar* csMasks, XnStringsHash* pMasksHash)
{
	pMasksHash->Clear();

	XnChar* pMask = strtok(csMasks, XN_LOG_MASKS_DELIM);
	while (pMask != NULL)
	{
		pMasksHash->Set(pMask, NULL);
		pMask = strtok(NULL, XN_LOG_MASKS_DELIM);
	}
}

void xnLogReadMasksFromINI(const XnChar* cpINIFileName, const XnChar* cpSectionName, const XnChar* cpKey, XnLogFilteringType* pFilteringType, XnStringsHash* pMasksHash)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnChar csTemp[XN_INI_MAX_LEN] = "";

	*pFilteringType = XN_LOG_WRITE_NONE;

	nRetVal = xnOSReadStringFromINI(cpINIFileName, cpSectionName, cpKey, csTemp, XN_INI_MAX_LEN);
	if (nRetVal == XN_STATUS_OK)
	{
		if (strcmp(csTemp, XN_LOG_MASK_ALL) == 0)
			*pFilteringType = XN_LOG_WRITE_ALL;
		else if (csTemp[0] == '\0')	// empty
			*pFilteringType = XN_LOG_WRITE_NONE;
		else
			*pFilteringType = XN_LOG_WRITE_MASKS;
	}

	xnLogSetMasks(csTemp, pMasksHash);
}

XN_C_API XnStatus xnLogInitFromINIFile(const XnChar* cpINIFileName, const XnChar* cpSectionName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnUInt32 nTemp;

	xnLogReadMasksFromINI(cpINIFileName, cpSectionName, "LogMasks", &g_xnLoggerData.m_nLogFilteringType, &g_xnLoggerData.m_LogMasks);
	xnLogReadMasksFromINI(cpINIFileName, cpSectionName, "DumpMasks", &g_xnLoggerData.m_nDumpFilteringType, &g_xnLoggerData.m_DumpMasks);

	g_xnLoggerData.m_nFilterSeverity = XN_LOG_ERROR;
	nRetVal = xnOSReadIntFromINI(cpINIFileName, cpSectionName, "LogLevel", &nTemp);
	if (nRetVal == XN_STATUS_OK)
		g_xnLoggerData.m_nFilterSeverity = (XnLogSeverity)nTemp;

	g_xnLoggerData.m_bWriteToConsole = FALSE;
	nRetVal = xnOSReadIntFromINI(cpINIFileName, cpSectionName, "LogWriteToConsole", &nTemp);
	if (nRetVal == XN_STATUS_OK)
		g_xnLoggerData.m_bWriteToConsole = nTemp;

	g_xnLoggerData.m_bWriteToFile = TRUE;
	nRetVal = xnOSReadIntFromINI(cpINIFileName, cpSectionName, "LogWriteToFile", &nTemp);
	if (nRetVal == XN_STATUS_OK)
		g_xnLoggerData.m_bWriteToFile = nTemp;

	g_xnLoggerData.m_bWriteLineInfo = TRUE;
	nRetVal = xnOSReadIntFromINI(cpINIFileName, cpSectionName, "LogWriteLineInfo", &nTemp);
	if (nRetVal == XN_STATUS_OK)
		g_xnLoggerData.m_bWriteLineInfo = nTemp;

	return xnLogInitSystem();
}

XN_C_API XnStatus xnLogInitFromXmlFile(const XnChar* strFileName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnLogInitSystem();
	XN_IS_STATUS_OK(nRetVal);

	TiXmlDocument doc;
	nRetVal = xnXmlLoadDocument(doc, strFileName);
	XN_IS_STATUS_OK(nRetVal);

	TiXmlElement* pRootElem = doc.RootElement();
	if (pRootElem != NULL)
	{
		TiXmlElement* pLog = pRootElem->FirstChildElement("Log");
		if (pLog != NULL)
		{
			// configure
			XnBool bOn;

			if (pLog->Attribute("writeToConsole"))
			{
				nRetVal = xnXmlReadBoolAttribute(pLog, "writeToConsole", &bOn);
				XN_IS_STATUS_OK(nRetVal);

				nRetVal = xnLogSetConsoleOutput(bOn);
				XN_IS_STATUS_OK(nRetVal);
			}

			if (pLog->Attribute("writeToFile"))
			{
				nRetVal = xnXmlReadBoolAttribute(pLog, "writeToFile", &bOn);
				XN_IS_STATUS_OK(nRetVal);

				nRetVal = xnLogSetFileOutput(bOn);
				XN_IS_STATUS_OK(nRetVal);
			}

			if (pLog->Attribute("writeLineInfo"))
			{
				nRetVal = xnXmlReadBoolAttribute(pLog, "writeLineInfo", &bOn);
				XN_IS_STATUS_OK(nRetVal);

				nRetVal = xnLogSetLineInfo(bOn);
				XN_IS_STATUS_OK(nRetVal);
			}
			TiXmlElement* pLogLevel = pLog->FirstChildElement("LogLevel");
			if (pLogLevel != NULL)
			{
				XnInt nValue;
				nRetVal = xnXmlReadIntAttribute(pLogLevel, "value", &nValue);
				XN_IS_STATUS_OK(nRetVal);

				nRetVal = xnLogSetSeverityFilter((XnLogSeverity)nValue);
				XN_IS_STATUS_OK(nRetVal);
			}

			TiXmlElement* pMasks = pLog->FirstChildElement("Masks");
			if (pMasks != NULL)
			{
				TiXmlElement* pMask = pMasks->FirstChildElement("Mask");
				while (pMask != NULL)
				{
					const XnChar* strName;
					nRetVal = xnXmlReadStringAttribute(pMask, "name", &strName);
					XN_IS_STATUS_OK(nRetVal);

					nRetVal = xnXmlReadBoolAttribute(pMask, "on", &bOn);
					XN_IS_STATUS_OK(nRetVal);

					nRetVal = xnLogSetMaskState(strName, bOn);
					XN_IS_STATUS_OK(nRetVal);

					pMask = pMask->NextSiblingElement("Mask");
				}
			}

			// Dumps
			TiXmlElement* pDumps = pLog->FirstChildElement("Dumps");
			if (pDumps != NULL)
			{
				TiXmlElement* pDump = pDumps->FirstChildElement("Dump");
				while (pDump != NULL)
				{
					const XnChar* strName;
					nRetVal = xnXmlReadStringAttribute(pDump, "name", &strName);
					XN_IS_STATUS_OK(nRetVal);

					nRetVal = xnXmlReadBoolAttribute(pDump, "on", &bOn);
					XN_IS_STATUS_OK(nRetVal);

					nRetVal = xnDumpSetMaskState(strName, bOn);
					XN_IS_STATUS_OK(nRetVal);

					pDump = pDump->NextSiblingElement("Dump");
				}
			}
		}
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLogStartNewFile()
{
	g_xnLoggerData.CloseFile();
	return xnLogInitSystem();
}

XN_C_API XnStatus xnLogClose()
{
	g_xnLoggerData.Reset();

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnLogSetMaskState(const XnChar* csMask, XnBool bEnabled)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (strcmp(csMask, XN_LOG_MASK_ALL) == 0)
	{
		g_xnLoggerData.m_nLogFilteringType = bEnabled ? XN_LOG_WRITE_ALL : XN_LOG_WRITE_NONE;
		xnLogConfigurationChanged();
	}
	else
	{
		g_xnLoggerData.m_nLogFilteringType = XN_LOG_WRITE_MASKS;

		if (bEnabled)
		{
			nRetVal = g_xnLoggerData.m_LogMasks.Set(csMask, NULL);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			XnValue val = NULL;
			nRetVal = g_xnLoggerData.m_LogMasks.Remove(csMask, val);
			if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_NO_MATCH)
			{
				return (XN_STATUS_OK);
			}
		}
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnDumpSetMaskState(const XnChar* csMask, XnBool bEnabled)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (strcmp(csMask, XN_LOG_MASK_ALL) == 0)
	{
		g_xnLoggerData.m_nDumpFilteringType = bEnabled ? XN_LOG_WRITE_ALL : XN_LOG_WRITE_NONE;
	}
	else
	{
		g_xnLoggerData.m_nDumpFilteringType = XN_LOG_WRITE_MASKS;

		if (bEnabled)
		{
			nRetVal = g_xnLoggerData.m_DumpMasks.Set(csMask, NULL);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			XnValue val = NULL;
			nRetVal = g_xnLoggerData.m_DumpMasks.Remove(csMask, val);
			if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_NO_MATCH)
			{
				return (XN_STATUS_OK);
			}
		}
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLogSetSeverityFilter(XnLogSeverity nMinSeverity)
{
	g_xnLoggerData.m_nFilterSeverity = nMinSeverity;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLogSetConsoleOutput(XnBool bConsoleOutput)
{
	g_xnLoggerData.m_bWriteToConsole = bConsoleOutput;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLogSetFileOutput(XnBool bFileOutput)
{
	g_xnLoggerData.m_bWriteToFile = bFileOutput;
	xnLogConfigurationChanged();
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLogSetLineInfo(XnBool bLineInfo)
{
	g_xnLoggerData.m_bWriteLineInfo = bLineInfo;
	xnLogConfigurationChanged();
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLogSetOutputFolder(const XnChar* strOutputFolder)
{
	g_xnLoggerData.CloseFile();
	sprintf(g_xnLoggerData.m_csLogDir, "%s%s", strOutputFolder, XN_FILE_DIR_SEP);
	xnLogInitSystem();
	return (XN_STATUS_OK);
}

XnBool xnLogIsMaskEnabledImpl(const XnChar* csLogMask)
{
	XN_VALIDATE_INPUT_PTR(csLogMask);

	switch (g_xnLoggerData.m_nLogFilteringType)
	{
	case XN_LOG_WRITE_ALL:
		return TRUE;
	case XN_LOG_WRITE_NONE:
		return FALSE;
	case XN_LOG_WRITE_MASKS:
		{
			XnStringsHash::Iterator it = g_xnLoggerData.m_LogMasks.end();
			return (XN_STATUS_OK == g_xnLoggerData.m_LogMasks.Find(csLogMask, it));
		}
	default:
		printf("Log: Unknown filter type: %d", g_xnLoggerData.m_nLogFilteringType);
		return FALSE;
	}
}

XN_C_API XnBool xnLogIsEnabled(const XnChar* csLogMask, XnLogSeverity nSeverity)
{
	// check severity
	if (nSeverity < g_xnLoggerData.m_nFilterSeverity)
		return FALSE;

	// check filtering policy
	if (!xnLogIsMaskEnabledImpl(csLogMask))
		return FALSE;

	return TRUE;
}

const XnChar* xnLogGetSeverityString(XnLogSeverity nSeverity)
{
	switch (nSeverity)
	{
	case XN_LOG_VERBOSE:
		return "VERBOSE";
	case XN_LOG_INFO:
		return "INFO";
	case XN_LOG_WARNING:
		return "WARNING";
	case XN_LOG_ERROR:
		return "ERROR";
	default:
		return "UNKNOWN";
	}
}

void xnLogWriteImplV(const XnChar* /*csLogMask*/, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, va_list args)
{
	// take time
	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);

	// write timestamp and severity
	const XnChar* csSeverity = xnLogGetSeverityString(nSeverity);

	const XnUInt32 nMaxMessageSize = 2047;
	XnChar csMessage[nMaxMessageSize+1];
	XnUInt32 nChars = 0;
	XnUInt32 nMessageLen = 0;

	// write timestamp
	xnOSStrFormat(csMessage + nMessageLen, nMaxMessageSize - nMessageLen, &nChars, "%8llu\t[%s]\t", nNow, csSeverity);
	nMessageLen += nChars;

	XnUInt32 nLineInfoBegin = nMessageLen;

	if (g_xnLoggerData.m_bWriteLineInfo)
	{
		// write line info
		XnChar fileName[XN_FILE_MAX_PATH];
		XnStatus nRetVal = xnOSGetFileName(csFile, fileName, sizeof(fileName));
		if (nRetVal == XN_STATUS_OK)
		{
			xnOSStrFormat(csMessage + nMessageLen, nMaxMessageSize - nMessageLen, &nChars, "%s\t%d\t", fileName, nLine);
			nMessageLen += nChars;
		}
	}

	XnUInt32 nLineInfoEnd = nMessageLen;

	// write message
	xnOSStrFormatV(csMessage + nMessageLen, nMaxMessageSize - nMessageLen, &nChars, csFormat, args);
	nMessageLen += nChars;
	XnUInt32 nUserMsgLen = nChars;

	// write end-of-line
	xnOSStrFormat(csMessage + nMessageLen, nMaxMessageSize - nMessageLen, &nChars, XN_NEW_LINE_SEP);
	nMessageLen += nChars;

	if (g_xnLoggerData.m_bWriteToFile)
	{
		xnOSWriteFile(g_xnLoggerData.m_fLogFile, csMessage, nMessageLen);
	}

	if (g_xnLoggerData.m_bWriteToConsole)
	{
		// Write to console without line info
		XnChar cSaveChar = csMessage[nLineInfoBegin];
		csMessage[nLineInfoBegin] = '\0';
		printf("%s", csMessage);
		csMessage[nLineInfoBegin] = cSaveChar;
		//printf should know how to translate '\n' to proper newline on every platform so we don't need XN_NEW_LINE_SEP here
		printf("%s", csMessage + nLineInfoEnd);
	}

}

XN_C_API void xnLogWrite(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, ...)
{
	if (!xnLogIsEnabled(csLogMask, nSeverity))
		return;

	// write message
	va_list args;
	va_start(args, csFormat);
	xnLogWriteImplV(csLogMask, nSeverity, csFile, nLine, csFormat, args);
	va_end(args);
}

XN_C_API void xnLogWriteNoEntry(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFormat, ...)
{
	if (!xnLogIsEnabled(csLogMask, nSeverity))
		return;

	if (g_xnLoggerData.m_bWriteToFile)
	{
		va_list args;
		va_start(args, csFormat);

		const XnUInt32 nMaxMessageSize = 1024;
		XnChar csMessage[nMaxMessageSize+1];
		XnUInt32 nChars;
		xnOSStrFormatV(csMessage, nMaxMessageSize, &nChars, csFormat, args);

		va_end(args);

		xnOSWriteFile(g_xnLoggerData.m_fLogFile, csMessage, nChars);
	}

	if (g_xnLoggerData.m_bWriteToConsole)
	{
		va_list args;
		va_start(args, csFormat);
		vprintf(csFormat, args);
		va_end(args);
	}
}

XN_C_API void xnLogWriteBinaryData(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, XnUChar* pBinData, XnUInt32 nDataSize, const XnChar* csFormat, ...)
{
	if (!xnLogIsEnabled(csLogMask, nSeverity))
		return;

	// first write preceding message:
	va_list args;
	va_start(args, csFormat);
	xnLogWriteImplV(csLogMask, nSeverity, csFile, nLine, csFormat, args);
	va_end(args);

	// now write binary data (in lines of 16 bytes)
	XnChar csLine[256];
	XnUInt32 pos = 0;

	for (XnUInt32 i = 0; i < nDataSize; ++i)
	{
		if ((i % 16) == 0) // first byte in line
		{
			// start a new line
			pos = sprintf(csLine, "%6u: ", i);
		}

		pos += sprintf(csLine + pos, "%02x ", pBinData[i]);

		if ((i % 16) == 15 || (i == nDataSize-1)) // last byte in line
		{
			xnLogWriteImpl(csLogMask, nSeverity, csFile, nLine, "%s", csLine);
		}
	}
}

XnBool xnLogIsDumpMaskEnabled(const XnChar* csDumpMask)
{
	XN_VALIDATE_INPUT_PTR(csDumpMask);

	switch (g_xnLoggerData.m_nDumpFilteringType)
	{
	case XN_LOG_WRITE_ALL:
		return TRUE;
	case XN_LOG_WRITE_NONE:
		return FALSE;
	case XN_LOG_WRITE_MASKS:
		{
			XnStringsHash::Iterator it = g_xnLoggerData.m_DumpMasks.end();
			return (XN_STATUS_OK == g_xnLoggerData.m_DumpMasks.Find(csDumpMask, it));
		}
	default:
		printf("Log: Unknown filter type: %d", g_xnLoggerData.m_nDumpFilteringType);
		return FALSE;
	}
}

void xnDumpCreateFileNameImpl(XnChar* csFullFileName, const XnChar* csFileNameFormat, va_list args)
{
	XnChar csFileName[XN_FILE_MAX_PATH];
	vsprintf(csFileName, csFileNameFormat, args);

	XN_PROCESS_ID nProcID = 0;
	xnOSGetCurrentProcessID(&nProcID);

	XnUInt32 nBytesWritten;
	xnOSStrFormat(csFullFileName, XN_FILE_MAX_PATH, &nBytesWritten, "%s%s_%d.%s", g_xnLoggerData.m_csLogDir, g_xnLoggerData.m_csTime, nProcID, csFileName);
}

void xnDumpCreate(XnDump* pDump, const XnChar* csHeader, const XnChar* csFileNameFormat, va_list args)
{
	XnChar strFileName[XN_FILE_MAX_PATH];
	xnDumpCreateFileNameImpl(strFileName, csFileNameFormat, args);

	if (XN_STATUS_OK != xnLogCreateFile(strFileName, &pDump->hFile))
	{
		// we don't have much to do if files can't be open. Dump will not be written
		xnLogWarning(XN_MASK_LOG, "Couldn't create dump file %s! Dump will not be written", strFileName);
	}

	if (csHeader != NULL)
	{
		xnDumpWriteStringImpl(*pDump, csHeader);
	}
}

XN_C_API void xnDumpInit(XnDump* pDump, const XnChar* csDumpMask, const XnChar* csHeader, const XnChar* csFileNameFormat, ...)
{
	if (pDump->hFile == XN_INVALID_FILE_HANDLE && xnLogIsDumpMaskEnabled(csDumpMask))
	{
		va_list args;
		va_start(args, csFileNameFormat);
		xnDumpCreate(pDump, csHeader, csFileNameFormat, args);
		va_end(args);
	}
}

XN_C_API void xnDumpForceInit(XnDump* pDump, const XnChar* csHeader, const XnChar* csFileNameFormat, ...)
{
	if (pDump->hFile == XN_INVALID_FILE_HANDLE)
	{
		va_list args;
		va_start(args, csFileNameFormat);
		xnDumpCreate(pDump, csHeader, csFileNameFormat, args);
		va_end(args);
	}
}

XN_C_API void xnDumpClose(XnDump* pDump)
{
	if (pDump->hFile != XN_INVALID_FILE_HANDLE)
	{
		xnOSCloseFile(&pDump->hFile);
		*pDump = XN_DUMP_CLOSED;
	}
}

XN_C_API void xnDumpWriteBufferImpl(XnDump dump, const void* pBuffer, XnUInt32 nBufferSize)
{
	if (dump.hFile != XN_INVALID_FILE_HANDLE) 
	{
		xnOSWriteFile(dump.hFile, pBuffer, nBufferSize);
	}
}

XN_C_API void xnDumpWriteStringImpl(XnDump dump, const XnChar* csFormat, ...)
{
	if (dump.hFile != XN_INVALID_FILE_HANDLE)
	{
		const XnUInt32 nStringLength = 1024;
		XnChar csString[1024];

		va_list args;
		va_start(args, csFormat);

		// format message
		XnUInt32 nChars;
		xnOSStrFormatV(csString, nStringLength, &nChars, csFormat, args);

		// and write it to file
		xnOSWriteFile(dump.hFile, csString, nChars);

		va_end(args);
	}
}

XN_C_API void xnDumpFlush(XnDump dump)
{
	if (dump.hFile != XN_INVALID_FILE_HANDLE)
	{
		xnOSFlushFile(dump.hFile);
	}
}
