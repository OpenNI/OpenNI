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

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
struct _XnEvent
{
	XnBool bOn;
	XnBool bManual;
};

XN_C_API XnStatus xnOSCreateEvent(XN_EVENT_HANDLE* pEventHandle, XnBool bManualReset)
{
	XN_EVENT_HANDLE hEvent;
	XN_VALIDATE_CALLOC(hEvent, _XnEvent, 1);
	hEvent->bManual = bManualReset;
	*pEventHandle = hEvent;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateNamedEvent(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bManualReset)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pEventHandle);

	XN_IMPLEMENT_OS;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseEvent(XN_EVENT_HANDLE* pEventHandle)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pEventHandle);

	xnOSFree(*pEventHandle);
	*pEventHandle = NULL;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSetEvent(const XN_EVENT_HANDLE EventHandle)
{
	XN_VALIDATE_INPUT_PTR(EventHandle);

	EventHandle->bOn = TRUE;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSResetEvent(const XN_EVENT_HANDLE EventHandle)
{
	XN_VALIDATE_INPUT_PTR(EventHandle);

	EventHandle->bOn = FALSE;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWaitEvent(const XN_EVENT_HANDLE EventHandle, XnUInt32 nMilliseconds)
{
	XN_VALIDATE_INPUT_PTR(EventHandle);

	if (!EventHandle->bOn)
	{
		// problem: this is a non-threaded environment, so the event will never be set!
		_brk();
		return XN_STATUS_ERROR;
	}

	if (!EventHandle->bManual)
	{
		// reset
		EventHandle->bOn = FALSE;
	}

	// All is good...
	return (XN_STATUS_OK);
}
