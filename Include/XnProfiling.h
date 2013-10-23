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
#ifndef _XN_PROFILING_H_
#define _XN_PROFILING_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define INVALID_PROFILING_HANDLE	-1

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef XnInt32 XnProfilingHandle;

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------

/**
* Initializes profiling.
* 
* @param	nProfilingInterval	[in]	The number of milliseconds between profiler printings. A value
*										of zero means profiling is off.
*/
XN_C_API XnStatus XN_C_DECL xnProfilingInit(XnUInt32 nProfilingInterval = 0);

/**
* Initializes using INI file.
*
* @param	cpINIFileName	[in]	Name of INI file.
* @param	cpSectionName	[in]	Name of section in INI file.
*/
XN_C_API XnStatus XN_C_DECL xnProfilingInitFromINI(const XnChar* cpINIFileName, const XnChar* cpSectionName);

/**
* Shuts down profiling.
*/
XN_C_API XnStatus XN_C_DECL xnProfilingShutdown();

/**
* Returns TRUE if profiling is active, or FALSE otherwise.
*/
XN_C_API XnBool XN_C_DECL xnProfilingIsActive();

/**
* Start a profiled section. This function is not meant to be used directly. Please use the 
* XN_PROFILING_START_SECTION macro.
*
* @param	csSectionName	[in]		The name of the profiled section.
* @param	bMT				[in]		TRUE if this section is multi-threaded, FALSE otherwise.
* @param	pHandle			[out]		A handle to be used each time this section executes again.
*/
XN_C_API XnStatus XN_C_DECL xnProfilingSectionStart(const char* csSectionName, XnBool bMT, XnProfilingHandle* pHandle);

/**
* Ends a profiled section. This function is not meant to be used directly. Please use the 
* XN_PROFILING_END_SECTION macro.
*
* @param	pHandle		[in]	A handle returned from xnProfilingSectionStart.
*/
XN_C_API XnStatus XN_C_DECL xnProfilingSectionEnd(XnProfilingHandle* pHandle);


/**
* Starts a profiled section. The code section between this declaration
* and the following XN_PROFILING_END_SECTION declaration will be time-measured.
*
* @param	name		[in]	The name of the section (for printing purposes).
* @param	mt			[in]	TRUE if this section is multi-threaded, FALSE otherwise.
*/
#define _XN_PROFILING_START_SECTION(name, mt)									\
	{																			\
		static XnProfilingHandle __profiling = INVALID_PROFILING_HANDLE;		\
		if (xnProfilingIsActive())												\
		{																		\
			xnProfilingSectionStart(name, mt, &__profiling);					\
		}

#define XN_PROFILING_START_SECTION(name) _XN_PROFILING_START_SECTION(name, FALSE)
#define XN_PROFILING_START_MT_SECTION(name) _XN_PROFILING_START_SECTION(name, TRUE)

/**
* Ends a profiled section. 
*/
#define XN_PROFILING_END_SECTION							\
		if (__profiling != INVALID_PROFILING_HANDLE)		\
		{													\
			xnProfilingSectionEnd(&__profiling);			\
		}													\
	}

/**
* Starts a profiled section, giving it the name of the current function. 
* The code section between this declaration and the following XN_PROFILING_END_FUNCTION 
* declaration will be time-measured.
*
* @param	name		[in]	The name of the section (for printing purposes).
*/
#define XN_PROFILING_START_FUNCTION		XN_PROFILING_START_SECTION(__FUNCTION__)

/**
* Ends a profiled function. 
*/
#define XN_PROFILING_END_FUNCTION		XN_PROFILING_END_SECTION

#endif //_XN_PROFILING_H_
