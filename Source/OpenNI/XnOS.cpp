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
#include <XnLog.h>

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
static XnBool g_xnOSWasInit = FALSE;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSInit()
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Was the O/S subsystem already initialized?
	if (g_xnOSWasInit == FALSE)
	{
		// Start the global high res timer
		nRetVal = xnOSStartHighResTimer(&g_xnOSHighResGlobalTimer);
		XN_IS_STATUS_OK(nRetVal);

		g_xnOSWasInit = TRUE;
	}
	else
	{
		// Trying to init twice...
		return (XN_STATUS_OS_ALREADY_INIT);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSShutdown()
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Was the O/S subsystem initialized?
	if (g_xnOSWasInit == TRUE)
	{
		// Stop the global high res timer
		nRetVal = xnOSStopTimer(&g_xnOSHighResGlobalTimer);
		XN_IS_STATUS_OK(nRetVal);

		g_xnOSWasInit = FALSE;
	}
	else
	{
		// Trying to shutdown without doing init...
		return (XN_STATUS_OS_NOT_INIT);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSStrFormat(XnChar* cpDestString, const XnUInt32 nDestLength, XnUInt32* pnCharsWritten, const XnChar* cpFormat, ...)
{
	XnStatus nRetVal = XN_STATUS_OK;

	va_list args;
	va_start(args, cpFormat);

	nRetVal = xnOSStrFormatV(cpDestString, nDestLength, pnCharsWritten, cpFormat, args);
	XN_IS_STATUS_OK(nRetVal);

	va_end(args);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWaitAndTerminateThread(XN_THREAD_HANDLE* pThreadHandle, XnUInt32 nMilliseconds)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pThreadHandle);

	// first, wait for thread to exit on its own
	nRetVal = xnOSWaitForThreadExit(*pThreadHandle, nMilliseconds);
	if (nRetVal != XN_STATUS_OK)
	{
		// thread did not exit on its own. Kill it.
		xnLogWarning(XN_MASK_OS, "Thread did not shutdown in %u ms. Thread will be killed...", nMilliseconds);
		xnOSTerminateThread(pThreadHandle);
	}
	else
	{
		// thread exited. Just close the handle.
		xnOSCloseThread(pThreadHandle);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSLoadFile(const XnChar* cpFileName, void* pBuffer, const XnUInt32 nBufferSize)
{
	// Local function variables
	XN_FILE_HANDLE FileHandle;
	XnUInt32 nReadBytes = nBufferSize;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pBuffer);
	
	if (nBufferSize == 0)
	{
		return XN_STATUS_NULL_OUTPUT_PTR;
	}

	nRetVal = xnOSOpenFile(cpFileName, XN_OS_FILE_READ, &FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSReadFile(FileHandle, pBuffer, &nReadBytes);
	if ((nRetVal != XN_STATUS_OK) || (nReadBytes != nBufferSize))
	{
		xnOSCloseFile(&FileHandle);
		return (XN_STATUS_OS_FILE_READ_FAILED);
	}

	nRetVal = xnOSCloseFile(&FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSaveFile(const XnChar* cpFileName, const void* pBuffer, const XnUInt32 nBufferSize)
{
	// Local function variables
	XN_FILE_HANDLE FileHandle;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_INPUT_PTR(pBuffer);

	nRetVal = xnOSOpenFile(cpFileName, XN_OS_FILE_WRITE | XN_OS_FILE_TRUNCATE, &FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSWriteFile(FileHandle, pBuffer, nBufferSize);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSCloseFile(&FileHandle);
		return (XN_STATUS_OS_FILE_WRITE_FAILED);
	}

	nRetVal = xnOSCloseFile(&FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSAppendFile(const XnChar* cpFileName, const void* pBuffer, const XnUInt32 nBufferSize)
{
	// Local function variables
	XN_FILE_HANDLE FileHandle;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_INPUT_PTR(pBuffer);

	nRetVal = xnOSOpenFile(cpFileName, XN_OS_FILE_WRITE | XN_OS_FILE_APPEND, &FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSWriteFile(FileHandle, pBuffer, nBufferSize);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSCloseFile(&FileHandle);
		return (XN_STATUS_OS_FILE_WRITE_FAILED);
	}

	nRetVal = xnOSCloseFile(&FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus xnOSWaitForCondition(const XN_EVENT_HANDLE EventHandle, XnUInt32 nMilliseconds, XnConditionFunc pConditionFunc, void* pConditionData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// take read start time (for timeout purposes)
	XnUInt64 nStarted;
	nRetVal = xnOSGetTimeStamp(&nStarted);
	XN_IS_STATUS_OK(nRetVal);

	XnBool bTimeout = FALSE;

	// as long as condition isn't met
	while (!pConditionFunc(pConditionData))
	{
		// check if timeout occurred
		XnUInt64 nNow;
		nRetVal = xnOSGetTimeStamp(&nNow);
		XN_IS_STATUS_OK(nRetVal);

		if (nNow - nStarted > nMilliseconds)
		{
			bTimeout = TRUE;
		}
		else
		{
			// not yet. Wait for event to be set
			nRetVal = xnOSWaitEvent(EventHandle, (XnUInt32)(nMilliseconds - (nNow - nStarted)));
			if (nRetVal == XN_STATUS_OS_EVENT_TIMEOUT)
			{
				bTimeout = TRUE;
			}
			else if (nRetVal != XN_STATUS_OK)
			{
				xnLogWarning(XN_MASK_OS, "Failed waiting on event for condition...");
			}
		}

		if (bTimeout)
		{
			return (XN_STATUS_OS_EVENT_TIMEOUT);
		}
	}

	// condition was met
	return (XN_STATUS_OK);
}

XN_C_API XnChar* xnOSStrDup(const XnChar* strSource)
{
	XnSizeT nLen = strlen(strSource);
	++nLen;

	XnChar* result = (XnChar*)xnOSMalloc(nLen);
	strcpy(result, strSource);
	return result;
}
