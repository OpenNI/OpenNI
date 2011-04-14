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
