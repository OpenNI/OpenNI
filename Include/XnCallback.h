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
#ifndef __XN_CALLBACK_H__
#define __XN_CALLBACK_H__

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/**
* Holds a pair of function pointer and user cookie to be used as callbacks.
*/
struct XnCallback
{
	XnCallback() : pFuncPtr(NULL), pCookie(NULL) {}
	XnCallback(XnFuncPtr pFuncPtr, void* pCookie) : pFuncPtr(pFuncPtr), pCookie(pCookie) {}
	void Set(XnFuncPtr pFuncPtr, void* pCookie) { this->pFuncPtr = pFuncPtr; this->pCookie = pCookie; }

	XnFuncPtr pFuncPtr;
	void* pCookie;
};

#endif //__XN_CALLBACK_H__
