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
#include <XnLog.h>
#include <XnStringsHash.h>
#include <XnVersion.h>
#include <stdarg.h>
#include "XnXml.h"
#include <XnList.h>

#include "XnLogConsoleWriter.h"
#include "XnLogFileWriter.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_LOG_MASKS_DELIM			";"
#define XN_LOG_MAX_MESSAGE_LENGTH	2048

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
XN_DECLARE_LIST(const XnLogWriter*, XnLogWritersList);

class XnBufferedLogEntry : public XnLogEntry
{
public:
	XnBufferedLogEntry()
	{
		this->strMessage = m_strBuffer;
	}

	XnChar* Buffer() { return m_strBuffer; }

private:
	XnChar m_strBuffer[XN_LOG_MAX_MESSAGE_LENGTH];
};

class LogData
{
public:
	LogData()
	{
		Reset();
	}

	~LogData()
	{
		// This is important. During process shutdown all static objects are destroyed, including log objects
		// (like writers list). But the order can't be controlled, so some objects might be destroyed *after*
		// log has. Those objects might write down to the log during destruction which will cause access violation.
		// So, when the log is destroyed, we're turning it off, so that no writing will take place.
		Reset();
	}

	void Reset()
	{
		m_strLogDir[0] = '\0';
		m_nLogFilteringType = XN_LOG_WRITE_NONE;
		m_nFilterSeverity = XN_LOG_ERROR;
		m_strTime[0] = '\0';
	}

	XnChar m_strLogDir[XN_FILE_MAX_PATH];
	XnStringsHash m_LogMasks;
	XnLogFilteringType m_nLogFilteringType;
	XnLogSeverity m_nFilterSeverity;
	XnLogWritersList m_writers;
	XnChar m_strTime[25];

	// Writers
	XnLogConsoleWriter m_consoleWriter;
	XnLogFileWriter m_fileWriter;
};

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
static LogData g_logData;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
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

void xnLogCreateEntryV(XnBufferedLogEntry* pEntry, const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, va_list args)
{
	// format message
	const XnUInt32 nMaxMessageSize = 2047;
	XnUInt32 nChars;
	xnOSStrFormatV(pEntry->Buffer(), XN_LOG_MAX_MESSAGE_LENGTH, &nChars, csFormat, args);

	// create log entry
	xnOSGetHighResTimeStamp(&pEntry->nTimestamp);
	pEntry->nSeverity = nSeverity;
	pEntry->strSeverity = xnLogGetSeverityString(nSeverity);
	pEntry->strMask = csLogMask;
	pEntry->strFile = csFile;
	pEntry->nLine = nLine;
}

void xnLogCreateEntry(XnBufferedLogEntry* pEntry, const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, ...)
{
	va_list args;
	va_start(args, csFormat);
	xnLogCreateEntryV(pEntry, csLogMask, nSeverity, csFile, nLine, csFormat, args);
	va_end(args);
}

void xnLogGetMasksString(XnChar* csString)
{
	switch (g_logData.m_nLogFilteringType)
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

			for (XnStringsHash::Iterator it = g_logData.m_LogMasks.begin(); it != g_logData.m_LogMasks.end(); ++it)
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

void xnLogWriteEntry(XnLogEntry* pEntry)
{
	for (XnLogWritersList::ConstIterator it = g_logData.m_writers.begin(); it != g_logData.m_writers.end(); ++it)
	{
		const XnLogWriter* pWriter = *it;
		pWriter->WriteEntry(pEntry, pWriter->pCookie);
	}
}

void xnLogWriteImplV(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, va_list args)
{
	// check if there are any writers registered
	if (g_logData.m_writers.IsEmpty())
	{
		// don't waste time formatting anything.
		return;
	}

	XnBufferedLogEntry entry;
	xnLogCreateEntryV(&entry, csLogMask, nSeverity, csFile, nLine, csFormat, args);

	// write it down
	xnLogWriteEntry(&entry);
}

void xnLogWriteImpl(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, ...)
{
	va_list args;
	va_start(args, csFormat);
	xnLogWriteImplV(csLogMask, nSeverity, csFile, nLine, csFormat, args);
	va_end(args);
}

XN_C_API XnStatus xnLogCreateFile(const XnChar* csFileName, XN_FILE_HANDLE* phFile)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// set log directory
	if (g_logData.m_strLogDir[0] == '\0')
	{
		nRetVal = xnLogSetOutputFolder(XN_LOG_DIR_NAME);
		XN_IS_STATUS_OK(nRetVal);
	}

	if (g_logData.m_strTime[0] == '\0')
	{
		time_t currtime;
		time(&currtime);
		strftime(g_logData.m_strTime, sizeof(g_logData.m_strTime)-1, "%Y_%m_%d__%H_%M_%S", localtime(&currtime)); 
	}

	XN_PROCESS_ID nProcID = 0;
	xnOSGetCurrentProcessID(&nProcID);

	// create full path file name - add process start time and process ID
	XnChar strFilePath[XN_FILE_MAX_PATH];
	sprintf(strFilePath, "%s%s_%u.%s", g_logData.m_strLogDir, g_logData.m_strTime, nProcID, csFileName);

	// and open the file
	nRetVal = xnOSOpenFile(strFilePath, XN_OS_FILE_WRITE | XN_OS_FILE_TRUNCATE, phFile);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

void xnLogCreateFilterChangedMessage(XnBufferedLogEntry* pEntry)
{
	XnChar csMasks[XN_LOG_MASKS_STRING_LEN];
	xnLogGetMasksString(csMasks);
	xnLogCreateEntry(pEntry, XN_MASK_LOG, XN_LOG_INFO, __FILE__, __LINE__, "Filter Info - minimum severity: %s, masks: %s", xnLogGetSeverityString(g_logData.m_nFilterSeverity), csMasks);
}

void xnLogFilterChanged()
{
	XnBufferedLogEntry entry;
	xnLogCreateFilterChangedMessage(&entry);
	xnLogWriteEntry(&entry);

	for (XnLogWritersList::ConstIterator it = g_logData.m_writers.begin(); it != g_logData.m_writers.end(); ++it)
	{
		const XnLogWriter* pWriter = *it;
		pWriter->OnConfigurationChanged(pWriter->pCookie);
	}
}

XN_C_API void xnLogWriteBanner(const XnLogWriter* pWriter)
{
	XnBufferedLogEntry entry;

	// write version
	xnLogCreateEntry(&entry, XN_MASK_LOG, XN_LOG_INFO, __FILE__, __LINE__, "OpenNI version is %s", XN_VERSION_STRING);
	pWriter->WriteEntry(&entry, pWriter->pCookie);

	// write filters
	xnLogCreateFilterChangedMessage(&entry);
	pWriter->WriteEntry(&entry, pWriter->pCookie);
}

XN_C_API XnStatus xnLogInitSystem()
{
	// make sure xnOS is initialized (we need timers)
	XnStatus nRetVal = xnOSInit();
	if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_OS_ALREADY_INIT)
	{
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

typedef XnStatus (*XnSetMaskFunc)(const XnChar* csMask, XnBool bEnabled);

XnStatus xnLogSetMasks(XnChar* csMasks, XnSetMaskFunc pSetMaskFunc)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnLogSetMaskState(XN_LOG_MASK_ALL, FALSE);
	XN_IS_STATUS_OK(nRetVal);

	XnChar* pMask = strtok(csMasks, XN_LOG_MASKS_DELIM);
	while (pMask != NULL)
	{
		nRetVal = pSetMaskFunc(pMask, TRUE);
		XN_IS_STATUS_OK(nRetVal);

		// get next
		pMask = strtok(NULL, XN_LOG_MASKS_DELIM);
	}

	return (XN_STATUS_OK);
}

XnStatus xnLogReadMasksFromINI(const XnChar* cpINIFileName, const XnChar* cpSectionName, const XnChar* cpKey, XnSetMaskFunc pSetMaskFunc)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnChar csTemp[XN_INI_MAX_LEN] = "";

	nRetVal = xnOSReadStringFromINI(cpINIFileName, cpSectionName, cpKey, csTemp, XN_INI_MAX_LEN);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = xnLogSetMasks(csTemp, pSetMaskFunc);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLogInitFromINIFile(const XnChar* cpINIFileName, const XnChar* cpSectionName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XnUInt32 nTemp;

	nRetVal = xnLogInitSystem();
	XN_IS_STATUS_OK(nRetVal);

	// read filters
	xnLogReadMasksFromINI(cpINIFileName, cpSectionName, "LogMasks", xnLogSetMaskState);
	xnLogReadMasksFromINI(cpINIFileName, cpSectionName, "DumpMasks", xnDumpSetMaskState);

	g_logData.m_nFilterSeverity = XN_LOG_ERROR;
	nRetVal = xnOSReadIntFromINI(cpINIFileName, cpSectionName, "LogLevel", &nTemp);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = xnLogSetSeverityFilter((XnLogSeverity)nTemp);
		XN_IS_STATUS_OK(nRetVal);
	}

	// configure writers
	nRetVal = xnOSReadIntFromINI(cpINIFileName, cpSectionName, "LogWriteToConsole", &nTemp);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = xnLogSetConsoleOutput(nTemp);
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = xnOSReadIntFromINI(cpINIFileName, cpSectionName, "LogWriteToFile", &nTemp);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = xnLogSetFileOutput(nTemp);
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = xnOSReadIntFromINI(cpINIFileName, cpSectionName, "LogWriteLineInfo", &nTemp);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = xnLogSetLineInfo(nTemp);
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
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
			XnBool bOn;

			// configure filters
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

			// configure writers
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

XN_C_API XnStatus xnLogRegisterLogWriter(const XnLogWriter* pWriter)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = g_logData.m_writers.AddLast(pWriter);
	XN_IS_STATUS_OK(nRetVal);

	xnLogWriteBanner(pWriter);
	
	return (XN_STATUS_OK);
}

XN_C_API void xnLogUnregisterLogWriter(const XnLogWriter* pWriter)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = g_logData.m_writers.Remove(pWriter);
	XN_ASSERT(nRetVal == XN_STATUS_OK);
}

XN_C_API XnStatus xnLogStartNewFile()
{
	if (!g_logData.m_fileWriter.IsRegistered())
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	g_logData.m_fileWriter.Unregister();
	return g_logData.m_fileWriter.Register();
}

XN_C_API XnStatus xnLogClose()
{
	// notify all writers (while allowing them to unregister themselves)
	XnLogWritersList::ConstIterator it = g_logData.m_writers.begin();
	while (it != g_logData.m_writers.end())
	{
		XnLogWritersList::ConstIterator curr = it;
		++it;

		const XnLogWriter* pWriter = *curr;
		pWriter->OnClosing(pWriter->pCookie);
	}

	g_logData.m_strLogDir[0] = '\0';
	g_logData.m_strTime[0] = '\0';
	g_logData.m_LogMasks.Clear();
	g_logData.m_nLogFilteringType = XN_LOG_WRITE_NONE;
	g_logData.m_nFilterSeverity = XN_LOG_ERROR;

	// turn off all dumps
	xnDumpSetMaskState(XN_LOG_MASK_ALL, FALSE);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnLogSetMaskState(const XnChar* csMask, XnBool bEnabled)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (strcmp(csMask, XN_LOG_MASK_ALL) == 0)
	{
		XnLogFilteringType filter = bEnabled ? XN_LOG_WRITE_ALL : XN_LOG_WRITE_NONE;

		if (g_logData.m_nLogFilteringType != filter)
		{
			g_logData.m_nLogFilteringType = filter;
			xnLogFilterChanged();
		}
	}
	else
	{
		g_logData.m_nLogFilteringType = XN_LOG_WRITE_MASKS;

		XnStringsHash::ConstIterator it = g_logData.m_LogMasks.end();
		g_logData.m_LogMasks.Find(csMask, it);

		if (bEnabled && it == g_logData.m_LogMasks.end())
		{
			nRetVal = g_logData.m_LogMasks.Set(csMask, NULL);
			XN_IS_STATUS_OK(nRetVal);

			xnLogFilterChanged();
		}
		else if (!bEnabled && it != g_logData.m_LogMasks.end())
		{
			nRetVal = g_logData.m_LogMasks.Remove(it);
			XN_IS_STATUS_OK(nRetVal);

			xnLogFilterChanged();
		}
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLogSetSeverityFilter(XnLogSeverity nMinSeverity)
{
	if (g_logData.m_nFilterSeverity != nMinSeverity)
	{
		g_logData.m_nFilterSeverity = nMinSeverity;
		xnLogFilterChanged();
	}
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLogSetConsoleOutput(XnBool bConsoleOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (bConsoleOutput)
	{
		nRetVal = g_logData.m_consoleWriter.Register();
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		g_logData.m_consoleWriter.Unregister();
	}

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnLogSetFileOutput(XnBool bFileOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (bFileOutput)
	{
		nRetVal = g_logData.m_fileWriter.Register();
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		g_logData.m_fileWriter.Unregister();
	}

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnLogSetLineInfo(XnBool bLineInfo)
{
	g_logData.m_fileWriter.SetLineInfo(bLineInfo);
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLogSetOutputFolder(const XnChar* strOutputFolder)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// check if folder exists
	XnBool bDirExists = FALSE;
	nRetVal = xnOSDoesDirecotyExist(strOutputFolder, &bDirExists);
	XN_IS_STATUS_OK(nRetVal);

	if (!bDirExists)
	{
		// create it
		nRetVal = xnOSCreateDirectory(strOutputFolder);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	// place it in a temp buffer, just to make sure everything succeeds
	XnChar strDirName[XN_FILE_MAX_PATH];
	nRetVal = xnOSGetFullPathName(strOutputFolder, strDirName, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSStrAppend(strDirName, XN_FILE_DIR_SEP, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	// OK. replace
	xnOSStrCopy(g_logData.m_strLogDir, strDirName, XN_FILE_MAX_PATH);

	// restart file writer
	xnLogStartNewFile();

	return (XN_STATUS_OK);
}

XnBool xnLogIsMaskEnabledImpl(const XnChar* csLogMask)
{
	XN_VALIDATE_INPUT_PTR(csLogMask);

	switch (g_logData.m_nLogFilteringType)
	{
	case XN_LOG_WRITE_ALL:
		return TRUE;
	case XN_LOG_WRITE_NONE:
		return FALSE;
	case XN_LOG_WRITE_MASKS:
		{
			XnStringsHash::Iterator it = g_logData.m_LogMasks.end();
			return (XN_STATUS_OK == g_logData.m_LogMasks.Find(csLogMask, it));
		}
	default:
		printf("Log: Unknown filter type: %d", g_logData.m_nLogFilteringType);
		return FALSE;
	}
}

XN_C_API XnBool xnLogIsEnabled(const XnChar* csLogMask, XnLogSeverity nSeverity)
{
	// check severity
	if (nSeverity < g_logData.m_nFilterSeverity)
		return FALSE;

	// check filtering policy
	if (!xnLogIsMaskEnabledImpl(csLogMask))
		return FALSE;

	return TRUE;
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

	va_list args;
	va_start(args, csFormat);

	const XnUInt32 nMaxMessageSize = 1024;
	XnChar csMessage[nMaxMessageSize+1];
	XnUInt32 nChars;
	xnOSStrFormatV(csMessage, nMaxMessageSize, &nChars, csFormat, args);

	va_end(args);

	for (XnLogWritersList::ConstIterator it = g_logData.m_writers.begin(); it != g_logData.m_writers.end(); ++it)
	{
		const XnLogWriter* pWriter = *it;
		pWriter->WriteUnformatted(csMessage, pWriter->pCookie);
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
