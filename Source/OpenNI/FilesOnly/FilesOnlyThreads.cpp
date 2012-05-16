/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
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
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSCreateThread(XN_THREAD_PROC_PROTO pThreadProc, const XN_THREAD_PARAM pThreadParam, XN_THREAD_HANDLE* pThreadHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSTerminateThread(XN_THREAD_HANDLE* pThreadHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseThread(XN_THREAD_HANDLE* pThreadHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWaitForThreadExit(XN_THREAD_HANDLE ThreadHandle, XnUInt32 nMilliseconds)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetCurrentThreadID(XN_THREAD_HANDLE* pThreadID)
{
	*pThreadID = 0;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetCurrentCallStack(XnUInt32 nFramesToSkip, XnChar** astrFrames, XnUInt32 nMaxNameLength, XnUInt32* pnFrames)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetCurrentProcessID(XN_PROCESS_ID* pProcID)
{
	*pProcID = 0;
	return (XN_STATUS_OK);
}
