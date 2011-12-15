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
#ifndef __XNOSSTRINGS_H__
#define __XNOSSTRINGS_H__

#include "XnPlatform.h"
#include <stdarg.h>

// Strings
XN_C_API XnStatus XN_C_DECL xnOSStrPrefix(const XnChar* cpPrefixString, XnChar* cpDestString, const XnUInt32 nDestLength);
XN_C_API XnStatus XN_C_DECL xnOSStrAppend(XnChar* cpDestString, const XnChar* cpSrcString, const XnUInt32 nDestLength);
XN_C_API XnStatus XN_C_DECL xnOSStrCopy(XnChar* cpDestString, const XnChar* cpSrcString, const XnUInt32 nDestLength);
XN_C_API XnUInt32 XN_C_DECL xnOSStrLen(const XnChar* cpStr);
XN_C_API XnStatus XN_C_DECL xnOSStrNCopy(XnChar* cpDestString, const XnChar* cpSrcString, const XnUInt32 nCopyLength, const XnUInt32 nDestLength);
XN_C_API XnStatus XN_C_DECL xnOSStrCRC32(const XnChar* cpString, XnUInt32* nCRC32);
XN_C_API XnStatus XN_C_DECL xnOSStrNCRC32(XnUChar* cpBuffer, XnUInt32 nBufferSize, XnUInt32* nCRC32);
XN_C_API XnStatus XN_C_DECL xnOSStrFormat(XnChar* cpDestString, const XnUInt32 nDestLength, XnUInt32* pnCharsWritten, const XnChar* cpFormat, ...);
XN_C_API XnStatus XN_C_DECL xnOSStrFormatV(XnChar* cpDestString, const XnUInt32 nDestLength, XnUInt32* pnCharsWritten, const XnChar* cpFormat, va_list args);
XN_C_API XnInt32  XN_C_DECL xnOSStrCmp(const XnChar* cpFirstString, const XnChar* cpSecondString);
XN_C_API XnInt32  XN_C_DECL xnOSStrCaseCmp(const XnChar* cpFirstString, const XnChar* cpSecondString);
XN_C_API void     XN_C_DECL xnOSItoA(XnInt32 nValue, XnChar* cpStr, XnInt32 nBase);
/** Should be freed using @ref xnOSFree() */
XN_C_API XnChar* XN_C_DECL xnOSStrDup(const XnChar* strSource);
XN_C_API XnStatus XN_C_DECL xnOSGetEnvironmentVariable(const XnChar* strEnv, XnChar* strDest, XnUInt32 nDestSize);
XN_C_API XnStatus XN_C_DECL xnOSExpandEnvironmentStrings(const XnChar* strSrc, XnChar* strDest, XnUInt32 nDestSize);


#endif // __XNOSSTRINGS_H__