/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
// NOTE: you must define XN_OS_IMPL before including XnOS.h, otherwise, when mem profiling this file will not compile.
#define XN_OS_IMPL
#undef XN_CROSS_PLATFORM
#include <XnOS.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_CORE_API void* XnOSMalloc(const XN_SIZE_T nAllocSize)
{
	// Return a pointer to the requested allocation size
	return (malloc(nAllocSize));
}

XN_CORE_API void* XnOSMallocAligned(const XN_SIZE_T nAllocSize, const XN_SIZE_T nAlignment)
{
	// TODO: fix alignment
	return XnOSMalloc(nAllocSize);
}

XN_CORE_API void* XnOSCalloc(const XN_SIZE_T nAllocNum, const XN_SIZE_T nAllocSize)
{
	// Return a pointer to the requested allocation size and write zeros on the entire memory block (C Alloc)
	return (calloc(nAllocNum, nAllocSize));
}

XN_CORE_API void* XnOSCallocAligned(const XN_SIZE_T nAllocNum, const XN_SIZE_T nAllocSize, const XN_SIZE_T nAlignment)
{
	// Local function variables
	void* pMemBlock = NULL;
	XN_UINT32 nBlockSize = nAllocNum * nAllocSize;

	// Allocate the aligned memory block
	pMemBlock = XnOSMallocAligned(nBlockSize, nAlignment);

	// If it succeeded (the returned block wasn't NULL, let's zero it)
	if (pMemBlock != NULL)
	{
		XnOSMemSet(pMemBlock, 0, nBlockSize);
	}

	return (pMemBlock);
}

XN_CORE_API void* XnOSRealloc(void* pMemory, const XN_SIZE_T nAllocSize)
{
	XN_IMPLEMENT_OS;
	return (NULL);
}

XN_CORE_API void* XnOSReallocAligned(void* pMemory, const XN_SIZE_T nAllocSize, const XN_SIZE_T nAlignment)
{
	XN_IMPLEMENT_OS;
	return NULL;
}

XN_CORE_API void* XnOSRecalloc(void* pMemory, const XN_SIZE_T nAllocNum, const XN_SIZE_T nAllocSize)
{
	XN_IMPLEMENT_OS;
	return NULL;
}

XN_CORE_API void XnOSFree(const void* pMemBlock)
{
	// Note: No need to check if pMemBlock is NULL because free is guaranteed to handle this case correctly.

	// Free the requested memory block
	free ((void*)pMemBlock);
}

XN_CORE_API void  XnOSFreeAligned(const void* pMemBlock)
{
	// Note: No need to check if pMemBlock is NULL because free is guaranteed to handle this case correctly.

	// Free the requested aligned memory block
	free((void*)pMemBlock);
}

XN_CORE_API void XnOSMemCopy(void* pDest, const void* pSource, XN_UINT32 nCount)
{
	memcpy(pDest, pSource, nCount);
}

XN_CORE_API void XnOSMemSet(void* pDest, XN_UINT8 nValue, XN_UINT32 nCount)
{
	memset(pDest, nValue, nCount);
}

XN_CORE_API XN_UINT64  XnOSEndianSwapUINT64(XN_UINT64 nValue)
{
	return ((nValue >> 56) ) | ((nValue >> 40) & 0x000000000000ff00ULL) |
			((nValue >> 24) & 0x0000000000ff0000ULL) | ((nValue >> 8 ) & 0x00000000ff000000ULL) |
			((nValue << 8 ) & 0x000000ff00000000ULL) | ((nValue << 24) & 0x0000ff0000000000ULL) |
			((nValue << 40) & 0x00ff000000000000ULL) | ((nValue << 56) );
}

XN_CORE_API XN_UINT32  XnOSEndianSwapUINT32(XN_UINT32 nValue)
{
	return  (nValue>>24) | 
			((nValue<<8) & 0x00FF0000) |
			((nValue>>8) & 0x0000FF00) |
			(nValue<<24);
}

XN_CORE_API XN_UINT16 XnOSEndianSwapUINT16(XN_UINT16 nValue)
{
	return ((nValue>>8) | (nValue<<8));
}
XN_CORE_API XN_FLOAT XnOSEndianSwapFLOAT(XN_FLOAT fValue)
{
	XN_UINT32* pnValue = (XN_UINT32*)&fValue;
	XN_UINT32 nValue = XnOSEndianSwapUINT32(*pnValue);
	XN_FLOAT* pfValue = (XN_FLOAT*)&nValue;
	return *pfValue;
}