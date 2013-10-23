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
#include "MockDepthGenerator.h"
#include <XnPropNames.h>
#include <XnOS.h>
#include <XnLog.h>

MockDepthGenerator::MockDepthGenerator(xn::Context& context, const XnChar* strName) : 
	MockMapGenerator(context, strName),
	m_nDeviceMaxDepth(0),
	m_nSupportedUserPositionsCount(0),
	m_bSupportedUserPositionsCountReceived(FALSE),
	m_pUserPositions(NULL)
{
	xnOSMemSet(&m_FOV, 0, sizeof(m_FOV));
}

MockDepthGenerator::~MockDepthGenerator()
{
	XN_DELETE_ARR(m_pUserPositions);
}

XnBool MockDepthGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	//TODO: Support user position interface
	return MockMapGenerator::IsCapabilitySupported(strCapabilityName);
}

XnStatus MockDepthGenerator::SetIntProperty(const XnChar* strName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	if (strcmp(strName, XN_PROP_DEVICE_MAX_DEPTH) == 0)
	{
		m_nDeviceMaxDepth = (XnDepthPixel)nValue;
	}
	else if (strcmp(strName, XN_PROP_SUPPORTED_USER_POSITIONS_COUNT) == 0)
	{
		m_nSupportedUserPositionsCount = (XnUInt32)nValue;
		m_bSupportedUserPositionsCountReceived = TRUE;
	}
	else
	{
		nRetVal = MockMapGenerator::SetIntProperty(strName, nValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	return XN_STATUS_OK;
}

XnStatus MockDepthGenerator::SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer)
{
	XN_VALIDATE_INPUT_PTR(strName);
	XN_VALIDATE_INPUT_PTR(pBuffer);

	XnStatus nRetVal = XN_STATUS_OK;
	if (strcmp(strName, XN_PROP_FIELD_OF_VIEW) == 0)
	{
		if (nBufferSize != sizeof(XnFieldOfView))
		{
			XN_LOG_ERROR_RETURN(XN_STATUS_INVALID_BUFFER_SIZE, XN_MASK_OPEN_NI, "Cannot set XN_PROP_FIELD_OF_VIEW - buffer size is incorrect");
		}
		
		const XnFieldOfView* pFOV = (const XnFieldOfView*)pBuffer;
		nRetVal = SetFieldOfView(*pFOV);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (strcmp(strName, XN_PROP_USER_POSITIONS) == 0)
	{
		if (m_bSupportedUserPositionsCountReceived)
		{
			m_bSupportedUserPositionsCountReceived = FALSE;
			if (nBufferSize != m_nSupportedUserPositionsCount * sizeof(XnBoundingBox3D))
			{
				XN_LOG_ERROR_RETURN(XN_STATUS_INVALID_BUFFER_SIZE, XN_MASK_OPEN_NI, "Cannot set XN_PROP_USER_POSITIONS - buffer size is incorrect");
			}

			XN_DELETE_ARR(m_pUserPositions);
			m_pUserPositions = XN_NEW_ARR(XnBoundingBox3D, m_nSupportedUserPositionsCount);
			XN_VALIDATE_ALLOC_PTR(m_pUserPositions);
			xnOSMemCopy(m_pUserPositions, pBuffer, nBufferSize);
		}
		else
		{
			/*We got XN_PROP_USER_POSITIONS without 
			  XN_PROP_SUPPORTED_USER_POSITIONS_COUNT before it - that's an error*/
			XN_ASSERT(FALSE);
			XN_LOG_ERROR_RETURN(XN_STATUS_ERROR, XN_MASK_OPEN_NI, "got XN_PROP_USER_POSITIONS without XN_PROP_SUPPORTED_USER_POSITIONS_COUNT before it.")
		}
	}
	else
	{
		nRetVal = MockMapGenerator::SetGeneralProperty(strName, nBufferSize, pBuffer);
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
}

XnDepthPixel* MockDepthGenerator::GetDepthMap()
{
	return (XnDepthPixel*)GetData();
}

XnDepthPixel MockDepthGenerator::GetDeviceMaxDepth()
{
	return m_nDeviceMaxDepth;
}

void MockDepthGenerator::GetFieldOfView(XnFieldOfView& FOV)
{
	FOV = m_FOV;
}

XnStatus MockDepthGenerator::RegisterToFieldOfViewChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_fieldOfViewChangeEvent.Register(handler, pCookie, hCallback);
}

void MockDepthGenerator::UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback)
{
	m_fieldOfViewChangeEvent.Unregister(hCallback);
}

xn::ModuleUserPositionInterface* MockDepthGenerator::GetUserPositionInterface()
{
	//TODO: Support user position
	return NULL;
}

XnStatus MockDepthGenerator::SetFieldOfView(const XnFieldOfView& FOV)
{
	XnStatus nRetVal = XN_STATUS_OK;
	if (xnOSMemCmp(&m_FOV, &FOV, sizeof(m_FOV)) != 0)
	{
		m_FOV = FOV;
		nRetVal = m_fieldOfViewChangeEvent.Raise();
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
}
