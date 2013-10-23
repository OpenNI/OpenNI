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
