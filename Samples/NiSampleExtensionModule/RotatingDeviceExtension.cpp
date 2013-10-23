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
