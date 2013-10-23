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
// Macros
//---------------------------------------------------------------------------
/* Translates from struct timespec to microseconds. */
#define TIMESPEC_TO_MICRO_SECS(ts)	(ts.tv_sec * 1000000 + ts.tv_nsec / 1000)

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSGetEpochTime(XnUInt32* nEpochTime)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

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
	// Sleep via the OS (function needs time in microseconds)
	usleep(nMilliseconds*1000);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSStartTimer(XnOSTimer* pTimer)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pTimer);
	
	if (XN_STATUS_OK != xnOSGetMonoTime(&pTimer->tStartTime))
	{
		return XN_STATUS_OS_TIMER_QUERY_FAILED;
	}
	
	pTimer->bHighRes = false;
	
	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSStartHighResTimer(XnOSTimer* pTimer)
{
	// start a normal timer
	XnStatus nRetVal = xnOSStartTimer(pTimer);
	XN_IS_STATUS_OK(nRetVal);
	
	// now make it high-res
	pTimer->bHighRes = true;
	
	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSQueryTimer(XnOSTimer Timer, XnUInt64* pnTimeSinceStart)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pnTimeSinceStart);

	struct timespec now;
	
	if (XN_STATUS_OK != xnOSGetMonoTime(&now))
	{
		return XN_STATUS_OS_TIMER_QUERY_FAILED;
	}
	
	*pnTimeSinceStart = ((now.tv_sec - Timer.tStartTime.tv_sec) * 1E6 + (now.tv_nsec - Timer.tStartTime.tv_nsec) / 1E3);
	
	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSStopTimer(XnOSTimer* pTimer)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pTimer);

	memset(pTimer, 0, sizeof(XnOSTimer));

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetMonoTime(struct timespec* pTime)
{
#ifndef XN_PLATFORM_HAS_NO_CLOCK_GETTIME
	if (0 != clock_gettime(CLOCK_REALTIME, pTime))
	{
		return (XN_STATUS_OS_EVENT_WAIT_FAILED);
	}
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	pTime->tv_sec = tv.tv_sec;
	pTime->tv_nsec = tv.tv_usec * 1000;  
#endif
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetTimeout(struct timespec* pTime, XnUInt32 nMilliseconds)
{
	pTime->tv_sec = (nMilliseconds / 1000);
	pTime->tv_nsec = ((nMilliseconds % 1000) * 1000000);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSGetAbsTimeout(struct timespec* pTime, XnUInt32 nMilliseconds)
{
	XnStatus nRetVal = xnOSGetMonoTime(pTime);
	XN_IS_STATUS_OK(nRetVal);
	
	pTime->tv_sec += (nMilliseconds / 1000);
	pTime->tv_nsec += ((nMilliseconds % 1000) * 1000000);

	if (pTime->tv_nsec >= 1000000000)
	{
		pTime->tv_nsec -= 1000000000;
		pTime->tv_sec++;
	}
	
	return XN_STATUS_OK;
}
