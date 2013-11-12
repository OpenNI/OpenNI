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
#ifndef _XN_VERSION_H_
#define _XN_VERSION_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnPlatform.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
/** OpenNI major version. */ 
#define XN_MAJOR_VERSION 1
/** OpenNI minor version. */ 
#define XN_MINOR_VERSION 5
/** OpenNI maintenance version. */ 
#define XN_MAINTENANCE_VERSION 7
/** OpenNI build version. */ 
#define XN_BUILD_VERSION 10

/** OpenNI version (in brief string format): "Major.Minor.Maintenance (Build)" */ 
#define XN_BRIEF_VERSION_STRING \
	XN_STRINGIFY(XN_MAJOR_VERSION) "." \
	XN_STRINGIFY(XN_MINOR_VERSION) "." \
	XN_STRINGIFY(XN_MAINTENANCE_VERSION) \
	" (Build " XN_STRINGIFY(XN_BUILD_VERSION) ")"

/** OpenNI version (in numeric format): (OpenNI major version * 100000000 + OpenNI minor version * 1000000 + OpenNI maintenance version * 10000 + OpenNI build version). */
#define XN_VERSION (XN_MAJOR_VERSION*100000000 + XN_MINOR_VERSION*1000000 + XN_MAINTENANCE_VERSION*10000 + XN_BUILD_VERSION)

/** OpenNI version (in string format): "Major.Minor.Maintenance.Build-Platform (MMM DD YYYY HH:MM:SS)". */ 
#define XN_VERSION_STRING \
		XN_BRIEF_VERSION_STRING  "-" \
		XN_PLATFORM_STRING " (" XN_TIMESTAMP ")"

#endif //_XN_VERSION_H_

