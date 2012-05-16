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

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSGetCurrentProcessID(XN_PROCESS_ID* pProcID)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pProcID);

	// Get the current thread id
	*pProcID = GetCurrentProcessId();

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateProcess(const XnChar* strExecutable, XnUInt32 nArgs, const XnChar** pstrArgs, XN_PROCESS_ID* pProcID)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	const XnUInt32 nSize = 1024;
	XnChar strArguments[nSize] = "";
	XnUInt32 nDummy = 0;
	xnOSStrFormat(strArguments, nSize, &nDummy, "\"%s\"", strExecutable);

	for (XnUInt i = 0; i < nArgs; ++i)
	{
		XN_VALIDATE_STR_APPEND(strArguments, " \"", nSize, nRetVal);
		XN_VALIDATE_STR_APPEND(strArguments, pstrArgs[i], nSize, nRetVal);
		XN_VALIDATE_STR_APPEND(strArguments, "\"", nSize, nRetVal);
	}

	// let it start in the executable dir
	XnChar strDirName[XN_FILE_MAX_PATH];
	nRetVal = xnOSGetDirName(strExecutable, strDirName, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	if (0 == CreateProcess(strExecutable, strArguments, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, strDirName, &si, &pi))
	{
		xnLogWarning(XN_MASK_OS, "Failed to start process! Win32 error code is %d.", GetLastError());
		return XN_STATUS_OS_PROCESS_CREATION_FAILED;
	}

	*pProcID = pi.dwProcessId;
	
	return (XN_STATUS_OK);
}
