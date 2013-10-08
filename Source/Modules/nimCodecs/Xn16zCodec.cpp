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
#include "Xn16zCodec.h"
#include "XnStreamCompression.h"
#include <XnCodecIDs.h>

/**************/
/* Xn16zCodec */
/**************/
XnCodecID Xn16zCodec::GetCodecID() const
{
	return XN_CODEC_16Z;
}

XnFloat Xn16zCodec::GetWorseCompressionRatio() const
{
	return XN_STREAM_COMPRESSION_DEPTH16Z_WORSE_RATIO;
}

XnUInt32 Xn16zCodec::GetOverheadSize() const
{
	return 0;
}

XnStatus Xn16zCodec::CompressImpl(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize) const
{
	return XnStreamCompressDepth16Z((XnUInt16*)pData, nDataSize, pCompressedData, pnCompressedDataSize);
}

XnStatus Xn16zCodec::DecompressImpl(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize) const
{
	return XnStreamUncompressDepth16Z(pCompressedData, nCompressedDataSize, (XnUInt16*)pData, pnDataSize);
}

/********************/
/* Exported16zCodec */
/********************/
Exported16zCodec::Exported16zCodec() : ExportedCodec(XN_CODEC_16Z) 
{
}

XnCodec* Exported16zCodec::CreateCodec() 
{ 
	return XN_NEW(Xn16zCodec); 
}

