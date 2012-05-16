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
#include "SampleRotatingDevice.h"

//---------------------------------------------------------------------------
// Sample Device implementation
//---------------------------------------------------------------------------
SampleRotatingDevice::SampleRotatingDevice() : m_dAngle(0.0)
{
}

SampleRotatingDevice::~SampleRotatingDevice()
{
}

XnStatus SampleRotatingDevice::PlayBeep()
{
	printf("\a\a\a");
	return XN_STATUS_OK;
}

XnStatus SampleRotatingDevice::SetViewAngle(XnDouble dAngle)
{
	if (dAngle < 0 || dAngle > 3.14)
	{
		return XN_STATUS_BAD_PARAM;
	}

	m_dAngle = dAngle;

	m_angleEvent.Raise();

	return XN_STATUS_OK;
}

XnDouble SampleRotatingDevice::GetViewAngle()
{
	return m_dAngle;
}

XnStatus SampleRotatingDevice::RegisterToViewAngleChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_angleEvent.Register(handler, pCookie, hCallback);
}

void SampleRotatingDevice::UnregisterFromViewAngleChange(XnCallbackHandle hCallback)
{
	m_angleEvent.Unregister(hCallback);
}

