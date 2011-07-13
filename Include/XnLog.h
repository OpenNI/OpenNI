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
#ifndef _XN_LOG_H_
#define _XN_LOG_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnOS.h"
#include "XnLogTypes.h"
#include "XnDump.h"

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------

/**
* This function initializes the log system.
*/
XN_C_API XnStatus XN_C_DECL xnLogInitSystem();

/**
* This function initializes the log from an INI file.
* 
* @param	csINIFile		[in]	The name of the INI file.
* @param	csSectionName	[in]	The name of the section to read values from.
*/
XN_C_API XnStatus XN_C_DECL xnLogInitFromINIFile(const XnChar* csINIFile, const XnChar* csSectionName);

/**
* This function initializes the log from an XML file.
* 
* @param	strFileName		[in]	The name of the XML file.
*/
XN_C_API XnStatus XN_C_DECL xnLogInitFromXmlFile(const XnChar* strFileName);

/**
 * Registers a new Log Writer to receive log entries.
 *
 * @param	pWriter			[in]	The writer to register
 */
XN_C_API XnStatus XN_C_DECL xnLogRegisterLogWriter(const XnLogWriter* pWriter);

/**
 * Unregisters a Log Writer from receiving log entries.
 *
 * @param	pWriter			[in]	The writer to unregister
 */
XN_C_API void XN_C_DECL xnLogUnregisterLogWriter(const XnLogWriter* pWriter);

/**
* This function closes current log file, and starts a new one
*/
XN_C_API XnStatus XN_C_DECL xnLogStartNewFile();

/**
* This function closes the log.
*/
XN_C_API XnStatus XN_C_DECL xnLogClose();

/**
* Configures if a specific log mask is enabled.
*
* @param	csMask		[in]	The mask to set.
* @param	bEnabled	[in]	TRUE to enable this mask, FALSE otherwise.
*/
XN_C_API XnStatus XN_C_DECL xnLogSetMaskState(const XnChar* csMask, XnBool bEnabled);

/**
* Configures the minimum severity the log will output.
*
* @param	nMinSeverity	[in]	The minimum severity to output.
*/
XN_C_API XnStatus XN_C_DECL xnLogSetSeverityFilter(XnLogSeverity nMinSeverity);

/**
* Configures if log entries will be printed to console.
*
* @param	bConsoleOutput	[in]	TRUE to print log entries to console, FALSE otherwise.
*/
XN_C_API XnStatus XN_C_DECL xnLogSetConsoleOutput(XnBool bConsoleOutput);

/**
* Configures if log entries will be printed to a log file.
*
* @param	bFileOutput	[in]	TRUE to print log entries to the file, FALSE otherwise.
*/
XN_C_API XnStatus XN_C_DECL xnLogSetFileOutput(XnBool bFileOutput);

/**
* Configures if log entries will include the file and line that caused them.
*
* @param	bLineInfo	[in]	TRUE to print file and line, FALSE otherwise
*/
XN_C_API XnStatus XN_C_DECL xnLogSetLineInfo(XnBool bLineInfo);

/**
 * Configures the folder under which logs will be written.
 * 
 * @param	strOutputFolder	[in]	Folder to write to
 */ 
XN_C_API XnStatus XN_C_DECL xnLogSetOutputFolder(const XnChar* strOutputFolder);

/**
* This function checks if a mask is enabled.
*
* @param	csLogMask	[in]	The mask that should be checked.
* @param	nSeverity	[in]	The severity.
*/
XN_C_API XnBool XN_C_DECL xnLogIsEnabled(const XnChar* csLogMask, XnLogSeverity nSeverity);

/**
* This function writes a formatted string to the log (only if the mask is turned on)
* 
* @param	csLogMask	[in]	The mask under which this log should be written.
* @param	nSeverity	[in]	The severity of this log.
* @param	csFile		[in]	The name of the file writing this log entry.
* @param	nLine		[in]	The line number writing this log entry.
* @param	csFormat	[in]	A format string.
*/
XN_C_API void XN_C_DECL xnLogWrite(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, ...);

/**
* This function writes a formatted string to the log (only if the mask is turned on), but without
* marking the timestamp, and without placing the end-of-line marker.
* It is used for writing complex data to the log file.
* 
* @param	csLogMask	[in]	The mask under which this log should be written.
* @param	nSeverity	[in]	The severity of this log.
* @param	csFormat	[in]	A format string.
*/
XN_C_API void XN_C_DECL xnLogWriteNoEntry(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFormat, ...);

/**
* This function writes binary data as hex bytes (only if the mask is turned on).
* 
* @param	csLogMask	[in]	The mask under which this log should be written.
* @param	nSeverity	[in]	The severity of this log.
* @param	csFile		[in]	The name of the file writing this log entry.
* @param	nLine		[in]	The line number writing this log entry.
* @param	pBinData	[in]	The binary data to be written.
* @param	nDataSize	[in]	The number of bytes in the binary data.
* @param	csFormat	[in]	A format string to precede the binary data.
*/
XN_C_API void XN_C_DECL xnLogWriteBinaryData(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, XnUChar* pBinData, XnUInt32 nDataSize, const XnChar* csFormat, ...);

/**
 * Creates a file under the logs directory.
 *
 * @param	csFileName	[in]	Name of the file to create
 * @param	phFile		[out]	The file handle.
 */
XN_C_API XnStatus XN_C_DECL xnLogCreateFile(const XnChar* csFileName, XN_FILE_HANDLE* phFile);

#define XN_MASK_RETVAL_CHECKS "RetValChecks"

/** Validates return value and writes log message with appropriate status string **/
#define XN_IS_STATUS_OK_LOG_ERROR(what, nRetVal)													\
	if (nRetVal != XN_STATUS_OK)																	\
	{																								\
		xnLogError(XN_MASK_RETVAL_CHECKS, "Failed to " what ": %s", xnGetStatusString(nRetVal));	\
		XN_ASSERT(FALSE);																			\
		return (nRetVal);																			\
	}

#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define xnLogVerbose(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_VERBOSE, __FILE__, __LINE__, csFormat, __VA_ARGS__)
	#define xnLogInfo(csLogMask, csFormat, ...)		xnLogWrite(csLogMask, XN_LOG_INFO, __FILE__, __LINE__, csFormat, __VA_ARGS__)
	#define xnLogWarning(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_WARNING, __FILE__, __LINE__, csFormat, __VA_ARGS__)
	#define xnLogError(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_ERROR, __FILE__, __LINE__, csFormat, __VA_ARGS__)

	/* Writes to the log and returns nRetVal */
	#define XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat, ...)					\
		{																				\
			xnLogWrite(csLogMask, nSeverity, __FILE__, __LINE__, csFormat, __VA_ARGS__);\
			return (nRetVal);															\
		}

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_WARNING_RETURN(nRetVal, csLogMask, csFormat, ...)					\
		XN_LOG_RETURN(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, __VA_ARGS__)

	/* Logs an error and returns nRetVal */
	#define XN_LOG_ERROR_RETURN(nRetVal, csLogMask, csFormat, ...)						\
		XN_LOG_RETURN(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, __VA_ARGS__)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define xnLogVerbose(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_VERBOSE, __FILE__, __LINE__, csFormat, ##__VA_ARGS__)
	#define xnLogInfo(csLogMask, csFormat, ...)		xnLogWrite(csLogMask, XN_LOG_INFO, __FILE__, __LINE__, csFormat, ##__VA_ARGS__)
	#define xnLogWarning(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_WARNING, __FILE__, __LINE__, csFormat, ##__VA_ARGS__)
	#define xnLogError(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_ERROR, __FILE__, __LINE__, csFormat, ##__VA_ARGS__)

	/* Writes to the log and returns nRetVal */
	#define XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat, ...)					\
	{																					\
		xnLogWrite(csLogMask, nSeverity, __FILE__, __LINE__, csFormat, ##__VA_ARGS__);	\
		return (nRetVal);																\
	}

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_WARNING_RETURN(nRetVal, csLogMask, csFormat, ...)					\
		XN_LOG_RETURN(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, ##__VA_ARGS__)

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_ERROR_RETURN(nRetVal, csLogMask, csFormat, ...)						\
		XN_LOG_RETURN(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, ##__VA_ARGS__)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_ARC_VAARGS_STYLE
	#define xnLogVerbose(csLogMask, csFormat...)	xnLogWrite(csLogMask, XN_LOG_VERBOSE, __FILE__, __LINE__, csFormat)
	#define xnLogInfo(csLogMask, csFormat...)		xnLogWrite(csLogMask, XN_LOG_INFO, __FILE__, __LINE__, csFormat)
	#define xnLogWarning(csLogMask, csFormat...)	xnLogWrite(csLogMask, XN_LOG_WARNING, __FILE__, __LINE__, csFormat)
	#define xnLogError(csLogMask, csFormat...)		xnLogWrite(csLogMask, XN_LOG_ERROR, __FILE__, __LINE__, csFormat)

	/* Writes to the log and returns nRetVal */
	#define XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat...)					\
	{																					\
		xnLogWrite(csLogMask, nSeverity, __FILE__, __LINE__, csFormat);					\
		return (nRetVal);																\
	}

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_WARNING_RETURN(nRetVal, csLogMask, csFormat...)						\
		XN_LOG_RETURN(nRetVal, XN_LOG_WARNING, csLogMask, csFormat)

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_ERROR_RETURN(nRetVal, csLogMask, csFormat...)						\
		XN_LOG_RETURN(nRetVal, XN_LOG_ERROR, csLogMask, csFormat)

	/* If nRetVal is not ok, writes to the log and returns nRetVal */
	#define XN_IS_STATUS_OK_LOG(nRetVal, nSeverity, csLogMask, csFormat...)				\
		if (nRetVal != XN_STATUS_OK)													\
	{																					\
		XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat)							\
	}																						

	/* If nRetVal is not ok, logs a warning and returns nRetVal */
	#define XN_IS_STATUS_OK_WARNING(nRetVal, csLogMask, csFormat...)					\
		XN_IS_STATUS_OK_LOG(nRetVal, XN_LOG_WARNING, csLogMask, csFormat)

	/* If nRetVal is not ok, logs an error and returns nRetVal */
	#define XN_IS_STATUS_OK_ERROR(nRetVal, csLogMask, csFormat...)						\
		XN_IS_STATUS_OK_LOG(nRetVal, XN_LOG_ERROR, csLogMask, csFormat)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_NO_VAARGS
	#define xnLogVerbose(csLogMask, csFormat, args)	xnLogWrite(csLogMask, XN_LOG_VERBOSE, __FILE__, __LINE__, csFormat, args)
	#define xnLogInfo(csLogMask, csFormat, args)	xnLogWrite(csLogMask, XN_LOG_INFO, __FILE__, __LINE__, csFormat, args)
	#define xnLogWarning(csLogMask, csFormat, args)	xnLogWrite(csLogMask, XN_LOG_WARNING, __FILE__, __LINE__, csFormat, args)
	#define xnLogError(csLogMask, csFormat, args)	xnLogWrite(csLogMask, XN_LOG_ERROR, __FILE__, __LINE__, csFormat, args)

	/* Writes to the log and returns nRetVal */
	#define XN_LOG_RETURN(nRetVal, nSeverity csLogMask, csFormat, args)					\
	{																					\
		xnLogWrite(csLogMask, nSeverity, __FILE__, __LINE__, csFormat, args);			\
		return (nRetVal);																\
	}

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_WARNING_RETURN(nRetVal, csLogMask, csFormat, args)					\
		XN_LOG_RETURN(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, args)

	/* Logs an error and returns nRetVal */
	#define XN_LOG_ERROR_RETURN(nRetVal, csLogMask, csFormat, args)						\
		XN_LOG_RETURN(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, args)

#else
	#error Xiron Log - Unknown VAARGS type!
#endif

#endif //_XN_LOG_H_

