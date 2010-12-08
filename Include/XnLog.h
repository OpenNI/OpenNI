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




#ifndef _XN_LOG_H_
#define _XN_LOG_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnOS.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_LOG_DIR_NAME			"Log"
#define XN_LOG_MASKS_STRING_LEN	600
#define XN_MASK_LOG				"Log"
#define XN_LOG_MASK_ALL			"ALL"

//---------------------------------------------------------------------------
// Enums
//---------------------------------------------------------------------------
typedef enum XnLogSeverity
{
	XN_LOG_VERBOSE,
	XN_LOG_INFO,
	XN_LOG_WARNING,
	XN_LOG_ERROR
} XnLogSeverity;

typedef enum
{
	XN_LOG_WRITE_NONE,
	XN_LOG_WRITE_ALL,
	XN_LOG_WRITE_MASKS
} XnLogFilteringType;

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

typedef struct XnDump 
{
	XN_FILE_HANDLE hFile;
} XnDump;

const XnDump XN_DUMP_CLOSED = { XN_INVALID_FILE_HANDLE };
 
//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------

/**
* This function initializes the log system.
*/
XN_C_API XnStatus xnLogInitSystem();

/**
* This function initializes the log from an INI file.
* 
* @param	csINIFile		[in]	The name of the INI file.
* @param	csSectionName	[in]	The name of the section to read values from.
*/
XN_C_API XnStatus xnLogInitFromINIFile(const XnChar* csINIFile, const XnChar* csSectionName);

/**
* This function initializes the log from an XML file.
* 
* @param	strFileName		[in]	The name of the XML file.
*/
XN_C_API XnStatus xnLogInitFromXmlFile(const XnChar* strFileName);

/**
* This function closes current log file, and starts a new one
*/
XN_C_API XnStatus xnLogStartNewFile();

/**
* This function closes the log.
*/
XN_C_API XnStatus xnLogClose();

/**
* Configures if a specific log mask is enabled.
*
* @param	csMask		[in]	The mask to set.
* @param	bEnabled	[in]	TRUE to enable this mask, FALSE otherwise.
*/
XN_C_API XnStatus xnLogSetMaskState(const XnChar* csMask, XnBool bEnabled);

/**
* Configures if a specific dump mask is enabled.
*
* @param	csMask		[in]	The mask to set.
* @param	bEnabled	[in]	TRUE to enable this mask, FALSE otherwise.
*/
XN_C_API XnStatus xnDumpSetMaskState(const XnChar* csMask, XnBool bEnabled);

/**
* Configures the minimum severity the log will output.
*
* @param	nMinSeverity	[in]	The minimum severity to output.
*/
XN_C_API XnStatus xnLogSetSeverityFilter(XnLogSeverity nMinSeverity);

/**
* Configures if log entries will be printed to console.
*
* @param	bConsoleOutput	[in]	TRUE to print log entries to console, FALSE otherwise.
*/
XN_C_API XnStatus xnLogSetConsoleOutput(XnBool bConsoleOutput);

/**
* Configures if log entries will be printed to a log file.
*
* @param	bFileOutput	[in]	TRUE to print log entries to the file, FALSE otherwise.
*/
XN_C_API XnStatus xnLogSetFileOutput(XnBool bFileOutput);

/**
* Configures if log entries will include the file and line that caused them.
*
* @param	bLineInfo	[in]	TRUE to print file and line, FALSE otherwise
*/
XN_C_API XnStatus xnLogSetLineInfo(XnBool bLineInfo);

/**
 * Configures the folder under which logs will be written.
 * 
 * @param	strOutputFolder	[in]	Folder to write to
 */ 
XN_C_API XnStatus xnLogSetOutputFolder(const XnChar* strOutputFolder);

/**
* This function checks if a mask is enabled.
*
* @param	csLogMask	[in]	The mask that should be checked.
* @param	nSeverity	[in]	The severity.
*/
XN_C_API XnBool xnLogIsEnabled(const XnChar* csLogMask, XnLogSeverity nSeverity);

/**
* This function writes a formatted string to the log (only if the mask is turned on)
* 
* @param	csLogMask	[in]	The mask under which this log should be written.
* @param	nSeverity	[in]	The severity of this log.
* @param	csFile		[in]	The name of the file writing this log entry.
* @param	nLine		[in]	The line number writing this log entry.
* @param	csFormat	[in]	A format string.
*/
XN_C_API void xnLogWrite(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, const XnChar* csFormat, ...);

/**
* This function writes a formatted string to the log (only if the mask is turned on), but without
* marking the timestamp, and without placing the end-of-line marker.
* It is used for writing complex data to the log file.
* 
* @param	csLogMask	[in]	The mask under which this log should be written.
* @param	nSeverity	[in]	The severity of this log.
* @param	csFormat	[in]	A format string.
*/
XN_C_API void xnLogWriteNoEntry(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFormat, ...);

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
XN_C_API void xnLogWriteBinaryData(const XnChar* csLogMask, XnLogSeverity nSeverity, const XnChar* csFile, XnUInt32 nLine, XnUChar* pBinData, XnUInt32 nDataSize, const XnChar* csFormat, ...);

/**
* This function checks if a dump mask is enabled
*
* @param	csDumpMask	[in]	The mask that should be checked.
*/
XN_C_API XnBool xnLogIsDumpMaskEnabled(const XnChar* csDumpMask);

/**
* This function creates an XnDump object through which dumps can be written. Dump will only be
* created if the mask is enabled.
*
* @param	pDump				[in]	A handle to the dump.
* @param	csDumpMask			[in]	The mask under which this dump should be written.
* @param	csHeader			[in]	A header line to be written to the file. When this param is NULL, file will
*										be opened as a binary file. When param is not NULL, the string will be written
*										to the file once it is open.
* @param	csFileNameFormat	[in]	Format of the name to give the file. 
* @param	...					[in]	Arguments for the file name format string.
*/
XN_C_API void xnDumpInit(XnDump* pDump, const XnChar* csDumpMask, const XnChar* csHeader, const XnChar* csFileNameFormat, ...);

/**
* This function creates an XnDump object through which dumps can be written. This dump will be created
* anyway, and does not depend on log configuration.
*
* @param	pDump				[in]	A handle to the dump.
* @param	csHeader			[in]	A header line to be written to the file. When this param is NULL, file will
*										be opened as a binary file. When param is not NULL, the string will be written
*										to the file once it is open.
* @param	csFileNameFormat	[in]	Format of the name to give the file. 
* @param	...					[in]	Arguments for the file name format string.
*/
XN_C_API void xnDumpForceInit(XnDump* pDump, const XnChar* csHeader, const XnChar* csFileNameFormat, ...);

/**
* This function closes a dump file (use it once file is no longer needed).
* You can later use xnDumpWriteString and xnDumpWriteBuffer to write to this file.
*
* @param	pDump	[in]	A pointer to an XnDump struct returned from a call to xnDumpInit.
*/
XN_C_API void xnDumpClose(XnDump* pDump);

/**
* Writes a buffer to the dump.
* 
* @param	dump		[in]	The dump to write to.
* @param	pBuffer		[in]	A pointer to the buffer to write.
* @param	nBufferSize	[in]	The number of bytes to write from the buffer.
*/
XN_C_API void xnDumpWriteBufferImpl(XnDump dump, const void* pBuffer, XnUInt32 nBufferSize);

inline void xnDumpWriteBuffer(XnDump dump, const void* pBuffer, XnUInt32 nBufferSize)
{
	if (dump.hFile != XN_INVALID_FILE_HANDLE)
	{
		xnDumpWriteBufferImpl(dump, pBuffer, nBufferSize);
	}
}

/**
* Writes a formatted string to the dump.
* 
* @param	dump		[in]	The dump to write to.
* @param	csFormat	[in]	A format string.
* @param	...			[in]	Arguments to the format string.
*/
XN_C_API void xnDumpWriteStringImpl(XnDump dump, const XnChar* csFormat, ...);

#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define xnDumpWriteString(dump, csFormat, ...)					\
		if ((dump).hFile != XN_INVALID_FILE_HANDLE) {					\
			xnDumpWriteStringImpl((dump), csFormat, __VA_ARGS__);		\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define xnDumpWriteString(dump, csFormat, ...)					\
		if ((dump).hFile != XN_INVALID_FILE_HANDLE) {					\
			xnDumpWriteStringImpl((dump), csFormat, ##__VA_ARGS__);	\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_ARC_VAARGS_STYLE
	#define xnDumpWriteString(dump, csFormat, args...)				\
		if ((dump).hFile != XN_INVALID_FILE_HANDLE) {					\
			xnDumpWriteStringImpl((dump), csFormat, args);			\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_NO_VAARGS
	#define xnDumpWriteString(dump, csFormat, arg)					\
		if ((dump).hFile != XN_INVALID_FILE_HANDLE) {					\
			xnDumpWriteStringImpl((dump), csFormat, arg);				\
		}
#else
	#error Xiron Log - Unknown VAARGS type!
#endif

/**
* Flushes a dump to the disk.
* 
* @param	dump		[in]	The dump to flush.
*/
XN_C_API void xnDumpFlush(XnDump dump);


#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define xnLogVerbose(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_VERBOSE, __FILE__, __LINE__, csFormat, __VA_ARGS__)
	#define xnLogInfo(csLogMask, csFormat, ...)		xnLogWrite(csLogMask, XN_LOG_INFO, __FILE__, __LINE__, csFormat, __VA_ARGS__)
	#define xnLogWarning(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_WARNING, __FILE__, __LINE__, csFormat, __VA_ARGS__)
	#define xnLogError(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_ERROR, __FILE__, __LINE__, csFormat, __VA_ARGS__)

	/* Writes to the log and returns nRetVal */
	#define XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat, ...)							\
		{																						\
			xnLogWrite(csLogMask, nSeverity, __FILE__, __LINE__, csFormat, __VA_ARGS__);		\
			return (nRetVal);																	\
		}

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_WARNING_RETURN(nRetVal, csLogMask, csFormat, ...)						\
		XN_LOG_RETURN(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, __VA_ARGS__)

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_ERROR_RETURN(nRetVal, csLogMask, csFormat, ...)							\
		XN_LOG_RETURN(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, __VA_ARGS__)

	/* If nRetVal is not ok, writes to the log and returns nRetVal */
	#define XN_IS_STATUS_OK_LOG(nRetVal, nSeverity, csLogMask, csFormat, ...)					\
		if (nRetVal != XN_STATUS_OK)															\
		{																						\
			XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat, __VA_ARGS__)					\
		}																						

	/* If nRetVal is not ok, logs a warning and returns nRetVal */
	#define XN_IS_STATUS_OK_WARNING(nRetVal, csLogMask, csFormat, ...) \
		XN_IS_STATUS_OK_LOG(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, __VA_ARGS__)

	/* If nRetVal is not ok, logs an error and returns nRetVal */
	#define XN_IS_STATUS_OK_ERROR(nRetVal, csLogMask, csFormat, ...) \
		XN_IS_STATUS_OK_LOG(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, __VA_ARGS__)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define xnLogVerbose(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_VERBOSE, __FILE__, __LINE__, csFormat, ##__VA_ARGS__)
	#define xnLogInfo(csLogMask, csFormat, ...)		xnLogWrite(csLogMask, XN_LOG_INFO, __FILE__, __LINE__, csFormat, ##__VA_ARGS__)
	#define xnLogWarning(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_WARNING, __FILE__, __LINE__, csFormat, ##__VA_ARGS__)
	#define xnLogError(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_ERROR, __FILE__, __LINE__, csFormat, ##__VA_ARGS__)

	/* Writes to the log and returns nRetVal */
	#define XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat, ...)						\
	{																						\
		xnLogWrite(csLogMask, nSeverity, __FILE__, __LINE__, csFormat, ##__VA_ARGS__);		\
		return (nRetVal);																	\
	}

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_WARNING_RETURN(nRetVal, csLogMask, csFormat, ...)						\
		XN_LOG_RETURN(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, ##__VA_ARGS__)

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_ERROR_RETURN(nRetVal, csLogMask, csFormat, ...)							\
		XN_LOG_RETURN(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, ##__VA_ARGS__)

	/* If nRetVal is not ok, writes to the log and returns nRetVal */
	#define XN_IS_STATUS_OK_LOG(nRetVal, nSeverity, csLogMask, csFormat, ...)					\
		if (nRetVal != XN_STATUS_OK)															\
	{																						\
		XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat, ##__VA_ARGS__)					\
	}																						

	/* If nRetVal is not ok, logs a warning and returns nRetVal */
	#define XN_IS_STATUS_OK_WARNING(nRetVal, csLogMask, csFormat, ...) \
		XN_IS_STATUS_OK_LOG(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, ##__VA_ARGS__)

	/* If nRetVal is not ok, logs an error and returns nRetVal */
	#define XN_IS_STATUS_OK_ERROR(nRetVal, csLogMask, csFormat, ...) \
		XN_IS_STATUS_OK_LOG(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, ##__VA_ARGS__)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_ARC_VAARGS_STYLE
	#define xnLogVerbose(csLogMask, csFormat, args...)	xnLogWrite(csLogMask, XN_LOG_VERBOSE, __FILE__, __LINE__, csFormat, args)
	#define xnLogInfo(csLogMask, csFormat, args...)		xnLogWrite(csLogMask, XN_LOG_INFO, __FILE__, __LINE__, csFormat, args)
	#define xnLogWarning(csLogMask, csFormat, args...)	xnLogWrite(csLogMask, XN_LOG_WARNING, __FILE__, __LINE__, csFormat, args)
	#define xnLogError(csLogMask, csFormat, args...)	xnLogWrite(csLogMask, XN_LOG_ERROR, __FILE__, __LINE__, csFormat, args)

	/* Writes to the log and returns nRetVal */
	#define XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat, args...)					\
	{																						\
		xnLogWrite(csLogMask, nSeverity, __FILE__, __LINE__, csFormat, args);				\
		return (nRetVal);																	\
	}

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_WARNING_RETURN(nRetVal, csLogMask, csFormat, args...)					\
		XN_LOG_RETURN(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, args)

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_ERROR_RETURN(nRetVal, csLogMask, csFormat, args...)						\
		XN_LOG_RETURN(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, args)

	/* If nRetVal is not ok, writes to the log and returns nRetVal */
	#define XN_IS_STATUS_OK_LOG(nRetVal, nSeverity, csLogMask, csFormat, args...)			\
		if (nRetVal != XN_STATUS_OK)														\
	{																						\
		XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat, args)						\
	}																						

	/* If nRetVal is not ok, logs a warning and returns nRetVal */
	#define XN_IS_STATUS_OK_WARNING(nRetVal, csLogMask, csFormat, args...) \
		XN_IS_STATUS_OK_LOG(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, args)

	/* If nRetVal is not ok, logs an error and returns nRetVal */
	#define XN_IS_STATUS_OK_ERROR(nRetVal, csLogMask, csFormat, args...) \
		XN_IS_STATUS_OK_LOG(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, args)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_NO_VAARGS
	#define xnLogVerbose(csLogMask, csFormat, args)	xnLogWrite(csLogMask, XN_LOG_VERBOSE, __FILE__, __LINE__, csFormat, args)
	#define xnLogInfo(csLogMask, csFormat, args)	xnLogWrite(csLogMask, XN_LOG_INFO, __FILE__, __LINE__, csFormat, args)
	#define xnLogWarning(csLogMask, csFormat, args)	xnLogWrite(csLogMask, XN_LOG_WARNING, __FILE__, __LINE__, csFormat, args)
	#define xnLogError(csLogMask, csFormat, args)	xnLogWrite(csLogMask, XN_LOG_ERROR, __FILE__, __LINE__, csFormat, args)

	/* Writes to the log and returns nRetVal */
	#define XN_LOG_RETURN(nRetVal, nSeverity csLogMask, csFormat, args)						\
	{																						\
		xnLogWrite(csLogMask, nSeverity, __FILE__, __LINE__, csFormat, args);				\
		return (nRetVal);																	\
	}

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_WARNING_RETURN(nRetVal, csLogMask, csFormat, args)						\
		XN_LOG_RETURN(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, args)

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_ERROR_RETURN(nRetVal, csLogMask, csFormat, args)							\
		XN_LOG_RETURN(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, args)

	/* If nRetVal is not ok, writes to the log and returns nRetVal */
	#define XN_IS_STATUS_OK_LOG(nRetVal, nSeverity, csLogMask, csFormat, args)				\
		if (nRetVal != XN_STATUS_OK)														\
		{																					\
			XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat, args)					\
		}																						

	/* If nRetVal is not ok, logs a warning and returns nRetVal */
	#define XN_IS_STATUS_OK_WARNING(nRetVal, csLogMask, csFormat, args) \
		XN_IS_STATUS_OK_LOG(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, args)

	/* If nRetVal is not ok, logs an error and returns nRetVal */
	#define XN_IS_STATUS_OK_ERROR(nRetVal, csLogMask, csFormat, args) \
		XN_IS_STATUS_OK_LOG(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, args)

#else
	#error Xiron Log - Unknown VAARGS type!
#endif

//#define XN_NO_LOGS

#if (defined(XN_NO_LOGS) && !defined(XN_LOG_IMPL))
	#define xnLogWrite
	#define xnLogWriteBinaryData
	#define xnLogIsDumpMaskEnabled(mask) FALSE
	#define xnDumpInit(dump,mask,header,format,...) NULL
	#define xnDumpForceInit(dump,header,format,...) NULL
	#define xnDumpClose
	#define xnDumpWriteBufferImpl
	#define xnDumpWriteStringImpl
	#undef xnDumpWriteBuffer
	#define xnDumpWriteBuffer
	#undef xnDumpWriteString
	#define xnDumpWriteString
	#undef xnLogVerbose
	#define xnLogVerbose
	#undef xnLogInfo
	#define xnLogInfo
	#undef xnLogWarning
	#define xnLogWarning
	#undef xnLogError
	#define xnLogError
#endif

#endif //_XN_LOG_H_

