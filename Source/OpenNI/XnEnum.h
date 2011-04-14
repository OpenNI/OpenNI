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
#ifndef __XN_ENUM_H__
#define __XN_ENUM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnLog.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnEnumString
{
	XnInt nValue;
	const XnChar* strValue;
} XnEnumString;

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define XN_MASK_ENUMS	"Enums"

#define _XN_ENUM_MAP_NAME(enumType)		_g_ ## enumType ## _strings

#define XN_ENUM_MAP_START(enumType)									\
	static XnEnumString _XN_ENUM_MAP_NAME(enumType)[] = {

#define XN_ENUM_MAP_ENTRY(val, valName)								\
		{ val, valName },

#define XN_ENUM_MAP_END()											\
		{ 0, NULL }													\
	};

#define XN_ENUM_FROM_STRING(nRetVal, enumType, strValue, nValue)						\
	do {																				\
		XnInt _nTemp;																	\
		nRetVal = _xnEnumFromString(XN_STRINGIFY(enumType),								\
			_XN_ENUM_MAP_NAME(enumType), strValue, &_nTemp);							\
		nValue = (enumType)_nTemp;														\
	} while (0)

#define XN_ENUM_TO_STRING(nRetVal, enumType, nValue, strValue)							\
	nRetVal = _xnEnumToString(XN_STRINGIFY(enumType),									\
		_XN_ENUM_MAP_NAME(enumType), (XnInt)nValue, &strValue)

#define XN_ENUM_VALIDATE_FROM_STRING(enumType, strValue, nValue)			\
	do {																	\
			XnStatus _nTempRetVal;											\
			XN_ENUM_FROM_STRING(_nTempRetVal, enumType, strValue, nValue);	\
			XN_IS_STATUS_OK(_nTempRetVal);									\
	} while (0)

#define XN_ENUM_VALIDATE_TO_STRING(enumType, nValue, strValue)				\
	do {																	\
			XnStatus _nTempRetVal;											\
			XN_ENUM_TO_STRING(_nTempRetVal, enumType, nValue, strValue);	\
			XN_IS_STATUS_OK(_nTempRetVal);									\
	} while (0)

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------
inline XnStatus _xnEnumToString(const XnChar* enumType, XnEnumString* pStrings, XnInt nValue, const XnChar** pstrValue)
{
	while (pStrings->strValue != NULL)
	{
		if (pStrings->nValue == nValue)
		{
			*pstrValue = pStrings->strValue;
			return (XN_STATUS_OK);
		}

		pStrings++;
	}

	XN_LOG_WARNING_RETURN(XN_STATUS_NO_MATCH, XN_MASK_ENUMS, "Unknown %s value: %u", enumType, nValue);
}

inline XnStatus _xnEnumFromString(const XnChar* enumType, XnEnumString* pStrings, const XnChar* strValue, XnInt* pnValue)
{
	while (pStrings->strValue != NULL)
	{
		if (strcmp(pStrings->strValue, strValue) == 0)
		{
			*pnValue = pStrings->nValue;
			return (XN_STATUS_OK);
		}

		pStrings++;
	}

	XN_LOG_WARNING_RETURN(XN_STATUS_NO_MATCH, XN_MASK_ENUMS, "Unknown %s value: %s", enumType, strValue);
}

#endif // __XN_ENUM_H__