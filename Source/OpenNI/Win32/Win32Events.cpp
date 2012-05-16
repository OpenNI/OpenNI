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
#include <XnLog.h>
#include "XnOSWin32Internal.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus XN_C_DECL xnOSCreateEvent(XN_EVENT_HANDLE* pEventHandle, XnBool bManualReset)
{
	return (xnOSCreateNamedEvent(pEventHandle, NULL, bManualReset));
}

XN_C_API XnStatus XN_C_DECL xnOSCreateNamedEvent(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bManualReset)
{
	return xnOSCreateNamedEventEx(pEventHandle, cpEventName, bManualReset, FALSE);
}

XN_C_API XnStatus XN_C_DECL xnOSCreateNamedEventEx(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bManualReset, XnBool bAllowOtherUsers)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pEventHandle);

	XnChar strEventOSName[MAX_PATH];
	XnChar* pEventOSName = NULL;
	SECURITY_ATTRIBUTES* pSecurityAttributes = NULL;

	if (cpEventName != NULL)
	{
		nRetVal = XnWin32CreateKernelObjectName(strEventOSName, MAX_PATH, cpEventName, bAllowOtherUsers);
		if (nRetVal != XN_STATUS_OK)
		{
			return XN_STATUS_OS_EVENT_CREATION_FAILED;
		}

		pEventOSName = strEventOSName;

		nRetVal = XnWin32GetSecurityAttributes(bAllowOtherUsers, &pSecurityAttributes);
		if (nRetVal != XN_STATUS_OK)
		{
			return XN_STATUS_OS_MUTEX_CREATION_FAILED;
		}
	}

	// Create a named event via the OS
	*pEventHandle = CreateEvent(pSecurityAttributes, bManualReset, FALSE, pEventOSName);

	// Make sure it succeeded (return value is not null)
	if (*pEventHandle == NULL)
	{
		xnLogError(XN_MASK_OS, "CreateEvent() failed with error %u", GetLastError());
		return XN_STATUS_OS_EVENT_CREATION_FAILED;
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XN_C_DECL xnOSOpenNamedEvent(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName)
{
	return xnOSOpenNamedEventEx(pEventHandle, cpEventName, FALSE);
}

XN_C_API XnStatus XN_C_DECL xnOSOpenNamedEventEx(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bAllowOtherUsers)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_VALIDATE_INPUT_PTR(cpEventName);
	XN_VALIDATE_OUTPUT_PTR(pEventHandle);
	
	XnChar strEventOSName[MAX_PATH];
	nRetVal = XnWin32CreateKernelObjectName(strEventOSName, MAX_PATH, cpEventName, bAllowOtherUsers);
	if (nRetVal != XN_STATUS_OK)
	{
		return XN_STATUS_OS_EVENT_CREATION_FAILED;
	}

	*pEventHandle = OpenEvent(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, cpEventName);
	if (*pEventHandle == NULL)
	{
		return XN_STATUS_OS_EVENT_OPEN_FAILED;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseEvent(XN_EVENT_HANDLE* pEventHandle)
{
	// Local function variables
	XnBool bRetVal = FALSE;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pEventHandle);

	// Make sure the actual event handle isn't NULL
	XN_RET_IF_NULL(*pEventHandle, XN_STATUS_OS_INVALID_EVENT);

	// Close the event via the OS
	bRetVal = CloseHandle(*pEventHandle);

	// Make sure it succeeded (return value is true)
	if (bRetVal != TRUE)
	{
		xnLogVerbose(XN_MASK_OS, "CloseHandle() failed with error %u", GetLastError());
		return (XN_STATUS_OS_EVENT_CLOSE_FAILED);
	}

	// Null the output event
	*pEventHandle = NULL;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSetEvent(const XN_EVENT_HANDLE EventHandle)
{
	// Local function variables
	XnBool bRetVal = FALSE;

	// Make sure the actual event handle isn't NULL
	XN_RET_IF_NULL(EventHandle, XN_STATUS_OS_INVALID_EVENT);

	// Set the event via the OS
	bRetVal = SetEvent(EventHandle);

	// Make sure it succeeded (return value is true)
	if (bRetVal != TRUE)
	{
		xnLogVerbose(XN_MASK_OS, "SetEvent() failed with error %u", GetLastError());
		return (XN_STATUS_OS_EVENT_SET_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSResetEvent(const XN_EVENT_HANDLE EventHandle)
{
	// Local function variables
	XnBool bRetVal = FALSE;

	// Make sure the actual event handle isn't NULL
	XN_RET_IF_NULL(EventHandle, XN_STATUS_OS_INVALID_EVENT);

	// Reset the event via the OS
	bRetVal = ResetEvent(EventHandle);

	// Make sure it succeeded (return value is true)
	if (bRetVal != TRUE)
	{
		xnLogVerbose(XN_MASK_OS, "ResetEvent() failed with error %u", GetLastError());
		return (XN_STATUS_OS_EVENT_RESET_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWaitEvent(const XN_EVENT_HANDLE EventHandle, XnUInt32 nMilliseconds)
{
	// Local function variables
	DWORD nRetVal = 0;
	
	// Make sure the actual event handle isn't NULL
	XN_RET_IF_NULL(EventHandle, XN_STATUS_OS_INVALID_EVENT);

	// Wait for the event for a period if time (can be infinite)
	nRetVal = WaitForSingleObject(EventHandle, nMilliseconds);
	
	// Check the return value (WAIT_OBJECT_0 is OK)
	if (nRetVal != WAIT_OBJECT_0)
	{
		// Handle the timeout failure
		if (nRetVal == WAIT_TIMEOUT)
		{
			return (XN_STATUS_OS_EVENT_TIMEOUT);
		}
		else
		{
			xnLogVerbose(XN_MASK_OS, "WaitForSingleObject() failed with error %u", GetLastError());
			return (XN_STATUS_OS_EVENT_WAIT_FAILED);
		}
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnBool xnOSIsEventSet(const XN_EVENT_HANDLE EventHandle)
{
	return (xnOSWaitEvent(EventHandle, 0) == XN_STATUS_OK);
}
