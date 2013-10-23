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

#ifndef ANDROID
	// This define enables Large File Support (64bit version of I/O functions and data types).
	// for more information - 'man 7 feature_test_macros'
	// Moreover, it MUST be defined before ANY other include from this file.
	#define _FILE_OFFSET_BITS	64
	
	#define OFF_T off_t
	#define LSEEK lseek
#else
	#define OFF_T off64_t
	#define LSEEK lseek64
#endif

#include <XnOS.h>
#include <libgen.h>
#include <errno.h>
#include <limits.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
#ifndef XN_PLATFORM_LINUX_NO_GLOB
#include <glob.h>

XN_C_API XnStatus xnOSGetFileList(const XnChar* cpSearchPattern, const XnChar* cpPrefixPath, XnChar cpFileList[][XN_FILE_MAX_PATH], const XnUInt32 nMaxFiles, XnUInt32* pnFoundFiles)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSearchPattern);
	XN_VALIDATE_OUTPUT_PTR(cpFileList);
	XN_VALIDATE_OUTPUT_PTR(pnFoundFiles);

	// Reset the number of found files counter
	*pnFoundFiles = 0;

	// now call the OS glob function
	glob_t tGlob;
	glob(cpSearchPattern, 0, NULL, &tGlob);
	
	XnUInt32 nFoundFiles = XN_MIN(tGlob.gl_pathc, nMaxFiles);
	for (XnUInt32 i = 0; i < nFoundFiles; ++i)
	{
		// Copy the file string into its place in the file list
		xnOSStrCopy(cpFileList[i], tGlob.gl_pathv[i], XN_FILE_MAX_PATH);

		if (cpPrefixPath != NULL)
		{
			xnOSStrPrefix(cpPrefixPath, cpFileList[i], XN_FILE_MAX_PATH);
		}
	}
	
	// free memory allocated by OS
	globfree(&tGlob);
	
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
#else

XN_C_API XnStatus xnOSGetFileList(const XnChar* cpSearchPattern, const XnChar* cpPrefixPath, XnChar cpFileList[][XN_FILE_MAX_PATH], const XnUInt32 nMaxFiles, XnUInt32* pnFoundFiles)
{
	XN_ASSERT(FALSE);
	return XN_STATUS_OS_FILE_NOT_FOUND;
}

#endif

XN_C_API XnStatus xnOSOpenFile(const XnChar* cpFileName, const XnUInt32 nFlags, XN_FILE_HANDLE* pFile)
{
	// Local function variables
	XnUInt32 nOSOpenFlags = 0;
	
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pFile);
	
	// Update the OS Open flags according the user request
	if ((nFlags & XN_OS_FILE_READ) && (nFlags & XN_OS_FILE_WRITE))
	{
		nOSOpenFlags |= O_RDWR | O_CREAT;
	}
	else if (nFlags & XN_OS_FILE_READ)
	{
		nOSOpenFlags |= O_RDONLY;
	}
	else if (nFlags & XN_OS_FILE_WRITE)
	{
		nOSOpenFlags |= O_WRONLY | O_CREAT;
	}
	
	if (nFlags & XN_OS_FILE_CREATE_NEW_ONLY)
	{
		// It's OK to create a new file, but fail if the file already exist
		nOSOpenFlags |= O_EXCL;
	}
	
	if (nFlags & XN_OS_FILE_TRUNCATE)
	{
		// If the file exists, we need to truncate it to zero
		nOSOpenFlags |= O_TRUNC;
	}
	
	if ((nFlags & XN_OS_FILE_WRITE) && (nFlags & XN_OS_FILE_AUTO_FLUSH))
	{
		nOSOpenFlags |= O_SYNC;
	}
	
	if (nFlags & XN_OS_FILE_APPEND)
	{
		nOSOpenFlags |= O_APPEND;
	}

	// Open the file via the OS (give read permissions to ALL)
	*pFile = open(cpFileName, nOSOpenFlags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	// handle failure...
	if (*pFile == XN_INVALID_FILE_HANDLE)
	{
		switch (errno)
		{
			case EEXIST:
				return XN_STATUS_OS_FILE_ALREDY_EXISTS;
			case ENOENT:
				return XN_STATUS_OS_FILE_NOT_FOUND;
			default:
				return XN_STATUS_OS_FILE_OPEN_FAILED;
		}
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseFile(XN_FILE_HANDLE* pFile)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pFile);
	
	// make sure this is a valid file descriptor
	if (*pFile == XN_INVALID_FILE_HANDLE)
	{
		return XN_STATUS_OS_INVALID_FILE;
	}
	
	if (0 != close(*pFile))
	{
		// Something went wrong while trying to close the file...
		return (XN_STATUS_OS_FILE_CLOSE_FAILED);
	}
	
	// make the user file descriptor invalid
	*pFile = XN_INVALID_FILE_HANDLE;
	
	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSReadFile(const XN_FILE_HANDLE File, void* pBuffer, XnUInt32* pnBufferSize)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pBuffer);
	XN_VALIDATE_INPUT_PTR(pnBufferSize);

	// Make sure the actual file handle isn't invalid
	if (File == XN_INVALID_FILE_HANDLE)
	{
		return XN_STATUS_OS_INVALID_FILE;
	}

	// Read a buffer from a file handle via the OS
	ssize_t nBytesRead = read(File, pBuffer, *pnBufferSize);
	if (nBytesRead == -1)
	{
		return XN_STATUS_OS_FILE_READ_FAILED;
	}
	
	// update the number of bytes read
	*pnBufferSize = nBytesRead;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWriteFile(const XN_FILE_HANDLE File, const void* pBuffer, const XnUInt32 nBufferSize)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pBuffer);

	// Make sure the actual file handle isn't invalid
	if (File == XN_INVALID_FILE_HANDLE)
	{
		return XN_STATUS_OS_INVALID_FILE;
	}

	// Write a buffer to a file handle via the OS
	ssize_t nBytesWritten = write(File, pBuffer, nBufferSize);
	
	// Make sure it succeeded (return value is not -1) and that the correct number of bytes were written
	if ((nBytesWritten == -1) || (nBufferSize != nBytesWritten))
	{
		return XN_STATUS_OS_FILE_WRITE_FAILED;
	}
	
	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSeekFile(const XN_FILE_HANDLE File, const XnOSSeekType SeekType, const XnInt32 nOffset)
{
	// Local function variables
	int nRealSeekType = 0;
	OFF_T nRetOffset = 0;

	// Make sure the actual file handle isn't invalid
	if (File == XN_INVALID_FILE_HANDLE)
	{
		return XN_STATUS_OS_INVALID_FILE;
	}

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
	nRetOffset = LSEEK(File, (OFF_T)nOffset, nRealSeekType);

	// Make sure it succeeded (return value is valid) and that we reached the expected file offset
	if (nRetOffset == (OFF_T)-1)
	{
		return (XN_STATUS_OS_FILE_SEEK_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSeekFile64(const XN_FILE_HANDLE File, const XnOSSeekType SeekType, const XnInt64 nOffset)
{
	// Local function variables
	int nRealSeekType = 0;
	OFF_T nRetOffset = 0;

	// Make sure the actual file handle isn't invalid
	if (File == XN_INVALID_FILE_HANDLE)
	{
		return XN_STATUS_OS_INVALID_FILE;
	}
	
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
	nRetOffset = LSEEK(File, nOffset, nRealSeekType);

	// Make sure it succeeded (return value is valid) and that we reached the expected file offset
	if (nRetOffset == (OFF_T)-1)
	{
		return (XN_STATUS_OS_FILE_SEEK_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSTellFile(const XN_FILE_HANDLE File, XnUInt32* pnFilePos)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pnFilePos);
	
	// Make sure the actual file handle isn't invalid
	if (File == XN_INVALID_FILE_HANDLE)
	{
		return XN_STATUS_OS_INVALID_FILE;
	}
	
	// Seek a file handle by 0 bytes in order to read the file position
	OFF_T nFilePos = LSEEK(File, 0, SEEK_CUR);

	// Make sure it succeeded (return value is valid)
	if (nFilePos == (OFF_T)-1)
	{
		return (XN_STATUS_OS_FILE_TELL_FAILED);
	}

	// Enforce uint32 limitation
	if ((nFilePos >> 32) != 0)
	{
		return XN_STATUS_INTERNAL_BUFFER_TOO_SMALL;
	}
	
	*pnFilePos = (XnUInt32)nFilePos;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSTellFile64(const XN_FILE_HANDLE File, XnUInt64* pnFilePos)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pnFilePos);
	
	// Make sure the actual file handle isn't invalid
	if (File == XN_INVALID_FILE_HANDLE)
	{
		return XN_STATUS_OS_INVALID_FILE;
	}
	
	// Seek a file handle by 0 bytes in order to read the file position
	OFF_T nFilePos = LSEEK(File, 0, SEEK_CUR);

	// Make sure it succeeded (return value is valid)
	if (nFilePos == (OFF_T)-1)
	{
		return (XN_STATUS_OS_FILE_TELL_FAILED);
	}
	
	*pnFilePos = nFilePos;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSFlushFile(const XN_FILE_HANDLE File)
{
	// Make sure the actual file handle isn't invalid
	if (File == XN_INVALID_FILE_HANDLE)
	{
		return XN_STATUS_OS_INVALID_FILE;
	}
	
	// flush via the OS
	if (-1 == fsync(File))
	{
		return XN_STATUS_OS_FILE_FLUSH_FAILED;
	}
	
	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSFileExists(const XnChar* cpFileName, XnBool* bResult)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(bResult);

	// Reset the output result
	*bResult = FALSE;

	// Check if the file exists and update the result accordingly
	if ((access(cpFileName, F_OK)) != -1)
	{
		*bResult = TRUE;
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetFileSize(const XnChar* cpFileName, XnUInt32* pnFileSize)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pnFileSize);
	
	struct stat fileStat;
	if (-1 == stat(cpFileName, &fileStat))
	{
		return (XN_STATUS_OS_FILE_GET_SIZE_FAILED);	
	}
	
	// Enforce uint32 limitation
	if (fileStat.st_size >> 32)
	{
		return XN_STATUS_INTERNAL_BUFFER_TOO_SMALL;
	}

	*pnFileSize = (XnUInt32)fileStat.st_size;
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetFileSize64(const XnChar* cpFileName, XnUInt64* pnFileSize)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pnFileSize);
	
	struct stat fileStat;
	if (-1 == stat(cpFileName, &fileStat))
	{
		return (XN_STATUS_OS_FILE_GET_SIZE_FAILED);	
	}
	
	*pnFileSize = fileStat.st_size;
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateDirectory(const XnChar* cpDirName)
{
	// Local function variables
	XnInt32 nRetVal = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpDirName);

	nRetVal = mkdir(cpDirName, S_IRWXU | S_IRWXG | S_IRWXO);
	if (nRetVal != 0)
	{
		return (XN_STATUS_OS_FAILED_TO_CREATE_DIR);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetDirName(const XnChar* cpFilePath, XnChar* cpDirName, const XnUInt32 nBufferSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// first copy the string (OS may change the argument)
	XnChar cpInput[XN_FILE_MAX_PATH];
	nRetVal = xnOSStrCopy(cpInput, cpFilePath, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);
	
	// now call the OS
	XnChar* cpResult = dirname(cpInput);
	
	// copy result to user buffer
	nRetVal = xnOSStrCopy(cpDirName, cpResult, nBufferSize);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetFileName(const XnChar* cpFilePath, XnChar* cpDirName, const XnUInt32 nBufferSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// first copy the string (OS may change the argument)
	XnChar cpInput[XN_FILE_MAX_PATH];
	nRetVal = xnOSStrCopy(cpInput, cpFilePath, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);
	
	// now call the OS
	XnChar* cpResult = basename(cpInput);
	
	// copy result to user buffer
	nRetVal = xnOSStrCopy(cpDirName, cpResult, nBufferSize);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetFullPathName(const XnChar* strFilePath, XnChar* strFullPath, XnUInt32 nBufferSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// pass a temp string (with max size)
	XnChar strResult[PATH_MAX];

	if (NULL == realpath(strFilePath, strResult))
	{
		xnLogWarning(XN_MASK_OS, "Failed getting full path name: errno is %d", errno);
		return XN_STATUS_ERROR;
	}

	// now check if we can copy
	if (strlen(strResult) >= nBufferSize)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	// and copy
	strcpy(strFullPath, strResult);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetCurrentDir(XnChar* cpDirName, const XnUInt32 nBufferSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (NULL == getcwd(cpDirName, nBufferSize))
	{
		if (errno == ERANGE)
			return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
		else
			return (XN_STATUS_ERROR);
	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSetCurrentDir(const XnChar* cpDirName)
{
	if (0 != chdir(cpDirName))
	{
		return (XN_STATUS_ERROR);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSDeleteFile(const XnChar* cpFileName)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);

	if (0 != unlink(cpFileName))
	{
		return (XN_STATUS_OS_FAILED_TO_DELETE_FILE);
	}
	
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

	// Check if the file exists and update the result accordingly
	struct stat nodeStat;
	if (stat(cpDirName, &nodeStat) == 0 &&
		S_ISDIR(nodeStat.st_mode))
	{
		*pbResult = TRUE;
	}

	// All is good...
	return (XN_STATUS_OK);
}

