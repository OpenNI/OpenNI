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
//---------------------------------------------------------------------------
// Include
//---------------------------------------------------------------------------
#include <XnTypes.h>

#ifdef ROTATING_DEVICE_EXPORT
	#define SAMPLE_DEVICE_API XN_API_EXPORT
#else
	#define SAMPLE_DEVICE_API XN_API_IMPORT
#endif

//---------------------------------------------------------------------------
// Rotating Device C API
//---------------------------------------------------------------------------
SAMPLE_DEVICE_API XnStatus xnPlayBeep(XnNodeHandle hNode);
SAMPLE_DEVICE_API XnStatus xnSetViewAngle(XnNodeHandle hNode, XnDouble dAngle);
SAMPLE_DEVICE_API XnDouble xnGetViewAngle(XnNodeHandle hNode);
SAMPLE_DEVICE_API XnStatus xnRegisterToViewAngleChange(XnNodeHandle hNode, XnStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback);
SAMPLE_DEVICE_API void xnUnregisterFromViewAngleChange(XnNodeHandle hNode, XnCallbackHandle hCallback);

// Stores the type of the RotatingDevice extension node
extern SAMPLE_DEVICE_API XnProductionNodeType XN_NODE_TYPE_ROTATING_DEVICE;

#ifdef __cplusplus

//---------------------------------------------------------------------------
// Rotating Device C++ Wrapper
//---------------------------------------------------------------------------
#include <XnCppWrapper.h>

namespace Sample
{
	class RotatingDevice : public xn::Device
	{
	public:
		XnStatus Create(xn::Context& context, xn::Query* pQuery = NULL, xn::EnumerationErrors* pErrors = NULL)
		{
			XnNodeHandle hNode;
			XnStatus nRetVal = XN_STATUS_OK;
			nRetVal = xnCreateAnyProductionTree(context.GetUnderlyingObject(), XN_NODE_TYPE_ROTATING_DEVICE, 
				pQuery == NULL ? NULL : pQuery->GetUnderlyingObject(),
				&hNode,
				pErrors == NULL ? NULL : pErrors->GetUnderlying());
			XN_IS_STATUS_OK(nRetVal);
			TakeOwnership(hNode);
			return XN_STATUS_OK;
		}

		XnStatus PlayBeep()
		{
			return xnPlayBeep(GetHandle());
		}

		XnStatus SetViewAngle(XnDouble dAngle)
		{
			return xnSetViewAngle(GetHandle(), dAngle);
		}

		XnDouble GetViewAngle()
		{
			return xnGetViewAngle(GetHandle());
		}

		XnStatus RegisterToViewAngleChange(xn::StateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
		{
			return xn::_RegisterToStateChange(xnRegisterToViewAngleChange, GetHandle(), handler, pCookie, hCallback);
		}

		void UnregisterFromViewAngleChange(XnCallbackHandle hCallback)
		{
			xn::_UnregisterFromStateChange(xnUnregisterFromViewAngleChange, GetHandle(), hCallback);
		}
	};
}

#endif