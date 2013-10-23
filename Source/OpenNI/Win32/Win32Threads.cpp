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
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pThreadProc);
	XN_VALIDATE_OUTPUT_PTR(pThreadHandle);

	// Create a thread via the OS
	*pThreadHandle = CreateThread(NULL, 0, pThreadProc, pThreadParam, 0, NULL);

	// Make sure it succeeded (return value is not null)
	XN_VALIDATE_PTR(*pThreadHandle, XN_STATUS_OS_THREAD_CREATION_FAILED);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSTerminateThread(XN_THREAD_HANDLE* pThreadHandle)
{
	// Local function variables
	XnBool bRetVal = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pThreadHandle);

	// Make sure the actual thread handle isn't NULL
	XN_RET_IF_NULL(*pThreadHandle, XN_STATUS_OS_INVALID_THREAD);

	// Close the thread via the OS
	bRetVal = TerminateThread(*pThreadHandle, 0);

	// Make sure it succeeded (return value is true)
	if (bRetVal != TRUE)
	{
		return (XN_STATUS_OS_THREAD_TERMINATION_FAILED);
	}

	// Close the handle
	nRetVal = xnOSCloseThread(pThreadHandle);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);

}

XN_C_API XnStatus xnOSCloseThread(XN_THREAD_HANDLE* pThreadHandle)
{
	// Local function variables
	XnBool bRetVal = FALSE;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pThreadHandle);

	// Make sure the actual thread handle isn't NULL
	XN_RET_IF_NULL(*pThreadHandle, XN_STATUS_OS_INVALID_THREAD);

	// Close the thread via the OS
	bRetVal = CloseHandle(*pThreadHandle);

	// Make sure it succeeded (return value is true)
	if (bRetVal != TRUE)
	{
		return (XN_STATUS_OS_THREAD_CLOSE_FAILED);
	}

	// Null the output thread
	*pThreadHandle = NULL;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWaitForThreadExit(XN_THREAD_HANDLE ThreadHandle, XnUInt32 nMilliseconds)
{
	// Local function variables
	XnInt32 nRetVal = 0;

	// Make sure the actual thread handle isn't NULL
	XN_RET_IF_NULL(ThreadHandle, XN_STATUS_OS_INVALID_THREAD);

	// Lock the mutex for a period if time (can be infinite)
	nRetVal = WaitForSingleObject(ThreadHandle, nMilliseconds);
	
	// Check the return value (WAIT_OBJECT_0 is OK)
	if (nRetVal != WAIT_OBJECT_0)
	{
		// Handle the timeout failure
		if (nRetVal == WAIT_TIMEOUT)
		{
			return (XN_STATUS_OS_THREAD_TIMEOUT);
		}
		else
		{
			return (XN_STATUS_OS_THREAD_WAIT_FAILED);
		}
	}
	
	// All is good...
	return (XN_STATUS_OK);

}

XN_C_API XnStatus xnOSSetThreadPriority(XN_THREAD_HANDLE ThreadHandle, XnThreadPriority nPriority)
{
	int nWinPriority = 0;
	switch (nPriority)
	{
		case XN_PRIORITY_CRITICAL:
			nWinPriority = THREAD_PRIORITY_TIME_CRITICAL;
			break;
		case XN_PRIORITY_HIGH:
			nWinPriority = THREAD_PRIORITY_HIGHEST;
			break;
		case XN_PRIORITY_LOW:
			nWinPriority = THREAD_PRIORITY_LOWEST;
			break;
		case XN_PRIORITY_NORMAL:
			nWinPriority = THREAD_PRIORITY_NORMAL;
			break;
		default:
			XN_ASSERT(FALSE);
			return XN_STATUS_OS_THREAD_UNSUPPORTED_PRIORITY;
	}

	if (!SetThreadPriority(ThreadHandle, nWinPriority))
	{
		return XN_STATUS_OS_THREAD_SET_PRIORITY_FAILED;
	}

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSGetCurrentThreadID(XN_THREAD_ID* pThreadID)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pThreadID);

	// Get the current thread id
	*pThreadID = GetCurrentThreadId();

	// All is good...
	return (XN_STATUS_OK);
}

