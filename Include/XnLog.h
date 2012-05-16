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
 * @name Initialization
 * Functions for initializing and shutting down the logger.
 * @{
 */

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
* This function closes the log.
*/
XN_C_API XnStatus XN_C_DECL xnLogClose();

// @}

/** 
 * @name Filtering Log Messages
 * Functions for managing filters on the log system.
 * @{
 */

/**
* Sets the minimum severity to be output from a specific mask (logger name).
*
* @param	strMask		[in]	Name of the logger.
* @param	minSeverity	[in]	Minimum severity to be output. Use @ref XN_LOG_SEVERITY_NONE to output nothing.
*/
XN_C_API XnStatus XN_C_DECL xnLogSetMaskMinSeverity(const XnChar* strMask, XnLogSeverity minSeverity);

/**
* Gets the minimum severity to be output from a specific mask (logger name).
*
* @param	strMask		[in]	Name of the logger.
*
* @returns The minimum severity, or @ref XN_LOG_SEVERITY_NONE is mask will output nothing.
*/
XN_C_API XnLogSeverity XN_C_DECL xnLogGetMaskMinSeverity(const XnChar* strMask);

// @}

/** 
 * @name Log Writers
 * Functions for managing which writers are active in the log system (i.e. which outputs
 * will be generated).
 * @{
 */

/**
 * Registers a new Log Writer to receive log entries.
 *
 * @param	pWriter			[in]	The writer to register
 */
XN_C_API XnStatus XN_C_DECL xnLogRegisterLogWriter(XnLogWriter* pWriter);

/**
 * Unregisters a Log Writer from receiving log entries.
 *
 * @param	pWriter			[in]	The writer to unregister
 */
XN_C_API void XN_C_DECL xnLogUnregisterLogWriter(XnLogWriter* pWriter);

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

// @}

/** 
 * @name File Output
 * Functions for configuring how files are created.
 * @{
 */

/**
* This function closes current log file, and starts a new one (if file writer is currently active)
*/
XN_C_API XnStatus XN_C_DECL xnLogStartNewFile();

/**
* Configures if log entries in file will include the file and line that caused them.
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
 * Gets current log file name
 * 
 * @param	strFileName		[in]	A buffer to be filled
 * @param	nBufferSize		[in]	The size of the buffer
 */ 
XN_C_API XnStatus XN_C_DECL xnLogGetFileName(XnChar* strFileName, XnUInt32 nBufferSize);

// @}

/** 
 * @name Logger API
 * Functions for writing entries to the log (used mainly by middleware developers)
 * @{
 */

/**
* Opens a logger for writing.
*
* @param	strMask		[in]	Name of the logger to open.
*/
XN_C_API XnLogger* XN_C_DECL xnLoggerOpen(const XnChar* strMask);

/**
* Writes a single log entry.
*
* @param	pLogger		[in]	Logger to write to
* @param	severity	[in]	Severity of the log entry
* @param	strFile		[in]	Name of the source file
* @param	nLine		[in]	Line in the source file
* @param	strFormat	[in]	Format string
*
* It is advised to use one of the @ref xnLoggerVerbose, @ref xnLoggerInfo, @ref xnLoggerWarning or
* @ref xnLoggerError macros instead of calling this method directly.
*/
XN_C_API void XN_C_DECL xnLoggerWrite(XnLogger* pLogger, XnLogSeverity severity, const XnChar* strFile, XnUInt32 nLine, const XnChar* strFormat, ...);

/**
* Writes to a logger without an entry format (i.e. no timestamp, mask name, etc.)
*
* @param	pLogger		[in]	Logger to write to
* @param	severity	[in]	Severity of the log entry
* @param	strFormat	[in]	Format string
*/
XN_C_API void XN_C_DECL xnLoggerWriteNoEntry(XnLogger* pLogger, XnLogSeverity severity, const XnChar* strFormat, ...);

/**
* Writes binary data to a logger, formatting it to readable text.
*
* @param	pLogger		[in]	Logger to write to
* @param	severity	[in]	Severity of the log entry
* @param	strFile		[in]	Name of the source file
* @param	nLine		[in]	Line in the source file
* @param	pBinData	[in]	A pointer to the binary data to be written
* @param	nDataSize	[in]	The number of bytes to write
* @param	strFormat	[in]	Format string for the binary data header
*/
XN_C_API void XN_C_DECL xnLoggerWriteBinaryData(XnLogger* pLogger, XnLogSeverity severity, const XnChar* strFile, XnUInt32 nLine, XnUChar* pBinData, XnUInt32 nDataSize, const XnChar* strFormat, ...);

/**
* Checks if a specific severity is enabled for this logger.
*
* @param	pLogger		[in]	Logger to check
* @param	severity	[in]	Severity to check.
*/
XN_C_API XnBool XN_C_DECL xnLoggerIsEnabled(XnLogger* pLogger, XnLogSeverity severity);

/**
* Closes a logger previsouly opened using @ref xnLoggerOpen().
*
* @param	pLogger		[in]	The logger to be closed.
*/
XN_C_API void XN_C_DECL _xnLoggerClose(XnLogger* pLogger);

/**
* Closes a logger and NULLs the handle.
*
* @param	pLogger		[in]	The logger to be closed.
*/
#define xnLoggerClose(pLogger)			\
	{									\
		_xnLoggerClose(pLogger);		\
		pLogger = NULL;					\
	}

#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	/**
	* Helper macro for logging. Checks inline if logger is enabled and adds source file and line info.
	*/
	#define xnLoggerWriteHelper(pLogger, severity, csFormat, ...)								\
		if (pLogger != NULL && severity >= pLogger->nMinSeverity)								\
		{																						\
			xnLoggerWrite(pLogger, severity, __FILE__, __LINE__, csFormat, __VA_ARGS__);		\
		}

	/**
	* Helper macro for logging. Issues a verbose log entry.
	*/
	#define xnLoggerVerbose(pLogger, csFormat, ...) xnLoggerWriteHelper(pLogger, XN_LOG_VERBOSE, csFormat, __VA_ARGS__)
	/**
	* Helper macro for logging. Issues an info log entry.
	*/
	#define xnLoggerInfo(pLogger, csFormat, ...)    xnLoggerWriteHelper(pLogger, XN_LOG_INFO, csFormat, __VA_ARGS__)
	/**
	* Helper macro for logging. Issues a warning log entry.
	*/
	#define xnLoggerWarning(pLogger, csFormat, ...) xnLoggerWriteHelper(pLogger, XN_LOG_WARNING, csFormat, __VA_ARGS__)
	/**
	* Helper macro for logging. Issues an error log entry.
	*/
	#define xnLoggerError(pLogger, csFormat, ...)   xnLoggerWriteHelper(pLogger, XN_LOG_ERROR, csFormat, __VA_ARGS__)

	/**
	* Helper macro for returning from a function while logging
	*
	* @param	pLogger		[in]	The logger to be closed.
	* @param	nRetVal		[in]	The return value from the function.
	* @param	severity	[in]	Severity of the log entry
	* @param	csFormat	[in]	Format string
	*/
	#define XN_RETURN_WITH_LOG(pLogger, nRetVal, severity, csFormat, ...)					\
		{																					\
			xnLoggerWriteHelper(pLogger, severity, csFormat, __VA_ARGS__);					\
			return (nRetVal);																\
		}

	/**
	* Helper macro for returning from a function while logging a warning
	*
	* @param	pLogger		[in]	The logger to be closed.
	* @param	nRetVal		[in]	The return value from the function.
	* @param	csFormat	[in]	Format string
	*/
	#define XN_RETURN_WITH_WARNING_LOG(pLogger, nRetVal, csFormat, ...)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_WARNING, csFormat, __VA_ARGS__)

	/**
	* Helper macro for returning from a function while logging an error
	*
	* @param	pLogger		[in]	The logger to be closed.
	* @param	nRetVal		[in]	The return value from the function.
	* @param	csFormat	[in]	Format string
	*/
	#define XN_RETURN_WITH_ERROR_LOG(pLogger, nRetVal, csFormat, ...)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_ERROR, csFormat, __VA_ARGS__)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define xnLoggerWriteHelper(pLogger, severity, csFormat, ...)								\
		if (pLogger != NULL && severity >= pLogger->nMinSeverity)								\
		{																						\
			xnLoggerWrite(pLogger, severity, __FILE__, __LINE__, csFormat, ##__VA_ARGS__);		\
		}

	#define xnLoggerVerbose(pLogger, csFormat, ...) xnLoggerWriteHelper(pLogger, XN_LOG_VERBOSE, csFormat,## __VA_ARGS__)
	#define xnLoggerInfo(pLogger, csFormat, ...)    xnLoggerWriteHelper(pLogger, XN_LOG_INFO, csFormat, ##__VA_ARGS__)
	#define xnLoggerWarning(pLogger, csFormat, ...) xnLoggerWriteHelper(pLogger, XN_LOG_WARNING, csFormat, ##__VA_ARGS__)
	#define xnLoggerError(pLogger, csFormat, ...)   xnLoggerWriteHelper(pLogger, XN_LOG_ERROR, csFormat, ##__VA_ARGS__)

	/* Writes to the log and returns nRetVal */
	#define XN_RETURN_WITH_LOG(pLogger, nRetVal, severity, csFormat, ...)					\
		{																					\
			xnLoggerWriteHelper(pLogger, severity, csFormat, ##__VA_ARGS__);				\
			return (nRetVal);																\
		}

	/* Logs a warning and returns nRetVal */
	#define XN_RETURN_WITH_WARNING_LOG(pLogger, nRetVal, csFormat, ...)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_WARNING, csFormat, ##__VA_ARGS__)

	/* Logs an error and returns nRetVal */
	#define XN_RETURN_WITH_ERROR_LOG(pLogger, nRetVal, csFormat, ...)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_ERROR, csFormat, ##__VA_ARGS__)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_ARC_VAARGS_STYLE
	#define xnLoggerWriteHelper(pLogger, severity, csFormat...)									\
		if (pLogger != NULL && severity >= pLogger->nMinSeverity)								\
		{																						\
			xnLoggerWrite(pLogger, severity, __FILE__, __LINE__, csFormat);						\
		}

	#define xnLoggerVerbose(pLogger, csFormat...) xnLoggerWriteHelper(pLogger, XN_LOG_VERBOSE, csFormat)
	#define xnLoggerInfo(pLogger, csFormat...)    xnLoggerWriteHelper(pLogger, XN_LOG_INFO, csFormat)
	#define xnLoggerWarning(pLogger, csFormat...) xnLoggerWriteHelper(pLogger, XN_LOG_WARNING, csFormat)
	#define xnLoggerError(pLogger, csFormat...)   xnLoggerWriteHelper(pLogger, XN_LOG_ERROR, csFormat)

	/* Writes to the log and returns nRetVal */
	#define XN_RETURN_WITH_LOG(pLogger, nRetVal, severity, csFormat...)					\
		{																				\
			xnLoggerWriteHelper(pLogger, severity, csFormat);							\
			return (nRetVal);															\
		}

	/* Logs a warning and returns nRetVal */
	#define XN_RETURN_WITH_WARNING_LOG(pLogger, nRetVal, csFormat...)					\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_WARNING, csFormat)

	/* Logs an error and returns nRetVal */
	#define XN_RETURN_WITH_ERROR_LOG(pLogger, nRetVal, csFormat...)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_ERROR, csFormat)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_NO_VAARGS
	#define xnLoggerWriteHelper(pLogger, severity, csFormat, arg)								\
		if (pLogger != NULL && severity >= pLogger->nMinSeverity)								\
		{																						\
			xnLoggerWrite(pLogger, severity, __FILE__, __LINE__, csFormat, arg);				\
		}

	#define xnLoggerVerbose(pLogger, csFormat, arg) xnLoggerWriteHelper(pLogger, XN_LOG_VERBOSE, csFormat, arg)
	#define xnLoggerInfo(pLogger, csFormat, arg)    xnLoggerWriteHelper(pLogger, XN_LOG_INFO, csFormat, arg)
	#define xnLoggerWarning(pLogger, csFormat, arg) xnLoggerWriteHelper(pLogger, XN_LOG_WARNING, csFormat, arg)
	#define xnLoggerError(pLogger, csFormat, arg)   xnLoggerWriteHelper(pLogger, XN_LOG_ERROR, csFormat, arg)

	/* Writes to the log and returns nRetVal */
	#define XN_RETURN_WITH_LOG(pLogger, nRetVal, severity, csFormat)					\
		{																				\
			xnLoggerWriteHelper(pLogger, severity, csFormat);							\
			return (nRetVal);															\
		}

	/* Logs a warning and returns nRetVal */
	#define XN_RETURN_WITH_WARNING_LOG(pLogger, nRetVal, csFormat)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_WARNING, csFormat)

	/* Logs an error and returns nRetVal */
	#define XN_RETURN_WITH_ERROR_LOG(pLogger, nRetVal, csFormat)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_ERROR, csFormat)

#else
	#error Xiron Log - Unknown VAARGS type!
#endif

// @}

/** 
 * @name Misc.
 * Miscellaneous functions regarding the log system.
 * @{
 */

/**
 * Creates a new file under the logs directory.
 *
 * @param	strName			[in]		Name of the file to create
 * @param	bSessionBased	[in]		TRUE for a session-based file, FALSE otherwise. A session based
 *										file also includes the timestamp and process ID of the running
 *										process as a prefix to its name.
 * @param	csFullPath		[in/out]	A buffer to be filled with full path of the created file
 * @param	nPathBufferSize	[in]		The size of the <c>csFullPath</c> buffer
 * @param	phFile			[out]		The file handle
 */
XN_C_API XnStatus XN_C_DECL xnLogCreateNewFile(const XnChar* strName, XnBool bSessionBased, XnChar* csFullPath, XnUInt32 nPathBufferSize, XN_FILE_HANDLE* phFile);

// @}

#define XN_MASK_RETVAL_CHECKS "RetValChecks"

#if XN_PLATFORM == XN_PLATFORM_ARC
extern "C" XnLogger* XN_LOGGER_RETVAL_CHECKS;
#else
XN_C_API XnLogger* XN_LOGGER_RETVAL_CHECKS;
#endif

/** Validates return value and writes log message with appropriate status string **/
#define XN_IS_STATUS_OK_LOG_ERROR(what, nRetVal)														\
	if (nRetVal != XN_STATUS_OK)																		\
	{																									\
		xnLoggerError(XN_LOGGER_RETVAL_CHECKS, "Failed to " what ": %s", xnGetStatusString(nRetVal));	\
		XN_ASSERT(FALSE);																				\
		return (nRetVal);																				\
	}


#ifndef __XN_NO_BC__

XN_C_API XnStatus XN_API_DEPRECATED("Please use xnLogSetMaskMinSeverity() instead") XN_C_DECL xnLogSetMaskState(const XnChar* csMask, XnBool bEnabled);
XN_C_API XnStatus XN_API_DEPRECATED("Please use xnLogSetMaskMinSeverity() instead") XN_C_DECL xnLogSetSeverityFilter(XnLogSeverity nMinSeverity);
XN_C_API XnBool XN_C_DECL xnLogIsEnabled(const XnChar* csLogMask, XnLogSeverity nSeverity);
XN_C_API void XN_C_DECL xnLogWrite(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, ...);
XN_C_API void XN_C_DECL xnLogWriteNoEntry(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFormat, ...);
XN_C_API void XN_C_DECL xnLogWriteBinaryData(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, XnUChar* pBinData, XnUInt32 nDataSize, const XnChar* csFormat, ...);
XN_C_API XnStatus XN_API_DEPRECATED("Use xnLogCreateNewFile() instead") XN_C_DECL xnLogCreateFile(const XnChar* strFileName, XN_FILE_HANDLE* phFile);
XN_C_API XnStatus XN_API_DEPRECATED("Use xnLogCreateNewFile() instead") XN_C_DECL xnLogCreateFileEx(const XnChar* strFileName, XnBool bSessionBased, XN_FILE_HANDLE* phFile);

#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define xnLogVerbose(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_VERBOSE, __FILE__, __LINE__, csFormat, __VA_ARGS__)
	#define xnLogInfo(csLogMask, csFormat, ...)		xnLogWrite(csLogMask, XN_LOG_INFO, __FILE__, __LINE__, csFormat, __VA_ARGS__)
	#define xnLogWarning(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_WARNING, __FILE__, __LINE__, csFormat, __VA_ARGS__)
	#define xnLogError(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_ERROR, __FILE__, __LINE__, csFormat, __VA_ARGS__)

	/* Writes to the log and returns nRetVal */
	#define XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat, ...)					\
	{																					\
		xnLogWrite(csLogMask, nSeverity, __FILE__, __LINE__, csFormat, __VA_ARGS__);	\
		return (nRetVal);																\
	}

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_WARNING_RETURN(nRetVal, csLogMask, csFormat, ...)					\
		XN_LOG_RETURN(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, __VA_ARGS__)

	/* Logs a warning and returns nRetVal */
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

#endif // ifndef __XN_NO_BC__

#endif //_XN_LOG_H_

