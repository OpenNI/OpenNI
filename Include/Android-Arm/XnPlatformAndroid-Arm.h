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
#ifndef _XN_PLATFORM_ANDROID_ARM_H_
#define _XN_PLATFORM_ANDROID_ARM_H_

// Start with Linux-x86, and override what's different
#include "../Linux-x86/XnPlatformLinux-x86.h"

//---------------------------------------------------------------------------
// Platform Basic Definition
//---------------------------------------------------------------------------
#undef XN_PLATFORM
#undef XN_PLATFORM_STRING

#define XN_PLATFORM XN_PLATFORM_ANDROID_ARM
#define XN_PLATFORM_STRING "Android-Arm"

#define XN_PLATFORM_HAS_NO_TIMED_OPS
#define XN_PLATFORM_HAS_NO_CLOCK_GETTIME
#define XN_PLATFORM_HAS_NO_SCHED_PARAM
#define XN_PLATFORM_HAS_BUILTIN_SEMUN

#define XN_PLATFORM_LINUX_NO_GLIBC
#define XN_PLATFORM_LINUX_NO_SYSV
#define XN_PLATFORM_LINUX_NO_GLOB
#define XN_PLATFORM_LINUX_NO_POSIX_MEMALIGN
#define XN_PLATFORM_LINUX_NO_SHM
#define XN_PLATFORM_LINUX_NO_PTHREAD_CANCEL

#endif //_XN_PLATFORM_LINUX_ARM_H_
