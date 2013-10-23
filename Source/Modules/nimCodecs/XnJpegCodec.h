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