/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
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
#ifndef __MOCKMAPGENERATOR_H__
#define __MOCKMAPGENERATOR_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include "MockGenerator.h"

class MockMapGenerator : 
	public MockGenerator,
	virtual public xn::ModuleMapGenerator,
	virtual public xn::ModuleCroppingInterface
{
public:
	MockMapGenerator(const XnChar* strName);
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

#endif // __MOCKMAPGENERATOR_H__