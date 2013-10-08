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
#ifndef __XNINTERNALFUNCS_H__
#define __XNINTERNALFUNCS_H__

#include <XnModuleInterface.h>

//---------------------------------------------------------------------------
// ExtendedSerialization
//---------------------------------------------------------------------------
XnStatus xnNotifyExState(XnNodeHandle hInstance, XnNodeNotifications* pNotifications, void* pCookie);
void xnUnregisterExNotifications(XnNodeHandle hInstance);

//---------------------------------------------------------------------------
// Others
//---------------------------------------------------------------------------
XnOpenNIModuleInterface* GetOpenNIModuleInterface();
void GetOpenNIScriptNodeDescription(XnProductionNodeDescription* pDescription);
XnStatus xnGetOpenNIConfFilesPath(XnChar* strDest, XnUInt32 nBufSize);

#endif // __XNINTERNALFUNCS_H__