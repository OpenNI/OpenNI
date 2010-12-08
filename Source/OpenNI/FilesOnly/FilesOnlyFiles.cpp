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
#undef XN_CROSS_PLATFORM
#include <XnOS.h>
#include <unistd.h>
//#include <direct.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_CORE_API XnStatus XnOSGetFileList(const XN_CHAR* cpSearchPattern, const XN_CHAR* cpPrefixPath, XN_CHAR cpFileList[][XN_FILE_MAX_PATH], const XN_UINT32 nMaxFiles, XN_UINT32* pnFoundFiles)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSearchPattern);
	XN_VALIDATE_OUTPUT_PTR(cpFileList);
	XN_VALIDATE_OUTPUT_PTR(pnFoundFiles);

	XN_IMPLEMENT_OS;

	// All is good...
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSOpenFile(const XN_CHAR* cpFileName, const XN_UINT32 nFlags, XN_FILE_HANDLE* pFile)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pFile);

	XN_CHAR csFlags[10];
	XnOSMemSet(csFlags, 0, 10);
	XN_UINT32 nFlagsCount = 0;

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

XN_CORE_API XnStatus XnOSCloseFile(XN_FILE_HANDLE* pFile)
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

XN_CORE_API XnStatus XnOSReadFile(const XN_FILE_HANDLE File, void* pBuffer, XN_UINT32* pnBufferSize)
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

XN_CORE_API XnStatus XnOSWriteFile(const XN_FILE_HANDLE File, const void* pBuffer, const XN_UINT32 nBufferSize)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pBuffer);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Write a buffer to a file handle via the OS
	XN_UINT32 nBytesWritten = fwrite(pBuffer, 1, nBufferSize, File);

	// Make sure it succeeded (return value is true) and that the correct number of bytes were written
	if (nBufferSize != nBytesWritten)
	{
		return (XN_STATUS_OS_FILE_WRITE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSSeekFile(const XN_FILE_HANDLE File, const XnOSSeekType SeekType, const XN_UINT32 nOffset)
{
	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	XN_UINT32 nRealSeekType = 0;

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

XN_CORE_API XnStatus XnOSTellFile(const XN_FILE_HANDLE File, XN_UINT32* nFilePos)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(nFilePos);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	*nFilePos = ftell(File);

	// All is good...
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSFlushFile(const XN_FILE_HANDLE File)
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

XN_CORE_API XnStatus XnOSFileExists(const XN_CHAR* cpFileName, XN_BOOL* bResult)
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

XN_CORE_API XnStatus XnOSGetFileSize(const XN_CHAR* cpFileName, XN_UINT32* pnFileSize)
{
	// Local function variables
	XN_FILE_HANDLE FileHandle;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pnFileSize);

	nRetVal = XnOSOpenFile(cpFileName, XN_OS_FILE_READ, &FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnOSSeekFile(FileHandle, XN_OS_SEEK_END, 0);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnOSTellFile(FileHandle, pnFileSize);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnOSCloseFile(&FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSCreateDirectory(const XN_CHAR* cpDirName)
{
	// Local function variables
	XN_BOOL bRetVal = FALSE;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpDirName);

//	if (0 != _mkdir(cpDirName))
	{
		return (XN_STATUS_OS_FAILED_TO_CREATE_DIR);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSGetCurrentDir(XN_CHAR* cpDirName, const XN_UINT32 nBufferSize)
{
	XN_VALIDATE_INPUT_PTR(cpDirName);

	if (0 != _getcwd(cpDirName, nBufferSize))
	{
		return (XN_STATUS_ERROR);
	}

	return (XN_STATUS_OK);
}
