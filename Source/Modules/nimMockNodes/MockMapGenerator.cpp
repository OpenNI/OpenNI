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
#include "MockMapGenerator.h"
#include <XnPropNames.h>
#include <XnOS.h>
#include <XnLog.h>

MockMapGenerator::MockMapGenerator(xn::Context& context, const XnChar* strName) : 
	MockGenerator(context, strName),
	m_nSupportedMapOutputModesCount(0),
	m_bSupportedMapOutputModesCountReceived(0),
	m_pSupportedMapOutputModes(NULL),
	m_nBytesPerPixel(0)
{
	xnOSMemSet(&m_mapOutputMode, 0, sizeof(m_mapOutputMode));
	xnOSMemSet(&m_cropping, 0, sizeof(m_cropping));
}

MockMapGenerator::~MockMapGenerator()
{
	XN_DELETE_ARR(m_pSupportedMapOutputModes);
}

XnBool MockMapGenerator::IsCapabilitySupported(const XnChar* strCapabilityName)
{
	if (strcmp(strCapabilityName, XN_CAPABILITY_CROPPING) == 0)
		return TRUE;
	
	return MockGenerator::IsCapabilitySupported(strCapabilityName);
}

XnStatus MockMapGenerator::SetIntProperty(const XnChar* strName, XnUInt64 nValue)
{
	if (strcmp(strName, XN_PROP_SUPPORTED_MAP_OUTPUT_MODES_COUNT) == 0)
	{
		m_nSupportedMapOutputModesCount = (XnUInt32)nValue;
		m_bSupportedMapOutputModesCountReceived = TRUE;
	}
	else if (strcmp(strName, XN_PROP_BYTES_PER_PIXEL) == 0)
	{
		m_nBytesPerPixel = (XnUInt32)nValue;
	}
	else
	{
		return MockGenerator::SetIntProperty(strName, nValue);
	}

	return XN_STATUS_OK;
}

XnStatus MockMapGenerator::SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer)
{
	XN_VALIDATE_INPUT_PTR(strName);
	XN_VALIDATE_INPUT_PTR(pBuffer);
	XnStatus nRetVal = XN_STATUS_OK;
	if (strcmp(strName, XN_PROP_MAP_OUTPUT_MODE) == 0)
	{
		if (nBufferSize != sizeof(m_mapOutputMode))
		{
			XN_LOG_ERROR_RETURN(XN_STATUS_INVALID_BUFFER_SIZE, XN_MASK_OPEN_NI, "Cannot set XN_PROP_MAP_OUTPUT_MODE - buffer size is incorrect");
		}
		const XnMapOutputMode* pOutputMode = (const XnMapOutputMode*)pBuffer;
		nRetVal = SetMapOutputMode(*pOutputMode);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (strcmp(strName, XN_PROP_SUPPORTED_MAP_OUTPUT_MODES) == 0)
	{
		if (m_bSupportedMapOutputModesCountReceived)		
		{
			m_bSupportedMapOutputModesCountReceived = FALSE; //For next time
			if (nBufferSize != m_nSupportedMapOutputModesCount * sizeof(XnMapOutputMode))
			{
				XN_LOG_ERROR_RETURN(XN_STATUS_INVALID_BUFFER_SIZE, XN_MASK_OPEN_NI, "Cannot set XN_PROP_SUPPORTED_MAP_OUTPUT_MODES - buffer size is incorrect");
			}

			XN_DELETE_ARR(m_pSupportedMapOutputModes);
			m_pSupportedMapOutputModes = XN_NEW_ARR(XnMapOutputMode, m_nSupportedMapOutputModesCount);
			XN_VALIDATE_ALLOC_PTR(m_pSupportedMapOutputModes);
			xnOSMemCopy(m_pSupportedMapOutputModes, pBuffer, nBufferSize);
		}
		else
		{
			XN_ASSERT(FALSE);
			XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Got XN_PROP_SUPPORTED_MAP_OUTPUT_MODES without XN_PROP_SUPPORTED_MAP_OUTPUT_MODES_COUNT before it");
		}
	}
	else if (strcmp(strName, XN_PROP_CROPPING) == 0)
	{
		if (nBufferSize != sizeof(m_cropping))
		{
			XN_LOG_ERROR_RETURN(XN_STATUS_INVALID_BUFFER_SIZE, XN_MASK_OPEN_NI, "Cannot set XN_PROP_CROPPING - buffer size is incorrect");
		}
		const XnCropping* pCropping = (const XnCropping*)pBuffer;
		nRetVal = SetCropping(*pCropping);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (strcmp(strName, XN_PROP_NEWDATA) == 0)
	{
		// Check buffer size. Note: the expected size is the minimum one. We allow bigger frames (sometimes generators
		// place debug information *after* the data)
		XnUInt32 nExpectedSize = GetExpectedBufferSize();
		if (nBufferSize < nExpectedSize)
		{
			xnLogWarning(XN_MASK_OPEN_NI, "%s: Got new data with illegal buffer size (%u) - ignoring.", m_strName, nBufferSize);
		}
		else
		{
			//Send it to be handled by our base class
			nRetVal = MockGenerator::SetGeneralProperty(strName, nBufferSize, pBuffer);
			XN_IS_STATUS_OK(nRetVal);
		}
	}
	else
	{
		nRetVal = MockGenerator::SetGeneralProperty(strName, nBufferSize, pBuffer);
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
}

XnUInt32 MockMapGenerator::GetSupportedMapOutputModesCount()
{
	return m_nSupportedMapOutputModesCount;
}

XnStatus MockMapGenerator::GetSupportedMapOutputModes(XnMapOutputMode aModes[], XnUInt32& nCount)
{
	XN_VALIDATE_PTR(m_pSupportedMapOutputModes, XN_STATUS_PROPERTY_NOT_SET);
	nCount = XN_MIN(nCount, m_nSupportedMapOutputModesCount);
	xnOSMemCopy(aModes, m_pSupportedMapOutputModes, nCount * sizeof(m_pSupportedMapOutputModes[0]));
	return XN_STATUS_OK;
}

XnStatus MockMapGenerator::SetMapOutputMode(const XnMapOutputMode& mode)
{
	XnStatus nRetVal = XN_STATUS_OK;
	xnLogVerbose(XN_MASK_OPEN_NI, "%s: Setting map output mode to %ux%u, %u fps", m_strName, mode.nXRes, mode.nYRes, mode.nFPS);

	//TODO: Check if mode is in supported modes. If not, return error.

	if (xnOSMemCmp(&mode, &m_mapOutputMode, sizeof(mode)) != 0)
	{
		m_mapOutputMode = mode;
		nRetVal = m_outputModeChangeEvent.Raise();
		XN_IS_STATUS_OK(nRetVal);
	}
	return XN_STATUS_OK;
}

XnStatus MockMapGenerator::GetMapOutputMode(XnMapOutputMode& mode)
{
	mode = m_mapOutputMode;
	return XN_STATUS_OK;
}

XnStatus MockMapGenerator::RegisterToMapOutputModeChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_outputModeChangeEvent.Register(handler, pCookie, hCallback);
}

void MockMapGenerator::UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback)
{
	m_outputModeChangeEvent.Unregister(hCallback);
}

XnUInt32 MockMapGenerator::GetBytesPerPixel()
{
	return m_nBytesPerPixel;
}

xn::ModuleCroppingInterface* MockMapGenerator::GetCroppingInterface()
{
	return this;
}

XnStatus MockMapGenerator::SetCropping(const XnCropping &Cropping)
{
	XnStatus nRetVal = XN_STATUS_OK;
	if (xnOSMemCmp(&Cropping, &m_cropping, sizeof(Cropping)) != 0)
	{
		m_cropping = Cropping;
		nRetVal = m_croppingChangeEvent.Raise();
		XN_IS_STATUS_OK(nRetVal);
	}
	return XN_STATUS_OK;
}

XnStatus MockMapGenerator::GetCropping(XnCropping &Cropping)
{
	Cropping = m_cropping;
	return XN_STATUS_OK;
}

XnStatus MockMapGenerator::RegisterToCroppingChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_croppingChangeEvent.Register(handler, pCookie, hCallback);
}

void MockMapGenerator::UnregisterFromCroppingChange(XnCallbackHandle hCallback)
{
	m_croppingChangeEvent.Unregister(hCallback);
}

XnUInt32 MockMapGenerator::GetRequiredBufferSize()
{
	XnUInt32 nPixels = m_mapOutputMode.nXRes * m_mapOutputMode.nYRes;
	XnUInt32 nBytes = nPixels * GetBytesPerPixel();
	return nBytes;
}

XnUInt32 MockMapGenerator::GetExpectedBufferSize()
{
	XnUInt32 nPixels = 0;

	if (m_cropping.bEnabled)
	{
		nPixels = m_cropping.nXSize * m_cropping.nYSize;
	}
	else
	{
		nPixels = m_mapOutputMode.nXRes * m_mapOutputMode.nYRes;
	}

	XnUInt32 nBytes = nPixels * GetBytesPerPixel();
	return nBytes;
}
