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
#ifndef __XN_STATUS_REGISTER_H__
#define __XN_STATUS_REGISTER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnStatus.h"

//---------------------------------------------------------------------------
// Structs
//---------------------------------------------------------------------------
typedef struct XnErrorCodeData
{
	/** The code of this error. */
	XnUInt32 nCode;
	/** The name of this error. */
	const XnChar* csName;
	/** A user message describing this error. */
	const XnChar* csMessage;
} XnErrorCodeData;

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------
/**
* This function registers error codes to the message map.
* 
* @param	nGroup			[in]	The group for which these errors belong to.
* @param	nFirst			[in]	The first value.
* @param	nCount			[in]	The number of status messages to register.
* @param	pErrorCodeData	[in]	An array of status messages.
*/
XN_C_API XnStatus XN_C_DECL xnRegisterErrorCodeMessages(XnUInt16 nGroup, XnUInt16 nFirst, XnUInt16 nCount, XnErrorCodeData* pErrorCodeData);

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#undef XN_STATUS_MESSAGE_MAP_START_FROM
#undef XN_STATUS_MESSAGE
#undef XN_STATUS_MESSAGE_MAP_END_FROM

#define _XN_ERROR_GROUP_NAME(group, first)	_s_##group##first##_errors

#define XN_STATUS_MESSAGE_MAP_START_FROM(group, first)					\
	static XnErrorCodeData _XN_ERROR_GROUP_NAME(group, first)[] =		\
	{																	\
		{ first, XN_STRINGIFY(group) "_OK", XN_STRINGIFY(group) " OK" },

/** Adds an entry to the message map. */ 
#define XN_STATUS_MESSAGE(csName, csMessage)		\
		{ 0, XN_STRINGIFY(csName), csMessage },

/** Marks the end of a message map. */
#define XN_STATUS_MESSAGE_MAP_END_FROM(group, first)	\
	};													\
	static XnStatus s_##group##first##_result = xnRegisterErrorCodeMessages(group, first, sizeof(_XN_ERROR_GROUP_NAME(group, first)) / sizeof(XnErrorCodeData), _XN_ERROR_GROUP_NAME(group, first));

#define XN_STATUS_REGISTER

#endif //__XN_STATUS_REGISTER_H__
