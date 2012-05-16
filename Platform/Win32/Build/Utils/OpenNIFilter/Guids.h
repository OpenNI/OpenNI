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
