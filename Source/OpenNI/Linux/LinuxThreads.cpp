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
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSCreateThread(XN_THREAD_PROC_PROTO pThreadProc, const XN_THREAD_PARAM pThreadParam, XN_THREAD_HANDLE* pThreadHandle)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pThreadProc);
	XN_VALIDATE_OUTPUT_PTR(pThreadHandle);

	// allocate thread handle
	XN_VALIDATE_ALLOC(*pThreadHandle, pthread_t);

	// Create a thread via the OS
	int rc = pthread_create(*pThreadHandle, NULL, pThreadProc, pThreadParam);
	if (rc != 0)
	{
		XN_FREE_AND_NULL(*pThreadHandle);
		return (XN_STATUS_OS_THREAD_CREATION_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSTerminateThread(XN_THREAD_HANDLE* pThreadHandle)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pThreadHandle);

	// Make sure the actual thread handle isn't NULL
	XN_RET_IF_NULL(*pThreadHandle, XN_STATUS_OS_INVALID_THREAD);
	
	XN_THREAD_HANDLE handle = *pThreadHandle;

#ifndef XN_PLATFORM_LINUX_NO_PTHREAD_CANCEL
	if (0 != pthread_cancel(*handle))
#else
	// send the KILL signal to the thread
	if (0 != pthread_kill(*handle, SIGUSR1))
#endif
	{
		return (XN_STATUS_OS_THREAD_TERMINATION_FAILED);
	}
	
	// Close the handle
	XnStatus nRetVal = xnOSCloseThread(pThreadHandle);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseThread(XN_THREAD_HANDLE* pThreadHandle)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pThreadHandle);

	// Make sure the actual thread handle isn't NULL
	XN_RET_IF_NULL(*pThreadHandle, XN_STATUS_OS_INVALID_THREAD);

	// free handle
	XN_FREE_AND_NULL(*pThreadHandle);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWaitForThreadExit(XN_THREAD_HANDLE ThreadHandle, XnUInt32 nMilliseconds)
{
	int rc = 0;
	
	// Make sure the actual thread handle isn't NULL
	XN_RET_IF_NULL(ThreadHandle, XN_STATUS_OS_INVALID_THREAD);
	
	if (nMilliseconds == XN_WAIT_INFINITE)
	{
		// join via the OS
		void* pReturnValue;
		rc = pthread_join(*ThreadHandle, &pReturnValue);
	}
	else
	{
		// calculate timeout absolute time. First we take current time
		struct timespec time;
		XnStatus nRetVal = xnOSGetAbsTimeout(&time, nMilliseconds);
		if (nRetVal != XN_STATUS_OK)
		{
			return XN_STATUS_OS_THREAD_TERMINATION_FAILED;
		}
		
		// join via the OS
		void* pReturnValue;
#ifndef XN_PLATFORM_HAS_NO_TIMED_OPS
		rc = pthread_timedjoin_np(*ThreadHandle, &pReturnValue, &time);
#else
		rc = pthread_join(*ThreadHandle, &pReturnValue);
#endif
	}

	// check for failures
	if (rc == ETIMEDOUT)
	{
		return (XN_STATUS_OS_THREAD_TIMEOUT);
	}
	else if (rc != 0)
	{
		return (XN_STATUS_OS_THREAD_TERMINATION_FAILED);
	}
	
	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSetThreadPriority(XN_THREAD_HANDLE ThreadHandle, XnThreadPriority nPriority)
{
	// Make sure the actual thread handle isn't NULL
	XN_RET_IF_NULL(ThreadHandle, XN_STATUS_OS_INVALID_THREAD);

	int nPolicy = 0;
	sched_param param;
	int rc = 0;
	
	if (nPriority == XN_PRIORITY_CRITICAL)
	{
		memset(&param, 0, sizeof(param));
	
#ifndef XN_PLATFORM_HAS_NO_SCHED_PARAM
		param.__sched_priority = 5;
#endif
		nPolicy = SCHED_RR;
		
#if XN_PLATFORM == XN_PLATFORM_ANDROID_ARM
		//Note: It's only going to work if it runs as root! (but if not it fails anyway...)		
		param.sched_priority = sched_get_priority_max(nPolicy) - 1;
#endif
	}
	else
	{
		return (XN_STATUS_OS_THREAD_UNSUPPORTED_PRIORITY);
	}
	
	rc = pthread_setschedparam(*ThreadHandle, nPolicy, &param);
	if (rc != 0)
	{
#if XN_PLATFORM == XN_PLATFORM_ANDROID_ARM
		//This should also work as a non-root user...
		xnLogWarning(XN_MASK_OS, "Failed to use pthread_setschedparam (%d). Trying setpriority instead...", errno);

		rc = setpriority(PRIO_PROCESS, gettid(), -8); //-8 is defined as ANDROID_PRIORITY_URGENT_DISPLAY.
		if (rc < 0)
		{
				xnLogWarning(XN_MASK_OS, "Failed to use setpriority (%d)", rc);
				return (XN_STATUS_OS_THREAD_SET_PRIORITY_FAILED);
		}
#else
		xnLogWarning(XN_MASK_OS, "Failed to set thread priority (%d)", errno);
		return (XN_STATUS_OS_THREAD_SET_PRIORITY_FAILED);
#endif

	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetCurrentThreadID(XN_THREAD_ID* pThreadID)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pThreadID);

	// Get the current thread id
	*pThreadID = pthread_self();

	// All is good...
	return (XN_STATUS_OK);
}

