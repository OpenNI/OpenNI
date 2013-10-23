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
