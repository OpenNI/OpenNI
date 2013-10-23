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
