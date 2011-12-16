/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
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
#include <XnArray.h>

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

XN_DECLARE_STRINGS_HASH(XnLogger, XnLogMasksHash);

class XnBufferedLogEntry : public XnLogEntry
{
public:
	XnBufferedLogEntry()
	{
		this->strMessage = m_strBuffer;
	}

	XnChar* Buffer() { return m_strBuffer; }
	XnUInt32 MaxBufferSize() const { return XN_LOG_MAX_MESSAGE_LENGTH; }

private:
	XnChar m_strBuffer[XN_LOG_MAX_MESSAGE_LENGTH];
};

class LogData
{
public:
	static LogData& GetInstance()
	{
		static LogData data;
		return data;
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
		SetMinSeverityGlobally(XN_LOG_SEVERITY_NONE);
		this->strLogDir[0] = '\0';
		this->strSessionTimestamp[0] = '\0';
	}

	void SetMinSeverityGlobally(XnLogSeverity severity)
	{
		this->defaultMinSeverity = severity;
		for (XnLogMasksHash::Iterator it = this->pMasksHash->begin(); it != this->pMasksHash->end(); ++it)
		{
			it.Value().nMinSeverity = severity;
		}
	}

	XnChar strLogDir[XN_FILE_MAX_PATH];
	XnLogMasksHash* pMasksHash;
	XnLogSeverity defaultMinSeverity;
	XnLogWritersList writers;
	XnChar strSessionTimestamp[25];

	// Writers
	XnLogConsoleWriter consoleWriter;
	XnLogFileWriter fileWriter;

private:
	LogData()
	{
		// NOTE: as our log implementation returns a pointer directly into this hash, we can't
		// free this memory in our dtor (static objects dtors are called in unknown order. modules
		// might still access this memory after our dtor is called).
		// As in any case, this is a static object which will only be destroyed when the process goes 
		// down - we can allow this.
		this->pMasksHash = XN_NEW(XnLogMasksHash);
		Reset();
	}
};

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
XnLogger* XN_LOGGER_RETVAL_CHECKS = xnLoggerOpen(XN_MASK_RETVAL_CHECKS);

//---------------------------------------------------------------------------
// Forward-Declaration
//---------------------------------------------------------------------------
static XnStatus xnLogBCSetMaskState(const XnChar* strMask, XnBool bEnabled);
static XnStatus xnLogBCSetSeverityFilter(XnLogSeverity nMinSeverity);

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
static const XnChar* xnLogGetSeverityString(XnLogSeverity nSeverity)
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
	case XN_LOG_SEVERITY_NONE:
		return "NONE";
	default:
		return "UNKNOWN";
	}
}

static void xnLogCreateEntryV(XnBufferedLogEntry* pEntry, const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, va_list args)
{
	// format message
	XnUInt32 nChars;
	xnOSStrFormatV(pEntry->Buffer(), pEntry->MaxBufferSize(), &nChars, csFormat, args);

	// create log entry
	xnOSGetHighResTimeStamp(&pEntry->nTimestamp);
	pEntry->nSeverity = nSeverity;
	pEntry->strSeverity = xnLogGetSeverityString(nSeverity);
	pEntry->strMask = csLogMask;
	pEntry->strFile = csFile;
	pEntry->nLine = nLine;
}

static void xnLogCreateEntry(XnBufferedLogEntry* pEntry, const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, ...)
{
	va_list args;
	va_start(args, csFormat);
	xnLogCreateEntryV(pEntry, csLogMask, nSeverity, csFile, nLine, csFormat, args);
	va_end(args);
}

static void xnLogWriteEntry(XnLogEntry* pEntry)
{
	LogData& logData = LogData::GetInstance();
	for (XnLogWritersList::ConstIterator it = logData.writers.begin(); it != logData.writers.end(); ++it)
	{
		const XnLogWriter* pWriter = *it;
		pWriter->WriteEntry(pEntry, pWriter->pCookie);
	}
}

static void xnLogWriteImplV(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, va_list args)
{
	// check if there are any writers registered
	LogData& logData = LogData::GetInstance();
	if (logData.writers.IsEmpty())
	{
		// don't waste time formatting anything.
		return;
	}

	XnBufferedLogEntry entry;
	xnLogCreateEntryV(&entry, csLogMask, nSeverity, csFile, nLine, csFormat, args);

	// write it down
	xnLogWriteEntry(&entry);
}

static void xnLogWriteImpl(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, ...)
{
	va_list args;
	va_start(args, csFormat);
	xnLogWriteImplV(csLogMask, nSeverity, csFile, nLine, csFormat, args);
	va_end(args);
}

XN_C_API XnStatus XN_C_DECL xnLogCreateFileEx(const XnChar* csFileName, XnBool bSessionBased, XN_FILE_HANDLE* phFile)
{
	XnStatus nRetVal = XN_STATUS_OK;

	LogData& logData = LogData::GetInstance();

	// set log directory
	if (logData.strLogDir[0] == '\0')
	{
		nRetVal = xnLogSetOutputFolder(XN_LOG_DIR_NAME);
		XN_IS_STATUS_OK(nRetVal);
	}

	if (logData.strSessionTimestamp[0] == '\0')
	{
		time_t currtime;
		time(&currtime);
		strftime(logData.strSessionTimestamp, sizeof(logData.strSessionTimestamp)-1, "%Y_%m_%d__%H_%M_%S", localtime(&currtime)); 
	}

	XN_PROCESS_ID nProcID = 0;
	xnOSGetCurrentProcessID(&nProcID);

	// create full path file name - add process start time and process ID
	XnChar strFilePath[XN_FILE_MAX_PATH];
	XnUInt32 nPathSize = 0;
	XnUInt32 nCharsWritten = 0;
	nRetVal = xnOSStrFormat(strFilePath, XN_FILE_MAX_PATH - nPathSize, &nCharsWritten, "%s", logData.strLogDir);
	XN_IS_STATUS_OK(nRetVal);
	nPathSize += nCharsWritten;

	if (bSessionBased)
	{
		nRetVal = xnOSStrFormat(strFilePath + nPathSize, XN_FILE_MAX_PATH - nPathSize, &nCharsWritten, "%s_%u.", logData.strSessionTimestamp, nProcID);
		XN_IS_STATUS_OK(nRetVal);
		nPathSize += nCharsWritten;
	}

	nRetVal = xnOSStrFormat(strFilePath + nPathSize, XN_FILE_MAX_PATH - nPathSize, &nCharsWritten, "%s", csFileName);
	XN_IS_STATUS_OK(nRetVal);
	nPathSize += nCharsWritten;

	// and open the file
	nRetVal = xnOSOpenFile(strFilePath, XN_OS_FILE_WRITE | XN_OS_FILE_TRUNCATE, phFile);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnLogCreateFile(const XnChar* csFileName, XN_FILE_HANDLE* phFile)
{
	return xnLogCreateFileEx(csFileName, TRUE, phFile);
}

static void xnLogCreateFilterChangedMessage(XnBufferedLogEntry* pEntry)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	LogData& logData = LogData::GetInstance();

	XnChar strConfigMessage[XN_LOG_MAX_MESSAGE_LENGTH];
	XnUInt32 nCharsWritten = 0;
	XnUInt32 nMessageLength = 0;
	nRetVal = xnOSStrFormat(strConfigMessage, XN_LOG_MAX_MESSAGE_LENGTH, &nCharsWritten, "--- Filter Info --- Minimum Severity: %s", xnLogGetSeverityString(logData.defaultMinSeverity));
	XN_ASSERT(nRetVal == XN_STATUS_OK);
	nMessageLength += nCharsWritten;

	XnBool bOverrides = FALSE;

	// go over masks, and see if there are any overrides
	for (XnLogMasksHash::ConstIterator it = logData.pMasksHash->begin(); it != logData.pMasksHash->end(); ++it)
	{
		XnLogSeverity maskSeverity = it.Value().nMinSeverity;
		if (maskSeverity != logData.defaultMinSeverity)
		{
			if (!bOverrides)
			{
				nRetVal = xnOSStrFormat(strConfigMessage + nMessageLength, sizeof(strConfigMessage) - nMessageLength, &nCharsWritten, ". Overriding Masks - ");
				XN_ASSERT(nRetVal == XN_STATUS_OK);
				bOverrides = TRUE;
				nMessageLength += nCharsWritten;
			}
			else
			{
				nRetVal = xnOSStrFormat(strConfigMessage + nMessageLength, sizeof(strConfigMessage) - nMessageLength, &nCharsWritten, ", ");
				XN_ASSERT(nRetVal == XN_STATUS_OK);
				nMessageLength += nCharsWritten;
			}

			nRetVal = xnOSStrFormat(strConfigMessage + nMessageLength, sizeof(strConfigMessage) - nMessageLength, &nCharsWritten, "'%s': %s", it.Key(), xnLogGetSeverityString(maskSeverity));
			XN_ASSERT(nRetVal == XN_STATUS_OK);
			nMessageLength += nCharsWritten;
		}
	}

	xnLogCreateEntry(pEntry, XN_MASK_LOG, XN_LOG_INFO, __FILE__, __LINE__, "%s", strConfigMessage);
}

static void xnLogFilterChanged()
{
	XnBufferedLogEntry entry;
	xnLogCreateFilterChangedMessage(&entry);
	xnLogWriteEntry(&entry);

	LogData& logData = LogData::GetInstance();
	for (XnLogWritersList::ConstIterator it = logData.writers.begin(); it != logData.writers.end(); ++it)
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

static XnStatus xnLogSetMasks(XnChar* csMasks, XnSetMaskFunc pSetMaskFunc)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnLogBCSetMaskState(XN_LOG_MASK_ALL, FALSE);
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

static XnStatus xnLogReadMasksFromINI(const XnChar* cpINIFileName, const XnChar* cpSectionName, const XnChar* cpKey, XnSetMaskFunc pSetMaskFunc)
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
	xnLogReadMasksFromINI(cpINIFileName, cpSectionName, "LogMasks", xnLogBCSetMaskState);
	xnLogReadMasksFromINI(cpINIFileName, cpSectionName, "DumpMasks", xnDumpSetMaskState);

	LogData::GetInstance().Reset();
	nRetVal = xnOSReadIntFromINI(cpINIFileName, cpSectionName, "LogLevel", &nTemp);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = xnLogBCSetSeverityFilter((XnLogSeverity)nTemp);
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

				nRetVal = xnLogBCSetSeverityFilter((XnLogSeverity)nValue);
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

					nRetVal = xnLogBCSetMaskState(strName, bOn);
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
	
	LogData& logData = LogData::GetInstance();
	nRetVal = logData.writers.AddLast(pWriter);
	XN_IS_STATUS_OK(nRetVal);

	xnLogWriteBanner(pWriter);
	
	return (XN_STATUS_OK);
}

XN_C_API void xnLogUnregisterLogWriter(const XnLogWriter* pWriter)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	LogData& logData = LogData::GetInstance();
	nRetVal = logData.writers.Remove(pWriter);
	XN_ASSERT(nRetVal == XN_STATUS_OK);
}

XN_C_API XnStatus xnLogStartNewFile()
{
	LogData& logData = LogData::GetInstance();
	if (!logData.fileWriter.IsRegistered())
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	logData.fileWriter.Unregister();
	return logData.fileWriter.Register();
}

XN_C_API XnStatus xnLogClose()
{
	// notify all writers (while allowing them to unregister themselves)
	LogData& logData = LogData::GetInstance();
	XnLogWritersList::ConstIterator it = logData.writers.begin();
	while (it != logData.writers.end())
	{
		XnLogWritersList::ConstIterator curr = it;
		++it;

		const XnLogWriter* pWriter = *curr;
		pWriter->OnClosing(pWriter->pCookie);
	}

	logData.strLogDir[0] = '\0';
	logData.strSessionTimestamp[0] = '\0';
	logData.pMasksHash->Clear();
	logData.defaultMinSeverity = XN_LOG_SEVERITY_NONE;

	// turn off all dumps
	xnDumpSetMaskState(XN_LOG_MASK_ALL, FALSE);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnLogSetConsoleOutput(XnBool bConsoleOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	LogData& logData = LogData::GetInstance();
	if (bConsoleOutput)
	{
		nRetVal = logData.consoleWriter.Register();
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		logData.consoleWriter.Unregister();
	}

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnLogSetFileOutput(XnBool bFileOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	LogData& logData = LogData::GetInstance();
	if (bFileOutput)
	{
		nRetVal = logData.fileWriter.Register();
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		logData.fileWriter.Unregister();
	}

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnLogSetLineInfo(XnBool bLineInfo)
{
	LogData& logData = LogData::GetInstance();
	logData.fileWriter.SetLineInfo(bLineInfo);
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
	LogData& logData = LogData::GetInstance();
	xnOSStrCopy(logData.strLogDir, strDirName, XN_FILE_MAX_PATH);

	// restart file writer
	xnLogStartNewFile();

	return (XN_STATUS_OK);
}

XnLogger* xnLogGetLoggerForMask(const XnChar* csLogMask, XnBool bCreate)
{
	XnLogger* pLogger = NULL;
	LogData& logData = LogData::GetInstance();
	if (XN_STATUS_OK == logData.pMasksHash->Get(csLogMask, pLogger))
	{
		return pLogger;
	}
	else if (bCreate)
	{
		XnLogger logger;
		logger.nMinSeverity = logData.defaultMinSeverity;

		// first of all, add it to the map
		if (XN_STATUS_OK != logData.pMasksHash->Set(csLogMask, logger))
		{
			// failed to add it to the map
			XN_ASSERT(FALSE);
			return NULL;
		}

		// now find it in the map
		XnLogMasksHash::Iterator it = logData.pMasksHash->end();
		if (XN_STATUS_OK != logData.pMasksHash->Find(csLogMask, it))
		{
			XN_ASSERT(FALSE);
			return NULL;
		}

		it.Value().pInternal = (void*)it.Key();

		return &it.Value();
	}
	else
	{
		return NULL;
	}
}

void xnLogWriteNoEntryImplV(const XnChar* csFormat, va_list args)
{
	const XnUInt32 nMaxMessageSize = 1024;
	XnChar csMessage[nMaxMessageSize+1];
	XnUInt32 nChars;
	xnOSStrFormatV(csMessage, nMaxMessageSize, &nChars, csFormat, args);

	LogData& logData = LogData::GetInstance();
	for (XnLogWritersList::ConstIterator it = logData.writers.begin(); it != logData.writers.end(); ++it)
	{
		const XnLogWriter* pWriter = *it;
		pWriter->WriteUnformatted(csMessage, pWriter->pCookie);
	}
}

void xnLogWriteBinaryDataImplV(const XnChar* strMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, XnUChar* pBinData, XnUInt32 nDataSize, const XnChar* csFormat, va_list args)
{
	// first write preceding message:
	xnLogWriteImplV(strMask, nSeverity, csFile, nLine, csFormat, args);

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
			xnLogWriteImpl(strMask, nSeverity, csFile, nLine, "%s", csLine);
		}
	}
}

XN_C_API XnStatus XN_C_DECL xnLogSetMaskMinSeverity(const XnChar* strMask, XnLogSeverity minSeverity)
{
	LogData& logData = LogData::GetInstance();
	if (strcmp(strMask, XN_LOG_MASK_ALL) == 0)
	{
		logData.SetMinSeverityGlobally(minSeverity);
	}
	else
	{
		XnLogger* pLogger = xnLogGetLoggerForMask(strMask, TRUE);
		if (pLogger == NULL)
		{
			XN_ASSERT(FALSE);
			return XN_STATUS_ERROR;
		}

		pLogger->nMinSeverity = minSeverity;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnLogSeverity XN_C_DECL xnLogGetMaskMinSeverity(const XnChar* strMask)
{
	XnLogger* pLogger = xnLogGetLoggerForMask(strMask, FALSE);
	if (pLogger == NULL)
	{
		LogData& logData = LogData::GetInstance();
		return logData.defaultMinSeverity;
	}
	else
	{
		return pLogger->nMinSeverity;
	}
}

XN_C_API XnLogger* XN_C_DECL xnLoggerOpen(const XnChar* strMask)
{
	return xnLogGetLoggerForMask(strMask, TRUE);
}

XN_C_API void XN_C_DECL xnLoggerWrite(XnLogger* pLogger, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, ...)
{
	if (!xnLoggerIsEnabled(pLogger, nSeverity))
		return;

	const XnChar* strMask = (const XnChar*)pLogger->pInternal;

	// write message
	va_list args;
	va_start(args, csFormat);
	xnLogWriteImplV(strMask, nSeverity, csFile, nLine, csFormat, args);
	va_end(args);
}

XN_C_API void XN_C_DECL xnLoggerWriteNoEntry(XnLogger* pLogger, XnLogSeverity nSeverity, const XnChar* csFormat, ...)
{
	if (!xnLoggerIsEnabled(pLogger, nSeverity))
		return;

	va_list args;
	va_start(args, csFormat);
	xnLogWriteNoEntryImplV(csFormat, args);
	va_end(args);
}

XN_C_API void XN_C_DECL xnLoggerWriteBinaryData(XnLogger* pLogger, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, XnUChar* pBinData, XnUInt32 nDataSize, const XnChar* csFormat, ...)
{
	if (!xnLoggerIsEnabled(pLogger, nSeverity))
		return;

	const XnChar* strMask = (const XnChar*)pLogger->pInternal;

	va_list args;
	va_start(args, csFormat);
	xnLogWriteBinaryDataImplV(strMask, nSeverity, csFile, nLine, pBinData, nDataSize, csFormat, args);
	va_end(args);
}

XN_C_API void XN_C_DECL _xnLoggerClose(XnLogger* pLogger)
{
	if (pLogger == NULL)
	{
		return;
	}

	const XnChar* strMask = (const XnChar*)pLogger->pInternal;
	LogData& logData = LogData::GetInstance();
	logData.pMasksHash->Remove(strMask);
}

XN_C_API XnBool XN_C_DECL xnLoggerIsEnabled(XnLogger* pLogger, XnLogSeverity severity)
{
	if (pLogger == NULL)
	{
		return FALSE;
	}
	else
	{
		return (severity >= pLogger->nMinSeverity);
	}
}

#ifndef __XN_NO_BC__

//---------------------------------------------------------------------------
// Backwards-Compatibility Code
//---------------------------------------------------------------------------

// Note: in old log implementation, you could first set global min severity, and
// then turn on/off masks. To achieve the same behavior, a BC "turned-off" mask
// will have the MSB turned on, so that it has a very high min severity, and nothing
// will be output.
#define XN_LOG_BC_MASK_OFF_FLAG		(1 << 30)

static XnLogSeverity xnLogBCCalcSeverityByState(XnLogSeverity severity, XnBool bEnabled)
{
	if (bEnabled)
	{
		return XnLogSeverity(severity & ~XN_LOG_BC_MASK_OFF_FLAG);
	}
	else
	{
		return XnLogSeverity(severity | XN_LOG_BC_MASK_OFF_FLAG);
	}
}

static XnStatus xnLogBCSetMaskState(const XnChar* strMask, XnBool bEnabled)
{
	LogData& logData = LogData::GetInstance();
	if (strcmp(strMask, XN_LOG_MASK_ALL) == 0)
	{
		XnLogSeverity newGlobalSeverity = xnLogBCCalcSeverityByState(logData.defaultMinSeverity, bEnabled);
		logData.SetMinSeverityGlobally(newGlobalSeverity);
	}
	else
	{
		XnLogger* pLogger = xnLogGetLoggerForMask(strMask, TRUE);
		if (pLogger == NULL)
		{
			XN_ASSERT(FALSE);
			return XN_STATUS_ERROR;
		}

		pLogger->nMinSeverity = xnLogBCCalcSeverityByState(pLogger->nMinSeverity, bEnabled);
	}

	return (XN_STATUS_OK);
}

static XnStatus xnLogBCSetSeverityFilter(XnLogSeverity nMinSeverity)
{
	LogData& logData = LogData::GetInstance();
	if ((logData.defaultMinSeverity & XN_LOG_BC_MASK_OFF_FLAG) != 0)
	{
		nMinSeverity = XnLogSeverity(nMinSeverity | XN_LOG_BC_MASK_OFF_FLAG);
	}

	logData.SetMinSeverityGlobally(nMinSeverity);
	xnLogFilterChanged();
	return (XN_STATUS_OK);
}

XN_C_API XnBool xnLogIsEnabled(const XnChar* csLogMask, XnLogSeverity nSeverity)
{
	XnLogger* pLogger = xnLogGetLoggerForMask(csLogMask, FALSE);
	if (pLogger == NULL)
	{
		// no such logger exists. Mask uses default severity
		LogData& logData = LogData::GetInstance();
		return (nSeverity >= logData.defaultMinSeverity);
	}
	else
	{
		return (nSeverity >= pLogger->nMinSeverity);
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

	va_list args;
	va_start(args, csFormat);

	const XnUInt32 nMaxMessageSize = 1024;
	XnChar csMessage[nMaxMessageSize+1];
	XnUInt32 nChars;
	xnOSStrFormatV(csMessage, nMaxMessageSize, &nChars, csFormat, args);

	va_end(args);

	LogData& logData = LogData::GetInstance();
	for (XnLogWritersList::ConstIterator it = logData.writers.begin(); it != logData.writers.end(); ++it)
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

XN_C_API XnStatus xnLogSetMaskState(const XnChar* csMask, XnBool bEnabled)
{
	return xnLogBCSetMaskState(csMask, bEnabled);
}

XN_C_API XnStatus xnLogSetSeverityFilter(XnLogSeverity nMinSeverity)
{
	return xnLogBCSetSeverityFilter(nMinSeverity);
}

#endif // #ifndef __XN_NO_BC__