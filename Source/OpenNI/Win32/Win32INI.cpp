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
XN_C_API XnStatus xnOSReadStringFromINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, XnChar* cpDest, const XnUInt32 nDestLength)
{
	// Local function variables
	XnUInt32 nReadBytes = 0;
	XnBool bINIFileExists = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(cpDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);
	
	// Read the string from the INI file via the OS
	nReadBytes = GetPrivateProfileString (cpSection, cpKey, NULL, cpDest, nDestLength, cpINIFile);

	// Make sure the value was read properly
	if (nReadBytes == 0)
	{
		return (XN_STATUS_OS_INI_READ_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSReadFloatFromINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, XnFloat* fDest)
{
	// Local function variables
	XnChar cpTempBuffer[XN_INI_MAX_LEN];
	XnUInt32 nReadBytes = 0;
	XnBool bINIFileExists = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(fDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Read the string from the INI file via the OS
	nReadBytes = GetPrivateProfileString (cpSection, cpKey, NULL, cpTempBuffer, XN_INI_MAX_LEN, cpINIFile);

	// Make sure the value was read properly
	if (nReadBytes == 0)
	{
		return (XN_STATUS_OS_INI_READ_FAILED);
	}

	// Convert the string into float
	*fDest = (XnFloat)atof(cpTempBuffer);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSReadDoubleFromINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, XnDouble* fDest)
{
	// Local function variables
	XnChar cpTempBuffer[XN_INI_MAX_LEN];
	XnUInt32 nReadBytes = 0;
	XnBool bINIFileExists = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(fDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Read the string from the INI file via the OS
	nReadBytes = GetPrivateProfileString (cpSection, cpKey, NULL, cpTempBuffer, XN_INI_MAX_LEN, cpINIFile);

	// Make sure the value was read properly
	if (nReadBytes == 0)
	{
		return (XN_STATUS_OS_INI_READ_FAILED);
	}

	// Convert the string into double
	*fDest = atof(cpTempBuffer);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSReadIntFromINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, XnUInt32* nDest)
{
	// Local function variables
	XnChar cpTempBuffer[XN_INI_MAX_LEN];
	XnUInt32 nReadBytes = 0;
	XnBool bINIFileExists = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(nDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Read the string from the INI file via the OS
	nReadBytes = GetPrivateProfileString (cpSection, cpKey, NULL, cpTempBuffer, XN_INI_MAX_LEN, cpINIFile);

	// Make sure the value was read properly
	if (nReadBytes == 0)
	{
		return (XN_STATUS_OS_INI_READ_FAILED);
	}

	// Convert the string into float
	*nDest = atoi(cpTempBuffer);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWriteStringToINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, const XnChar* cpSrc)
{
	// Local function variables
	XnBool bRetVal = FALSE;
	XnBool bINIFileExists = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpSrc);
	XN_VALIDATE_INPUT_PTR(cpINIFile);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Write the string to the INI file via the OS
	bRetVal = WritePrivateProfileString (cpSection, cpKey, cpSrc, cpINIFile);
	
	// Make sure the value was written properly
	if (bRetVal == FALSE)
	{
		return (XN_STATUS_OS_INI_WRITE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWriteFloatToINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, const XnFloat fSrc)
{
	// Local function variables
	XnChar cpTempBuffer[XN_INI_MAX_LEN];
	XnBool bRetVal = FALSE;
	XnBool bINIFileExists = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Convert the float into a string
	sprintf(cpTempBuffer, "%f", fSrc);

	// Write the string to the INI file via the OS
	bRetVal = WritePrivateProfileString (cpSection, cpKey, cpTempBuffer, cpINIFile);

	// Make sure the value was written properly
	if (bRetVal == FALSE)
	{
		return (XN_STATUS_OS_INI_WRITE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWriteDoubleToINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, const XnDouble fSrc)
{
	// Local function variables
	XnChar cpTempBuffer[XN_INI_MAX_LEN];
	XnBool bRetVal = FALSE;
	XnBool bINIFileExists = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Convert the double into a string
	sprintf(cpTempBuffer, "%f", fSrc);

	// Write the string to the INI file via the OS
	bRetVal = WritePrivateProfileString (cpSection, cpKey, cpTempBuffer, cpINIFile);

	// Make sure the value was written properly
	if (bRetVal == FALSE)
	{
		return (XN_STATUS_OS_INI_WRITE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWriteIntToINI(const XnChar* cpINIFile, const XnChar* cpSection, const XnChar* cpKey, const XnUInt32 nSrc)
{
	// Local function variables
	XnChar cpTempBuffer[XN_INI_MAX_LEN];
	XnBool bRetVal = FALSE;
	XnBool bINIFileExists = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Convert the integer into a string
	_itoa(nSrc, cpTempBuffer, 10); 

	// Write the string to the INI file via the OS
	bRetVal = WritePrivateProfileString (cpSection, cpKey, cpTempBuffer, cpINIFile);

	// Make sure the value was written properly
	if (bRetVal == FALSE)
	{
		return (XN_STATUS_OS_INI_WRITE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}
