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
#ifndef _XN_PLATFORM_LINUX_ARM_H_
#define _XN_PLATFORM_LINUX_ARM_H_

// Start with Linux-x86, and override what's different
#include "../Linux-x86/XnPlatformLinux-x86.h"

//---------------------------------------------------------------------------
// Platform Basic Definition
//---------------------------------------------------------------------------
#undef XN_PLATFORM
#undef XN_PLATFORM_STRING
#define XN_PLATFORM XN_PLATFORM_LINUX_ARM
#define XN_PLATFORM_STRING "Linux-Arm"

#endif //_XN_PLATFORM_LINUX_ARM_H_

