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
#ifndef __XN_OPEN_NI_STATUS_H__
#define __XN_OPEN_NI_STATUS_H__

#include "XnPlatform.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/** Defines the XnStatus type. 
 *	The high word represents the group to which this error belongs to.
 *	The low word is a sequential number inside the group. */
typedef XnUInt32 XnStatus;

/** Definition of the OK error code. */
#define XN_STATUS_OK ((XnStatus)0)

//---------------------------------------------------------------------------
// API
//---------------------------------------------------------------------------

/**
 * Converts a Xiron Status enumerator into a meaningful error string.
 *
 * @param	Status	[in]	The input Xiron Status to be converted to a string.
 *
 * @return A string representation of the Xiron status.
 */
XN_C_API const XnChar* XN_C_DECL xnGetStatusString(const XnStatus Status);

/**
* Gets the name of a Xiron Status as a string.
*
* @param	Status	[in]	The input Xiron Status.
*
* @return A string representation of the Xiron status name.
*/
XN_C_API const XnChar* XN_C_DECL xnGetStatusName(const XnStatus Status);

/**
* Prints a user message with a description of the error.
*
* @param	Status			[in]	The input Xiron Status.
* @param	csUserMessage	[in]	A user message.
*
*/
XN_C_API void XN_C_DECL xnPrintError(const XnStatus Status, const XnChar* csUserMessage);

//---------------------------------------------------------------------------
// Enums
//---------------------------------------------------------------------------
/** A list of modules for Xiron status. */
typedef enum XnErrorGroup
{
	XN_ERROR_GROUP_NI = 1,
	XN_ERROR_GROUP_OS = 2,
	XN_ERROR_GROUP_PRIMESENSE = 3,
} XnErrorGroup;

/** Constructs a status code from a module and an error code. */
#define XN_STATUS_MAKE(group, code)		((group << 16) | code)

/** Returns the group of the status. */
#define XN_STATUS_GROUP(status)			XnUInt16(status >> 16)

/** Returns the code of the status. */
#define XN_STATUS_CODE(status)			XnUInt16(status & 0x0000FFFF)

/** Marks the beginning of a message map of a specific module. */ 
#define XN_STATUS_MESSAGE_MAP_START_FROM(group, first)		\
	enum _##group##first##Errors							\
	{														\
		group##first##_OK = XN_STATUS_MAKE(group, first),

#define XN_STATUS_MESSAGE_MAP_START(group)					\
	XN_STATUS_MESSAGE_MAP_START_FROM(group, 0)

/** Adds an entry to the message map. */ 
#define XN_STATUS_MESSAGE(csName, csMessage)	\
		csName,

/** Marks the end of a message map. */
#define XN_STATUS_MESSAGE_MAP_END_FROM(group, first)	\
	};

#define XN_STATUS_MESSAGE_MAP_END(group)	\
	XN_STATUS_MESSAGE_MAP_END_FROM(group, 0)

#endif // __XN_OPEN_NITE_STATUS_H__
