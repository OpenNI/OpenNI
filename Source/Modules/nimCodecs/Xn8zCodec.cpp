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
#include "Xn8zCodec.h"
#include "XnStreamCompression.h"
#include <XnCodecIDs.h>

/*************/
/* Xn8zCodec */
/*************/
XnCodecID Xn8zCodec::GetCodecID() const
{
	return XN_CODEC_8Z;
}

XnFloat Xn8zCodec::GetWorseCompressionRatio() const
{
	return XN_STREAM_COMPRESSION_IMAGE8Z_WORSE_RATIO;
}

XnUInt32 Xn8zCodec::GetOverheadSize() const
{
	return 0;
}

XnStatus Xn8zCodec::CompressImpl(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize) const
{
	return XnStreamCompressImage8Z(pData, nDataSize, pCompressedData, pnCompressedDataSize);
}

XnStatus Xn8zCodec::DecompressImpl(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize) const
{
	return XnStreamUncompressImage8Z(pCompressedData, nCompressedDataSize, pData, pnDataSize);
}

/*******************/
/* Exported8zCodec */
/*******************/
Exported8zCodec::Exported8zCodec() : ExportedCodec(XN_CODEC_8Z)
{
}

XnCodec* Exported8zCodec::CreateCodec()
{
	return XN_NEW(Xn8zCodec);
}
