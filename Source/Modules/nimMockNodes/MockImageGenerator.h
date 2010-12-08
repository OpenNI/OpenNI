/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/


#ifndef __MOCKIMAGEGENERATOR_H__
#define __MOCKIMAGEGENERATOR_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include "MockMapGenerator.h"

class MockImageGenerator :
	public MockMapGenerator,
	virtual public xn::ModuleImageGenerator
{
public:
	MockImageGenerator(const XnChar* strName);
	virtual ~MockImageGenerator();
	virtual XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue);
	virtual XnStatus SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer);
	virtual XnUInt8* GetImageMap();
	virtual XnBool IsPixelFormatSupported(XnPixelFormat Format);
	virtual XnStatus SetPixelFormat(XnPixelFormat Format);
	virtual XnPixelFormat GetPixelFormat();
	virtual XnStatus RegisterToPixelFormatChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromPixelFormatChange(XnCallbackHandle hCallback);

protected:
	virtual XnUInt32 GetBytesPerPixel();

private:
	PropChangeEvent m_pixelFormatChangeEvent;
	XnPixelFormat m_pixelFormat;
	XnSupportedPixelFormats m_supportedPixelFormats;
};

#endif // __MOCKIMAGEGENERATOR_H__
