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
#ifndef _XN_OSLINUX_X86_H_
#define _XN_OSLINUX_X86_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <sys/types.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

//---------------------------------------------------------------------------
// Files
//---------------------------------------------------------------------------
/** A generic handle type. */ 
typedef	void* XN_HANDLE;

/** A file handle type. */ 
typedef XnInt XN_FILE_HANDLE;

/** The value of an invalid file handle. */
#define XN_INVALID_FILE_HANDLE -1

/** A string that specifies the current directory. */ 
#define XN_FILE_LOCAL_DIR "./"

/** The file directory separator. */ 
#define XN_FILE_DIR_SEP "/"

/** The file extension separator. */ 
#define XN_FILE_EXT_SEP "."

/** The file "all" wildcard. */ 
#define XN_FILE_ALL_WILDCARD "*"

/** The newline separation string. */
#define XN_NEW_LINE_SEP "\n"

//---------------------------------------------------------------------------
// INI Files
//---------------------------------------------------------------------------
/** A string that specifies the extension of INI files. */ 
#define XN_INI_FILE_EXT "ini"

/** The maximum allowed INI string length (in bytes). */
// Note: This must always be big enough to contain a 32-bit number!
#define XN_INI_MAX_LEN 256

//---------------------------------------------------------------------------
// Shared Libraries
//---------------------------------------------------------------------------
/** A shared library handle type. */ 
typedef	void* XN_LIB_HANDLE;

/** A string that specifies the prefix of shared library files. */ 
#define XN_SHARED_LIBRARY_PREFIX "lib"

/** A string that specifies the postfix of shared library files. */ 
#define XN_SHARED_LIBRARY_POSTFIX ".so"

//---------------------------------------------------------------------------
// Threads
//---------------------------------------------------------------------------
/** A Xiron thread type. */ 
typedef	pthread_t* XN_THREAD_HANDLE;

/** A Xiron thread ID. */ 
typedef	pthread_t XN_THREAD_ID;

/** A Xiron process ID. */
typedef pid_t XN_PROCESS_ID;

/** The thread entry point function prototype. */
typedef void* (*XN_THREAD_PROC_PROTO)(void* arg);

/** The thread entry point function definition. */
#define	XN_THREAD_PROC void*

/** The thread return function. */
#define	XN_THREAD_PROC_RETURN(ret) return((void*)ret)

/** The thread passable data pointer type. */ 
typedef	void* XN_THREAD_PARAM;

//---------------------------------------------------------------------------
// Time Outs
//---------------------------------------------------------------------------
/** The mutex lock infinite timeout. */
#define	XN_WAIT_INFINITE 0xFFFFFFFF

//---------------------------------------------------------------------------
// Mutex
//---------------------------------------------------------------------------
/** A Xiron mutex type. */
struct XnMutex;
typedef	struct XnMutex* XN_MUTEX_HANDLE;

//---------------------------------------------------------------------------
// Critical Sections
//---------------------------------------------------------------------------
/** A Xiron critical sections type. */ 
typedef	XN_MUTEX_HANDLE XN_CRITICAL_SECTION_HANDLE;

//---------------------------------------------------------------------------
// Events
//---------------------------------------------------------------------------
/** A Xiron event type. */ 
struct _XnEvent;
typedef struct _XnEvent _XnEvent, *XN_EVENT_HANDLE ;

//---------------------------------------------------------------------------
// Semaphores
//---------------------------------------------------------------------------
/** A Xiron event type. */
struct _XnSemaphore;
typedef struct _XnSemaphore *XN_SEMAPHORE_HANDLE;

//---------------------------------------------------------------------------
// Timer
//---------------------------------------------------------------------------
/** The Xiron OS timer structure. */ 
typedef struct XnOSTimer 
{
	struct timespec tStartTime;
	XnBool bHighRes;
} XnOSTimer;

//---------------------------------------------------------------------------
// Network
//---------------------------------------------------------------------------
/** The network host name and port separator. */ 
#define XN_NETWORK_HOST_PORT_SEP ":"

//---------------------------------------------------------------------------
// Swaps
//---------------------------------------------------------------------------
#define XN_PREPARE_VAR16_IN_BUFFER(var) (var)
#define XN_PREPARE_VAR32_IN_BUFFER(var) (var)
#define XN_PREPARE_VAR64_IN_BUFFER(var) (var)
#define XN_PREPARE_VAR_FLOAT_IN_BUFFER(var) (var)

#endif //_XN_OSLINUX_X86_H_
