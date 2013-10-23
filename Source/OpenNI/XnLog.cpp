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
#include <XnLog.h>
#include <XnStringsHashT.h>
#include <XnVersion.h>
#include <stdarg.h>
#include "XnXml.h"
#include <XnListT.h>
#include <XnArray.h>
#include <XnOSCpp.h>

#include "XnLogConsoleWriter.h"
#include "XnLogFileWriter.h"
#if XN_PLATFORM == XN_PLATFORM_ANDROID_ARM
#include "XnLogAndroidWriter.h"
#endif

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_LOG_MASKS_DELIM			";"
#define XN_LOG_MAX_MESSAGE_LENGTH	2048

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef XnListT<XnLogWriter*> XnLogWritersList;

typedef XnStringsHashT<XnLogger> XnLogMasksHash;

class XnBufferedLogEntry : public XnLogEntry
{
public:
	XnBufferedLogEntry()
	{
		m_strBuffer[0] = '\0';
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
		for (XnLogMasksHash::Iterator it = this->pMasksHash->Begin(); it != this->pMasksHash->End(); ++it)
		{
			it->Value().nMinSeverity = severity;
		}
	}

	XnChar strLogDir[XN_FILE_MAX_PATH];
	XnLogMasksHash* pMasksHash;
	XnLogSeverity defaultMinSeverity;
	XnLogWritersList writers;
	XnBool anyWriters;
	XnChar strSessionTimestamp[25];
	XN_CRITICAL_SECTION_HANDLE hLock;

	// Writers
	XnLogConsoleWriter consoleWriter;
	XnLogFileWriter fileWriter;
#if XN_PLATFORM == XN_PLATFORM_ANDROID_ARM
	XnLogAndroidWriter AndroidWriter;
#endif

private:
	LogData()
	{
		// NOTE: as our log implementation returns a pointer directly into this hash, we can't
		// free this memory in our dtor (static objects dtors are called in unknown order. modules
		// might still access this memory after our dtor is called).
		// As in any case, this is a static object which will only be destroyed when the process goes 
		// down - we can allow this.
		this->pMasksHash = XN_NEW(XnLogMasksHash);
		XN_ASSERT(this->pMasksHash != NULL);

		// We need a critical section to guard the writers list - it is accessed with every log entry written
		// and also by the application (mainly, when calling xnStartNewLogFile()).
		XnStatus nRetVal = xnOSCreateCriticalSection(&this->hLock);
		XN_ASSERT(nRetVal == XN_STATUS_OK);
		XN_REFERENCE_VARIABLE(nRetVal);

		this->anyWriters = FALSE;

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
	XnAutoCSLocker locker(logData.hLock);
	for (XnLogWritersList::ConstIterator it = logData.writers.Begin(); it != logData.writers.End(); ++it)
	{
		const XnLogWriter* pWriter = *it;
		pWriter->WriteEntry(pEntry, pWriter->pCookie);
	}
}

static void xnLogWriteImplV(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, va_list args)
{
	// check if there are any writers registered
	LogData& logData = LogData::GetInstance();

	// optimization: check if any writer is registered *before* locking.
	if (!logData.anyWriters)
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

XN_C_API XnStatus XN_C_DECL xnLogCreateNewFile(const XnChar* strName, XnBool bSessionBased, XnChar* csFullPath, XnUInt32 nPathBufferSize, XN_FILE_HANDLE* phFile)
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
	XnUInt32 nPathSize = 0;
	XnUInt32 nCharsWritten = 0;
	nRetVal = xnOSStrFormat(csFullPath, nPathBufferSize - nPathSize, &nCharsWritten, "%s", logData.strLogDir);
	XN_IS_STATUS_OK(nRetVal);
	nPathSize += nCharsWritten;

	if (bSessionBased)
	{
		nRetVal = xnOSStrFormat(csFullPath + nPathSize, nPathBufferSize - nPathSize, &nCharsWritten, "%s_%u.", logData.strSessionTimestamp, nProcID);
		XN_IS_STATUS_OK(nRetVal);
		nPathSize += nCharsWritten;
	}

	nRetVal = xnOSStrFormat(csFullPath + nPathSize, nPathBufferSize - nPathSize, &nCharsWritten, "%s", strName);
	XN_IS_STATUS_OK(nRetVal);
	nPathSize += nCharsWritten;

	// and open the file
	nRetVal = xnOSOpenFile(csFullPath, XN_OS_FILE_WRITE | XN_OS_FILE_TRUNCATE, phFile);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

static void xnLogCreateFilterChangedMessage(XnBufferedLogEntry* pEntry)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_REFERENCE_VARIABLE(nRetVal);
	
	LogData& logData = LogData::GetInstance();

	XnChar strConfigMessage[XN_LOG_MAX_MESSAGE_LENGTH];
	XnUInt32 nCharsWritten = 0;
	XnUInt32 nMessageLength = 0;
	nRetVal = xnOSStrFormat(strConfigMessage, XN_LOG_MAX_MESSAGE_LENGTH, &nCharsWritten, "--- Filter Info --- Minimum Severity: %s", xnLogGetSeverityString(logData.defaultMinSeverity));
	XN_ASSERT(nRetVal == XN_STATUS_OK);
	nMessageLength += nCharsWritten;

	XnBool bOverrides = FALSE;

	// go over masks, and see if there are any overrides
	for (XnLogMasksHash::ConstIterator it = logData.pMasksHash->Begin(); it != logData.pMasksHash->End(); ++it)
	{
		XnLogSeverity maskSeverity = it->Value().nMinSeverity;
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

			nRetVal = xnOSStrFormat(strConfigMessage + nMessageLength, sizeof(strConfigMessage) - nMessageLength, &nCharsWritten, "'%s': %s", it->Key(), xnLogGetSeverityString(maskSeverity));
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
	XnAutoCSLocker locker(logData.hLock);
	for (XnLogWritersList::ConstIterator it = logData.writers.Begin(); it != logData.writers.End(); ++it)
	{
		const XnLogWriter* pWriter = *it;
		pWriter->OnConfigurationChanged(pWriter->pCookie);
	}
}

XN_C_API void xnLogWriteBanner(const XnLogWriter* pWriter)
{
	XnBufferedLogEntry entry;

	// write system time
	time_t currtime;
	time(&currtime);
	XnChar strTime[200];
	strftime(strTime, sizeof(strTime) - 1, "%Y-%m-%d %H:%M:%S", localtime(&currtime)); 
	xnLogCreateEntry(&entry, XN_MASK_LOG, XN_LOG_INFO, __FILE__, __LINE__, "New log started on %s", strTime);
	pWriter->WriteEntry(&entry, pWriter->pCookie);

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

	LogData::GetInstance().SetMinSeverityGlobally(XN_LOG_SEVERITY_NONE);

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

#if XN_PLATFORM == XN_PLATFORM_ANDROID_ARM
	nRetVal = xnOSReadIntFromINI(cpINIFileName, cpSectionName, "LogWriteToAndroidLog", &nTemp);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = xnLogSetAndroidOutput(nTemp);
		XN_IS_STATUS_OK(nRetVal);
	}
#endif

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

#if XN_PLATFORM == XN_PLATFORM_ANDROID_ARM
			if (pLog->Attribute("writeToAndroidLog"))
			{
				nRetVal = xnXmlReadBoolAttribute(pLog, "writeToAndroidLog", &bOn);
				XN_IS_STATUS_OK(nRetVal);

				nRetVal = xnLogSetAndroidOutput(bOn);
				XN_IS_STATUS_OK(nRetVal);
			}
#endif

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

XN_C_API XnStatus xnLogRegisterLogWriter(XnLogWriter* pWriter)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	LogData& logData = LogData::GetInstance();

	{
		XnAutoCSLocker locker(logData.hLock);
		nRetVal = logData.writers.AddLast(pWriter);
	}
	XN_IS_STATUS_OK(nRetVal);

	logData.anyWriters = TRUE;

	xnLogWriteBanner(pWriter);
	
	return (XN_STATUS_OK);
}

XN_C_API void xnLogUnregisterLogWriter(XnLogWriter* pWriter)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_REFERENCE_VARIABLE(nRetVal);
	
	LogData& logData = LogData::GetInstance();

	XnAutoCSLocker locker(logData.hLock);
	nRetVal = logData.writers.Remove(pWriter);
	XN_ASSERT(nRetVal == XN_STATUS_OK);

	logData.anyWriters = !logData.writers.IsEmpty();
}

XN_C_API XnStatus xnLogStartNewFile()
{
	LogData& logData = LogData::GetInstance();
	if (!logData.fileWriter.IsRegistered())
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	logData.fileWriter.Unregister();

	// reset our session timestamp, a new one will be created
	logData.strSessionTimestamp[0] = '\0';

	return logData.fileWriter.Register();
}

XN_C_API XnStatus xnLogClose()
{
	// notify all writers (while allowing them to unregister themselves)
	LogData& logData = LogData::GetInstance();

	XnAutoCSLocker locker(logData.hLock);
	XnLogWritersList::ConstIterator it = logData.writers.Begin();
	while (it != logData.writers.End())
	{
		XnLogWritersList::ConstIterator curr = it;
		++it;

		const XnLogWriter* pWriter = *curr;
		pWriter->OnClosing(pWriter->pCookie);
	}

	logData.strLogDir[0] = '\0';
	logData.strSessionTimestamp[0] = '\0';
	logData.SetMinSeverityGlobally(XN_LOG_SEVERITY_NONE);

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

#if XN_PLATFORM == XN_PLATFORM_ANDROID_ARM
XN_C_API XnStatus xnLogSetAndroidOutput(XnBool bAndroidOutput)
{
	XnStatus nRetVal = XN_STATUS_OK;

	LogData& logData = LogData::GetInstance();
	if (bAndroidOutput)
	{
		nRetVal = logData.AndroidWriter.Register();
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		logData.AndroidWriter.Unregister();
	}

	return XN_STATUS_OK;
}
#endif

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

XN_C_API XnStatus XN_C_DECL xnLogGetFileName(XnChar* strFileName, XnUInt32 nBufferSize)
{
	LogData& logData = LogData::GetInstance();
	if (!logData.fileWriter.IsRegistered())
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	return xnOSStrCopy(strFileName, logData.fileWriter.GetFileName(), nBufferSize);
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
		XnLogMasksHash::Iterator it = logData.pMasksHash->Find(csLogMask);
		if (it == logData.pMasksHash->End())
		{
			XN_ASSERT(FALSE);
			return NULL;
		}

		it->Value().pInternal = (void*)it->Key();

		return &it->Value();
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
	XnAutoCSLocker locker(logData.hLock);
	for (XnLogWritersList::ConstIterator it = logData.writers.Begin(); it != logData.writers.End(); ++it)
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
	XnAutoCSLocker locker(logData.hLock);
	for (XnLogWritersList::ConstIterator it = logData.writers.Begin(); it != logData.writers.End(); ++it)
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

XN_C_API XnStatus xnLogCreateFile(const XnChar* csFileName, XN_FILE_HANDLE* phFile)
{
	XnChar strFullPath[XN_FILE_MAX_PATH];
	return xnLogCreateNewFile(csFileName, TRUE, strFullPath, XN_FILE_MAX_PATH, phFile);
}

XN_C_API XnStatus xnLogCreateFileEx(const XnChar* strFileName, XnBool bSessionBased, XN_FILE_HANDLE* phFile)
{
	XnChar strFullPath[XN_FILE_MAX_PATH];
	return xnLogCreateNewFile(strFileName, bSessionBased, strFullPath, XN_FILE_MAX_PATH, phFile);
}

#endif // #ifndef __XN_NO_BC__
