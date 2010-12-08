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


#ifndef __MOCKDEPTHGENERATOR_H__
#define __MOCKDEPTHGENERATOR_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include "MockMapGenerator.h"

class MockDepthGenerator : 
	public MockMapGenerator,
	virtual public xn::ModuleDepthGenerator
{
public:
	MockDepthGenerator(const XnChar* strName);
	virtual ~MockDepthGenerator();

	/*ProductionNode*/
	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName);
	virtual XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue);
	virtual XnStatus SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer);

	/*DepthGenerator*/
	virtual XnDepthPixel* GetDepthMap();
	virtual void GetFieldOfView(XnFieldOfView& FOV);
	virtual XnStatus RegisterToFieldOfViewChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback);
	virtual XnDepthPixel GetDeviceMaxDepth();
	virtual xn::ModuleUserPositionInterface* GetUserPositionInterface();

protected:
	virtual XnUInt32 GetBytesPerPixel();
	XnStatus SetFieldOfView(const XnFieldOfView& FOV);

	PropChangeEvent m_fieldOfViewChangeEvent;

	XnDepthPixel m_nDeviceMaxDepth;
	XnFieldOfView m_FOV;
	XnUInt32 m_nSupportedUserPositionsCount;
	XnBool m_bSupportedUserPositionsCountReceived;
	XnBoundingBox3D* m_pUserPositions;
};

#endif // __MOCKDEPTHGENERATOR_H__