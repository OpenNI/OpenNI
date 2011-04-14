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
