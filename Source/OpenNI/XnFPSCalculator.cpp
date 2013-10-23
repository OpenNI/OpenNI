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
#include "XnFPSCalculator.h"
#include <XnOS.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
struct XnFPSDataImpl
{
	/** An array of the time of each frame. */
	XnUInt64* anTimes;
	/** The size of the array. */
	XnUInt32 nArraySize;
	/** Index of the next place to write to. */
	XnUInt32 nCurrIndex;
};

typedef struct XnFPSDataImpl XnFPSDataImpl;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus xnFPSInit(XnFPSData* pFPS, XnUInt32 nSamplesCount)
{
	XN_VALIDATE_OUTPUT_PTR(pFPS);

	// make sure xnOS is init
	XnStatus nRetVal = xnOSInit();
	if (nRetVal != XN_STATUS_OK && nRetVal != XN_STATUS_OS_ALREADY_INIT)
		return nRetVal;

	// Allocate struct
	XN_VALIDATE_CALLOC(*pFPS, XnFPSDataImpl, 1);
	XnFPSDataImpl* pData = *pFPS;

	// Allocate array
	XN_VALIDATE_ALIGNED_CALLOC(pData->anTimes, XnUInt64, nSamplesCount, XN_DEFAULT_MEM_ALIGN);

	pData->nArraySize = nSamplesCount;

	return XN_STATUS_OK;
}

XnStatus xnFPSMarkFrame(XnFPSData* pFPS, XnUInt64 nNow /* = 0 */)
{
	XN_VALIDATE_INPUT_PTR(pFPS);
	XnFPSDataImpl* pData = *pFPS;

	if (nNow == 0)
		xnOSGetHighResTimeStamp(&nNow);

	// mark this time
	pData->anTimes[pData->nCurrIndex] = nNow;

	// increment array index
	if (++pData->nCurrIndex == pData->nArraySize)
		pData->nCurrIndex = 0;

	return XN_STATUS_OK;
}

XnDouble xnFPSCalc(XnFPSData* pFPS, XnUInt32 nAverageOver /* = 3000 */, XnUInt64 nNow /* = 0 */)
{
	XN_VALIDATE_INPUT_PTR(pFPS);
	XnFPSDataImpl* pData = *pFPS;

	if (nNow == 0)
		xnOSGetHighResTimeStamp(&nNow);

	XnUInt64 nSince = nNow - nAverageOver*1000;

	// search for first index
	XnUInt32 nFirst = pData->nCurrIndex;
	XnUInt32 nLast = (pData->nCurrIndex + pData->nArraySize - 1) % pData->nArraySize;

	// no valid samples
	if (pData->anTimes[nLast] < nSince)
		return 0.0;

	// move first to the first sample BEFORE time 'since'.
	while (nFirst != nLast && pData->anTimes[nFirst] < nSince)
		nFirst = (nFirst + 1) % pData->nArraySize;

	// calculate frames per second
	XnUInt32 nFrames = ((nLast - nFirst + pData->nArraySize) % pData->nArraySize) + 1;

	// if have no statistics...
	if (nFrames <= 1)
		return 0;

	// number of frames received divided by amount of time passed
	return (nFrames * 1e6 / (nNow - pData->anTimes[nFirst]));
}

XnStatus xnFPSFree(XnFPSData* pFPS)
{
	XN_VALIDATE_INPUT_PTR(pFPS);
	XnFPSDataImpl* pData = *pFPS;

	if (pData != NULL)
	{
		if (pData->anTimes != NULL)
		{
			XN_ALIGNED_FREE_AND_NULL(pData->anTimes);
		}

		if (*pFPS != NULL)
		{
			XN_FREE_AND_NULL(*pFPS);
		}
	}

	return XN_STATUS_OK;
}
