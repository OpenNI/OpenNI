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
