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

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSCreateSemaphore(XN_SEMAPHORE_HANDLE* pSemaphoreHandle, XnUInt32 nInitialCount)
{
	XN_VALIDATE_OUTPUT_PTR(pSemaphoreHandle);
	*pSemaphoreHandle = CreateSemaphore(NULL, nInitialCount, nInitialCount, NULL);
	if (*pSemaphoreHandle == NULL)
	{
		xnLogError(XN_MASK_OS, "Failed to create semaphore. Last error was %u.", GetLastError());
		XN_ASSERT(FALSE);
		return XN_STATUS_OS_SEMAPHORE_CREATION_FAILED;
	}

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSLockSemaphore(XN_SEMAPHORE_HANDLE hSemaphore, XnUInt32 nMilliseconds)
{
	XN_RET_IF_NULL(hSemaphore, XN_STATUS_OS_INVALID_SEMAPHORE);
	DWORD nRetVal = WaitForSingleObject(hSemaphore, nMilliseconds);

	switch (nRetVal)
	{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			return XN_STATUS_OS_SEMAPHORE_TIMEOUT;
		default:
			xnLogError(XN_MASK_OS, "Failed to wait for semaphore. Last error was %u.", GetLastError());
			XN_ASSERT(FALSE);
			return XN_STATUS_OS_SEMAPHORE_LOCK_FAILED;
	}
	
	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSUnlockSemaphore(XN_SEMAPHORE_HANDLE hSemaphore)
{
	XN_RET_IF_NULL(hSemaphore, XN_STATUS_OS_INVALID_SEMAPHORE);
	if (!ReleaseSemaphore(hSemaphore, 1, NULL))
	{
		xnLogError(XN_MASK_OS, "Failed to release semaphore. Last error was %u.", GetLastError());
		XN_ASSERT(FALSE);
		return XN_STATUS_OS_SEMAPHORE_UNLOCK_FAILED;
	}
	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSCloseSemaphore(XN_SEMAPHORE_HANDLE* pSemaphoreHandle)
{
	XN_VALIDATE_INPUT_PTR(pSemaphoreHandle);
	XN_RET_IF_NULL(*pSemaphoreHandle, XN_STATUS_OS_INVALID_SEMAPHORE);
	if (!CloseHandle(*pSemaphoreHandle))
	{
		xnLogError(XN_MASK_OS, "Failed to release semaphore. Last error was %u.", GetLastError());
		XN_ASSERT(FALSE);
		return XN_STATUS_OS_SEMAPHORE_CLOSE_FAILED;
	}
	*pSemaphoreHandle = NULL;
	return XN_STATUS_OK;
}
