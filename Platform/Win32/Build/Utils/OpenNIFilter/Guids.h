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
#ifndef __GUIDS_H__
#define __GUIDS_H__

// {07A5CA52-F27A-4c53-ADA6-F8D5BE62FE2B}
DEFINE_GUID(CLSID_OpenNIVideo, 
			0x7a5ca52, 0xf27a, 0x4c53, 0xad, 0xa6, 0xf8, 0xd5, 0xbe, 0x62, 0xfe, 0x2b);

static const wchar_t g_videoName[] = L"OpenNI Video";

// {D3103D50-8E58-4b65-9E3E-4B98431EF174}
DEFINE_GUID(IID_IAdditionalOpenNIControls, 
			0xd3103d50, 0x8e58, 0x4b65, 0x9e, 0x3e, 0x4b, 0x98, 0x43, 0x1e, 0xf1, 0x74);

// {ED0B4C97-69BF-42c7-8BE5-FCC021D7CDEB}
DEFINE_GUID(CLSID_AdditionalOpenNIControlsPropertyPage, 
			0xed0b4c97, 0x69bf, 0x42c7, 0x8b, 0xe5, 0xfc, 0xc0, 0x21, 0xd7, 0xcd, 0xeb);

static const wchar_t g_propertyPageName[] = L"Additional Controls";

#define XN_MASK_FILTER "VideoFilter"

#endif //__GUIDS_H__
