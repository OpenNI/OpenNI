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
#ifndef _XN_PLATFORM_ARC_H_
#define _XN_PLATFORM_ARC_H_

//---------------------------------------------------------------------------
// Prerequisites
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

//---------------------------------------------------------------------------
// Platform Basic Definition
//---------------------------------------------------------------------------
#define XN_PLATFORM XN_PLATFORM_ARC
#define XN_PLATFORM_STRING "ARC"

//---------------------------------------------------------------------------
// Platform Capabilities
//---------------------------------------------------------------------------
#define XN_PLATFORM_ENDIAN_TYPE XN_PLATFORM_IS_LITTLE_ENDIAN
#define XN_PLATFORM_VAARGS_TYPE XN_PLATFORM_USE_ARC_VAARGS_STYLE
#define XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS 0

//---------------------------------------------------------------------------
// Basic Types
//---------------------------------------------------------------------------
/** Boolean TRUE/FALSE type. */ 
typedef	unsigned int		XnBool;

/** Signed character for strings. */ 
typedef	char				XnChar;
/** Unsigned character for strings. */ 
typedef	unsigned char		XnUChar;

/** Signed wide character for strings. */ 
typedef	wchar_t				XnWChar;

/** 8-bit signed integer. */ 
typedef	signed char			XnInt8;
/** 8-bit unsigned integer. */ 
typedef	unsigned char		XnUInt8;

/** 16-bit signed integer. */ 
typedef	short				XnInt16;
/** 16-bit unsigned integer. */ 
typedef	unsigned short		XnUInt16;

/** 32-bit signed integer. */ 
typedef	int					XnInt32;
/** 32-bit unsigned integer. */ 
typedef	unsigned int		XnUInt32;

/** 64-bit signed integer. */ 
typedef	long long			XnInt64;
/** 64-bit unsigned integer. */ 
typedef	unsigned long long	XnUInt64;

/** natural signed integer. */ 
typedef	int					XnInt;
/** natural unsigned integer. */ 
typedef	unsigned int		XnUInt;

/** Float (32bit) */ 
typedef	float				XnFloat;
/** Double (64bit) */ 
typedef	double				XnDouble;

/** Far procedures type (for shared libraries functions). */ 
typedef void (*XnFarProc)(void *);

/** Size type. */ 
typedef size_t				XnSizeT;

/** Max unsigned 8-bit value */ 
#define XN_MAX_UINT8 255
/** Max unsigned 16-bit value */ 
#define XN_MAX_UINT16 65535
/** Max unsigned 32-bit value */ 
#define XN_MAX_UINT32 4294967295UL
/** Max unsigned 64-bit value */ 
#define XN_MAX_UINT64 18446744073709551615ULL

/** Min signed 8-bit value */ 
#define XN_MIN_INT8 -128
/** Min signed 16-bit value */ 
#define XN_MIN_INT16 -32768
/** Min signed 32-bit value */ 
#define XN_MIN_INT32 -2147483648
/** Min signed 64-bit value */ 
#define XN_MIN_INT64 -9223372036854775808LL

/** Max signed 8-bit value */ 
#define XN_MAX_INT8 127
/** Max signed 16-bit value */ 
#define XN_MAX_INT16 32767
/** Max signed 32-bit value */ 
#define XN_MAX_INT32 2147483647
/** Max signed 64-bit value */ 
#define XN_MAX_INT64 9223372036854775807LL

//---------------------------------------------------------------------------
// Memory
//---------------------------------------------------------------------------
/** The default memory alignment. */ 
#define XN_DEFAULT_MEM_ALIGN 16

/** The thread static declarator (using TLS). */
#define XN_THREAD_STATIC __declspec(thread)

//---------------------------------------------------------------------------
// Files
//---------------------------------------------------------------------------
/** The maximum allowed file path size (in bytes). */ 
#define XN_FILE_MAX_PATH 256

//---------------------------------------------------------------------------
// Call back
//---------------------------------------------------------------------------
/** The std call type. */ 
#define XN_STDCALL __stdcall

/** The call back calling convention. */ 
#define XN_CALLBACK_TYPE 

/** The C and C++ calling convension. */
#define XN_C_DECL

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
/** Returns the date and time at compile time. */ 
#define XN_TIMESTAMP __DATE__ " " __TIME__

/** Converts n into a pre-processor string.  */ 
#define XN_STRINGIFY(n) XN_STRINGIFY_HELPER(n)
#define XN_STRINGIFY_HELPER(n) #n

/** Asserts an expression, only on Debug build. */
#define XN_ASSERT(x)

//---------------------------------------------------------------------------
// API Export/Import Macros
//---------------------------------------------------------------------------
/** Indicates an exported shared library function. */ 
#define XN_API_EXPORT
	
/** Indicates an imported shared library function. */ 
#define XN_API_IMPORT 

/** Indicates a deprecated function */
#define XN_API_DEPRECATED(msg)

#define XN_DEPRECATED_WARNING_IDS			
#define XN_HIDES_PARENT_METHOD_WARNING_ID	
#define XN_CONDITION_IS_CONST_WARNING_ID
#define XN_PRAGMA_START_DISABLED_WARNING_SECTION(warnings)
#define XN_PRAGMA_STOP_DISABLED_WARNING_SECTION

/** Declares a global shared library export function. */ 
#define XN_API_EXPORT_INIT()

#endif //_XN_PLATFORM_ARC_H_

