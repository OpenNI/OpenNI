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