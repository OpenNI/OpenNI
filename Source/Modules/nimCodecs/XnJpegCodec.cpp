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
#include "XnJpegCodec.h"
#include <XnCodecIDs.h>
#include <XnLog.h>

/***************/
/* XnJpegCodec */
/***************/
XnJpegCodec::XnJpegCodec() :
	m_bRGB(FALSE),
	m_nXRes(0),
	m_nYRes(0),
	m_nQuality(0),
	m_bValid(FALSE),
	m_hOutputModeCallback(NULL),
	m_hCroppingCallback(NULL),
	m_image(NULL),
	m_context(NULL)
{
	m_strNodeName[0] = '\0';
	xnOSMemSet(&m_CompJPEGContext, 0, sizeof(m_CompJPEGContext));
	xnOSMemSet(&m_UncompJPEGContext, 0, sizeof(m_UncompJPEGContext));
}

XnJpegCodec::~XnJpegCodec()
{
	XnStreamFreeCompressImageJ(&m_CompJPEGContext);
	XnStreamFreeUncompressImageJ(&m_UncompJPEGContext);

	// we can assume context still exists, but we'll have to check node still exists
	ImageGenerator imageGen;
	if (XN_STATUS_OK == m_context.GetProductionNodeByName(m_strNodeName, imageGen))
	{
		if (m_hOutputModeCallback)
		{
			imageGen.UnregisterFromMapOutputModeChange(m_hOutputModeCallback);
		}

		if (m_hCroppingCallback)
		{
			imageGen.GetCroppingCap().UnregisterFromCroppingChange(m_hCroppingCallback);
		}
	}
}

XnCodecID XnJpegCodec::GetCodecID() const
{
	return XN_CODEC_JPEG;
}

XnStatus XnJpegCodec::Init(const ProductionNode& node)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnCodec::Init(node);
	XN_IS_STATUS_OK(nRetVal);

	if (node.GetInfo().GetDescription().Type != XN_NODE_TYPE_IMAGE)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_BAD_PARAM, XN_MASK_OPEN_NI, "Codec JPEG requires an image node!");
	}

	strcpy(m_strNodeName, node.GetName());

	ImageGenerator image(node);
	image.GetContext(m_context);

	// register for changes in resolution or cropping
	nRetVal = image.RegisterToMapOutputModeChange(NodeConfigurationChangedCallback, this, m_hOutputModeCallback);
	XN_IS_STATUS_OK(nRetVal);

	if (image.IsCapabilitySupported(XN_CAPABILITY_CROPPING))
	{
		nRetVal = image.GetCroppingCap().RegisterToCroppingChange(NodeConfigurationChangedCallback, this, m_hCroppingCallback);
		XN_IS_STATUS_OK(nRetVal);
	}

	// now init
	nRetVal = XnStreamInitCompressImageJ(&m_CompJPEGContext);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnStreamInitUncompressImageJ(&m_UncompJPEGContext);
	if (nRetVal != XN_STATUS_OK)
	{
		XnStreamFreeCompressImageJ(&m_CompJPEGContext);
		return (nRetVal);
	}

	m_image = image;

	nRetVal = OnNodeConfigurationChanged();
	XN_IS_STATUS_OK(nRetVal);

	m_bValid = TRUE;

	return (XN_STATUS_OK);
}

XnFloat XnJpegCodec::GetWorseCompressionRatio() const
{
	return XN_STREAM_COMPRESSION_IMAGEJ_WORSE_RATIO;
}

XnUInt32 XnJpegCodec::GetOverheadSize() const
{
	return 0;
}

XnStatus XnJpegCodec::CompressImpl(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize) const
{
	if (!m_bValid)
	{
		return XN_STATUS_ERROR;
	}

	if (m_bRGB)
	{
		return XnStreamCompressImage24J(&m_CompJPEGContext, pData, pCompressedData, pnCompressedDataSize, m_nXRes, m_nYRes, m_nQuality);
	}
	else
	{
		return XnStreamCompressImage8J(&m_CompJPEGContext, pData, pCompressedData, pnCompressedDataSize, m_nXRes, m_nYRes, m_nQuality);
	}
}

XnStatus XnJpegCodec::DecompressImpl(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize) const
{
	if (!m_bValid)
	{
		return XN_STATUS_ERROR;
	}

	return XnStreamUncompressImageJ(&m_UncompJPEGContext, pCompressedData, nCompressedDataSize, pData, pnDataSize);
}

XnStatus XnJpegCodec::OnNodeConfigurationChanged()
{
	XnStatus nRetVal = XN_STATUS_OK;

	m_bValid = FALSE;
	
	XnMapOutputMode outputMode;
	nRetVal = m_image.GetMapOutputMode(outputMode);
	XN_IS_STATUS_OK(nRetVal);

	m_nXRes = outputMode.nXRes;
	m_nYRes = outputMode.nYRes;

	if (m_image.IsCapabilitySupported(XN_CAPABILITY_CROPPING))
	{
		XnCropping cropping;
		nRetVal = m_image.GetCroppingCap().GetCropping(cropping);
		XN_IS_STATUS_OK(nRetVal);

		if (cropping.bEnabled)
		{
			m_nXRes = cropping.nXSize;
			m_nYRes = cropping.nYSize;
		}
	}

	XnPixelFormat pixelFormat = m_image.GetPixelFormat();

	switch (pixelFormat)
	{
	case XN_PIXEL_FORMAT_RGB24:
		m_bRGB = TRUE;
		break;
	case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
		m_bRGB = FALSE;
		break;
	case XN_PIXEL_FORMAT_YUV422:
	case XN_PIXEL_FORMAT_GRAYSCALE_16_BIT:
		XN_LOG_ERROR_RETURN(XN_STATUS_ERROR, XN_MASK_OPEN_NI, "Jpeg currently supports only RGB24 and Grayscale8 pixel formats!");
	}

	m_nQuality = XN_STREAM_COMPRESSION_JPEG_DEFAULT_QUALITY;

	m_bValid = TRUE;

	return (XN_STATUS_OK);
}

void XN_CALLBACK_TYPE XnJpegCodec::NodeConfigurationChangedCallback( ProductionNode& node, void* pCookie )
{
	XnJpegCodec* pThis = (XnJpegCodec*)pCookie;
	pThis->OnNodeConfigurationChanged();
}

/*********************/
/* ExportedJpegCodec */
/*********************/
ExportedJpegCodec::ExportedJpegCodec() : ExportedCodec(XN_CODEC_JPEG)
{

}

XnCodec* ExportedJpegCodec::CreateCodec()
{
	return XN_NEW(XnJpegCodec);
}