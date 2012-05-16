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