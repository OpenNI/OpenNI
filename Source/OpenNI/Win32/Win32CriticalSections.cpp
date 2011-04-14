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
XN_C_API XnStatus xnOSCreateCriticalSection(XN_CRITICAL_SECTION_HANDLE* pCriticalSectionHandle)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pCriticalSectionHandle);

	// allocate critical section memory
	XN_VALIDATE_ALLOC(*pCriticalSectionHandle, CRITICAL_SECTION);

	// initialize the critical section object via OS
	InitializeCriticalSection(*pCriticalSectionHandle);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseCriticalSection(XN_CRITICAL_SECTION_HANDLE* pCriticalSectionHandle)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pCriticalSectionHandle);

	// Make sure the actual critical section handle isn't NULL
	XN_RET_IF_NULL(*pCriticalSectionHandle, XN_STATUS_OS_INVALID_CRITICAL_SECTION);

	// Close the critical section via the OS
	DeleteCriticalSection(*pCriticalSectionHandle);

	// release allocated memory
	XN_FREE_AND_NULL(*pCriticalSectionHandle);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSEnterCriticalSection(XN_CRITICAL_SECTION_HANDLE* pCriticalSectionHandle)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pCriticalSectionHandle);

	// Make sure the actual critical section handle isn't NULL
	XN_RET_IF_NULL(*pCriticalSectionHandle, XN_STATUS_OS_INVALID_CRITICAL_SECTION);

	// Enter the critical section via the OS
	EnterCriticalSection(*pCriticalSectionHandle);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSLeaveCriticalSection(XN_CRITICAL_SECTION_HANDLE* pCriticalSectionHandle)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pCriticalSectionHandle);

	// Make sure the actual critical section handle isn't NULL
	XN_RET_IF_NULL(*pCriticalSectionHandle, XN_STATUS_OS_INVALID_CRITICAL_SECTION);

	// Leave the critical section via the OS
	LeaveCriticalSection(*pCriticalSectionHandle);

	// All is good...
	return (XN_STATUS_OK);
}
