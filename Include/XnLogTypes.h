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