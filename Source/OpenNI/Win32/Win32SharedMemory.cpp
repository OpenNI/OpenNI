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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include <XnLog.h>
#include "XnOSWin32Internal.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
struct XnOSSharedMemory
{
	HANDLE hMapFile;
	void* pAddress;
};

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
static XnStatus AccessFlagsToWin32MapFlags(XnUInt32 nAccessFlags, DWORD* pFlags)
{
	DWORD result = 0;

	if ((nAccessFlags & XN_OS_FILE_READ) == 0)
	{
		return (XN_STATUS_INVALID_OPERATION);
	}

	if ((nAccessFlags & XN_OS_FILE_WRITE) != 0)
	{
		result = PAGE_READWRITE;
	}
	else
	{
		result = PAGE_READONLY;
	}

	*pFlags = result;
	return (XN_STATUS_OK);
}

static XnStatus AccessFlagsToWin32ViewFlags(XnUInt32 nAccessFlags, DWORD* pFlags)
{
	DWORD result = 0;

	if ((nAccessFlags & XN_OS_FILE_READ) == 0)
	{
		return (XN_STATUS_INVALID_OPERATION);
	}

	if ((nAccessFlags & XN_OS_FILE_WRITE) != 0)
	{
		result = FILE_MAP_WRITE;
	}
	else
	{
		result = FILE_MAP_READ;
	}

	*pFlags = result;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XN_C_DECL xnOSCreateSharedMemory(const XnChar* strName, XnUInt32 nSize, XnUInt32 nAccessFlags, XN_SHARED_MEMORY_HANDLE* phSharedMem)
{
	return xnOSCreateSharedMemoryEx(strName, nSize, nAccessFlags, FALSE, phSharedMem);
}

XN_C_API XnStatus XN_C_DECL xnOSCreateSharedMemoryEx(const XnChar* strName, XnUInt32 nSize, XnUInt32 nAccessFlags, XnBool bAllowOtherUsers, XN_SHARED_MEMORY_HANDLE* phSharedMem)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(strName);
	XN_VALIDATE_OUTPUT_PTR(phSharedMem);

	DWORD mapflags;
	nRetVal = AccessFlagsToWin32MapFlags(nAccessFlags, &mapflags);
	XN_IS_STATUS_OK(nRetVal);

	DWORD viewflags;
	nRetVal = AccessFlagsToWin32ViewFlags(nAccessFlags, &viewflags);
	XN_IS_STATUS_OK(nRetVal);

	XnChar strWinName[XN_FILE_MAX_PATH];
	nRetVal = XnWin32CreateKernelObjectName(strWinName, MAX_PATH, strName, bAllowOtherUsers);
	if (nRetVal != XN_STATUS_OK)
	{
		return XN_STATUS_OS_EVENT_CREATION_FAILED;
	}

	SECURITY_ATTRIBUTES* pSecurityAttributes = NULL;
	nRetVal = XnWin32GetSecurityAttributes(bAllowOtherUsers, &pSecurityAttributes);
	if (nRetVal != XN_STATUS_OK)
	{
		return XN_STATUS_OS_EVENT_CREATION_FAILED;
	}

	// allocate handle
	XnOSSharedMemory* pHandle;
	XN_VALIDATE_CALLOC(pHandle, XnOSSharedMemory, 1);

	// create file mapping
	pHandle->hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		pSecurityAttributes,     // security 
		mapflags,		         // read/write access
		0,                       // max. object size 
		nSize,                   // buffer size  
		strWinName);             // name of mapping object

	if (pHandle->hMapFile == NULL) 
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_FAILED_TO_CREATE_SHARED_MEMORY, XN_MASK_OS, "Could not create file mapping object (%d).", GetLastError());
	}

	// map it to the process
	pHandle->pAddress = MapViewOfFile(
		pHandle->hMapFile,  // handle to map object
		viewflags,			// read/write permission
		0,                   
		0,                   
		nSize);           

	if (pHandle->pAddress == NULL)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_FAILED_TO_CREATE_SHARED_MEMORY, XN_MASK_OS, "Could not map view of file (%d).", GetLastError());
	}

	*phSharedMem = pHandle;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSOpenSharedMemory(const XnChar* strName, XnUInt32 nAccessFlags, XN_SHARED_MEMORY_HANDLE* phSharedMem)
{
	return xnOSOpenSharedMemoryEx(strName, nAccessFlags, FALSE, phSharedMem);
}

XN_C_API XnStatus XN_C_DECL xnOSOpenSharedMemoryEx(const XnChar* strName, XnUInt32 nAccessFlags, XnBool bAllowOtherUsers, XN_SHARED_MEMORY_HANDLE* phSharedMem)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(strName);
	XN_VALIDATE_OUTPUT_PTR(phSharedMem);

	DWORD flags;
	nRetVal = AccessFlagsToWin32ViewFlags(nAccessFlags, &flags);
	XN_IS_STATUS_OK(nRetVal);

	XnChar strWinName[XN_FILE_MAX_PATH];
	nRetVal = XnWin32CreateKernelObjectName(strWinName, MAX_PATH, strName, bAllowOtherUsers);
	if (nRetVal != XN_STATUS_OK)
	{
		return XN_STATUS_OS_FAILED_TO_OPEN_SHARED_MEMORY;
	}

	// allocate handle
	XnOSSharedMemory* pHandle;
	XN_VALIDATE_CALLOC(pHandle, XnOSSharedMemory, 1);

	// create file mapping
	pHandle->hMapFile = OpenFileMapping(
		flags,					 // read/write access
		FALSE,                   // do not inherit the name
		strWinName);             // name of mapping object

	if (pHandle->hMapFile == NULL) 
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_FAILED_TO_OPEN_SHARED_MEMORY, XN_MASK_OS, "Could not open file mapping object (%d).", GetLastError());
	}

	// map it to the process
	pHandle->pAddress = MapViewOfFile(
		pHandle->hMapFile,  // handle to map object
		flags,				// read/write permission
		0,                   
		0,                   
		0);           

	if (pHandle->pAddress == NULL)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_FAILED_TO_OPEN_SHARED_MEMORY, XN_MASK_OS, "Could not map view of file (%d).", GetLastError());
	}

	*phSharedMem = pHandle;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseSharedMemory(XN_SHARED_MEMORY_HANDLE hSharedMem)
{
	XN_VALIDATE_INPUT_PTR(hSharedMem);

	if (!UnmapViewOfFile(hSharedMem->pAddress))
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_FAILED_TO_CLOSE_SHARED_MEMORY, XN_MASK_OS, "Could not unmap view of file (%d).", GetLastError());
	}

	if (!CloseHandle(hSharedMem->hMapFile))
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_FAILED_TO_CLOSE_SHARED_MEMORY, XN_MASK_OS, "Could not close shared memory handle (%d).", GetLastError());
	}

	xnOSFree(hSharedMem);
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSharedMemoryGetAddress(XN_SHARED_MEMORY_HANDLE hSharedMem, void** ppAddress)
{
	XN_VALIDATE_INPUT_PTR(hSharedMem);
	XN_VALIDATE_OUTPUT_PTR(ppAddress);

	*ppAddress = hSharedMem->pAddress;
	
	return (XN_STATUS_OK);
}

