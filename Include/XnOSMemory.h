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
#ifndef __XNOSMEMORY_H__
#define __XNOSMEMORY_H__

#include "XnPlatform.h"

// Memory
XN_C_API void* XN_C_DECL xnOSMalloc(const XnSizeT nAllocSize);
XN_C_API void* XN_C_DECL xnOSMallocAligned(const XnSizeT nAllocSize, const XnSizeT nAlignment);
XN_C_API void* XN_C_DECL xnOSCalloc(const XnSizeT nAllocNum, const XnSizeT nAllocSize);
XN_C_API void* XN_C_DECL xnOSCallocAligned(const XnSizeT nAllocNum, const XnSizeT nAllocSize, const XnSizeT nAlignment);
XN_C_API void* XN_C_DECL xnOSRealloc(void* pMemory, const XnSizeT nAllocSize);
XN_C_API void* XN_C_DECL xnOSReallocAligned(void* pMemory, const XnSizeT nAllocSize, const XnSizeT nAlignment);
XN_C_API void* XN_C_DECL xnOSRecalloc(void* pMemory, const XnSizeT nAllocNum, const XnSizeT nAllocSize);
XN_C_API void XN_C_DECL xnOSFree(const void* pMemBlock);
XN_C_API void XN_C_DECL xnOSFreeAligned(const void* pMemBlock);
XN_C_API void XN_C_DECL xnOSMemCopy(void* pDest, const void* pSource, XnSizeT nCount);
XN_C_API XnInt32 XN_C_DECL xnOSMemCmp(const void *pBuf1, const void *pBuf2, XnSizeT nCount);
XN_C_API void XN_C_DECL xnOSMemSet(void* pDest, XnUInt8 nValue, XnSizeT nCount);
XN_C_API void XN_C_DECL xnOSMemMove(void* pDest, const void* pSource, XnSizeT nCount);
XN_C_API XnUInt64 XN_C_DECL xnOSEndianSwapUINT64(XnUInt64 nValue);
XN_C_API XnUInt32 XN_C_DECL xnOSEndianSwapUINT32(XnUInt32 nValue);
XN_C_API XnUInt16 XN_C_DECL xnOSEndianSwapUINT16(XnUInt16 nValue);
XN_C_API XnFloat XN_C_DECL xnOSEndianSwapFLOAT(XnFloat fValue);


#endif // __XNOSMEMORY_H__
