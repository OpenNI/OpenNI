/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include <XnCore/XnCoreGlobals.h>

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
XnOSTimer g_XnOSHighResGlobalTimer;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_CORE_API XnStatus XnOSGetEpochTime(XN_UINT32* nEpochTime)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSGetTimeStamp(XN_UINT64* nTimeStamp)
{
	XN_VALIDATE_OUTPUT_PTR(nTimeStamp);

	return XnOSQueryTimer(g_XnOSHighResGlobalTimer, nTimeStamp);
}

XN_CORE_API XnStatus XnOSGetHighResTimeStamp(XN_UINT64* nTimeStamp)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnOSGetTimeStamp(nTimeStamp);
	XN_IS_STATUS_OK(nRetVal);

	*nTimeStamp *= 1000;

	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSSleep(XN_UINT32 nMilliseconds)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// no OS, so just cycle this time passed
	XN_UINT64 nTill;
	nRetVal = XnOSGetTimeStamp(&nTill);
	XN_IS_STATUS_OK(nRetVal);

	nTill += nMilliseconds;

	XN_UINT64 nNow;

	do
	{
		nRetVal = XnOSGetTimeStamp(&nNow);
		XN_IS_STATUS_OK(nRetVal);
	} while (nNow < nTill);

	// All is good...
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSStartTimer(XnOSTimer* pTimer)
{
	XN_VALIDATE_INPUT_PTR(pTimer);

	pTimer->nStartTick = clock();

	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSStartHighResTimer(XnOSTimer* pTimer)
{
	return XnOSStartTimer(pTimer);
}

XN_CORE_API XnStatus XnOSQueryTimer(XnOSTimer Timer, XN_UINT64* pnTimeSinceStart)
{
	*pnTimeSinceStart = (clock() - Timer.nStartTick) * 1000 / CLOCKS_PER_SEC;

	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSStopTimer(XnOSTimer* pTimer)
{
	return (XN_STATUS_OK);
}
