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
#ifndef __XN_OS_H__
#define __XN_OS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnPlatform.h"
#include "XnMacros.h"
#include "XnStatusCodes.h"
#include "XnOSStrings.h"
#include "XnOSMemory.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MASK_OS "xnOS"

// uncomment next line to activate memory profiling
//#define XN_MEM_PROFILING

//---------------------------------------------------------------------------
// OS Identifier 
//---------------------------------------------------------------------------
#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	#include "Win32/XnOSWin32.h"
#elif (XN_PLATFORM == XN_PLATFORM_LINUX_X86 || XN_PLATFORM == XN_PLATFORM_LINUX_ARM || XN_PLATFORM == XN_PLATFORM_MACOSX || XN_PLATFORM == XN_PLATFORM_ANDROID_ARM)
	#include "Linux-x86/XnOSLinux-x86.h"
#elif defined(_ARC)
	#include "ARC/XnOSARC.h"
#else
	#error OpenNI OS Abstraction Layer - Unsupported Platform!
#endif

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
#define XN_MAX_OS_NAME_LENGTH 255

typedef struct xnOSInfo
{
	XnChar csOSName[XN_MAX_OS_NAME_LENGTH];
	XnChar csCPUName[XN_MAX_OS_NAME_LENGTH];
	XnUInt32 nProcessorsCount;
	XnUInt64 nTotalMemory;
} xnOSInfo;

typedef XnBool (XN_CALLBACK_TYPE* XnConditionFunc)(void* pConditionData);

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
/** The time since Xiron Core was initialized */ 
extern XnOSTimer g_xnOSHighResGlobalTimer;

//---------------------------------------------------------------------------
// Files
//---------------------------------------------------------------------------
// File open modes
/** Open the file for reading. */ 
#define XN_OS_FILE_READ				0x01
/** Open the file for writing. */ 
#define XN_OS_FILE_WRITE			0x02
/** Create a new file (but only if it doesn't exist). */ 
#define XN_OS_FILE_CREATE_NEW_ONLY	0x04
/** Truncate the file if it already exists. */ 
#define XN_OS_FILE_TRUNCATE			0x08
/** File be opened in append mode */ 
#define XN_OS_FILE_APPEND			0x10
/** All writes will be immediately written to disk */ 
#define XN_OS_FILE_AUTO_FLUSH		0x20

// Seek types
/** The seek type enum list. */ 
typedef enum {
	/** Absolute seek from the file beginning. */ 
	XN_OS_SEEK_SET = 0,
	/** Relative seek from the current location. */ 
	XN_OS_SEEK_CUR,
	/** Absolute seek from the file ending. */ 
	XN_OS_SEEK_END
} XnOSSeekType;

//---------------------------------------------------------------------------
// Network
//---------------------------------------------------------------------------
// Network socket type
/** The network socket type. */ 
typedef enum {
	/** UDP socket. */ 
	XN_OS_UDP_SOCKET = 0,
	/** TCP socket. */ 
	XN_OS_TCP_SOCKET
} XnOSSocketType;

#define XN_OS_NETWORK_LOCAL_HOST	"127.0.0.1"

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
// Memory
/** Validate that the input pointer X is not NULL. */ 
#define XN_VALIDATE_INPUT_PTR(x) XN_VALIDATE_PTR(x, XN_STATUS_NULL_INPUT_PTR)

/** Validate that the output pointer X is not NULL. */ 
#define XN_VALIDATE_OUTPUT_PTR(x) XN_VALIDATE_PTR(x, XN_STATUS_NULL_OUTPUT_PTR)
/** Validate that a X pointer after a memory allocation call is not NULL. */ 
#define XN_VALIDATE_ALLOC_PTR(x) XN_VALIDATE_PTR(x, XN_STATUS_ALLOC_FAILED)

/** Allocate Y bytes into the X pointer and make sure NULL wasn't returned. */ 
#define XN_VALIDATE_ALLOC(x,y)			\
		x = (y*)xnOSMalloc(sizeof(y));	\
		XN_VALIDATE_ALLOC_PTR(x);

/** Allocate Z elements of Y type into the X pointer and make sure NULL wasn't returned. */ 
#define XN_VALIDATE_CALLOC(x,y,z)			\
		x = (y*)xnOSCalloc(z, sizeof(y));	\
		XN_VALIDATE_ALLOC_PTR(x);

/** Allocate Y aligned bytes into the X pointer and make sure NULL wasn't returned. */ 
#define XN_VALIDATE_ALIGNED_ALLOC(x,y,w)			\
		x = (y*)xnOSMallocAligned(sizeof(y), w);	\
		XN_VALIDATE_ALLOC_PTR(x);

/** Allocate Z aligned elements of Y type into the X pointer and make sure NULL wasn't returned. */ 
#define XN_VALIDATE_ALIGNED_CALLOC(x,y,z,w)			\
		x = (y*)xnOSCallocAligned(z, sizeof(y), w);	\
		XN_VALIDATE_ALLOC_PTR(x);

/** Validate that the memory free request succeeded and set X to NULL. */ 
#define XN_FREE_AND_NULL(x)		\
		if (x != NULL)			\
		{						\
			xnOSFree(x);		\
			x = NULL;			\
		}
/** Validate that the aligned memory free request succeeded and set X to NULL. */ 
#define XN_ALIGNED_FREE_AND_NULL(x)		\
		if (x != NULL)			\
		{						\
			xnOSFreeAligned(x);	\
			x = NULL;			\
		}


/** Creates a new type object and validates that allocation succeeded. */
#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define XN_VALIDATE_NEW(ptr, type, ...)						\
		{														\
			(ptr) = XN_NEW(type, __VA_ARGS__);					\
			if ((ptr) == NULL)									\
			{													\
				return (XN_STATUS_ALLOC_FAILED);				\
			}													\
		}

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define XN_VALIDATE_NEW(ptr, type, ...)						\
		{														\
			(ptr) = XN_NEW(type, ##__VA_ARGS__);				\
			if ((ptr) == NULL)									\
			{													\
				return (XN_STATUS_ALLOC_FAILED);				\
			}													\
		} 
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_ARC_VAARGS_STYLE
	#define XN_VALIDATE_NEW(ptr, type...)						\
		{														\
			(ptr) = XN_NEW(type);								\
			if ((ptr) == NULL)									\
			{													\
				return (XN_STATUS_ALLOC_FAILED);				\
			}													\
		} 
#else
	#define XN_VALIDATE_NEW(ptr, type)							\
		{														\
			(ptr) = XN_NEW(type);								\
			if ((ptr) == NULL)									\
			{													\
				return (XN_STATUS_ALLOC_FAILED);				\
			}													\
		}
#endif

/** Creates a new type object, validates that allocation succeeded, and initializes the object (type must have an Init function). */
#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define XN_VALIDATE_NEW_AND_INIT(ptr, type, ...)			\
		{														\
			XN_VALIDATE_NEW(ptr, type, __VA_ARGS__);			\
			XnStatus rc = (ptr)->Init();						\
			if (rc != XN_STATUS_OK)								\
			{													\
				XN_DELETE(ptr);									\
				return (rc);									\
			}													\
		} 
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define XN_VALIDATE_NEW_AND_INIT(ptr, type, ...)			\
		{														\
			XN_VALIDATE_NEW(ptr, type, ##__VA_ARGS__);			\
			XnStatus rc = (ptr)->Init();						\
			if (rc != XN_STATUS_OK)								\
			{													\
				XN_DELETE(ptr);									\
				return (rc);									\
			}													\
		} 
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_ARC_VAARGS_STYLE
	#define XN_VALIDATE_NEW_AND_INIT(ptr, type...)				\
		{														\
			XN_VALIDATE_NEW(ptr, type);							\
			XnStatus rc = (ptr)->Init();						\
			if (rc != XN_STATUS_OK)								\
			{													\
				XN_DELETE(ptr);									\
				return (rc);									\
			}													\
		} 
#else
	#define XN_VALIDATE_NEW_AND_INIT(ptr, type)					\
		{														\
			XN_VALIDATE_NEW(ptr, type);							\
			XnStatus rc = (ptr)->Init();						\
			if (rc != XN_STATUS_OK)								\
			{													\
				XN_DELETE(ptr);									\
				return (rc);									\
			}													\
		} 
#endif

// Strings
/** Append x into y (with z as the max size) and check the status via z. */ 
#define XN_VALIDATE_STR_APPEND(w,x,y,z)	\
	z = xnOSStrAppend(w, x, y);			\
	XN_IS_STATUS_OK(z);

/** Prefix x into y (with z as the max size) and check the status via z. */ 
#define XN_VALIDATE_STR_PREFIX(w,x,y,z)	\
	z = xnOSStrPrefix(w, x, y);			\
	XN_IS_STATUS_OK(z);

#define XN_VALIDATE_STR_COPY(w,x,y,z)	\
	z = xnOSStrCopy(w, x, y);			\
	XN_IS_STATUS_OK(z);

#define XN_VALIDATE_STRN_COPY(v,w,x,y,z)	\
	z = xnOSStrNCopy(v, w, x, y);			\
	XN_IS_STATUS_OK(z);

// INI
/** Read a string from the INI file and check the status via z. */ 
#define XN_VALIDATE_READ_INI_STR(u,v,w,x,y,z)		\
		z = xnOSReadStringFromINI(u, v, w, x, y);	\
		XN_IS_STATUS_OK(z);

/** Read an int from the INI file and check the status via z. */ 
#define XN_VALIDATE_READ_INI_INT(v,w,x,y,z)		\
		z = xnOSReadIntFromINI(v, w, x, y);		\
		XN_IS_STATUS_OK(z);

/** Read a float from the INI file and check the status via z. */ 
#define XN_VALIDATE_READ_INI_FLOAT(v,w,x,y,z)	\
		z = xnOSReadFloatFromINI(v, w, x, y);	\
		XN_IS_STATUS_OK(z);

/** Read a double from the INI file and check the status via z. */ 
#define XN_VALIDATE_READ_INI_DOUBLE(v,w,x,y,z)	\
		z = xnOSReadDoubleFromINI(v, w, x, y);	\
		XN_IS_STATUS_OK(z);

// Mutex
/** Lock the mutex x for a y period of time and check the status via z. */ 
#define XN_VALIDATE_LOCK_MUTEX(x,y,z)	\
		z = xnOSLockMutex(x, y);		\
		XN_IS_STATUS_OK(z);

/** UnLock the mutex x and check the status via z. */ 
#define XN_VALIDATE_UNLOCK_MUTEX(x,z)	\
		z = xnOSUnLockMutex(x);			\
		XN_IS_STATUS_OK(z);

// Files
/** Returns XN_STATUS_OS_FILE_NOT_FOUND if the file x doesn't exists. */ 
#define XN_VALIDATE_FILE_EXISTS_RET(x,y,z,w)	\
		y = xnOSDoesFileExist(x, &z);				\
		XN_IS_STATUS_OK(y);						\
		if (z == FALSE)							\
		{										\
			return (w);							\
		}
#define XN_VALIDATE_FILE_EXISTS(x,y,z)	\
		XN_VALIDATE_FILE_EXISTS_RET(x,y,z,XN_STATUS_OS_FILE_NOT_FOUND)

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------
// Common
XN_C_API XnStatus XN_C_DECL xnOSInit();
XN_C_API XnStatus XN_C_DECL xnOSShutdown();
XN_C_API XnStatus XN_C_DECL xnOSGetInfo(xnOSInfo* pOSInfo);


#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define XN_NEW(type, ...)		new type(__VA_ARGS__)
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define XN_NEW(type, ...)		new type(__VA_ARGS__)
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_ARC_VAARGS_STYLE
	#define XN_NEW(type, arg...)	new type(arg)
#else
	#define XN_NEW(type, arg)		new type(arg)
#endif

#define XN_NEW_ARR(type, count)		new type[count]
#define XN_DELETE(p)				delete (p)
#define XN_DELETE_ARR(p)			delete[] (p)

typedef enum
{
	XN_ALLOCATION_MALLOC,
	XN_ALLOCATION_MALLOC_ALIGNED,
	XN_ALLOCATION_CALLOC,
	XN_ALLOCATION_CALLOC_ALIGNED,
	XN_ALLOCATION_NEW,
	XN_ALLOCATION_NEW_ARRAY
} XnAllocationType;

/**
* Memory Profiling - Logs an allocation of memory.
*/
XN_C_API void* XN_C_DECL xnOSLogMemAlloc(void* pMemBlock, XnAllocationType nAllocType, XnUInt32 nBytes, const XnChar* csFunction, const XnChar* csFile, XnUInt32 nLine, const XnChar* csAdditional);

/**
* Memory Profiling - Logs freeing of memory.
*/
XN_C_API void XN_C_DECL xnOSLogMemFree(const void* pMemBlock);

/**
* Memory Profiling - Prints a current memory report to requested file.
*/
XN_C_API void XN_C_DECL xnOSWriteMemoryReport(const XnChar* csFileName);

// for memory profiling, replace all malloc/calloc/free/new/delete calls
#if (defined XN_MEM_PROFILING) && (!defined(XN_OS_IMPL))
	#ifdef _MSC_VER 
		#pragma message("Compiling with Memory Profiling!")
	#elif defined(__INTEL_COMPILER)
		#warning "Compiling with Memory Profiling!"
	//TODO: Add warning for linux compiler(s)
	#endif

	#ifdef __cplusplus
		#include <new>
		static void* operator new(size_t size)
		{
			void* p = xnOSMalloc(size);
			return xnOSLogMemAlloc(p, XN_ALLOCATION_NEW, size, "", "", 0, "");
		}
		static void* operator new[](size_t size)
		{
			void* p = xnOSMalloc(size);
			return xnOSLogMemAlloc(p, XN_ALLOCATION_NEW, size, "", "", 0, "");
		}
		static void* operator new(size_t size, const XnChar* csFunction, const XnChar* csFile, XnUInt32 nLine, const XnChar* csAdditional)
		{
			void* p = xnOSMalloc(size);
			return xnOSLogMemAlloc(p, XN_ALLOCATION_NEW, size, csFunction, csFile, nLine, csAdditional);
		}

		// called only if ctor threw exception
		static void operator delete(void* p, const XnChar* /*csFunction*/, const XnChar* /*csFile*/, XnUInt32 /*nLine*/, const XnChar* /*csAdditional*/)
		{
			xnOSLogMemFree(p);
			xnOSFree(p);
		}

		static void operator delete(void* p)
		{
			xnOSLogMemFree(p);
			xnOSFree(p);
		}

		static void* operator new[](size_t size, const XnChar* csFunction, const XnChar* csFile, XnUInt32 nLine, const XnChar* csAdditional)
		{
			void* p = xnOSMalloc(size);
			return xnOSLogMemAlloc(p, XN_ALLOCATION_NEW_ARRAY, size, csFunction, csFile, nLine, csAdditional);
		}

		// called only if ctor threw exception
		static void operator delete[](void* p, const XnChar* /*csFunction*/, const XnChar* /*csFile*/, XnUInt32 /*nLine*/, const XnChar* /*csAdditional*/)
		{
			xnOSLogMemFree(p);
			xnOSFree(p);
		}

		static void operator delete[](void* p)
		{
			xnOSLogMemFree(p);
			xnOSFree(p);
		}

		#define xnOSMalloc(nAllocSize)									xnOSLogMemAlloc(xnOSMalloc(nAllocSize), XN_ALLOCATION_MALLOC, nAllocSize, __FUNCTION__, __FILE__, __LINE__, NULL)
		#define xnOSMallocAligned(nAllocSize, nAlignment)				xnOSLogMemAlloc(xnOSMallocAligned(nAllocSize, nAlignment), XN_ALLOCATION_MALLOC_ALIGNED, nAllocSize, __FUNCTION__, __FILE__, __LINE__, "Aligned to " XN_STRINGIFY(nAlignment))
		#define xnOSCalloc(nAllocNum, nAllocSize)						xnOSLogMemAlloc(xnOSCalloc(nAllocNum, nAllocSize), XN_ALLOCATION_CALLOC, nAllocNum*nAllocSize, __FUNCTION__, __FILE__, __LINE__, NULL)
		#define xnOSCallocAligned(nAllocNum, nAllocSize, nAlignment)	xnOSLogMemAlloc(xnOSCallocAligned(nAllocNum, nAllocSize, nAlignment), XN_ALLOCATION_CALLOC_ALIGNED, nAllocNum*nAllocSize, __FUNCTION__, __FILE__, __LINE__, "Aligned to " XN_STRINGIFY(nAlignment))
		#define xnOSFree(pMemBlock)										{ xnOSLogMemFree(pMemBlock); xnOSFree(pMemBlock); }
		#define xnOSFreeAligned(pMemBlock)								{ xnOSLogMemFree(pMemBlock); xnOSFreeAligned(pMemBlock); }

		#undef XN_NEW
		#undef XN_NEW_ARR

		#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
			#define XN_NEW(type, ...)		new (__FUNCTION__, __FILE__, __LINE__, XN_STRINGIFY(type)) type(__VA_ARGS__)
		#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
			#define XN_NEW(type, arg...)	new (__FUNCTION__, __FILE__, __LINE__, XN_STRINGIFY(type)) type(arg)
		#else
			#define XN_NEW(type, arg)		new (__FUNCTION__, __FILE__, __LINE__, XN_STRINGIFY(type)) type(arg)
		#endif

		#define XN_NEW_ARR(type, count)		new (__FUNCTION__, __FILE__, __LINE__, XN_STRINGIFY(count) " " XN_STRINGIFY(type)) type[count]

	#endif
#endif

// Files
XN_C_API XnStatus XN_C_DECL xnOSGetFileList(const XnChar* cpSearchPattern, const XnChar* cpPrefixPath, XnChar cpFileList[][XN_FILE_MAX_PATH], const XnUInt32 nMaxFiles, XnUInt32* pnFoundFiles);
XN_C_API XnStatus XN_C_DECL xnOSOpenFile(const XnChar* cpFileName, const XnUInt32 nFlags, XN_FILE_HANDLE* pFile);
XN_C_API XnStatus XN_C_DECL xnOSCloseFile(XN_FILE_HANDLE* pFile);
XN_C_API XnStatus XN_C_DECL xnOSReadFile(const XN_FILE_HANDLE File, void* pBuffer, XnUInt32* pnBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSWriteFile(const XN_FILE_HANDLE File, const void* pBuffer, const XnUInt32 nBufferSize);
XN_C_API XnStatus XN_API_DEPRECATED("Use xnOSSeekFile64() instead") XN_C_DECL 
			    xnOSSeekFile  (const XN_FILE_HANDLE File, const XnOSSeekType SeekType, const XnInt32 nOffset);
XN_C_API XnStatus XN_C_DECL xnOSSeekFile64(const XN_FILE_HANDLE File, const XnOSSeekType SeekType, const XnInt64 nOffset);
XN_C_API XnStatus XN_API_DEPRECATED("Use xnOSTellFile64() instead") XN_C_DECL 
			    xnOSTellFile  (const XN_FILE_HANDLE File, XnUInt32* nFilePos);
XN_C_API XnStatus XN_C_DECL xnOSTellFile64(const XN_FILE_HANDLE File, XnUInt64* nFilePos);
XN_C_API XnStatus XN_C_DECL xnOSFlushFile(const XN_FILE_HANDLE File);
XN_C_API XnStatus XN_C_DECL xnOSDoesFileExist(const XnChar* cpFileName, XnBool* pbResult);
XN_C_API XnStatus XN_C_DECL xnOSDoesDirecotyExist(const XnChar* cpDirName, XnBool* pbResult);
XN_C_API XnStatus XN_C_DECL xnOSLoadFile(const XnChar* cpFileName, void* pBuffer, const XnUInt32 nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSSaveFile(const XnChar* cpFileName, const void* pBuffer, const XnUInt32 nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSAppendFile(const XnChar* cpFileName, const void* pBuffer, const XnUInt32 nBufferSize);
XN_C_API XnStatus XN_API_DEPRECATED("Use xnOSGetFileSize64() instead") XN_C_DECL 
			    xnOSGetFileSize  (const XnChar* cpFileName, XnUInt32* pnFileSize);
XN_C_API XnStatus XN_C_DECL xnOSGetFileSize64(const XnChar* cpFileName, XnUInt64* pnFileSize);
XN_C_API XnStatus XN_C_DECL xnOSCreateDirectory(const XnChar* cpDirName);
XN_C_API XnStatus XN_C_DECL xnOSGetDirName(const XnChar* cpFilePath, XnChar* cpDirName, const XnUInt32 nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSGetFileName(const XnChar* cpFilePath, XnChar* cpFileName, const XnUInt32 nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSGetFullPathName(const XnChar* strFilePath, XnChar* strFullPath, XnUInt32 nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSGetCurrentDir(XnChar* cpDirName, const XnUInt32 nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSSetCurrentDir(const XnChar* cpDirName);
XN_C_API XnStatus XN_C_DECL xnOSDeleteFile(const XnChar* cpFileName);
XN_C_API XnStatus XN_C_DECL xnOSDeleteEmptyDirectory(const XnChar* strDirName);
XN_C_API XnStatus XN_C_DECL xnOSDeleteDirectoryTree(const XnChar* strDirName);

// INI
XN_C_API XnStatus XN_C_DECL xnOSReadStringFromINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, XnChar* cpDest, const XnUInt32 nDestLength);
XN_C_API XnStatus XN_C_DECL xnOSReadFloatFromINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, XnFloat* fDest);
XN_C_API XnStatus XN_C_DECL xnOSReadDoubleFromINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, XnDouble* fDest);
XN_C_API XnStatus XN_C_DECL xnOSReadIntFromINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, XnUInt32* nDest);
XN_C_API XnStatus XN_C_DECL xnOSWriteStringToINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, const XnChar* cpSrc);
XN_C_API XnStatus XN_C_DECL xnOSWriteFloatToINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, const XnFloat fSrc);
XN_C_API XnStatus XN_C_DECL xnOSWriteDoubleToINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, const XnDouble fSrc);
XN_C_API XnStatus XN_C_DECL xnOSWriteIntToINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, const XnUInt32 nSrc);

// Shared libraries
XN_C_API XnStatus XN_C_DECL xnOSLoadLibrary(const XnChar* cpFileName, XN_LIB_HANDLE* pLibHandle);
XN_C_API XnStatus XN_C_DECL xnOSFreeLibrary(const XN_LIB_HANDLE LibHandle);
XN_C_API XnStatus XN_C_DECL xnOSGetProcAddress(const XN_LIB_HANDLE LibHandle, const XnChar* cpProcName, XnFarProc* pProcAddr);

struct timespec;
	
// Time
XN_C_API XnStatus XN_C_DECL xnOSGetEpochTime(XnUInt32* nEpochTime);
XN_C_API XnStatus XN_C_DECL xnOSGetTimeStamp(XnUInt64* nTimeStamp);
XN_C_API XnStatus XN_C_DECL xnOSGetHighResTimeStamp(XnUInt64* nTimeStamp);
XN_C_API XnStatus XN_C_DECL xnOSSleep(XnUInt32 nMilliseconds);
XN_C_API XnStatus XN_C_DECL xnOSStartTimer(XnOSTimer* pTimer);
XN_C_API XnStatus XN_C_DECL xnOSStartHighResTimer(XnOSTimer* pTimer);
XN_C_API XnStatus XN_C_DECL xnOSQueryTimer(XnOSTimer Timer, XnUInt64* pnTimeSinceStart);
XN_C_API XnStatus XN_C_DECL xnOSStopTimer(XnOSTimer* pTimer);
XN_C_API XnStatus XN_C_DECL xnOSGetMonoTime(struct timespec* pTime);
XN_C_API XnStatus XN_C_DECL xnOSGetTimeout(struct timespec* pTime, XnUInt32 nMilliseconds);
XN_C_API XnStatus XN_C_DECL xnOSGetAbsTimeout(struct timespec* pTime, XnUInt32 nMilliseconds);

// Threads
typedef enum XnThreadPriority
{
	XN_PRIORITY_LOW,
	XN_PRIORITY_NORMAL,
	XN_PRIORITY_HIGH,
	XN_PRIORITY_CRITICAL
} XnThreadPriority;

XN_C_API XnStatus XN_C_DECL xnOSCreateThread(XN_THREAD_PROC_PROTO pThreadProc, const XN_THREAD_PARAM pThreadParam, XN_THREAD_HANDLE* pThreadHandle);
XN_C_API XnStatus XN_C_DECL xnOSTerminateThread(XN_THREAD_HANDLE* pThreadHandle);
XN_C_API XnStatus XN_C_DECL xnOSCloseThread(XN_THREAD_HANDLE* pThreadHandle);
XN_C_API XnStatus XN_C_DECL xnOSWaitForThreadExit(XN_THREAD_HANDLE ThreadHandle, XnUInt32 nMilliseconds);
XN_C_API XnStatus XN_C_DECL xnOSSetThreadPriority(XN_THREAD_HANDLE ThreadHandle, XnThreadPriority nPriority);
XN_C_API XnStatus XN_C_DECL xnOSGetCurrentThreadID(XN_THREAD_ID* pThreadID);
XN_C_API XnStatus XN_C_DECL xnOSWaitAndTerminateThread(XN_THREAD_HANDLE* pThreadHandle, XnUInt32 nMilliseconds);

// Processes
XN_C_API XnStatus XN_C_DECL xnOSGetCurrentProcessID(XN_PROCESS_ID* pProcID);
XN_C_API XnStatus XN_C_DECL xnOSCreateProcess(const XnChar* strExecutable, XnUInt32 nArgs, const XnChar** pstrArgs, XN_PROCESS_ID* pProcID);

// Mutex
XN_C_API XnStatus XN_C_DECL xnOSCreateMutex(XN_MUTEX_HANDLE* pMutexHandle);
XN_C_API XnStatus XN_C_DECL xnOSCreateNamedMutex(XN_MUTEX_HANDLE* pMutexHandle, const XnChar* cpMutexName);
XN_C_API XnStatus XN_C_DECL xnOSCreateNamedMutexEx(XN_MUTEX_HANDLE* pMutexHandle, const XnChar* cpMutexName, XnBool bAllowOtherUsers);
XN_C_API XnStatus XN_C_DECL xnOSCloseMutex(XN_MUTEX_HANDLE* pMutexHandle);
XN_C_API XnStatus XN_C_DECL xnOSLockMutex(const XN_MUTEX_HANDLE MutexHandle, XnUInt32 nMilliseconds);
XN_C_API XnStatus XN_C_DECL xnOSUnLockMutex(const XN_MUTEX_HANDLE MutexHandle);

// Critical Sections
XN_C_API XnStatus XN_C_DECL xnOSCreateCriticalSection(XN_CRITICAL_SECTION_HANDLE* pCriticalSectionHandle);
XN_C_API XnStatus XN_C_DECL xnOSCloseCriticalSection(XN_CRITICAL_SECTION_HANDLE* pCriticalSectionHandle);
XN_C_API XnStatus XN_C_DECL xnOSEnterCriticalSection(XN_CRITICAL_SECTION_HANDLE* pCriticalSectionHandle);
XN_C_API XnStatus XN_C_DECL xnOSLeaveCriticalSection(XN_CRITICAL_SECTION_HANDLE* pCriticalSectionHandle);

// Events
XN_C_API XnStatus XN_C_DECL xnOSCreateEvent(XN_EVENT_HANDLE* pEventHandle, XnBool bManualReset);
XN_C_API XnStatus XN_C_DECL xnOSCreateNamedEvent(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bManualReset);
XN_C_API XnStatus XN_C_DECL xnOSCreateNamedEventEx(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bManualReset, XnBool bAllowOtherUsers);
XN_C_API XnStatus XN_C_DECL xnOSOpenNamedEvent(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName);
XN_C_API XnStatus XN_C_DECL xnOSOpenNamedEventEx(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bAllowOtherUsers);
XN_C_API XnStatus XN_C_DECL xnOSCloseEvent(XN_EVENT_HANDLE* pEventHandle);
XN_C_API XnStatus XN_C_DECL xnOSSetEvent(const XN_EVENT_HANDLE EventHandle);
XN_C_API XnStatus XN_C_DECL xnOSResetEvent(const XN_EVENT_HANDLE EventHandle);
XN_C_API XnStatus XN_C_DECL xnOSWaitEvent(const XN_EVENT_HANDLE EventHandle, XnUInt32 nMilliseconds);
XN_C_API XnBool XN_C_DECL xnOSIsEventSet(const XN_EVENT_HANDLE EventHandle);

// Semaphores
XN_C_API XnStatus XN_C_DECL xnOSCreateSemaphore(XN_SEMAPHORE_HANDLE* pSemaphoreHandle, XnUInt32 nInitialCount);
XN_C_API XnStatus XN_C_DECL xnOSLockSemaphore(XN_SEMAPHORE_HANDLE hSemaphore, XnUInt32 nMilliseconds);
XN_C_API XnStatus XN_C_DECL xnOSUnlockSemaphore(XN_SEMAPHORE_HANDLE hSemaphore);
XN_C_API XnStatus XN_C_DECL xnOSCloseSemaphore(XN_SEMAPHORE_HANDLE* pSemaphoreHandle);

/** 
* Waits for a condition to be met. The condition is evaluated every time an event is set.
*
* @param	EventHandle		[in]	The Event handle.
* @param	nMilliseconds	[in]	A timeout in milliseconds to wait.
* @param	pConditionFunc	[in]	A function that should be called to evaluate condition.
* @param	pConditionData	[in]	A cookie to be passed to the condition functions.
*/
XN_C_API XnStatus XN_C_DECL xnOSWaitForCondition(const XN_EVENT_HANDLE EventHandle, XnUInt32 nMilliseconds, XnConditionFunc pConditionFunc, void* pConditionData);

// Network
struct xnOSSocket;
typedef struct xnOSSocket* XN_SOCKET_HANDLE;

#define XN_SOCKET_DEFAULT_TIMEOUT 0xFFFEFFFE

XN_C_API XnStatus XN_C_DECL xnOSInitNetwork();
XN_C_API XnStatus XN_C_DECL xnOSShutdownNetwork();
XN_C_API XnStatus XN_C_DECL xnOSCreateSocket(const XnOSSocketType SocketType, const XnChar* cpIPAddress, const XnUInt16 nPort, XN_SOCKET_HANDLE* SocketPtr);
XN_C_API XnStatus XN_C_DECL xnOSCloseSocket(XN_SOCKET_HANDLE Socket);
XN_C_API XnStatus XN_C_DECL xnOSBindSocket(XN_SOCKET_HANDLE Socket);
XN_C_API XnStatus XN_C_DECL xnOSListenSocket(XN_SOCKET_HANDLE Socket);
XN_C_API XnStatus XN_C_DECL xnOSAcceptSocket(XN_SOCKET_HANDLE ListenSocket, XN_SOCKET_HANDLE* AcceptSocketPtr, XnUInt32 nMillisecsTimeout);
XN_C_API XnStatus XN_C_DECL xnOSConnectSocket(XN_SOCKET_HANDLE Socket, XnUInt32 nMillisecsTimeout);
XN_C_API XnStatus XN_C_DECL xnOSSetSocketBufferSize(XN_SOCKET_HANDLE Socket, const XnUInt32 nSocketBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSSendNetworkBuffer(XN_SOCKET_HANDLE Socket, const XnChar* cpBuffer, const XnUInt32 nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSSendToNetworkBuffer(XN_SOCKET_HANDLE Socket, const XnChar* cpBuffer, const XnUInt32 nBufferSize, XN_SOCKET_HANDLE SocketTo);
XN_C_API XnStatus XN_C_DECL xnOSReceiveNetworkBuffer(XN_SOCKET_HANDLE Socket, XnChar* cpBuffer, XnUInt32* pnBufferSize, XnUInt32 nMillisecsTimeout);
XN_C_API XnStatus XN_C_DECL xnOSReceiveFromNetworkBuffer(XN_SOCKET_HANDLE Socket, XnChar* cpBuffer, XnUInt32* pnBufferSize, XN_SOCKET_HANDLE* SocketFrom);

// Shared Memory
typedef struct XnOSSharedMemory XnOSSharedMemory, *XN_SHARED_MEMORY_HANDLE;

/**
 * Creates a shared memory block and maps it to the process memory.
 *
 * @param	strName			[in]	A machine-unique name that will be used by other processes to open this block.
 * @param	nSize			[in]	The size of the buffer.
 * @param	nAccessFlags	[in]	Creation flags. Can contain XN_OS_FILE_READ, XN_OS_FILE_WRITE or both.
 * @param	phSharedMem		[out]	A handle to the shared-memory block.
 */
XN_C_API XnStatus XN_C_DECL xnOSCreateSharedMemory(const XnChar* strName, XnUInt32 nSize, XnUInt32 nAccessFlags, XN_SHARED_MEMORY_HANDLE* phSharedMem);

XN_C_API XnStatus XN_C_DECL xnOSCreateSharedMemoryEx(const XnChar* strName, XnUInt32 nSize, XnUInt32 nAccessFlags, XnBool bAllowOtherUsers, XN_SHARED_MEMORY_HANDLE* phSharedMem);

/**
 * Opens a shared memory block, and returns the address in which it was mapped to the process' memory.
 *
 * @param	strName			[in]	A machine-unique name that will be used by other processes to open this block.
 * @param	nAccessFlags	[in]	Creation flags. Must contain XN_OS_FILE_READ, and optionally XN_OS_FILE_WRITE.
 * @param	phSharedMem		[out]	A handle to the shared-memory block.
 */
XN_C_API XnStatus XN_C_DECL xnOSOpenSharedMemory(const XnChar* strName, XnUInt32 nAccessFlags, XN_SHARED_MEMORY_HANDLE* phSharedMem);

XN_C_API XnStatus XN_C_DECL xnOSOpenSharedMemoryEx(const XnChar* strName, XnUInt32 nAccessFlags, XnBool bAllowOtherUsers, XN_SHARED_MEMORY_HANDLE* phSharedMem);

/**
 * Closes a shared memory block.
 *
 * @param	hSharedMem		[in]	A handle to the block to be closed.
 */
XN_C_API XnStatus XN_C_DECL xnOSCloseSharedMemory(XN_SHARED_MEMORY_HANDLE hSharedMem);

/**
 * Gets the address in which the shared-memory block is mapped in this process.
 *
 * @param	hSharedMem		[in]	A handle to the shared memory block.
 * @param	ppAddress		[out]	The address.
 */
XN_C_API XnStatus XN_C_DECL xnOSSharedMemoryGetAddress(XN_SHARED_MEMORY_HANDLE hSharedMem, void** ppAddress);

// Keyboard
XN_C_API XnBool XN_C_DECL xnOSWasKeyboardHit();
XN_C_API XnChar XN_C_DECL xnOSReadCharFromInput();

// Debug Utilities
XN_C_API XnStatus XN_C_DECL xnOSGetCurrentCallStack(XnUInt32 nFramesToSkip, XnChar** astrFrames, XnUInt32 nMaxNameLength, XnUInt32* pnFrames);

XN_STATUS_MESSAGE_MAP_START(XN_ERROR_GROUP_OS)
XN_STATUS_MESSAGE(XN_STATUS_ALLOC_FAILED, "Memory allocation failed!")
XN_STATUS_MESSAGE(XN_STATUS_OS_ALREADY_INIT, "Xiron OS already initialized!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NOT_INIT, "Xiron OS was not initialized!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FILE_NOT_FOUND, "File not found!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INI_FILE_NOT_FOUND, "INI file not found!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FILE_ALREDY_EXISTS, "File already exists!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FILE_OPEN_FAILED, "Failed to open the file!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FILE_CLOSE_FAILED, "Failed to close the file!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FILE_READ_FAILED, "Failed to read from the file!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FILE_WRITE_FAILED, "Failed to write to the file!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FILE_SEEK_FAILED, "File seek failed!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FILE_TELL_FAILED, "File Tell failed!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FILE_FLUSH_FAILED, "File Flush failed!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FILE_GET_SIZE_FAILED, "Get File Size failed!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INI_READ_FAILED, "Failed to read from INI file!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INI_WRITE_FAILED, "Failed to write into INI file!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INVALID_SEEK_TYPE, "Invalid seek type!")
XN_STATUS_MESSAGE(XN_STATUS_OS_THREAD_CREATION_FAILED, "Xiron OS failed to create a thread!")
XN_STATUS_MESSAGE(XN_STATUS_OS_THREAD_TERMINATION_FAILED, "Xiron OS failed to terminate a thread!")
XN_STATUS_MESSAGE(XN_STATUS_OS_THREAD_CLOSE_FAILED, "Xiron OS failed to close a thread!")
XN_STATUS_MESSAGE(XN_STATUS_OS_THREAD_TIMEOUT, "Xiron OS got a thread timeout while waiting for a thread to exit!")
XN_STATUS_MESSAGE(XN_STATUS_OS_THREAD_WAIT_FAILED, "Xiron OS failed to wait for a thread to exit!")
XN_STATUS_MESSAGE(XN_STATUS_OS_THREAD_SET_PRIORITY_FAILED, "Xiron OS failed to set priority of a thread!")
XN_STATUS_MESSAGE(XN_STATUS_OS_THREAD_UNSUPPORTED_PRIORITY, "Thread priority is unsupported by Xiron OS!")
XN_STATUS_MESSAGE(XN_STATUS_OS_MUTEX_CREATION_FAILED, "Xiron OS failed to create a mutex!")
XN_STATUS_MESSAGE(XN_STATUS_OS_MUTEX_CLOSE_FAILED, "Xiron OS failed to close a mutex!")
XN_STATUS_MESSAGE(XN_STATUS_OS_MUTEX_LOCK_FAILED, "Xiron OS failed to lock a mutex!")
XN_STATUS_MESSAGE(XN_STATUS_OS_MUTEX_TIMEOUT, "Xiron OS got a mutex timeout!")
XN_STATUS_MESSAGE(XN_STATUS_OS_MUTEX_UNLOCK_FAILED, "Xiron OS failed to unlock a mutex!")
XN_STATUS_MESSAGE(XN_STATUS_OS_EVENT_CREATION_FAILED, "Xiron OS failed to create an event!")
XN_STATUS_MESSAGE(XN_STATUS_OS_EVENT_CLOSE_FAILED, "Xiron OS failed to close an event!")
XN_STATUS_MESSAGE(XN_STATUS_OS_EVENT_SET_FAILED, "Xiron OS failed to set an event!")
XN_STATUS_MESSAGE(XN_STATUS_OS_EVENT_RESET_FAILED, "Xiron OS failed to reset an event!")
XN_STATUS_MESSAGE(XN_STATUS_OS_EVENT_TIMEOUT, "Xiron OS got an event timeout!")
XN_STATUS_MESSAGE(XN_STATUS_OS_EVENT_WAIT_FAILED, "Xiron OS failed to wait on event!")
XN_STATUS_MESSAGE(XN_STATUS_OS_EVENT_CANCELED, "This Xiron OS event was canceled!")
XN_STATUS_MESSAGE(XN_STATUS_OS_CANT_LOAD_LIB, "Xiron OS failed to load shared library!")
XN_STATUS_MESSAGE(XN_STATUS_OS_CANT_FREE_LIB, "Xiron OS failed to free shared library!")
XN_STATUS_MESSAGE(XN_STATUS_OS_PROC_NOT_FOUND, "Xiron OS failed to get procedure address from shared library!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_INIT_FAILED, "Xiron OS failed to initialize the network subsystem!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_SHUTDOWN_FAILED, "Xiron OS failed to shutdown the network subsystem!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_SOCKET_CREATION_FAILED, "Xiron OS failed to create a network socket!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_INVALID_SOCKET_TYPE, "Invalid Xiron OS socket type!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_SOCKET_BUFFER_FAILED, "Failed to change the Xiron OS socket buffer size!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_SEND_FAILED, "Xiron OS failed to send a network buffer!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_RECEIVE_FAILED, "Xiron OS failed to receive a network buffer!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_SOCKET_BIND_FAILED, "Xiron OS failed to bind a network socket!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_SOCKET_LISTEN_FAILED, "Xiron OS failed to listen on a network socket!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_SOCKET_ACCEPT_FAILED, "Xiron OS failed to accept a network socket!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_SOCKET_CONNECT_FAILED, "Xiron OS failed to connect to a network socket!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_BAD_HOST_NAME, "Failed to resolve the host name!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_TIMEOUT, "Got a timeout while waiting for a network command to complete!")
XN_STATUS_MESSAGE(XN_STATUS_OS_TIMER_CREATION_FAILED, "Xiron OS failed to create a timer!")
XN_STATUS_MESSAGE(XN_STATUS_OS_TIMER_QUERY_FAILED, "Xiron OS failed to query a timer!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INVALID_TIMER, "This Xiron OS timer is invalid!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INVALID_FILE, "This Xiron OS file is invalid!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INVALID_SOCKET, "This Xiron OS socket is invalid!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INVALID_MUTEX, "This Xiron OS mutex is invalid!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INVALID_THREAD, "This Xiron OS thread is invalid!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INVALID_EVENT, "This Xiron OS event is invalid!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INVALID_LIBRARY, "This Xiron OS shared library is invalid!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INVALID_CRITICAL_SECTION, "This Xiron OS critical section is invalid!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INVALID_FORMAT_STRING, "Xiron OS got an invalid format string!")
XN_STATUS_MESSAGE(XN_STATUS_OS_UNSUPPORTED_FUNCTION, "This Xiron OS function is not supported!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FAILED_TO_CREATE_DIR, "Failed to create a directory!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FAILED_TO_DELETE_FILE, "Failed to delete a file!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FAILED_TO_CREATE_SHARED_MEMORY, "Failed to create shared memory!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FAILED_TO_OPEN_SHARED_MEMORY, "Failed to open shared memory!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FAILED_TO_CLOSE_SHARED_MEMORY, "Failed to close shared memory!")
XN_STATUS_MESSAGE(XN_STATUS_USB_ALREADY_INIT, "The Xiron USB subsystem was already initialize!")
XN_STATUS_MESSAGE(XN_STATUS_USB_NOT_INIT, "The Xiron USB subsystem was not initialized!")
XN_STATUS_MESSAGE(XN_STATUS_USB_INIT_FAILED, "The Xiron USB subsystem failed to initialize!")
XN_STATUS_MESSAGE(XN_STATUS_USB_SHUTDOWN_FAILED, "The Xiron USB subsystem failed to shutdown!")
XN_STATUS_MESSAGE(XN_STATUS_USB_ENUMERATE_FAILED, "The Xiron USB subsystem failed to enumerate devices!")
XN_STATUS_MESSAGE(XN_STATUS_USB_LOAD_FAILED, "The Xiron USB subsystem failed to load!")
XN_STATUS_MESSAGE(XN_STATUS_USB_FREE_FAILED, "The Xiron USB subsystem failed to free!")
XN_STATUS_MESSAGE(XN_STATUS_USB_REGISTER_FAILED, "The Xiron USB subsystem failed to register the device!")
XN_STATUS_MESSAGE(XN_STATUS_USB_UNREGISTER_FAILED, "The Xiron USB subsystem failed to unregister the device!")
XN_STATUS_MESSAGE(XN_STATUS_USB_DEVICE_NOT_VALID, "Invalid Xiron USB device handle!")
XN_STATUS_MESSAGE(XN_STATUS_USB_ENDPOINT_NOT_VALID, "Invalid Xiron USB endpoint handle!")
XN_STATUS_MESSAGE(XN_STATUS_USB_DRIVER_NOT_FOUND, "USB driver not found!")
XN_STATUS_MESSAGE(XN_STATUS_USB_DEVICE_NOT_FOUND, "USB device not found!")
XN_STATUS_MESSAGE(XN_STATUS_USB_DEVICE_OPEN_FAILED, "Failed to open the USB device!")
XN_STATUS_MESSAGE(XN_STATUS_USB_DEVICE_CLOSE_FAILED, "Failed to close the USB device!")
XN_STATUS_MESSAGE(XN_STATUS_USB_DEVICE_GETINFO_FAILED, "Failed to get information about the USB device!")
XN_STATUS_MESSAGE(XN_STATUS_USB_CONFIG_QUERY_FAILED, "USB config query failed!")
XN_STATUS_MESSAGE(XN_STATUS_USB_INTERFACE_QUERY_FAILED, "USB interface query failed!")
XN_STATUS_MESSAGE(XN_STATUS_USB_ENDPOINT_QUERY_FAILED, "USB endpoint query failed!")
XN_STATUS_MESSAGE(XN_STATUS_USB_SET_ENDPOINT_POLICY_FAILED, "Failed to set USB endpoint policy!")
XN_STATUS_MESSAGE(XN_STATUS_USB_UNKNOWN_ENDPOINT_TYPE, "Unknown USB endpoint type!")
XN_STATUS_MESSAGE(XN_STATUS_USB_UNKNOWN_ENDPOINT_DIRECTION, "Unknown USB endpoint direction!")
XN_STATUS_MESSAGE(XN_STATUS_USB_GET_SPEED_FAILED, "Failed to get the device speed!")
XN_STATUS_MESSAGE(XN_STATUS_USB_GET_DRIVER_VERSION, "Failed to get the USB driver version!")
XN_STATUS_MESSAGE(XN_STATUS_USB_UNKNOWN_DEVICE_SPEED, "Unknown USB device speed!")
XN_STATUS_MESSAGE(XN_STATUS_USB_CONTROL_SEND_FAILED, "Failed to send a USB control request!")
XN_STATUS_MESSAGE(XN_STATUS_USB_CONTROL_RECV_FAILED, "Failed to receive a USB control request!")
XN_STATUS_MESSAGE(XN_STATUS_USB_ENDPOINT_READ_FAILED, "Failed to read from a USB endpoint!")
XN_STATUS_MESSAGE(XN_STATUS_USB_ENDPOINT_WRITE_FAILED, "Failed to write into a USB endpoint!")
XN_STATUS_MESSAGE(XN_STATUS_USB_TRANSFER_TIMEOUT, "USB transfer timeout!")
XN_STATUS_MESSAGE(XN_STATUS_USB_TRANSFER_STALL, "USB transfer stall!")
XN_STATUS_MESSAGE(XN_STATUS_USB_TRANSFER_MICRO_FRAME_ERROR, "USB transfer micro frame error!")
XN_STATUS_MESSAGE(XN_STATUS_USB_TRANSFER_UNKNOWN_ERROR, "Unknown USB transfer error!")
XN_STATUS_MESSAGE(XN_STATUS_USB_ENDPOINT_NOT_FOUND, "USB endpoint not found on device!")
XN_STATUS_MESSAGE(XN_STATUS_USB_WRONG_ENDPOINT_TYPE, "Wrong USB endpoint type requested!")
XN_STATUS_MESSAGE(XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION, "Wrong USB endpoint direction requested!")
XN_STATUS_MESSAGE(XN_STATUS_USB_WRONG_CONTROL_TYPE, "Wrong USB control type requested!")
XN_STATUS_MESSAGE(XN_STATUS_USB_UNSUPPORTED_ENDPOINT_TYPE, "Unsupported USB endpoint type!")
XN_STATUS_MESSAGE(XN_STATUS_USB_GOT_UNEXPECTED_BYTES, "Got unexpected bytes in USB transfer!")
XN_STATUS_MESSAGE(XN_STATUS_USB_TOO_MUCH_DATA, "Got too much data in USB transfer!")
XN_STATUS_MESSAGE(XN_STATUS_USB_NOT_ENOUGH_DATA, "Didn't get enough data in USB transfer!")
XN_STATUS_MESSAGE(XN_STATUS_USB_BUFFER_TOO_SMALL, "USB Buffer is too small!")
XN_STATUS_MESSAGE(XN_STATUS_USB_OVERLAPIO_FAILED, "USB Overlapped I/O operation failed!")
XN_STATUS_MESSAGE(XN_STATUS_USB_ABORT_FAILED, "Failed to abort USB endpoint!")
XN_STATUS_MESSAGE(XN_STATUS_USB_FLUSH_FAILED, "Failed to flush USB endpoint!")
XN_STATUS_MESSAGE(XN_STATUS_USB_RESET_FAILED, "Failed to reset USB endpoint!")
XN_STATUS_MESSAGE(XN_STATUS_USB_SET_INTERFACE_FAILED, "Failed to set USB interface!")
XN_STATUS_MESSAGE(XN_STATUS_USB_GET_INTERFACE_FAILED, "Failed to get USB interface!")
XN_STATUS_MESSAGE(XN_STATUS_USB_READTHREAD_NOT_INIT, "Read thread is not initialized for this USB end point!")
XN_STATUS_MESSAGE(XN_STATUS_USB_READTHREAD_ALREADY_INIT, "Read thread is already initialized for this USB end point!")
XN_STATUS_MESSAGE(XN_STATUS_USB_READTHREAD_SHUTDOWN_FAILED, "Read thread failed to shutdown properly!")
XN_STATUS_MESSAGE(XN_STATUS_USB_IS_BUSY, "USB is busy!")
XN_STATUS_MESSAGE(XN_STATUS_USB_NOT_BUSY, "USB is not busy!")
XN_STATUS_MESSAGE(XN_STATUS_USB_SET_CONFIG_FAILED, "Failed to set USB config!")
XN_STATUS_MESSAGE(XN_STATUS_USB_GET_CONFIG_FAILED, "Failed to get USB config!")
XN_STATUS_MESSAGE(XN_STATUS_USB_OPEN_ENDPOINT_FAILED, "Failed to open an USB endpoint!")
XN_STATUS_MESSAGE(XN_STATUS_USB_CLOSE_ENDPOINT_FAILED, "Failed to close an USB endpoint!")
XN_STATUS_MESSAGE(XN_STATUS_USB_ALREADY_OPEN, "A device is already opened!")
XN_STATUS_MESSAGE(XN_STATUS_USB_TRANSFER_PENDING, "USB transfer is still pending!")
XN_STATUS_MESSAGE(XN_STATUS_USB_INTERFACE_NOT_SUPPORTED, "USB interface is not supported!")
XN_STATUS_MESSAGE(XN_STATUS_USB_FAILED_TO_REGISTER_CALLBACK, "Failed to register the USB device callback!")
XN_STATUS_MESSAGE(XN_STATUS_OS_NETWORK_CONNECTION_CLOSED, "The network connection has been closed!")
XN_STATUS_MESSAGE(XN_STATUS_OS_EVENT_OPEN_FAILED, "Xiron OS failed to open an event!")
XN_STATUS_MESSAGE(XN_STATUS_OS_PROCESS_CREATION_FAILED, "Xiron OS failed to create a process!")
XN_STATUS_MESSAGE(XN_STATUS_OS_SEMAPHORE_CREATION_FAILED, "Xiron OS Failed to create a semaphore!")
XN_STATUS_MESSAGE(XN_STATUS_OS_SEMAPHORE_CLOSE_FAILED, "Xiron OS failed to close a semaphore!")
XN_STATUS_MESSAGE(XN_STATUS_OS_SEMAPHORE_LOCK_FAILED, "Xiron OS failed to lock a semaphore!")
XN_STATUS_MESSAGE(XN_STATUS_OS_SEMAPHORE_UNLOCK_FAILED, "Xiron OS failed to unlock a semaphore!")
XN_STATUS_MESSAGE(XN_STATUS_OS_SEMAPHORE_TIMEOUT, "Xiron OS got a semaphore timeout!")
XN_STATUS_MESSAGE(XN_STATUS_OS_INVALID_SEMAPHORE, "This Xiron OS semaphore is invalid!")
XN_STATUS_MESSAGE(XN_STATUS_OS_ENV_VAR_NOT_FOUND, "The environment variable could not be found!")
XN_STATUS_MESSAGE(XN_STATUS_USB_NO_REQUEST_PENDING, "There is no request pending!")
XN_STATUS_MESSAGE(XN_STATUS_OS_FAILED_TO_DELETE_DIR, "Failed to delete a directory!")
XN_STATUS_MESSAGE_MAP_END(XN_ERROR_GROUP_OS)

#endif //__XN_OS_H__

