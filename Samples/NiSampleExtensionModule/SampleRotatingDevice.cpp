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

