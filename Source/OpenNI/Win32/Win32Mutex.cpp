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
XN_C_API XnStatus xnOSCreateMutex(XN_MUTEX_HANDLE* pMutexHandle)
{
	return (xnOSCreateNamedMutex(pMutexHandle, NULL));
}

XN_C_API XnStatus xnOSCreateNamedMutex(XN_MUTEX_HANDLE* pMutexHandle, const XnChar* cpMutexName)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;
	XnBool bRetVal = FALSE;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pMutexHandle);

	// Create a named mutex via the OS
	*pMutexHandle = CreateMutex(NULL, FALSE, cpMutexName);

	// Make sure it succeeded (return value is not null)
	XN_VALIDATE_PTR(*pMutexHandle, XN_STATUS_OS_MUTEX_CREATION_FAILED);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseMutex(XN_MUTEX_HANDLE* pMutexHandle)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;
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
