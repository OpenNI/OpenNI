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
#ifndef __XN_DUMP_H__
#define __XN_DUMP_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnPlatform.h"
#include "XnStatus.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
struct XnDumpFile;
typedef struct XnDumpFile XnDumpFile;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
* Configures if a specific dump mask is enabled.
*
* @param	strMask		[in]	The mask to set.
* @param	bEnabled	[in]	TRUE to enable this dump, FALSE otherwise.
*/
XN_C_API XnStatus XN_C_DECL xnDumpSetMaskState(const XnChar* strMask, XnBool bEnabled);

/**
* This function checks if a dump mask is enabled
*
* @param	strDumpMask	[in]	The mask that should be checked.
*/
XN_C_API XnBool XN_C_DECL xnLogIsDumpMaskEnabled(const XnChar* strDumpMask);

/**
* Opens a file for writing dump.
*
* @param	strDumpName		[in]	Name of the dump mask this file belongs to.
* @param	strNameFormat	[in]	A format string for the name of the file.
*
* @returns a file handle for writing data. The file should be closed using @ref xnDumpFileClose().
*/
XN_C_API XnDumpFile* XN_C_DECL xnDumpFileOpen(const XnChar* strDumpName, const XnChar* strNameFormat, ...);

/**
* Opens a file for writing dump using some advanced options.
* 
* You would usually prefer to use @ref xnDumpFileOpen().
*
* @param	strDumpName		[in]	Name of the dump mask this file belongs to.
* @param	bForce			[in]	When TRUE, file will be created even if dump is currently off.
* @param	bSessionDump	[in]	When TRUE, file will be created with current session timestamp as a prefix to its name.
* @param	strNameFormat	[in]	A format string for the name of the file.
*
* @returns a file handle for writing data. The file should be closed using @ref xnDumpFileClose().
*/
XN_C_API XnDumpFile* XN_C_DECL xnDumpFileOpenEx(const XnChar* strDumpName, XnBool bForce, XnBool bSessionDump, const XnChar* strNameFormat, ...);

/**
* Writes a buffer to a dump file.
*
* @param	pFile			[in]	Dump file to write to. A pointer retrieved from @ref xnDumpFileOpen.
* @param	pBuffer			[in]	Data to be written to file.
* @param	nBufferSize		[in]	Size of the buffer.
*/
XN_C_API void XN_C_DECL _xnDumpFileWriteBuffer(XnDumpFile* pFile, const void* pBuffer, XnUInt32 nBufferSize);

/**
* Writes a formatted string to a dump file.
*
* @param	pFile		[in]	Dump file to write to. A pointer retrieved from @ref xnDumpFileOpen.
* @param	strFormat	[in]	Format string.
*
* NOTE: the total length of the string must not exceed 8 KB. If it does, it will be truncated.
*/
XN_C_API void XN_C_DECL _xnDumpFileWriteString(XnDumpFile* pFile, const XnChar* strFormat, ...);

/**
* Closes a dump file.
*
* @param	hFile		[in]	Dump file to close. A pointer retrieved from @ref xnDumpFileOpen.
*/
XN_C_API void XN_C_DECL _xnDumpFileClose(XnDumpFile* pFile);

#define xnDumpFileWriteBuffer(pFile, pBuffer, nBufferSize)		\
	if ((pFile) != NULL)										\
	{															\
		_xnDumpFileWriteBuffer(pFile, pBuffer, nBufferSize);	\
	}															\

#define xnDumpFileClose(pFile)									\
	if ((pFile) != NULL)										\
	{															\
		_xnDumpFileClose(pFile);								\
		pFile = NULL;											\
	}															\

#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define xnDumpFileWriteString(pFile, strFormat, ...)			\
		if ((pFile) != NULL)										\
		{															\
			_xnDumpFileWriteString(pFile, strFormat, __VA_ARGS__);	\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define xnDumpFileWriteString(pFile, strFormat, ...)			\
		if ((pFile) != NULL)										\
		{															\
			_xnDumpFileWriteString(pFile, strFormat, ##__VA_ARGS__);\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_ARC_VAARGS_STYLE
	#define xnDumpFileWriteString(pFile, strFormat, ...)			\
		if ((pFile) != NULL)										\
		{															\
			_xnDumpFileWriteString(pFile, strFormat);				\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_NO_VAARGS
	#define xnDumpFileWriteString(pFile, strFormat, arg)			\
		if ((pFile) != NULL)										\
		{															\
			_xnDumpFileWriteString(pFile, strFormat,arg);			\
		}
#else
	#error Xiron Log - Unknown VAARGS type!
#endif


//---------------------------------------------------------------------------
// Backwards Compatibility Stuff
//---------------------------------------------------------------------------

#ifndef __XN_NO_BC__

#include "XnOS.h"

typedef struct XnDump 
{
	XN_FILE_HANDLE hFile;
} XnDump;

const XnDump XN_DUMP_CLOSED = { XN_INVALID_FILE_HANDLE };

XN_C_API void XN_API_DEPRECATED("Use xnDumpFileX methods instead") XN_C_DECL xnDumpInit(XnDump* pDump, const XnChar* csDumpMask, const XnChar* csHeader, const XnChar* csFileNameFormat, ...);
XN_C_API void XN_API_DEPRECATED("Use xnDumpFileX methods instead") XN_C_DECL xnDumpForceInit(XnDump* pDump, const XnChar* csHeader, const XnChar* csFileNameFormat, ...);
XN_C_API void XN_API_DEPRECATED("Use xnDumpFileX methods instead") XN_C_DECL xnDumpClose(XnDump* pDump);
XN_C_API void XN_API_DEPRECATED("Use xnDumpFileX methods instead") XN_C_DECL xnDumpWriteBufferImpl(XnDump dump, const void* pBuffer, XnUInt32 nBufferSize);
XN_C_API void XN_API_DEPRECATED("Use xnDumpFileX methods instead") XN_C_DECL xnDumpWriteStringImpl(XnDump dump, const XnChar* csFormat, ...);
XN_C_API void XN_API_DEPRECATED("Use xnDumpFileX methods instead") XN_C_DECL xnDumpFlush(XnDump dump);

#define xnDumpWriteBuffer(dump, pBuffer, nBufferSize)		\
	if (dump.hFile != XN_INVALID_FILE_HANDLE)				\
	{														\
		xnDumpWriteBufferImpl(dump, pBuffer, nBufferSize);	\
	}

#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define xnDumpWriteString(dump, csFormat, ...)						\
		if ((dump).hFile != XN_INVALID_FILE_HANDLE) {					\
			xnDumpWriteStringImpl((dump), csFormat, __VA_ARGS__);		\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define xnDumpWriteString(dump, csFormat, ...)						\
		if ((dump).hFile != XN_INVALID_FILE_HANDLE) {					\
			xnDumpWriteStringImpl((dump), csFormat, ##__VA_ARGS__);		\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_ARC_VAARGS_STYLE
	#define xnDumpWriteString(dump, csFormat...)						\
		if ((dump).hFile != XN_INVALID_FILE_HANDLE) {					\
			xnDumpWriteStringImpl((dump), csFormat);					\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_NO_VAARGS
	#define xnDumpWriteString(dump, csFormat, arg)						\
		if ((dump).hFile != XN_INVALID_FILE_HANDLE) {					\
			xnDumpWriteStringImpl((dump), csFormat, arg);				\
		}
#else
	#error Xiron Log - Unknown VAARGS type!
#endif

#endif // #ifndef __XN_NO_BC__

#endif // __XN_DUMP_H__