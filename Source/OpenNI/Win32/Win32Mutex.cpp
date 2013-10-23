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
#include <XnLog.h>
#include "XnOSWin32Internal.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSCreateMutex(XN_MUTEX_HANDLE* pMutexHandle)
{
	return (xnOSCreateNamedMutex(pMutexHandle, NULL));
}

XN_C_API XnStatus xnOSCreateNamedMutex(XN_MUTEX_HANDLE* pMutexHandle, const XnChar* cpMutexName)
{
	return xnOSCreateNamedMutexEx(pMutexHandle, cpMutexName, FALSE);
}

XN_C_API XnStatus XN_C_DECL xnOSCreateNamedMutexEx(XN_MUTEX_HANDLE* pMutexHandle, const XnChar* cpMutexName, XnBool bAllowOtherUsers)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pMutexHandle);

	XnChar strMutexOSName[MAX_PATH];
	XnChar* pMutexOSName = NULL;
	SECURITY_ATTRIBUTES* pSecurityAttributes = NULL;

	if (cpMutexName != NULL)
	{
		nRetVal = XnWin32CreateKernelObjectName(strMutexOSName, MAX_PATH, cpMutexName, bAllowOtherUsers);
		if (nRetVal != XN_STATUS_OK)
		{
			return XN_STATUS_OS_MUTEX_CREATION_FAILED;
		}

		pMutexOSName = strMutexOSName;

		nRetVal = XnWin32GetSecurityAttributes(bAllowOtherUsers, &pSecurityAttributes);
		if (nRetVal != XN_STATUS_OK)
		{
			return XN_STATUS_OS_MUTEX_CREATION_FAILED;
		}
	}

	// Create a named mutex via the OS
	*pMutexHandle = CreateMutex(pSecurityAttributes, FALSE, pMutexOSName);

	// Make sure it succeeded (return value is not null)
	if (*pMutexHandle == NULL)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_OS_MUTEX_CREATION_FAILED, XN_MASK_OS, "Failed to create mutex (%d).", GetLastError());
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseMutex(XN_MUTEX_HANDLE* pMutexHandle)
{
	// Local function variables
	XnBool bRetVal = FALSE;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pMutexHandle);

	// Make sure the actual mutex handle isn't NULL
	XN_RET_IF_NULL(*pMutexHandle, XN_STATUS_OS_INVALID_MUTEX);

	xnOSUnLockMutex(*pMutexHandle);
	
	// Close the mutex via the OS
	bRetVal = CloseHandle(*pMutexHandle);

	// Make sure it succeeded (return value is true)
	if (bRetVal != TRUE)
	{
		return (XN_STATUS_OS_MUTEX_CLOSE_FAILED);
	}

	// Null the output mutex
	*pMutexHandle = NULL;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSLockMutex(const XN_MUTEX_HANDLE MutexHandle, XnUInt32 nMilliseconds)
{
	// Local function variables
	XnInt32 nRetVal = 0;
	
	// Make sure the actual mutex handle isn't NULL
	XN_RET_IF_NULL(MutexHandle, XN_STATUS_OS_INVALID_MUTEX);

	// Wait for mutex to lock for a period of time (can be infinite)
	nRetVal = WaitForSingleObject(MutexHandle, nMilliseconds);
	
	// Check the return value (WAIT_OBJECT_0 is OK)
	if (nRetVal != WAIT_OBJECT_0)
	{
		// Handle the timeout failure
		if (nRetVal == WAIT_TIMEOUT)
		{
			return (XN_STATUS_OS_MUTEX_TIMEOUT);
		}
		else if (nRetVal == WAIT_ABANDONED)
		{
			xnLogWarning(XN_MASK_OS, "The mutex was abandoned (poor thing)");
			//No return! We now own the mutex so we can continue normally.
		}
		else
		{
			return (XN_STATUS_OS_MUTEX_LOCK_FAILED);
		}
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSUnLockMutex(const XN_MUTEX_HANDLE MutexHandle)
{
	// Local function variables
	XnBool bRetVal = FALSE;

	// Make sure the actual mutex handle isn't NULL
	XN_RET_IF_NULL(MutexHandle, XN_STATUS_OS_INVALID_MUTEX);

	// Release the mutex via the OS
	bRetVal = ReleaseMutex(MutexHandle);

	// Make sure it succeeded (return value is true)
	if (bRetVal != TRUE)
	{
		return (XN_STATUS_OS_MUTEX_UNLOCK_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}
