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

XN_C_API XnStatus xnOSCreateNamedEvent(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bManualReset)
{
	return CreateNamedEventObject(pEventHandle, cpEventName, TRUE, bManualReset);
}

XN_C_API XnStatus xnOSOpenNamedEvent(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName)
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
