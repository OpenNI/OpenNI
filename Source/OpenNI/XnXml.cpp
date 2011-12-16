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
#include "XnXml.h"
#include <XnOpenNI.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnStatus xnXmlLoadDocument(TiXmlDocument& doc, const XnChar* strFileName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnBool bExists = FALSE;
	nRetVal = xnOSDoesFileExist(strFileName, &bExists);
	XN_IS_STATUS_OK(nRetVal);

	if (!bExists)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_FILE_NOT_FOUND, XN_MASK_OPEN_NI,
			"Failed loading '%s': File does not exist!",
			strFileName);
	}

	if (!doc.LoadFile(strFileName))
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI,
			"Failed loading '%s': %s [row %d, column %d]",
			strFileName, doc.ErrorDesc(), doc.ErrorRow(), doc.ErrorCol());
	}
	
	return (XN_STATUS_OK);
}

XnStatus xnXmlGetChildElement(const TiXmlElement* pElem, const XnChar* strName, const TiXmlElement** ppChild)
{
	*ppChild = pElem->FirstChildElement(strName);
	if (*ppChild == NULL)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI,
			"Invalid '%s' xml entry - no '%s' child (line %u, col %u)!",
			pElem->Value(), strName, pElem->Row(), pElem->Column());
	}
	
	return (XN_STATUS_OK);
}

XnStatus xnXmlReadStringAttribute(const TiXmlElement* pElem, const XnChar* strName, const XnChar** pstrValue)
{
	*pstrValue = pElem->Attribute(strName);
	if (*pstrValue == NULL)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI,
			"Invalid '%s' xml entry - no '%s' attribute (line %u, col %u)!",
			pElem->Value(), strName, pElem->Row(), pElem->Column());
	}
	
	return (XN_STATUS_OK);
}

XnStatus xnXmlReadBoolAttribute(const TiXmlElement* pElem, const XnChar* strName, XnBool* pbValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	const XnChar* strValue;
	nRetVal = xnXmlReadStringAttribute(pElem, strName, &strValue);
	XN_IS_STATUS_OK(nRetVal);

	if (strcmp(strValue, "true") == 0)
	{
		*pbValue = TRUE;
	}
	else if (strcmp(strValue, "false") == 0)
	{
		*pbValue = FALSE;
	}
	else
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, 
			"Invalid '%s' xml entry - '%s' attribute value should be 'true' or 'false' (line %u, col %u)!", 
			pElem->Value(), strName, pElem->Row(), pElem->Column());
	}

	return (XN_STATUS_OK);
}

XnStatus xnXmlReadIntAttribute(const TiXmlElement* pElem, const XnChar* strName, XnInt* pnValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	const XnChar* strValue;
	nRetVal = xnXmlReadStringAttribute(pElem, strName, &strValue);
	XN_IS_STATUS_OK(nRetVal);

	if (sscanf(strValue, "%d", pnValue) == 0)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, 
			"Invalid '%s' xml entry - '%s' attribute value should be a number (line %u, col %u)!", 
			pElem->Value(), strName, pElem->Row(), pElem->Column());
	}
	
	return (XN_STATUS_OK);
}

XnStatus xnXmlReadUInt32Attribute(const TiXmlElement* pElem, const XnChar* strName, XnUInt32* pnValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	const XnChar* strValue;
	nRetVal = xnXmlReadStringAttribute(pElem, strName, &strValue);
	XN_IS_STATUS_OK(nRetVal);

	if (sscanf(strValue, "%u", pnValue) == 0)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, 
			"Invalid '%s' xml entry - '%s' attribute value should be a positive number (line %u, col %u)!", 
			pElem->Value(), strName, pElem->Row(), pElem->Column());
	}

	return (XN_STATUS_OK);
}

XnStatus xnXmlReadUInt16Attribute(const TiXmlElement* pElem, const XnChar* strName, XnUInt16* pnValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnUInt32 nValue;
	nRetVal = xnXmlReadUInt32Attribute(pElem, strName, &nValue);
	XN_IS_STATUS_OK(nRetVal);

	if (nValue > XN_MAX_UINT16)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_BAD_PARAM, XN_MASK_OPEN_NI, 
			"Invalid '%s' xml entry - '%s' attribute value should be unsigned 16-bit number (line %u, col %u)!",
			pElem->Value(), strName, pElem->Row(), pElem->Column());
	}

	*pnValue = (XnUInt16)nValue;
	
	return (XN_STATUS_OK);
}

XnStatus xnXmlReadUInt8Attribute(const TiXmlElement* pElem, const XnChar* strName, XnUInt8* pnValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt32 nValue;
	nRetVal = xnXmlReadUInt32Attribute(pElem, strName, &nValue);
	XN_IS_STATUS_OK(nRetVal);

	if (nValue > XN_MAX_UINT8)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_BAD_PARAM, XN_MASK_OPEN_NI, 
			"Invalid '%s' xml entry - '%s' attribute value should be unsigned 8-bit number (line %u, col %u)!",
			pElem->Value(), strName, pElem->Row(), pElem->Column());
	}

	*pnValue = (XnUInt8)nValue;

	return (XN_STATUS_OK);
}

XnStatus xnXmlReadRealAttribute(const TiXmlElement* pElem, const XnChar* strName, XnDouble* pdValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	const XnChar* strValue;
	nRetVal = xnXmlReadStringAttribute(pElem, strName, &strValue);
	XN_IS_STATUS_OK(nRetVal);

	if (sscanf(strValue, "%lf", pdValue) == 0)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, 
			"Invalid '%s' xml entry - '%s' attribute value should be a floating point (line %u, col %u)!", 
			pElem->Value(), strName, pElem->Row(), pElem->Column());
	}

	return (XN_STATUS_OK);
}

XnStatus xnXmlReadTextAsInt(const TiXmlElement* pElem, XnInt* pnValue)
{
	if (sscanf(pElem->GetText(), "%d", pnValue) == 0)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, 
			"Invalid '%s' xml entry - text should be a number (line %u, col %u)!", 
			pElem->Value(), pElem->Row(), pElem->Column());
	}

	return (XN_STATUS_OK);
}

