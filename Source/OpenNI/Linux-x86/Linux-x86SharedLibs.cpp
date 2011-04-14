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
#include <dlfcn.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSLoadLibrary(const XnChar* cpFileName, XN_LIB_HANDLE* pLibHandle)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pLibHandle);

	// Load the requested shared library via the OS
	*pLibHandle = dlopen(cpFileName, RTLD_NOW);
	
	// Make sure it succeeded (return value is not NULL). If not return an error....
	if (*pLibHandle == NULL)
	{
		xnLogWarning(XN_MASK_OS, "Failed loading lib: %s\n", dlerror());
		return XN_STATUS_OS_CANT_LOAD_LIB;
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSFreeLibrary(const XN_LIB_HANDLE LibHandle)
{
	// Make sure the actual shared library handle isn't NULL
	XN_RET_IF_NULL(LibHandle, XN_STATUS_OS_INVALID_LIBRARY);

	// Free the shared library via the OS and make sure it succeeded
	if (dlclose(LibHandle) != 0)
	{
		return (XN_STATUS_OS_CANT_FREE_LIB);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetProcAddress(const XN_LIB_HANDLE LibHandle, const XnChar* cpProcName, XnFarProc* pProcAddr)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpProcName);
	XN_VALIDATE_OUTPUT_PTR(pProcAddr);

	// Make sure the actual shared library handle isn't NULL
	XN_RET_IF_NULL(LibHandle, XN_STATUS_OS_INVALID_LIBRARY);

	// Get the requested procedure address from the shared library via the OS
	*pProcAddr = (XnFarProc)dlsym(LibHandle, cpProcName);

	// Make sure it succeeded (return value is not NULL). If not return an error....
	XN_VALIDATE_PTR(*pProcAddr, XN_STATUS_OS_PROC_NOT_FOUND);

	// All is good...
	return (XN_STATUS_OK);
}
