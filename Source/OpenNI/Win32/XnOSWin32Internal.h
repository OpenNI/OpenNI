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
#ifndef __XNOSWIN32INTERNAL_H__
#define __XNOSWIN32INTERNAL_H__ 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------
XnStatus XnWin32CreateKernelObjectName(XnChar* strDest, const XnUInt32 nDestLength, const XnChar* strSource, XnBool bAllowOtherUsers);
XnStatus XnWin32GetSecurityAttributes(XnBool bAllowOtherUsers, SECURITY_ATTRIBUTES** ppSecurityAttributes);

#endif // __XNOSWIN32INTERNAL_H__