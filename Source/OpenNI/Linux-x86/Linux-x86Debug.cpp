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