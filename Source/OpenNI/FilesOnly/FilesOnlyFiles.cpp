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
#undef XN_CROSS_PLATFORM
#include <XnOS.h>
#include <unistd.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSGetFileList(const XnChar* cpSearchPattern, const XnChar* cpPrefixPath, XnChar cpFileList[][XN_FILE_MAX_PATH], const XnUInt32 nMaxFiles, XnUInt32* pnFoundFiles)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSearchPattern);
	XN_VALIDATE_OUTPUT_PTR(cpFileList);
	XN_VALIDATE_OUTPUT_PTR(pnFoundFiles);

	XN_IMPLEMENT_OS;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSOpenFile(const XnChar* cpFileName, const XnUInt32 nFlags, XN_FILE_HANDLE* pFile)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pFile);

	XnChar csFlags[10];
	xnOSMemSet(csFlags, 0, 10);
	XnUInt32 nFlagsCount = 0;

	// Update the OS Create and Open flags according the user request
	if (nFlags & XN_OS_FILE_READ)
	{
		csFlags[nFlagsCount++] = 'r';
	}

	if (nFlags & XN_OS_FILE_WRITE)
	{
		if (nFlags & XN_OS_FILE_TRUNCATE)
		{
			csFlags[nFlagsCount++] = 'w';
		}
		else
		{
			csFlags[nFlagsCount++] = 'a';
		}
	}

	// make the file binary
	csFlags[nFlagsCount++] = 'b';

	*pFile = fopen(cpFileName, csFlags);

	// handle failure...
	if (*pFile == NULL)
	{
		if ((nFlags & XN_OS_FILE_WRITE) && (nFlags & XN_OS_FILE_CREATE_NEW_ONLY))
		{
			// If the file was opened for write and a create new only flag was specified, this probably means the file already exist
			return (XN_STATUS_OS_FILE_ALREDY_EXISTS);			
		}
		else if (nFlags & XN_OS_FILE_WRITE)
		{
			// If the file was opened for write but without the create only flag, return a generic file open failure
			return (XN_STATUS_OS_FILE_OPEN_FAILED);
		}
		else if (nFlags & XN_OS_FILE_READ) 
		{
			// If the file was opened for read, this probably means the file doesn't exist
			return (XN_STATUS_OS_FILE_NOT_FOUND);
		}
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseFile(XN_FILE_HANDLE* pFile)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pFile);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(*pFile, XN_STATUS_OS_INVALID_FILE);

	// Close the file handle via the OS
	if (fclose(*pFile) != 0)
	{
		// Something went wrong while trying to close the file...
		return (XN_STATUS_OS_FILE_CLOSE_FAILED);
	}

	*pFile = NULL;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSReadFile(const XN_FILE_HANDLE File, void* pBuffer, XnUInt32* pnBufferSize)
{
	// Note: The buffer will not always be filled to it's requested size. It's up to the caller to decide if this
	//       is a problem or not...

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pBuffer);
	XN_VALIDATE_INPUT_PTR(pnBufferSize);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	*pnBufferSize = fread(pBuffer, 1, *pnBufferSize, File);

	if (*pnBufferSize == 0)
	{
		return (XN_STATUS_OS_FILE_READ_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWriteFile(const XN_FILE_HANDLE File, const void* pBuffer, const XnUInt32 nBufferSize)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pBuffer);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Write a buffer to a file handle via the OS
	XnUInt32 nBytesWritten = fwrite(pBuffer, 1, nBufferSize, File);

	// Make sure it succeeded (return value is true) and that the correct number of bytes were written
	if (nBufferSize != nBytesWritten)
	{
		return (XN_STATUS_OS_FILE_WRITE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSeekFile(const XN_FILE_HANDLE File, const XnOSSeekType SeekType, const XnInt32 nOffset)
{
	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	XnUInt32 nRealSeekType = 0;

	// Convert the Xiron seek type into OS seek type
	switch (SeekType)
	{
		case XN_OS_SEEK_SET:
			// Absolute seek from the file beginning
			nRealSeekType = SEEK_SET;
			break;
		case XN_OS_SEEK_CUR:
			// Relative seek from the current location
			nRealSeekType = SEEK_CUR;
			break;
		case XN_OS_SEEK_END:
			// Absolute seek from the file ending
			nRealSeekType = SEEK_END;
			break;
		default:
			return (XN_STATUS_OS_INVALID_SEEK_TYPE);
	}

	// Seek a file handle via the OS
	if (0 != fseek(File, nOffset, nRealSeekType))
	{
		return (XN_STATUS_OS_FILE_SEEK_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSTellFile(const XN_FILE_HANDLE File, XnUInt32* nFilePos)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(nFilePos);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	*nFilePos = ftell(File);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSFlushFile(const XN_FILE_HANDLE File)
{
	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	if (0 != fflush(File))
	{
		return (XN_STATUS_OS_FILE_FLUSH_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSFileExists(const XnChar* cpFileName, XnBool* bResult)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(bResult);

	// Reset the output result
	*bResult = FALSE;

	// try to open it
	FILE* pFile = fopen(cpFileName, "r");
	if (pFile != NULL)
	{
		fclose(pFile);
		*bResult = TRUE;
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetFileSize(const XnChar* cpFileName, XnUInt32* pnFileSize)
{
	// Local function variables
	XN_FILE_HANDLE FileHandle;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pnFileSize);

	nRetVal = xnOSOpenFile(cpFileName, XN_OS_FILE_READ, &FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSSeekFile(FileHandle, XN_OS_SEEK_END, 0);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSTellFile(FileHandle, pnFileSize);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSCloseFile(&FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateDirectory(const XnChar* cpDirName)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpDirName);

	XN_IMPLEMENT_OS;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetDirName(const XnChar* cpFilePath, XnChar* cpDirName, const XnUInt32 nBufferSize)
{
	XN_IMPLEMENT_OS;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetFileName(const XnChar* cpFilePath, XnChar* cpFileName, const XnUInt32 nBufferSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	const XnChar* pLastPos = strrchr(cpFilePath, '/');
	if (pLastPos == NULL)
		pLastPos = strrchr(cpFilePath, '\\');

	if (pLastPos == NULL)
		pLastPos = cpFilePath;

	return xnOSStrCopy(cpFileName, pLastPos, nBufferSize);
}

XN_C_API XnStatus xnOSGetFullPathName(const XnChar* strFilePath, XnChar* strFullPath, XnUInt32 nBufferSize)
{
	XN_IMPLEMENT_OS;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetCurrentDir(XnChar* cpDirName, const XnUInt32 nBufferSize)
{
	XN_VALIDATE_INPUT_PTR(cpDirName);

	if (0 != _getcwd(cpDirName, nBufferSize))
	{
		return (XN_STATUS_ERROR);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSetCurrentDir(const XnChar* cpDirName)
{
	XN_IMPLEMENT_OS;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSDoesFileExist(const XnChar* cpFileName, XnBool* pbResult)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pbResult);

	// Reset the output result
	*pbResult = FALSE;

	// Check if the file exists and update the result accordingly
	if ((access(cpFileName, F_OK)) != -1)
	{
		*pbResult = TRUE;
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSDoesDirecotyExist(const XnChar* cpDirName, XnBool* pbResult)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpDirName);
	XN_VALIDATE_OUTPUT_PTR(pbResult);

	// Reset the output result
	*pbResult = FALSE;

	XN_IMPLEMENT_OS;

	// All is good...
	return (XN_STATUS_OK);
}
