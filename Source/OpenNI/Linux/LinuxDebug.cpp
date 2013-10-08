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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>

#ifndef XN_PLATFORM_LINUX_NO_GLIBC
#include <execinfo.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XN_C_API XnStatus xnOSGetCurrentCallStack(XnUInt32 nFramesToSkip, XnChar** astrFrames, XnUInt32 nMaxNameLength, XnUInt32* pnFrames)
{
	if (*pnFrames == 0 || nMaxNameLength == 0)
	{
		return XN_STATUS_OK;
	}
	
	void* aFrames[256];
	int nTotalFrames = backtrace(aFrames, *pnFrames + nFramesToSkip);
	
	if (nFramesToSkip >= nTotalFrames)
	{
		*pnFrames = 0;
        return (XN_STATUS_OK);
	}
	
	// now resolve names
    XnUInt32 nResolve = nTotalFrames - nFramesToSkip;
	char** pstrFrames = backtrace_symbols(aFrames + nFramesToSkip, nResolve);
    if (pstrFrames == NULL)
    {
        *pnFrames = 0;
        return (XN_STATUS_OK);
    }
    
    for (XnUInt i = 0; i < nResolve; ++i)
    {
        strncpy(astrFrames[i], pstrFrames[i], nMaxNameLength);
    }
    
    free(pstrFrames);
    
    *pnFrames = nResolve;

	return XN_STATUS_OK;
}

#else

XN_C_API XnStatus xnOSGetCurrentCallStack(XnUInt32 nFramesToSkip, XnChar** astrFrames, XnUInt32 nMaxNameLength, XnUInt32* pnFrames)
{
	*pnFrames = 0;
	return (XN_STATUS_OK);
}

#endif