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

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
XnOSTimer g_xnOSHighResGlobalTimer;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSGetEpochTime(XnUInt32* nEpochTime)
{
	// Local function variables
	*nEpochTime = (XnUInt32)time(NULL);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetTimeStamp(XnUInt64* nTimeStamp)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Get the high resolution global timer value
	nRetVal = xnOSGetHighResTimeStamp(nTimeStamp);
	XN_IS_STATUS_OK(nRetVal);

	// Convert from microseconds to milliseconds
	*nTimeStamp /= 1000;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetHighResTimeStamp(XnUInt64* nTimeStamp)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// TODO: Check if the core subsystem is initialized

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(nTimeStamp);

	// Get the high resolution global timer value
	nRetVal = xnOSQueryTimer(g_xnOSHighResGlobalTimer, nTimeStamp);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSleep(XnUInt32 nMilliseconds)
{
	// Sleep via the OS
	Sleep(nMilliseconds);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSStartTimer(XnOSTimer* pTimer)
{
	// Local function variables
	XnBool bRetVal = FALSE;
	LARGE_INTEGER nTicksPerSecond;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pTimer);

	bRetVal = QueryPerformanceFrequency(&nTicksPerSecond);
	XN_IS_BOOL_OK_RET(bRetVal, XN_STATUS_OS_TIMER_CREATION_FAILED);

	pTimer->dTicksPerTimeUnit = (XnDouble)nTicksPerSecond.QuadPart / 1000.0;

	bRetVal = QueryPerformanceCounter(&pTimer->nStartTick);
	XN_IS_BOOL_OK_RET(bRetVal, XN_STATUS_OS_TIMER_QUERY_FAILED);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSStartHighResTimer(XnOSTimer* pTimer)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnOSStartTimer(pTimer);
	XN_IS_STATUS_OK(nRetVal);

	// Convert from microseconds to milliseconds
	pTimer->dTicksPerTimeUnit /= 1000.0;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSQueryTimer(XnOSTimer Timer, XnUInt64* pnTimeSinceStart)
{
	// Local function variables
	XnBool bRetVal = FALSE;
	LARGE_INTEGER nCurrTick;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pnTimeSinceStart);

	if (Timer.dTicksPerTimeUnit == 0)
	{
		return (XN_STATUS_OS_INVALID_TIMER);
	}

	bRetVal = QueryPerformanceCounter(&nCurrTick);
	XN_IS_BOOL_OK_RET(bRetVal, XN_STATUS_OS_TIMER_QUERY_FAILED);

	*pnTimeSinceStart = XnUInt64((nCurrTick.QuadPart - Timer.nStartTick.QuadPart) / Timer.dTicksPerTimeUnit);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSStopTimer(XnOSTimer* pTimer)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pTimer);

	if (pTimer->dTicksPerTimeUnit == 0)
	{
		return (XN_STATUS_OS_INVALID_TIMER);
	}

	xnOSMemSet(pTimer, 0, sizeof(XnOSTimer));

	// All is good...
	return (XN_STATUS_OK);
}
