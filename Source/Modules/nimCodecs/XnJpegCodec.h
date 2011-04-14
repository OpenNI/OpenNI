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
#ifndef __XN_JPEG_CODEC_H__
#define __XN_JPEG_CODEC_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnCodec.h"
#include "XnStreamCompression.h"
#include "ExportedCodec.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnJpegCodec : public XnCodec
{
public:
	XnJpegCodec();
	virtual ~XnJpegCodec();
	virtual XnCodecID GetCodecID() const;
	virtual XnStatus Init(const ProductionNode& node);
	virtual XnFloat GetWorseCompressionRatio() const;
	virtual XnUInt32 GetOverheadSize() const;

protected:
	virtual XnStatus CompressImpl(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize) const;
	virtual XnStatus DecompressImpl(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize) const;

private:
	XnStatus OnNodeConfigurationChanged();
	static void XN_CALLBACK_TYPE NodeConfigurationChangedCallback(ProductionNode& node, void* pCookie);

	Context m_context;
	XnChar m_strNodeName[XN_MAX_NAME_LENGTH];
	ImageGenerator m_image;
	XnBool m_bValid;
	XnBool m_bRGB;
	XnUInt32 m_nXRes;
	XnUInt32 m_nYRes;
	XnUInt32 m_nQuality;
	mutable XnStreamCompJPEGContext m_CompJPEGContext;
	mutable XnStreamUncompJPEGContext m_UncompJPEGContext;
	XnCallbackHandle m_hOutputModeCallback;
	XnCallbackHandle m_hCroppingCallback;
};

class ExportedJpegCodec : public ExportedCodec
{
public:
	ExportedJpegCodec();
	virtual XnCodec* CreateCodec();
};

#endif //__XN_JPEG_CODEC_H__