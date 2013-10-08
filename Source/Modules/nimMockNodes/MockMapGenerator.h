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
#ifndef __MOCKMAPGENERATOR_H__
#define __MOCKMAPGENERATOR_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include "MockGenerator.h"

XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_INHERITS_VIA_DOMINANCE_WARNING_ID)

class MockMapGenerator : 
	public MockGenerator,
	virtual public xn::ModuleMapGenerator,
	virtual public xn::ModuleCroppingInterface
{
public:
	MockMapGenerator(xn::Context& context, const XnChar* strName);
	virtual ~MockMapGenerator();

	/*ProductionNode*/
	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName);
	virtual XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue);
	virtual XnStatus SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer);

	/*MapGenerator*/
	virtual XnUInt32 GetSupportedMapOutputModesCount();
	virtual XnStatus GetSupportedMapOutputModes(XnMapOutputMode aModes[], XnUInt32& nCount);
	virtual XnStatus SetMapOutputMode(const XnMapOutputMode& mode);
	virtual XnStatus GetMapOutputMode(XnMapOutputMode& mode);
	virtual XnStatus RegisterToMapOutputModeChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback);
	virtual XnUInt32 GetBytesPerPixel();
	virtual xn::ModuleCroppingInterface* GetCroppingInterface();

	/*Cropping*/
	virtual XnStatus SetCropping(const XnCropping &Cropping);
	virtual XnStatus GetCropping(XnCropping &Cropping);
	virtual XnStatus RegisterToCroppingChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromCroppingChange(XnCallbackHandle hCallback);

protected:
	virtual XnUInt32 GetRequiredBufferSize();
	XnUInt32 GetExpectedBufferSize();

	XnMapOutputMode m_mapOutputMode;
	XnCropping m_cropping;
	XnUInt32 m_nBytesPerPixel;

private:
	PropChangeEvent m_outputModeChangeEvent;
	PropChangeEvent m_croppingChangeEvent;
	XnUInt32 m_nSupportedMapOutputModesCount;
	XnBool m_bSupportedMapOutputModesCountReceived;
	XnMapOutputMode* m_pSupportedMapOutputModes;
};

XN_PRAGMA_STOP_DISABLED_WARNING_SECTION

#endif // __MOCKMAPGENERATOR_H__