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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
// NOTE: you must define XN_OS_IMPL before including xnOS.h, otherwise, when mem profiling this file will not compile.
#define XN_OS_IMPL
#include <XnOS.h>
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <sys/malloc.h>
#else
	#include <malloc.h>
#endif
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API void* xnOSMalloc(const XnSizeT nAllocSize)
{
	// Return a pointer to the requested allocation size
	return (malloc(nAllocSize));
}

XN_C_API void* xnOSMallocAligned(const XnSizeT nAllocSize, const XnSizeT nAlignment)
{
	void* pResult = NULL;
	
#ifndef XN_PLATFORM_LINUX_NO_POSIX_MEMALIGN
	if (0 != posix_memalign(&pResult, nAlignment, nAllocSize))
		return NULL;
#else
	pResult = memalign(nAlignment, nAllocSize);
#endif
		
	return pResult;
}

XN_C_API void* xnOSCalloc(const XnSizeT nAllocNum, const XnSizeT nAllocSize)
{
	// Return a pointer to the requested allocation size and write zeros on the entire memory block (C Alloc)
	return (calloc(nAllocNum, nAllocSize));
}

XN_C_API void* xnOSCallocAligned(const XnSizeT nAllocNum, const XnSizeT nAllocSize, const XnSizeT nAlignment)
{
	// Local function variables
	void* pMemBlock = NULL;
	XnUInt32 nBlockSize = nAllocNum * nAllocSize;

	// Allocate the aligned memory block
	pMemBlock = xnOSMallocAligned(nBlockSize, nAlignment);

	// If it succeeded (the returned block wasn't NULL, let's zero it)
	if (pMemBlock != NULL)
	{
		memset(pMemBlock, 0, nBlockSize);
	}

	return (pMemBlock);
}

XN_C_API void* xnOSRealloc(void* pMemory, const XnSizeT nAllocSize)
{
	// Return a pointer to the requested allocation size
	return (realloc(pMemory, nAllocSize));
}

XN_C_API void* xnOSReallocAligned(void* pMemory, const XnSizeT nAllocSize, const XnSizeT nAlignment)
{
	xnLogError("xnOS", "Does not know how to realloc aligned memory!");
	return NULL;
}

XN_C_API void* xnOSRecalloc(void* pMemory, const XnSizeT nAllocNum, const XnSizeT nAllocSize)
{
	xnLogError("xnOS", "Does not know how to realloc aligned memory!");
	// Return a pointer to the requested allocation size and write zeros on the entire memory block (C Alloc)
	return NULL;
}

XN_C_API void xnOSFree(const void* pMemBlock)
{
	// Note: No need to check if pMemBlock is NULL because free is guaranteed to handle this case correctly.

	// Free the requested memory block
	free ((void*)pMemBlock);
}

XN_C_API void  xnOSFreeAligned(const void* pMemBlock)
{
	// Note: No need to check if pMemBlock is NULL because free is guaranteed to handle this case correctly.

	// Free the requested memory block
	free ((void*)pMemBlock);
}

XN_C_API void xnOSMemCopy(void* pDest, const void* pSource, XnSizeT nCount)
{
	memcpy(pDest, pSource, nCount);
}

XN_C_API XnInt32 xnOSMemCmp(const void *pBuf1, const void *pBuf2, XnSizeT nCount)
{
	return memcmp(pBuf1, pBuf2, nCount);
}

XN_C_API void xnOSMemSet(void* pDest, XnUInt8 nValue, XnSizeT nCount)
{
	memset(pDest, nValue, nCount);
}

XN_C_API void xnOSMemMove(void* pDest, const void* pSource, XnSizeT nCount)
{
	memmove(pDest, pSource, nCount);
}

XN_C_API XnUInt64  xnOSEndianSwapUINT64(XnUInt64 nValue)
{
	return ((nValue >> 56) ) | ((nValue >> 40) & 0x000000000000ff00ULL) |
			((nValue >> 24) & 0x0000000000ff0000ULL) | ((nValue >> 8 ) & 0x00000000ff000000ULL) |
			((nValue << 8 ) & 0x000000ff00000000ULL) | ((nValue << 24) & 0x0000ff0000000000ULL) |
			((nValue << 40) & 0x00ff000000000000ULL) | ((nValue << 56) );
}

XN_C_API XnUInt32  xnOSEndianSwapUINT32(XnUInt32 nValue)
{
	return  (nValue>>24) | 
			((nValue<<8) & 0x00FF0000) |
			((nValue>>8) & 0x0000FF00) |
			(nValue<<24);
}

XN_C_API XnUInt16 xnOSEndianSwapUINT16(XnUInt16 nValue)
{
	return ((nValue>>8) | (nValue<<8));
}
XN_C_API XnFloat xnOSEndianSwapFLOAT(XnFloat fValue)
{
	XnUInt32* pnValue = (XnUInt32*)&fValue;
	XnUInt32 nValue = xnOSEndianSwapUINT32(*pnValue);
	XnFloat* pfValue = (XnFloat*)&nValue;
	return *pfValue;
}
