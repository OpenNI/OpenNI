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
#include <sys/mman.h>
#include <errno.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
#ifndef XN_PLATFORM_LINUX_NO_SHM

struct XnOSSharedMemory
{
	bool bCreate;
	XnChar strFileName[XN_FILE_MAX_PATH];
	int nSize;
	void* pAddress;
};

XnStatus AccessFlagsToMMapProt(XnUInt32 nAccessFlags, int* pProt)
{
	int result = PROT_NONE;

	if ((nAccessFlags & XN_OS_FILE_READ) != 0)
	{
		result |= PROT_READ;
	}

	if ((nAccessFlags & XN_OS_FILE_WRITE) != 0)
	{
		result |= PROT_WRITE;
	}

	*pProt = result;

	return XN_STATUS_OK;
}

XnStatus AccessFlagsToOpenFlags(XnUInt32 nAccessFlags, int* pFlags)
{
	int result = 0;

	if ((nAccessFlags & XN_OS_FILE_READ) == 0)
	{
		return (XN_STATUS_INVALID_OPERATION);
	}

	if ((nAccessFlags & XN_OS_FILE_WRITE) != 0)
	{
		result = O_RDWR;
	}
	else
	{
		result = O_RDONLY;
	}

	*pFlags = result;

	return (XN_STATUS_OK);
}

static void NameToFileName(const XnChar* strName, XnChar* strFileName)
{
	int i = 0;
	for (; strName[i] != '\0'; ++i)
	{
		// replace slashes with dots
		strFileName[i] = strName[i] == '/' ? '.' : strName[i];
	}
	strFileName[i] = '\0';
}

static XnStatus OpenSharedMemoryImpl(const XnChar* strName, XnUInt32 nAccessFlags, XN_SHARED_MEMORY_HANDLE* phSharedMem, XnUInt32 nSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(strName);
	XN_VALIDATE_OUTPUT_PTR(phSharedMem);
	
	// if (nSize) is a number - create, otherwise - open
	XnBool bCreate = (nSize != 0);

	// convert to local OS types
	int prot = 0;
	int nCreateFlags = 0;
	int nMode = 0;

	nRetVal = AccessFlagsToMMapProt(nAccessFlags, &prot);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = AccessFlagsToOpenFlags(nAccessFlags, &nCreateFlags);
	XN_IS_STATUS_OK(nRetVal);

	// allocate handle
	XnOSSharedMemory* pHandle;
	XN_VALIDATE_CALLOC(pHandle, XnOSSharedMemory, 1);

	pHandle->bCreate = bCreate;

	NameToFileName(strName, pHandle->strFileName);

	if (bCreate)
	{
		nCreateFlags |= O_CREAT;
		nMode |= S_IRWXU | S_IRWXG | S_IRWXO;
	}

	// open file
	int fd = shm_open(pHandle->strFileName, nCreateFlags, nMode);
	if (fd == -1)
	{
		xnOSFree(pHandle);
		XN_LOG_WARNING_RETURN(XN_STATUS_OS_FAILED_TO_CREATE_SHARED_MEMORY, XN_MASK_OS, "Could not create file '%s' for shared memory (%d).", pHandle->strFileName, errno);
	}

	if (bCreate)
	{
		// set it to the right size
		if (-1 == ftruncate(fd, nSize))
		{
			close(fd);
			shm_unlink(pHandle->strFileName);
			xnOSFree(pHandle);
			XN_LOG_WARNING_RETURN(XN_STATUS_OS_FAILED_TO_CREATE_SHARED_MEMORY, XN_MASK_OS, "Could not seek to position (%d).", pHandle->strFileName, errno);
		}

		pHandle->nSize = nSize;
	}
	else
	{
		// read shared object size
		pHandle->nSize = lseek(fd, 0, SEEK_END);
	}

	// and map it
	pHandle->pAddress = mmap(NULL, pHandle->nSize, prot, MAP_SHARED, fd, 0);
	if (pHandle->pAddress == MAP_FAILED)
	{
		close(fd);
		shm_unlink(pHandle->strFileName);
		xnOSFree(pHandle);
		XN_LOG_WARNING_RETURN(XN_STATUS_OS_FAILED_TO_CREATE_SHARED_MEMORY, XN_MASK_OS, "Could not create file mapping object (%d).", errno);
	}
	
	close(fd);

	*phSharedMem = pHandle;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus XN_C_DECL xnOSCreateSharedMemory(const XnChar* strName, XnUInt32 nSize, XnUInt32 nAccessFlags, XN_SHARED_MEMORY_HANDLE* phSharedMem)
{
	return xnOSCreateSharedMemoryEx(strName, nSize, nAccessFlags, FALSE, phSharedMem);
}

XN_C_API XnStatus XN_C_DECL xnOSCreateSharedMemoryEx(const XnChar* strName, XnUInt32 nSize, XnUInt32 nAccessFlags, XnBool bAllowOtherUsers, XN_SHARED_MEMORY_HANDLE* phSharedMem)
{
	return OpenSharedMemoryImpl(strName, nAccessFlags, phSharedMem, nSize);
}

XN_C_API XnStatus xnOSOpenSharedMemory(const XnChar* strName, XnUInt32 nAccessFlags, XN_SHARED_MEMORY_HANDLE* phSharedMem)
{
	return xnOSOpenSharedMemoryEx(strName, nAccessFlags, FALSE, phSharedMem);
}

XN_C_API XnStatus XN_C_DECL xnOSOpenSharedMemoryEx(const XnChar* strName, XnUInt32 nAccessFlags, XnBool bAllowOtherUsers, XN_SHARED_MEMORY_HANDLE* phSharedMem)
{
	return OpenSharedMemoryImpl(strName, nAccessFlags, phSharedMem, 0);
}

XN_C_API XnStatus xnOSCloseSharedMemory(XN_SHARED_MEMORY_HANDLE hSharedMem)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(hSharedMem);

	// unmap
	munmap(hSharedMem->pAddress, hSharedMem->nSize);

	if (hSharedMem->bCreate)
	{
		// remove the file
		shm_unlink(hSharedMem->strFileName);
	}
	
	xnOSFree(hSharedMem);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSharedMemoryGetAddress(XN_SHARED_MEMORY_HANDLE hSharedMem, void** ppAddress)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(hSharedMem);
	XN_VALIDATE_OUTPUT_PTR(ppAddress);

	*ppAddress = hSharedMem->pAddress;

	return (XN_STATUS_OK);
}

#else

struct XnOSSharedMemory
{
	void* pAddress;
};

XN_C_API XnStatus xnOSCreateSharedMemory(const XnChar* strName, XnUInt32 nSize, XnUInt32 nAccessFlags, XN_SHARED_MEMORY_HANDLE* phSharedMem)
{
	void* pAddress = xnOSMallocAligned(nSize, XN_DEFAULT_MEM_ALIGN);
	XN_VALIDATE_ALLOC_PTR(pAddress);
	
	*phSharedMem = (XN_SHARED_MEMORY_HANDLE)pAddress;
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateSharedMemoryEx(const XnChar* strName, XnUInt32 nSize, XnUInt32 nAccessFlags, XnBool bAllowOtherUsers, XN_SHARED_MEMORY_HANDLE* phSharedMem)
{
	void* pAddress = xnOSMallocAligned(nSize, XN_DEFAULT_MEM_ALIGN);
	XN_VALIDATE_ALLOC_PTR(pAddress);
	
	*phSharedMem = (XN_SHARED_MEMORY_HANDLE)pAddress;
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSOpenSharedMemory(const XnChar* strName, XnUInt32 nAccessFlags, XN_SHARED_MEMORY_HANDLE* phSharedMem)
{
	return XN_STATUS_OS_FAILED_TO_CREATE_SHARED_MEMORY;
}

XN_C_API XnStatus xnOSOpenSharedMemoryEx(const XnChar* strName, XnUInt32 nAccessFlags, XnBool bAllowOtherUsers, XN_SHARED_MEMORY_HANDLE* phSharedMem)
{
	return XN_STATUS_OS_FAILED_TO_CREATE_SHARED_MEMORY;
}

XN_C_API XnStatus xnOSCloseSharedMemory(XN_SHARED_MEMORY_HANDLE hSharedMem)
{
	xnOSFreeAligned(hSharedMem);
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSharedMemoryGetAddress(XN_SHARED_MEMORY_HANDLE hSharedMem, void** ppAddress)
{
	*ppAddress = hSharedMem;
	
	return (XN_STATUS_OK);
}

#endif
