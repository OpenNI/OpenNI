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
#include <XnModuleCppInterface.h>
#include <XnEvent.h>

class SampleRotatingDevice : public xn::ModuleDevice
{
public:
	SampleRotatingDevice();
	~SampleRotatingDevice();

	virtual XnStatus PlayBeep();
	virtual XnStatus SetViewAngle(XnDouble dAngle);
	virtual XnDouble GetViewAngle();
	virtual XnStatus RegisterToViewAngleChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromViewAngleChange(XnCallbackHandle hCallback);

private:
	XN_DECLARE_EVENT_0ARG(ChangeEvent, ChangeEventInterface);

	XnDouble m_dAngle;
	ChangeEvent m_angleEvent;
};
