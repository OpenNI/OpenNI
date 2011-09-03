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
#ifndef __XN_DUMP_H__
#define __XN_DUMP_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnOS.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnDump 
{
	XN_FILE_HANDLE hFile;
} XnDump;

const XnDump XN_DUMP_CLOSED = { XN_INVALID_FILE_HANDLE };

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
* Configures if a specific dump mask is enabled.
*
* @param	csMask		[in]	The mask to set.
* @param	bEnabled	[in]	TRUE to enable this mask, FALSE otherwise.
*/
XN_C_API XnStatus XN_C_DECL xnDumpSetMaskState(const XnChar* csMask, XnBool bEnabled);

/**
* This function checks if a dump mask is enabled
*
* @param	csDumpMask	[in]	The mask that should be checked.
*/
XN_C_API XnBool XN_C_DECL xnLogIsDumpMaskEnabled(const XnChar* csDumpMask);

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
XN_C_API void XN_C_DECL xnDumpInit(XnDump* pDump, const XnChar* csDumpMask, const XnChar* csHeader, const XnChar* csFileNameFormat, ...);

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
XN_C_API void XN_C_DECL xnDumpForceInit(XnDump* pDump, const XnChar* csHeader, const XnChar* csFileNameFormat, ...);

/**
* This function closes a dump file (use it once file is no longer needed).
* You can later use xnDumpWriteString and xnDumpWriteBuffer to write to this file.
*
* @param	pDump	[in]	A pointer to an XnDump struct returned from a call to xnDumpInit.
*/
XN_C_API void XN_C_DECL xnDumpClose(XnDump* pDump);

/**
* Writes a buffer to the dump.
* 
* @param	dump		[in]	The dump to write to.
* @param	pBuffer		[in]	A pointer to the buffer to write.
* @param	nBufferSize	[in]	The number of bytes to write from the buffer.
*/
XN_C_API void XN_C_DECL xnDumpWriteBufferImpl(XnDump dump, const void* pBuffer, XnUInt32 nBufferSize);

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
XN_C_API void XN_C_DECL xnDumpWriteStringImpl(XnDump dump, const XnChar* csFormat, ...);

/**
* Flushes a dump to the disk.
* 
* @param	dump		[in]	The dump to flush.
*/
XN_C_API void XN_C_DECL xnDumpFlush(XnDump dump);

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

#endif // __XN_DUMP_H__