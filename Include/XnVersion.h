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
#define XN_MAINTENANCE_VERSION 4
/** OpenNI build version. */ 
#define XN_BUILD_VERSION 0

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

