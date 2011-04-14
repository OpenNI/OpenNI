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
#ifndef __XNOSMEMORY_H__
#define __XNOSMEMORY_H__

#include "XnPlatform.h"

// Memory
XN_C_API void* xnOSMalloc(const XnSizeT nAllocSize);
XN_C_API void* xnOSMallocAligned(const XnSizeT nAllocSize, const XnSizeT nAlignment);
XN_C_API void* xnOSCalloc(const XnSizeT nAllocNum, const XnSizeT nAllocSize);
XN_C_API void* xnOSCallocAligned(const XnSizeT nAllocNum, const XnSizeT nAllocSize, const XnSizeT nAlignment);
XN_C_API void* xnOSRealloc(void* pMemory, const XnSizeT nAllocSize);
XN_C_API void* xnOSReallocAligned(void* pMemory, const XnSizeT nAllocSize, const XnSizeT nAlignment);
XN_C_API void* xnOSRecalloc(void* pMemory, const XnSizeT nAllocNum, const XnSizeT nAllocSize);
XN_C_API void xnOSFree(const void* pMemBlock);
XN_C_API void xnOSFreeAligned(const void* pMemBlock);
XN_C_API void xnOSMemCopy(void* pDest, const void* pSource, XnSizeT nCount);
XN_C_API XnInt32 xnOSMemCmp(const void *pBuf1, const void *pBuf2, XnSizeT nCount);
XN_C_API void xnOSMemSet(void* pDest, XnUInt8 nValue, XnSizeT nCount);
XN_C_API void xnOSMemMove(void* pDest, const void* pSource, XnSizeT nCount);
XN_C_API XnUInt64 xnOSEndianSwapUINT64(XnUInt64 nValue);
XN_C_API XnUInt32 xnOSEndianSwapUINT32(XnUInt32 nValue);
XN_C_API XnUInt16 xnOSEndianSwapUINT16(XnUInt16 nValue);
XN_C_API XnFloat xnOSEndianSwapFLOAT(XnFloat fValue);


#endif // __XNOSMEMORY_H__
