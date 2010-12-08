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
#include <XnOS.h>
#include <string.h>

#define XN_READ_IXN_BUFFER_SIZE	10*1024

#define XN_IS_SPACE(p)		(*p == ' ' || *p == '\t')
#define XN_IS_NEWLINE(p)	(*p == '\r' || *p == '\n')

#define XN_SKIP_LINE(p)							\
	/* first read till end of line. */			\
	while (*p && *p != '\n' && *p != '\r')		\
		p++;									\
	/* now skip end of line tokens. */			\
	while (*p && (*p == '\n' || *p == '\r'))	\
		p++;

#define XN_READ_TILL(p, endCond, dest, destSize)			\
	{														\
		XN_CHAR* pStart = p;								\
		destSize = 0;										\
		while (*p && !(endCond))							\
		{													\
			if (destSize + 1 > XN_IXN_MAX_LEN)				\
				break;										\
															\
			dest[destSize++] = *p;							\
			p++;											\
		}													\
		dest[destSize] = '\0';								\
	}			
			
//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus FindEntry(const XN_CHAR* cpINIFile, const XN_CHAR* cpSection, const XN_CHAR* cpKey, XN_CHAR* cpDest)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// get file size
	XN_UINT32 nFileSize;
	nRetVal = XnOSGetFileSize(cpINIFile, &nFileSize);
	XN_IS_STATUS_OK(nRetVal);
	
	// read entire file to memory
	XN_CHAR* csFileData = (XN_CHAR*)XnOSMalloc(sizeof(XN_CHAR)*nFileSize + 1);
	XN_VALIDATE_ALLOC_PTR(csFileData);
	
	nRetVal = XnOSLoadFile(cpINIFile, csFileData, nFileSize);
	if (nRetVal != XN_STATUS_OK)
	{
		XnOSFree(csFileData);
		return nRetVal;
	}
	
	// place NULL at the end
	csFileData[nFileSize] = '\0';
	
	// now parse file
	XN_CHAR* pCurPos = csFileData;
	XN_BOOL bIsInRequestedSection = FALSE;
	
	XN_CHAR csTempString[XN_IXN_MAX_LEN];
	XN_UINT32 nTempStringLength = 0;
	
	while (TRUE)
	{
		// ignore spaces
		while (*pCurPos && XN_IS_SPACE(pCurPos))
		{
			pCurPos++;
		}
			
		// check we haven't reached the end
		if (!*pCurPos)
		{
			break;
		}
		
		if (*pCurPos == ';' || *pCurPos == '#') // comment
		{
			XN_SKIP_LINE(pCurPos);
			continue;
		}
		
		if (*pCurPos == '[') // start of section
		{
			pCurPos++;
			XN_READ_TILL(pCurPos, *pCurPos == ']' || XN_IS_NEWLINE(pCurPos), csTempString, nTempStringLength);
			
			if (*pCurPos == ']') // valid section name
			{
				if (bIsInRequestedSection) 
				{
					// we're leaving the requested section, and string wasn't found
					XnOSFree(csFileData);
					return XN_STATUS_OS_IXN_READ_FAILED;
				}
				
				if (strcmp(csTempString, cpSection) == 0)
				{
					bIsInRequestedSection = TRUE;
				}
			}
			
			// in any case, move to the next line
			XN_SKIP_LINE(pCurPos);
			continue;
		} // section
		
		// if we're not in the right section, we don't really care what's written in this line. Just skip it.
		if (!bIsInRequestedSection)
		{
			XN_SKIP_LINE(pCurPos);
			continue;
		}
		
		// regular line. check if this is a key (look for the '=' sign)
		XN_READ_TILL(pCurPos, *pCurPos == '=' || XN_IS_NEWLINE(pCurPos), csTempString, nTempStringLength);
		
		if (*pCurPos == '=') // we found a key
		{
			if (strcmp(csTempString, cpKey) == 0)
			{
				// we found our key. The value is the rest of the line
				pCurPos++;
				XN_READ_TILL(pCurPos, XN_IS_NEWLINE(pCurPos), cpDest, nTempStringLength);
				XnOSFree(csFileData);
				return XN_STATUS_OK;
			}
		}
		
		// if we got here, skip to the next line
		XN_SKIP_LINE(pCurPos);
		
	} // while loop
	
	XnOSFree(csFileData);
	return (XN_STATUS_OS_IXN_READ_FAILED);
}

XN_CORE_API XnStatus XnOSReadStringFromINI(const XN_CHAR* cpINIFile, const XN_CHAR* cpSection, const XN_CHAR* cpKey, XN_CHAR* cpDest, const XN_UINT32 nDestLength)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_BOOL bINIFileExists = FALSE;
	
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(cpDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_IXN_FILE_NOT_FOUND);

	// find value
	XN_CHAR cpValueString[XN_IXN_MAX_LEN];
	nRetVal = FindEntry(cpINIFile, cpSection, cpKey, cpValueString);
	XN_IS_STATUS_OK(nRetVal);
	
	nRetVal = XnOSStrCopy(cpDest, cpValueString, nDestLength);
	XN_IS_STATUS_OK(nRetVal);
	
	return XN_STATUS_OK;
}

XN_CORE_API XnStatus XnOSReadFloatFromINI(const XN_CHAR* cpINIFile, const XN_CHAR* cpSection, const XN_CHAR* cpKey, XN_FLOAT* fDest)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(fDest);
	
	XN_DOUBLE dTemp;
	XnStatus nRetVal = XnOSReadDoubleFromINI(cpINIFile, cpSection, cpKey, &dTemp);
	XN_IS_STATUS_OK(nRetVal);
	
	*fDest = (XN_FLOAT)dTemp;
	
	return XN_STATUS_OK;
}

XN_CORE_API XnStatus XnOSReadDoubleFromINI(const XN_CHAR* cpINIFile, const XN_CHAR* cpSection, const XN_CHAR* cpKey, XN_DOUBLE* fDest)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_BOOL bINIFileExists = FALSE;
	
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(fDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_IXN_FILE_NOT_FOUND);

	// find value
	XN_CHAR cpValueString[XN_IXN_MAX_LEN];
	nRetVal = FindEntry(cpINIFile, cpSection, cpKey, cpValueString);
	XN_IS_STATUS_OK(nRetVal);
	
	*fDest = atof(cpValueString);
	
	return XN_STATUS_OK;
}

XN_CORE_API XnStatus XnOSReadIntFromINI(const XN_CHAR* cpINIFile, const XN_CHAR* cpSection, const XN_CHAR* cpKey, XN_UINT32* nDest)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_BOOL bINIFileExists = FALSE;
	
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(nDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_IXN_FILE_NOT_FOUND);

	// find value
	XN_CHAR cpValueString[XN_IXN_MAX_LEN];
	nRetVal = FindEntry(cpINIFile, cpSection, cpKey, cpValueString);
	XN_IS_STATUS_OK(nRetVal);
	
	*nDest = atoi(cpValueString);
	
	return XN_STATUS_OK;
}

XN_CORE_API XnStatus XnOSWriteStringToINI(const XN_CHAR* cpINIFile, const XN_CHAR* cpSection, const XN_CHAR* cpKey, const XN_CHAR* cpSrc)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_CORE_API XnStatus XnOSWriteFloatToINI(const XN_CHAR* cpINIFile, const XN_CHAR* cpSection, const XN_CHAR* cpKey, const XN_FLOAT fSrc)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_CORE_API XnStatus XnOSWriteDoubleToINI(const XN_CHAR* cpINIFile, const XN_CHAR* cpSection, const XN_CHAR* cpKey, const XN_DOUBLE fSrc)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_CORE_API XnStatus XnOSWriteIntToINI(const XN_CHAR* cpINIFile, const XN_CHAR* cpSection, const XN_CHAR* cpKey, const XN_UINT32 nSrc)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}
