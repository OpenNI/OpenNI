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
#ifndef __XN_LOG_TYPES_H__
#define __XN_LOG_TYPES_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnPlatform.h"
#include "XnTypes.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_LOG_DIR_NAME			"Log"
#define XN_MASK_LOG				"Log"
#define XN_LOG_MASK_ALL			"ALL"
#define XN_LOG_MAX_MASK_NAME	16

//---------------------------------------------------------------------------
// Enums
//---------------------------------------------------------------------------
typedef enum XnLogSeverity
{
	XN_LOG_VERBOSE = 0,
	XN_LOG_INFO = 1,
	XN_LOG_WARNING = 2,
	XN_LOG_ERROR = 3,
	XN_LOG_SEVERITY_NONE = 10,
} XnLogSeverity;

//---------------------------------------------------------------------------
// Structs
//---------------------------------------------------------------------------
typedef struct XnLogger
{
	volatile XnLogSeverity nMinSeverity;
	void* pInternal;
} XnLogger;

typedef struct XnLogEntry
{
	XnUInt64 nTimestamp;
	XnLogSeverity nSeverity;
	const XnChar* strSeverity;
	const XnChar* strMask;
	const XnChar* strMessage;
	const XnChar* strFile;
	XnUInt32 nLine;
} XnLogEntry;

typedef struct XnLogWriter
{
	void* pCookie;
	void (XN_CALLBACK_TYPE* WriteEntry)(const XnLogEntry* pEntry, void* pCookie);
	void (XN_CALLBACK_TYPE* WriteUnformatted)(const XnChar* strMessage, void* pCookie);
	void (XN_CALLBACK_TYPE* OnConfigurationChanged)(void* pCookie);
	void (XN_CALLBACK_TYPE* OnClosing)(void* pCookie);
} XnLogWriter;

#endif // __XN_LOG_TYPES_H__