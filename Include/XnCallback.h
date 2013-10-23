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
