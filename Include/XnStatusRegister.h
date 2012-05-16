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
