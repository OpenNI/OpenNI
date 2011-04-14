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
