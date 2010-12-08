/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_CORE_API XnStatus XnOSCreateEvent(XN_EVENT_HANDLE* pEventHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSCreateNamedEvent(XN_EVENT_HANDLE* pEventHandle, const XN_CHAR* cpEventName)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pEventHandle);

	XN_IMPLEMENT_OS;

	// All is good...
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSCloseEvent(XN_EVENT_HANDLE* pEventHandle)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pEventHandle);

	XN_IMPLEMENT_OS;

	// All is good...
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSSetEvent(const XN_EVENT_HANDLE EventHandle)
{
	XN_IMPLEMENT_OS;

	// All is good...
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSResetEvent(const XN_EVENT_HANDLE EventHandle)
{
	XN_IMPLEMENT_OS;

	// All is good...
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSWaitEvent(const XN_EVENT_HANDLE EventHandle, XN_UINT32 nMilliseconds)
{
	XN_IMPLEMENT_OS;

	// All is good...
	return (XN_STATUS_OK);
}
