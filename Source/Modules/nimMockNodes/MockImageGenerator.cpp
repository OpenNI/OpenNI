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
#include "MockImageGenerator.h"
#include <XnPropNames.h>
#include <XnLog.h>

MockImageGenerator::MockImageGenerator(xn::Context& context, const XnChar* strName) : 
	MockMapGenerator(context, strName),
	m_pixelFormat(XN_PIXEL_FORMAT_RGB24)
{
	xnOSMemSet(&m_supportedPixelFormats, 0, sizeof(m_supportedPixelFormats));
}

MockImageGenerator::~MockImageGenerator()
{
}

XnStatus MockImageGenerator::SetIntProperty(const XnChar* strName, XnUInt64 nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;
	if (strcmp(strName, XN_PROP_PIXEL_FORMAT) == 0)
	{
		nRetVal = SetPixelFormat((XnPixelFormat)nValue);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		return MockMapGenerator::SetIntProperty(strName, nValue);
	}
	
	return XN_STATUS_OK;
}

XnStatus MockImageGenerator::SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer)
{
	if (strcmp(strName, XN_PROP_SUPPORTED_PIXEL_FORMATS) == 0)
	{
		if (nBufferSize != sizeof(m_supportedPixelFormats))
		{
			XN_ASSERT(FALSE);
			XN_LOG_ERROR_RETURN(XN_STATUS_INVALID_BUFFER_SIZE, XN_MASK_OPEN_NI, "Cannot set XN_PROP_SUPPORTED_PIXEL_FORMATS - buffer size is incorrect");
		}
		xnOSMemCopy(&m_supportedPixelFormats, pBuffer, nBufferSize);
	}
	else
	{
		return MockMapGenerator::SetGeneralProperty(strName, nBufferSize, pBuffer);
	}

	return XN_STATUS_OK;
}

XnUInt8* MockImageGenerator::GetImageMap()
{
	return (XnUInt8*)GetData();;
}

XnBool MockImageGenerator::IsPixelFormatSupported(XnPixelFormat format)
{
	switch (format)
	{
		case XN_PIXEL_FORMAT_RGB24:
			return m_supportedPixelFormats.m_bRGB24;
		case XN_PIXEL_FORMAT_YUV422:
			return m_supportedPixelFormats.m_bYUV422;
		case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
			return m_supportedPixelFormats.m_bGrayscale8Bit;
		case XN_PIXEL_FORMAT_GRAYSCALE_16_BIT:
			return m_supportedPixelFormats.m_bGrayscale16Bit;
		case XN_PIXEL_FORMAT_MJPEG:
			return m_supportedPixelFormats.m_bMJPEG;
		default:
			XN_ASSERT(FALSE);
			return FALSE;
	}
}

XnStatus MockImageGenerator::SetPixelFormat(XnPixelFormat format)
{
	XnStatus nRetVal = XN_STATUS_OK;
	if (m_pixelFormat != format)
	{
		m_pixelFormat = format;
		nRetVal = m_pixelFormatChangeEvent.Raise();
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
}

XnPixelFormat MockImageGenerator::GetPixelFormat()
{
	return m_pixelFormat;
}

XnStatus MockImageGenerator::RegisterToPixelFormatChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_pixelFormatChangeEvent.Register(handler, pCookie, hCallback);
}

void MockImageGenerator::UnregisterFromPixelFormatChange(XnCallbackHandle hCallback)
{
	m_pixelFormatChangeEvent.Unregister(hCallback);
}
