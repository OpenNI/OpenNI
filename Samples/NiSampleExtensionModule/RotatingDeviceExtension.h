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