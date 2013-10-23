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
#define XN_MAX_UINT8 _UI8_MAX
/** Max unsigned 16-bit value */ 
#define XN_MAX_UINT16 _UI16_MAX
/** Max unsigned 32-bit value */ 
#define XN_MAX_UINT32 _UI32_MAX
/** Max unsigned 64-bit value */ 
#define XN_MAX_UINT64 _UI64_MAX

/** Min signed 8-bit value */ 
#define XN_MIN_INT8 _I8_MIN
/** Min signed 16-bit value */ 
#define XN_MIN_INT16 _I16_MIN
/** Min signed 32-bit value */ 
#define XN_MIN_INT32 _I32_MIN
/** Min signed 64-bit value */ 
#define XN_MIN_INT64 _I64_MIN

/** Max signed 8-bit value */ 
#define XN_MAX_INT8 _I8_MAX
/** Max signed 16-bit value */ 
#define XN_MAX_INT16 _I16_MAX
/** Max signed 32-bit value */ 
#define XN_MAX_INT32 _I32_MAX
/** Max signed 64-bit value */ 
#define XN_MAX_INT64 _I64_MAX

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
	#define XN_DEPRECATED_WARNING_IDS				1786
	#define XN_HIDES_PARENT_METHOD_WARNING_ID		1125
	#define XN_CONDITION_IS_CONST_WARNING_ID	
	#define XN_INHERITS_VIA_DOMINANCE_WARNING_ID
	#define XN_UNALIGNED_ADDRESS_WARNING_ID
	#define XN_STRUCT_PADDED_WARNING_ID
#else
	#define XN_DEPRECATED_WARNING_IDS				4995 4996
	#define XN_HIDES_PARENT_METHOD_WARNING_ID	
	#define XN_CONDITION_IS_CONST_WARNING_ID		4127
	#define XN_INHERITS_VIA_DOMINANCE_WARNING_ID	4250
	#define XN_UNALIGNED_ADDRESS_WARNING_ID			4366
	#define XN_STRUCT_PADDED_WARNING_ID				4324
#endif

#define XN_PRAGMA_START_DISABLED_WARNING_SECTION(warnings)			\
	__pragma(warning(push))											\
	__pragma(warning(disable: warnings))

#define XN_PRAGMA_STOP_DISABLED_WARNING_SECTION						\
	__pragma(warning(pop))

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
