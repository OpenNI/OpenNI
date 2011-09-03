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
#ifndef _XN_PLATFORM_WIN32_H_
#define _XN_PLATFORM_WIN32_H_

//---------------------------------------------------------------------------
// Prerequisites
//---------------------------------------------------------------------------
#ifndef WINVER						// Allow use of features specific to Windows XP or later
	#define WINVER 0x0501
#endif
#ifndef _WIN32_WINNT				// Allow use of features specific to Windows XP or later
	#define _WIN32_WINNT 0x0501
#endif						
#ifndef _WIN32_WINDOWS				// Allow use of features specific to Windows 98 or later
	#define _WIN32_WINDOWS 0x0410
#endif
#ifndef _WIN32_IE					// Allow use of features specific to IE 6.0 or later
	#define _WIN32_IE 0x0600
#endif
#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers

// Undeprecate CRT functions
#ifndef _CRT_SECURE_NO_DEPRECATE 
	#define _CRT_SECURE_NO_DEPRECATE 1
#endif

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <io.h>
#include <time.h>
#include <assert.h>
#include <float.h>
#include <crtdbg.h>

//---------------------------------------------------------------------------
// Platform Basic Definition
//---------------------------------------------------------------------------
#define XN_PLATFORM XN_PLATFORM_WIN32
#define XN_PLATFORM_STRING "Win32"

//---------------------------------------------------------------------------
// Platform Capabilities
//---------------------------------------------------------------------------
#define XN_PLATFORM_ENDIAN_TYPE XN_PLATFORM_IS_LITTLE_ENDIAN

#if _MSC_VER < 1400 // Before VS2005 there was no support for the vaargs macro...
	#define XN_PLATFORM_VAARGS_TYPE XN_PLATFORM_USE_NO_VAARGS
#else
	#define XN_PLATFORM_VAARGS_TYPE XN_PLATFORM_USE_WIN32_VAARGS_STYLE
#endif

#define XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS 1

//---------------------------------------------------------------------------
// Basic Types
//---------------------------------------------------------------------------
/** Boolean TRUE/FALSE type. */ 
typedef	BOOL				XnBool;

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
typedef	__int64				XnInt64;
/** 64-bit unsigned integer. */ 
typedef	unsigned __int64	XnUInt64;

/** natural signed integer. */ 
typedef	int					XnInt;
/** natural unsigned integer. */ 
typedef	unsigned int		XnUInt;

/** Float (32bit) */ 
typedef	float				XnFloat;
/** Double (64bit) */ 
typedef	double				XnDouble;

/** Far procedures type (for shared libraries functions). */ 
typedef FARPROC				XnFarProc;

/** Size type. */ 
typedef size_t				XnSizeT;

/** Max unsigned 8-bit value */ 
#define XN_MAX_UINT8 255
/** Max unsigned 16-bit value */ 
#define XN_MAX_UINT16 65535
/** Max unsigned 32-bit value */ 
#define XN_MAX_UINT32 4294967295
/** Max unsigned 64-bit value */ 
#define XN_MAX_UINT64 18446744073709551615

/** Min signed 8-bit value */ 
#define XN_MIN_INT8 -127
/** Min signed 16-bit value */ 
#define XN_MIN_INT16 -32767
/** Min signed 32-bit value */ 
#define XN_MIN_INT32 -2147483647
/** Min signed 64-bit value */ 
#define XN_MIN_INT64 -9223372036854775807

/** Max signed 8-bit value */ 
#define XN_MAX_INT8 127
/** Max signed 16-bit value */ 
#define XN_MAX_INT16 32767
/** Max signed 32-bit value */ 
#define XN_MAX_INT32 2147483647
/** Max signed 64-bit value */ 
#define XN_MAX_INT64 9223372036854775807

/** Min double value */
#define XN_MIN_DOUBLE DBL_MIN 
/** Max double value */
#define XN_MAX_DOUBLE DBL_MAX

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
#define XN_FILE_MAX_PATH MAX_PATH

//---------------------------------------------------------------------------
// Call backs
//---------------------------------------------------------------------------
/** The std call type. */ 
#define XN_STDCALL __stdcall

/** The call back calling convention. */ 
#define XN_CALLBACK_TYPE XN_STDCALL

/** The C and C++ calling convension. */
#define XN_C_DECL __cdecl

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
/** Returns the date and time at compile time. */ 
#define XN_TIMESTAMP __DATE__ " " __TIME__

/** Converts n into a pre-processor string.  */ 
#define XN_STRINGIFY(n) XN_STRINGIFY_HELPER(n)
#define XN_STRINGIFY_HELPER(n) #n

/** Asserts an expression, only on Debug build. */
#define XN_ASSERT(x)	_ASSERTE(x)

//---------------------------------------------------------------------------
// API Export/Import Macros
//---------------------------------------------------------------------------
/** Indicates an exported shared library function. */ 
#define XN_API_EXPORT __declspec(dllexport)

/** Indicates an imported shared library function. */ 
#define XN_API_IMPORT __declspec(dllimport)

/** Indicates a deprecated function */
#if _MSC_VER < 1400 // Before VS2005 there was no support for declspec deprecated...
	#define XN_API_DEPRECATED(msg)
#else
	#define XN_API_DEPRECATED(msg) __declspec(deprecated(msg))
#endif

#ifdef __INTEL_COMPILER
	#define XN_DEPRECATED_WARNING_IDS	1786
#else
	#define XN_DEPRECATED_WARNING_IDS	4995 4996
#endif

/** Declares a global shared library export function. */ 
#define XN_API_EXPORT_INIT()															\
	BOOL APIENTRY DllMain (HMODULE /*hModule*/, DWORD nReasonForCall, LPVOID /*lpReserved*/)	\
	{																					\
		switch (nReasonForCall)															\
		{																				\
		case DLL_PROCESS_ATTACH:														\
		case DLL_THREAD_ATTACH:															\
		case DLL_THREAD_DETACH:															\
		case DLL_PROCESS_DETACH:														\
			break;																		\
		}																				\
		return TRUE;																	\
	}

#endif //_XN_PLATFORM_WIN32_H_
