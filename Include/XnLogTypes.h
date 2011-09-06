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
#define XN_LOG_MASKS_STRING_LEN	600
#define XN_MASK_LOG				"Log"
#define XN_LOG_MASK_ALL			"ALL"

//---------------------------------------------------------------------------
// Enums
//---------------------------------------------------------------------------
typedef enum XnLogSeverity
{
	XN_LOG_VERBOSE,
	XN_LOG_INFO,
	XN_LOG_WARNING,
	XN_LOG_ERROR
} XnLogSeverity;

typedef enum XnLogFilteringType
{
	XN_LOG_WRITE_NONE,
	XN_LOG_WRITE_ALL,
	XN_LOG_WRITE_MASKS
} XnLogFilteringType;

//---------------------------------------------------------------------------
// Structs
//---------------------------------------------------------------------------
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