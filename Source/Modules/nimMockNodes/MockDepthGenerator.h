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
#ifndef __MOCKDEPTHGENERATOR_H__
#define __MOCKDEPTHGENERATOR_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include "MockMapGenerator.h"

XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_INHERITS_VIA_DOMINANCE_WARNING_ID)

class MockDepthGenerator : 
	public MockMapGenerator,
	virtual public xn::ModuleDepthGenerator
{
public:
	MockDepthGenerator(xn::Context& context, const XnChar* strName);
	virtual ~MockDepthGenerator();

	/*ProductionNode*/
	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName);
	virtual XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue);
	virtual XnStatus SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer);

	/*Generator*/
	virtual const void* GetData() { return MockMapGenerator::GetData(); }

	/*MapGenerator*/
	virtual XnUInt32 GetBytesPerPixel() { return xn::ModuleDepthGenerator::GetBytesPerPixel(); }

	/*DepthGenerator*/
	virtual XnDepthPixel* GetDepthMap();
	virtual void GetFieldOfView(XnFieldOfView& FOV);
	virtual XnStatus RegisterToFieldOfViewChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback);
	virtual XnDepthPixel GetDeviceMaxDepth();
	virtual xn::ModuleUserPositionInterface* GetUserPositionInterface();

protected:
	XnStatus SetFieldOfView(const XnFieldOfView& FOV);

	PropChangeEvent m_fieldOfViewChangeEvent;

	XnDepthPixel m_nDeviceMaxDepth;
	XnFieldOfView m_FOV;
	XnUInt32 m_nSupportedUserPositionsCount;
	XnBool m_bSupportedUserPositionsCountReceived;
	XnBoundingBox3D* m_pUserPositions;
};

XN_PRAGMA_STOP_DISABLED_WARNING_SECTION

#endif // __MOCKDEPTHGENERATOR_H__