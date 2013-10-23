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
	{																					\
		XnInt _nTemp;																	\
		nRetVal = _xnEnumFromString(XN_STRINGIFY(enumType),								\
			_XN_ENUM_MAP_NAME(enumType), strValue, &_nTemp);							\
		nValue = (enumType)_nTemp;														\
	}

#define XN_ENUM_TO_STRING(nRetVal, enumType, nValue, strValue)							\
	nRetVal = _xnEnumToString(XN_STRINGIFY(enumType),									\
		_XN_ENUM_MAP_NAME(enumType), (XnInt)nValue, &strValue)

#define XN_ENUM_VALIDATE_FROM_STRING(enumType, strValue, nValue)			\
	{																		\
			XnStatus _nTempRetVal;											\
			XN_ENUM_FROM_STRING(_nTempRetVal, enumType, strValue, nValue);	\
			XN_IS_STATUS_OK(_nTempRetVal);									\
	}

#define XN_ENUM_VALIDATE_TO_STRING(enumType, nValue, strValue)				\
	{																		\
			XnStatus _nTempRetVal;											\
			XN_ENUM_TO_STRING(_nTempRetVal, enumType, nValue, strValue);	\
			XN_IS_STATUS_OK(_nTempRetVal);									\
	}

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