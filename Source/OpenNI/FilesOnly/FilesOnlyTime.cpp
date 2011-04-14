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
