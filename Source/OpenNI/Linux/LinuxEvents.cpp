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
#include "LinuxPosixEvents.h"
#include "LinuxPosixNamedEvents.h"
#include "LinuxSysVNamedEvents.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSCreateEvent(XN_EVENT_HANDLE* pEventHandle, XnBool bManualReset)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pEventHandle);

	*pEventHandle = NULL;

	XnLinuxEvent* pEvent = NULL;
	XN_VALIDATE_NEW(pEvent, XnLinuxPosixEvent, bManualReset);

	nRetVal = pEvent->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pEvent);
		return (nRetVal);
	}

	*pEventHandle = pEvent;
	
	// All is good...
	return (XN_STATUS_OK);
}

XnStatus CreateNamedEventObject(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bCreate, XnBool bManualReset)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpEventName);
	XN_VALIDATE_OUTPUT_PTR(pEventHandle);

	*pEventHandle = NULL;

	XnLinuxNamedEvent* pEvent = NULL;

#ifdef XN_PLATFORM_LINUX_NO_SYSV
	XN_VALIDATE_NEW(pEvent, XnLinuxPosixNamedEvent, bManualReset, cpEventName, bCreate);
#else
	XN_VALIDATE_NEW(pEvent, XnLinuxSysVNamedEvent, bManualReset, cpEventName, bCreate);
#endif

	nRetVal = pEvent->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pEvent);
		return (nRetVal);
	}

	*pEventHandle = pEvent;

	return XN_STATUS_OK;
}

XN_C_API XnStatus XN_C_DECL xnOSCreateNamedEvent(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bManualReset)
{
	return xnOSCreateNamedEventEx(pEventHandle, cpEventName, bManualReset, FALSE);
}

XN_C_API XnStatus XN_C_DECL xnOSCreateNamedEventEx(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bManualReset, XnBool bAllowOtherUsers)
{
	return CreateNamedEventObject(pEventHandle, cpEventName, TRUE, bManualReset);
}

XN_C_API XnStatus XN_C_DECL xnOSOpenNamedEvent(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName)
{
	return xnOSOpenNamedEventEx(pEventHandle, cpEventName, FALSE);
}

XN_C_API XnStatus XN_C_DECL xnOSOpenNamedEventEx(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bAllowOtherUsers)
{
	return CreateNamedEventObject(pEventHandle, cpEventName, FALSE, FALSE);
}

XN_C_API XnStatus xnOSCloseEvent(XN_EVENT_HANDLE* pEventHandle)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pEventHandle);

	// Make sure the actual event handle isn't NULL
	XN_RET_IF_NULL(*pEventHandle, XN_STATUS_OS_INVALID_EVENT);

	XnLinuxEvent* pEvent = (XnLinuxEvent*)*pEventHandle;
	nRetVal = pEvent->Destroy();
	XN_IS_STATUS_OK(nRetVal);

	XN_DELETE(pEvent);

	*pEventHandle = NULL;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSetEvent(const XN_EVENT_HANDLE EventHandle)
{
	// Make sure the actual event handle isn't NULL
	XN_RET_IF_NULL(EventHandle, XN_STATUS_OS_INVALID_EVENT);

	XnLinuxEvent* pEvent = (XnLinuxEvent*)EventHandle;
	return pEvent->Set();
}

XN_C_API XnStatus xnOSResetEvent(const XN_EVENT_HANDLE EventHandle)
{
	// Make sure the actual event handle isn't NULL
	XN_RET_IF_NULL(EventHandle, XN_STATUS_OS_INVALID_EVENT);

	XnLinuxEvent* pEvent = (XnLinuxEvent*)EventHandle;
	return pEvent->Reset();
}

XN_C_API XnBool xnOSIsEventSet(const XN_EVENT_HANDLE EventHandle)
{
	return (xnOSWaitEvent(EventHandle, 0) == XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWaitEvent(const XN_EVENT_HANDLE EventHandle, XnUInt32 nMilliseconds)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// Make sure the actual event handle isn't NULL
	XN_RET_IF_NULL(EventHandle, XN_STATUS_OS_INVALID_EVENT);

	XnLinuxEvent* pEvent = (XnLinuxEvent*)EventHandle;
	return pEvent->Wait(nMilliseconds);
}
