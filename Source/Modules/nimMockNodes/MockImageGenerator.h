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
#ifndef __MOCKIMAGEGENERATOR_H__
#define __MOCKIMAGEGENERATOR_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include "MockMapGenerator.h"

XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_INHERITS_VIA_DOMINANCE_WARNING_ID)

class MockImageGenerator :
	public MockMapGenerator,
	virtual public xn::ModuleImageGenerator
{
public:
	MockImageGenerator(xn::Context& context, const XnChar* strName);
	virtual ~MockImageGenerator();
	/*Production Node*/
	virtual XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue);
	virtual XnStatus SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer);

	/*Generator*/
	virtual const void* GetData() { return MockMapGenerator::GetData(); }

	/*Map Generator*/
	virtual XnUInt32 GetBytesPerPixel() { return xn::ModuleImageGenerator::GetBytesPerPixel(); }

	/*Image Generator*/
	virtual XnUInt8* GetImageMap();
	virtual XnBool IsPixelFormatSupported(XnPixelFormat Format);
	virtual XnStatus SetPixelFormat(XnPixelFormat Format);
	virtual XnPixelFormat GetPixelFormat();
	virtual XnStatus RegisterToPixelFormatChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromPixelFormatChange(XnCallbackHandle hCallback);

private:
	PropChangeEvent m_pixelFormatChangeEvent;
	XnPixelFormat m_pixelFormat;
	XnSupportedPixelFormats m_supportedPixelFormats;
};

XN_PRAGMA_STOP_DISABLED_WARNING_SECTION

#endif // __MOCKIMAGEGENERATOR_H__
