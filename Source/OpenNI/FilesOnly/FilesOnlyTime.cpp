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
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetTimeStamp(XnUInt64* nTimeStamp)
{
	XN_VALIDATE_OUTPUT_PTR(nTimeStamp);

	return xnOSQueryTimer(g_xnOSHighResGlobalTimer, nTimeStamp);
}

XN_C_API XnStatus xnOSGetHighResTimeStamp(XnUInt64* nTimeStamp)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnOSGetTimeStamp(nTimeStamp);
	XN_IS_STATUS_OK(nRetVal);

	*nTimeStamp *= 1000;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSleep(XnUInt32 nMilliseconds)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// no OS, so just cycle this time passed
	XnUInt64 nTill;
	nRetVal = xnOSGetTimeStamp(&nTill);
	XN_IS_STATUS_OK(nRetVal);

	nTill += nMilliseconds;

	XnUInt64 nNow;

	do
	{
		nRetVal = xnOSGetTimeStamp(&nNow);
		XN_IS_STATUS_OK(nRetVal);
	} while (nNow < nTill);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSStartTimer(XnOSTimer* pTimer)
{
	XN_VALIDATE_INPUT_PTR(pTimer);

	pTimer->nStartTick = clock();

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSStartHighResTimer(XnOSTimer* pTimer)
{
	return xnOSStartTimer(pTimer);
}

XN_C_API XnStatus xnOSQueryTimer(XnOSTimer Timer, XnUInt64* pnTimeSinceStart)
{
	*pnTimeSinceStart = (clock() - Timer.nStartTick) * 1000 / CLOCKS_PER_SEC;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSStopTimer(XnOSTimer* pTimer)
{
	return (XN_STATUS_OK);
}
