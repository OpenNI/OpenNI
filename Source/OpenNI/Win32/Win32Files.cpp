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
#include <cstdlib>
#include <cerrno>
#include <shellapi.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSGetFileList(const XnChar* cpSearchPattern, const XnChar* cpPrefixPath, XnChar cpFileList[][XN_FILE_MAX_PATH], const XnUInt32 nMaxFiles, XnUInt32* pnFoundFiles)
{
	// Local function variables
	WIN32_FIND_DATA FindFileData;
	XN_HANDLE hFind = NULL;
	XnUInt32 nFoundFiles = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSearchPattern);
	XN_VALIDATE_OUTPUT_PTR(cpFileList);
	XN_VALIDATE_OUTPUT_PTR(pnFoundFiles);

	// Reset the number of found files counter
	*pnFoundFiles = 0;

	// Get the first file matching the search pattern
	hFind = FindFirstFile(cpSearchPattern, &FindFileData);

	// Keep looking for files as long as we have enough space in the filelist and as long as we didnt reach the end (represented by Invalid Handle)
	while ((hFind != INVALID_HANDLE_VALUE) && (nFoundFiles < nMaxFiles)) 
    {
		// Copy the file string into its place in the file list
		xnOSStrCopy(cpFileList[nFoundFiles], FindFileData.cFileName, XN_FILE_MAX_PATH);

		if (cpPrefixPath != NULL)
		{
			xnOSStrPrefix(cpPrefixPath, cpFileList[nFoundFiles], XN_FILE_MAX_PATH);
		}

		// Increase the temporary number of found files counter
		nFoundFiles++;

		// Get the next file in the list. If there are no more, FindNextFile returns FALSE and the while loop is aborted
		if (!FindNextFile(hFind, &FindFileData))
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
    }

	// Close the find file list
	FindClose(hFind);

	// Return a file not found error if no files were found...
	if (nFoundFiles == 0)
	{
		return (XN_STATUS_OS_FILE_NOT_FOUND);
	}

	// Write the temporary number of found files counter into the output
	*pnFoundFiles = nFoundFiles;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSOpenFile(const XnChar* cpFileName, const XnUInt32 nFlags, XN_FILE_HANDLE* pFile)
{
	// Local function variables
	XnUInt32 nOSOpenFlags = 0;
	XnUInt32 nOSCreateFlags = OPEN_ALWAYS;
	XnUInt32 nShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	XnUInt32 nAttributes = FILE_ATTRIBUTE_NORMAL;
	XnBool bFileExists = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pFile);

	// Update the OS Create and Open flags according the user request
	if (nFlags & XN_OS_FILE_READ)
	{
		// Add the generic read flags and also specify request that the file must exist
		nOSCreateFlags = OPEN_EXISTING;
		nOSOpenFlags |= GENERIC_READ;
	}
	if (nFlags & XN_OS_FILE_WRITE)
	{
		// Add the generic write flags
		nOSOpenFlags |= GENERIC_WRITE;
	}
	if (nFlags & XN_OS_FILE_CREATE_NEW_ONLY)
	{
		// It's OK to create a new file, but fail if the file already exist
		nOSCreateFlags = CREATE_NEW;
	}
	if (nFlags & XN_OS_FILE_TRUNCATE)
	{
		// If the file exists, we need to truncate it to zero
		nRetVal = xnOSDoesFileExist(cpFileName, &bFileExists);
		XN_IS_STATUS_OK(nRetVal);

		if (bFileExists == TRUE)
		{
			nOSCreateFlags = TRUNCATE_EXISTING;
		}
	}
	if ((nFlags & XN_OS_FILE_WRITE) && (nFlags & XN_OS_FILE_AUTO_FLUSH))
	{
		nAttributes	|= FILE_FLAG_NO_BUFFERING;
		nAttributes	|= FILE_FLAG_WRITE_THROUGH;
	}

	// Open the file via the OS
	*pFile = CreateFile(cpFileName, nOSOpenFlags, nShareMode, NULL, nOSCreateFlags, nAttributes, NULL);

	// handle failure...
	if (*pFile == INVALID_HANDLE_VALUE)
	{
		// Reset the user supplied handle
		*pFile = NULL;

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

	if ((nFlags & XN_OS_FILE_WRITE) && (nFlags & XN_OS_FILE_APPEND))
	{
		nRetVal = xnOSSeekFile64(*pFile, XN_OS_SEEK_END, 0);
		XN_IS_STATUS_OK(nRetVal);
		// ZZZZ Add real error checking?
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseFile(XN_FILE_HANDLE* pFile)
{
	// Local function variables
	XnBool bRetVal = FALSE;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pFile);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(*pFile, XN_STATUS_OS_INVALID_FILE);

	// Close the file handle via the OS
	bRetVal = CloseHandle(*pFile);

	// Make sure it succeeded (return value is true)
	if (bRetVal == TRUE)
	{
		// Reset the user supplied handle
		*pFile = NULL;

		// All is good...
		return (XN_STATUS_OK);
	}
	else
	{
		// Something went wrong while trying to close the file...
		return (XN_STATUS_OS_FILE_CLOSE_FAILED);
	}
}

XN_C_API XnStatus xnOSReadFile(const XN_FILE_HANDLE File, void* pBuffer, XnUInt32* pnBufferSize)
{
	// Note: The buffer will not always be filled to it's requested size. It's up to the caller to decide if this
	//       is a problem or not...

	// Local function variables
	XnBool bRetVal = FALSE;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pBuffer);
	XN_VALIDATE_INPUT_PTR(pnBufferSize);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Read a buffer from a file handle via the OS
	bRetVal	= ReadFile(File, pBuffer, *pnBufferSize, (DWORD*)pnBufferSize, NULL);

	// Make sure it succeeded (return value is true)
	if (bRetVal != TRUE)
	{
		return (XN_STATUS_OS_FILE_READ_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWriteFile(const XN_FILE_HANDLE File, const void* pBuffer, const XnUInt32 nBufferSize)
{
	// Local function variables
	XnBool bRetVal = FALSE;
    XnUInt32 nBytesToWrite = nBufferSize;
	DWORD nBytesWritten = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pBuffer);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Write a buffer to a file handle via the OS
	bRetVal	= WriteFile(File, pBuffer, nBytesToWrite, &nBytesWritten, NULL);

	// Make sure it succeeded (return value is true) and that the correct number of bytes were written
	if ((bRetVal != TRUE) || (nBytesToWrite != nBytesWritten))
	{
		return (XN_STATUS_OS_FILE_WRITE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSeekFile(const XN_FILE_HANDLE File, const XnOSSeekType SeekType, const XnInt32 nOffset)
{
	// Local function variables
	DWORD nRealSeekType = 0;
	DWORD nRetOffset = 0;

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Convert the ni seek type into OS seek type
	switch (SeekType)
	{
		case XN_OS_SEEK_SET:
			// Absolute seek from the file beginning
			nRealSeekType = FILE_BEGIN;
			break;
		case XN_OS_SEEK_CUR:
			// Relative seek from the current location
			nRealSeekType = FILE_CURRENT;
			break;
		case XN_OS_SEEK_END:
			// Absolute seek from the file ending
			nRealSeekType = FILE_END;
			break;
		default:
			return (XN_STATUS_OS_INVALID_SEEK_TYPE);
	}

	// Seek a file handle via the OS
	nRetOffset = SetFilePointer(File, nOffset, NULL, nRealSeekType);

	// Make sure it succeeded (return value is valid) and that we reached the expected file offset
	if (nRetOffset == INVALID_SET_FILE_POINTER)
	{
		return (XN_STATUS_OS_FILE_SEEK_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSeekFile64(const XN_FILE_HANDLE File, const XnOSSeekType SeekType, const XnInt64 nOffset)
{
	// Local function variables
	DWORD nRealSeekType = 0;
	LARGE_INTEGER liPos;
	BOOL bSucceeded = 0;

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Convert the ni seek type into OS seek type
	switch (SeekType)
	{
		case XN_OS_SEEK_SET:
			// Absolute seek from the file beginning
			nRealSeekType = FILE_BEGIN;
			break;
		case XN_OS_SEEK_CUR:
			// Relative seek from the current location
			nRealSeekType = FILE_CURRENT;
			break;
		case XN_OS_SEEK_END:
			// Absolute seek from the file ending
			nRealSeekType = FILE_END;
			break;
		default:
			return (XN_STATUS_OS_INVALID_SEEK_TYPE);
	}

	// Seek a file handle via the OS
	liPos.QuadPart = nOffset;
	bSucceeded = SetFilePointerEx(File, liPos, NULL, nRealSeekType);

	// Make sure it succeeded
	if (! bSucceeded)
	{
		return (XN_STATUS_OS_FILE_SEEK_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSTellFile(const XN_FILE_HANDLE File, XnUInt32* nFilePos)
{
	LARGE_INTEGER liPos;
	BOOL bSucceeded = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(nFilePos);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Seek a file handle by 0 bytes in order to read the file position
	liPos.QuadPart = 0;
	bSucceeded = SetFilePointerEx(File, liPos, &liPos, FILE_CURRENT);

	// Make sure it succeeded (return value is valid)
	if (! bSucceeded)
	{
		return (XN_STATUS_OS_FILE_TELL_FAILED);
	}

	// Enforce uint32 limitation
	if (liPos.HighPart)
	{
		return XN_STATUS_INTERNAL_BUFFER_TOO_SMALL;
	}

	*nFilePos = liPos.LowPart;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSTellFile64(const XN_FILE_HANDLE File, XnUInt64* nFilePos)
{
	LARGE_INTEGER liPos;
	BOOL bSucceeded = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(nFilePos);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Seek a file handle by 0 bytes in order to read the file position
	liPos.QuadPart = 0;
	bSucceeded = SetFilePointerEx(File, liPos, &liPos, FILE_CURRENT);

	// Make sure it succeeded (return value is valid)
	if (! bSucceeded)
	{
		return (XN_STATUS_OS_FILE_TELL_FAILED);
	}

	*nFilePos = liPos.QuadPart;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSFlushFile(const XN_FILE_HANDLE File)
{
	XnBool bRetVal = FALSE;

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	bRetVal = FlushFileBuffers(File);
	if (bRetVal == FALSE)
	{
		return (XN_STATUS_OS_FILE_FLUSH_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSDeleteFile(const XnChar* cpFileName)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);

	if (!DeleteFile(cpFileName))
	{
		return (XN_STATUS_OS_FAILED_TO_DELETE_FILE);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSDoesFileExist(const XnChar* cpFileName, XnBool* bResult)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(bResult);

	// Reset the output result
	*bResult = FALSE;

	// Check if the file exists and update the result accordingly
	if ((_access(cpFileName, 0)) != -1)
	{
		*bResult = TRUE;
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSDoesDirecotyExist(const XnChar* cpDirName, XnBool* pbResult)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpDirName);
	XN_VALIDATE_OUTPUT_PTR(pbResult);

	*pbResult = FALSE;

	DWORD attribs = ::GetFileAttributes(cpDirName);
	if (attribs != INVALID_FILE_ATTRIBUTES &&
		(attribs & FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		*pbResult = TRUE;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetFileSize(const XnChar* cpFileName, XnUInt32* pnFileSize)
{
	// Local function variables
	XN_FILE_HANDLE FileHandle;
	XnStatus nRetVal = XN_STATUS_OK;
	LARGE_INTEGER liSize;
	BOOL bSucceeded = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pnFileSize);

	nRetVal = xnOSOpenFile(cpFileName, XN_OS_FILE_READ, &FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	bSucceeded = GetFileSizeEx(FileHandle, &liSize);
	if (!bSucceeded)
		return XN_STATUS_OS_FILE_GET_SIZE_FAILED;

	// Enforce uint32 limitation
	if (liSize.HighPart)
	{
		return XN_STATUS_INTERNAL_BUFFER_TOO_SMALL;
	}

	*pnFileSize = liSize.LowPart;

	nRetVal = xnOSCloseFile(&FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetFileSize64(const XnChar* cpFileName, XnUInt64* pnFileSize)
{
	// Local function variables
	XN_FILE_HANDLE FileHandle;
	XnStatus nRetVal = XN_STATUS_OK;
	LARGE_INTEGER liSize;
	BOOL bSucceeded = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pnFileSize);

	nRetVal = xnOSOpenFile(cpFileName, XN_OS_FILE_READ, &FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	bSucceeded = GetFileSizeEx(FileHandle, &liSize);
	if (!bSucceeded)
		return XN_STATUS_OS_FILE_GET_SIZE_FAILED;
	*pnFileSize = liSize.QuadPart;

	nRetVal = xnOSCloseFile(&FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateDirectory(const XnChar* cpDirName)
{
	// Local function variables
	XnBool bRetVal = FALSE;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpDirName);

	bRetVal = CreateDirectory(cpDirName, NULL);
	if (bRetVal == FALSE)
	{
		return (XN_STATUS_OS_FAILED_TO_CREATE_DIR);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XN_C_DECL xnOSDeleteEmptyDirectory(const XnChar* strDirName)
{
	XN_VALIDATE_INPUT_PTR(strDirName);
	
	XnBool bRetVal = RemoveDirectory(strDirName);
	if (!bRetVal)
	{
		return XN_STATUS_OS_FAILED_TO_DELETE_DIR;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus XN_C_DECL xnOSDeleteDirectoryTree(const XnChar* strDirName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(strDirName);

	// file name must be double-null terminated
	XnChar strDirNameDoubleNull[MAX_PATH+1];
	xnOSMemSet(strDirNameDoubleNull, 0, sizeof(strDirNameDoubleNull));
	nRetVal = xnOSStrCopy(strDirNameDoubleNull, strDirName, MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	SHFILEOPSTRUCT shOp;
	shOp.hwnd = NULL;
	shOp.wFunc = FO_DELETE;
	shOp.pFrom = strDirNameDoubleNull;
	shOp.pTo = NULL;
	shOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	shOp.fAnyOperationsAborted = 0;
	shOp.hNameMappings = NULL;
	shOp.lpszProgressTitle = NULL;

	int ret = SHFileOperation(&shOp);
	if (ret != 0)
	{
		return XN_STATUS_OS_FAILED_TO_DELETE_DIR;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetCurrentDir(XnChar* cpDirName, const XnUInt32 nBufferSize)
{
	// check needed length
	DWORD nNeededLength = GetCurrentDirectory(NULL, 0);

	if (nNeededLength > nBufferSize)
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;

	// there is enough room, take it
	if (0 == GetCurrentDirectory(nBufferSize, cpDirName))
		return XN_STATUS_ERROR;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSetCurrentDir(const XnChar* cpDirName)
{
	if (0 == SetCurrentDirectory(cpDirName))
	{
		return (XN_STATUS_ERROR);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetDirName(const XnChar* cpFilePath, XnChar* cpDirName, const XnUInt32 nBufferSize)
{
	char strFullPath[XN_FILE_MAX_PATH];
	char* pFileName;
	DWORD res = GetFullPathName(cpFilePath, XN_FILE_MAX_PATH, strFullPath, &pFileName);
	if (res == 0)
	{
		return XN_STATUS_ERROR;
	}

	if (XnUInt32(pFileName - strFullPath) > nBufferSize)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	pFileName[0] = '\0';
	strcpy(cpDirName, strFullPath);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSGetFileName(const XnChar* cpFilePath, XnChar* cpFileName, const XnUInt32 nBufferSize)
{
	XnChar ext[XN_FILE_MAX_PATH];
	XN_VALIDATE_INPUT_PTR(cpFilePath);
	XN_VALIDATE_OUTPUT_PTR(cpFileName);

	errno_t err = _splitpath_s(cpFilePath, NULL, 0, NULL, 0, cpFileName, nBufferSize, ext, sizeof(ext));
	if (err == ERANGE)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}
	else if (err != 0)
	{
		return XN_STATUS_ERROR;
	}

	XnStatus nRetVal = xnOSStrAppend(cpFileName, ext, nBufferSize);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSGetFullPathName(const XnChar* strFilePath, XnChar* strFullPath, XnUInt32 nBufferSize)
{
	char* pFileName;
	DWORD res = GetFullPathName(strFilePath, nBufferSize, strFullPath, &pFileName);
	if (res > nBufferSize)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}
	else if (res == 0)
	{
		return XN_STATUS_ERROR;
	}

	return XN_STATUS_OK;
}
