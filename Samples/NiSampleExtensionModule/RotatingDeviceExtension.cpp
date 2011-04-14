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
#include "RotatingDeviceExtension.h"
#include "SampleRotatingDevice.h"

//---------------------------------------------------------------------------
// Helper Function
//---------------------------------------------------------------------------
static SampleRotatingDevice* GetRotatingDevicePointer(XnNodeHandle hNode)
{
	// take module handle
	xn::ModuleProductionNode* pNode = (xn::ModuleProductionNode*)xnGetModuleNodeHandle(hNode);
	// dynamic cast it to my type
	SampleRotatingDevice* pThis = dynamic_cast<SampleRotatingDevice*>(pNode);
	return pThis;
}

//---------------------------------------------------------------------------
// Exported Data
//---------------------------------------------------------------------------
SAMPLE_DEVICE_API XnProductionNodeType XN_NODE_TYPE_ROTATING_DEVICE;

//---------------------------------------------------------------------------
// Exported C Functions
//---------------------------------------------------------------------------
SAMPLE_DEVICE_API XnStatus xnPlayBeep(XnNodeHandle hNode)
{
	SampleRotatingDevice* pThis = GetRotatingDevicePointer(hNode);
	if (pThis == NULL)
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	return pThis->PlayBeep();
}

SAMPLE_DEVICE_API XnStatus xnSetViewAngle(XnNodeHandle hNode, XnDouble dAngle)
{
	SampleRotatingDevice* pThis = GetRotatingDevicePointer(hNode);
	if (pThis == NULL)
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	return pThis->SetViewAngle(dAngle);
}

SAMPLE_DEVICE_API XnDouble xnGetViewAngle(XnNodeHandle hNode)
{
	SampleRotatingDevice* pThis = GetRotatingDevicePointer(hNode);
	if (pThis == NULL)
	{
		return -1.0;
	}

	return pThis->GetViewAngle();
}

struct ViewAngleChangeCookie
{
	XnStateChangedHandler pUserCallback;
	void* pUserCookie;
	XnNodeHandle hNode;
	XnCallbackHandle hCallback;
};

static void XN_CALLBACK_TYPE ViewAngleChangedCallback(void* pCookie)
{
	ViewAngleChangeCookie* pViewAngleCookie = (ViewAngleChangeCookie*)pCookie;
	pViewAngleCookie->pUserCallback(pViewAngleCookie->hNode, pViewAngleCookie->pUserCookie);
}

SAMPLE_DEVICE_API XnStatus xnRegisterToViewAngleChange(XnNodeHandle hNode, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	XnStatus nRetVal = XN_STATUS_OK;

	SampleRotatingDevice* pThis = GetRotatingDevicePointer(hNode);
	if (pThis == NULL)
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	ViewAngleChangeCookie* pViewAngleCookie = XN_NEW(ViewAngleChangeCookie);
	pViewAngleCookie->pUserCallback = handler;
	pViewAngleCookie->pUserCookie = pCookie;
	pViewAngleCookie->hNode = hNode;
	
	nRetVal = pThis->RegisterToViewAngleChange(ViewAngleChangedCallback, pViewAngleCookie, pViewAngleCookie->hCallback);
	XN_IS_STATUS_OK(nRetVal);

	*phCallback = pViewAngleCookie;
	
	return (XN_STATUS_OK);
}

SAMPLE_DEVICE_API void xnUnregisterFromViewAngleChange(XnNodeHandle hNode, XnCallbackHandle hCallback)
{
	SampleRotatingDevice* pThis = GetRotatingDevicePointer(hNode);
	if (pThis == NULL)
	{
		return;
	}

	ViewAngleChangeCookie* pViewAngleCookie = (ViewAngleChangeCookie*)hCallback;
	pThis->UnregisterFromViewAngleChange(pViewAngleCookie->hCallback);
	XN_DELETE(pViewAngleCookie);
}
