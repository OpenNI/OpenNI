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
#ifndef __XN_GENERAL_BUFFER_H__
#define __XN_GENERAL_BUFFER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnPlatform.h"
#include "XnOS.h"
#include "XnStatusCodes.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/* Describes a general buffer. */
typedef struct XnGeneralBuffer
{
	/* A pointer to the actual data. */
	void* pData;
	/* The size of the data in bytes. */
	XnUInt32 nDataSize;
} XnGeneralBuffer;

//---------------------------------------------------------------------------
// Exported Functions
//---------------------------------------------------------------------------

/**
* Packs a pointer and a size into an XnGeneralBuffer struct.
*/
inline XnGeneralBuffer XnGeneralBufferPack(void* pData, XnUInt32 nDataSize)
{
	XnGeneralBuffer result;
	result.pData = pData;
	result.nDataSize = nDataSize;
	return result;
}

/**
* Copies one general buffer into another.
*/
inline XnStatus XnGeneralBufferCopy(XnGeneralBuffer* pDest, const XnGeneralBuffer* pSrc)
{
	XN_VALIDATE_INPUT_PTR(pDest);
	XN_VALIDATE_INPUT_PTR(pSrc);

	if (pSrc->nDataSize > pDest->nDataSize)
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;

	xnOSMemCopy(pDest->pData, pSrc->pData, pSrc->nDataSize);
	pDest->nDataSize = pSrc->nDataSize;
	return XN_STATUS_OK;
}

inline XnStatus XnGeneralBufferAlloc(XnGeneralBuffer* pDest, XnUInt32 nSize)
{
	XN_VALIDATE_INPUT_PTR(pDest);

	void* pData;
	pData = xnOSMalloc(nSize);
	XN_VALIDATE_ALLOC_PTR(pData);

	pDest->pData = pData;
	pDest->nDataSize = nSize;
	return XN_STATUS_OK;
}

inline XnStatus XnGeneralBufferRealloc(XnGeneralBuffer* pDest, XnUInt32 nSize)
{
	XN_VALIDATE_INPUT_PTR(pDest);

	void* pData;
	pData = xnOSRealloc(pDest, nSize);
	XN_VALIDATE_ALLOC_PTR(pData);

	pDest->pData = pData;
	pDest->nDataSize = nSize;
	return XN_STATUS_OK;
}

inline void XnGeneralBufferFree(XnGeneralBuffer* pDest)
{
	XN_FREE_AND_NULL(pDest->pData);
	pDest->nDataSize = 0;
}

//---------------------------------------------------------------------------
// Helper Macros
//---------------------------------------------------------------------------
#define XN_PACK_GENERAL_BUFFER(x)		XnGeneralBufferPack(&x, sizeof(x))

#define XN_VALIDATE_GENERAL_BUFFER_TYPE(gb, t)	\
	if ((gb).nDataSize != sizeof(t))			\
	{											\
		return XN_STATUS_INVALID_BUFFER_SIZE;	\
	}

#endif //__XN_GENERAL_BUFFER_H__