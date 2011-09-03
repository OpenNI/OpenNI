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
