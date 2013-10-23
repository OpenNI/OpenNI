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
#include "XnUncompressedCodec.h"
#include "XnCodecIDs.h"
#include <XnLog.h>

XnCodecID XnUncompressedCodec::GetCodecID() const
{
	return XN_CODEC_UNCOMPRESSED;
}

XnFloat XnUncompressedCodec::GetWorseCompressionRatio() const
{
	return 1.0;
}

XnUInt32 XnUncompressedCodec::GetOverheadSize() const
{
	return 0;
}

XnStatus XnUncompressedCodec::CompressImpl(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize) const
{
	if (nDataSize > *pnCompressedDataSize)
	{
		xnLogError(XN_MASK_OPEN_NI, "Output buffer overflow");
		return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
	}

	xnOSMemCopy(pCompressedData, pData, nDataSize);
	*pnCompressedDataSize = nDataSize;
	return (XN_STATUS_OK);
}

XnStatus XnUncompressedCodec::DecompressImpl(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize) const
{
	if (nCompressedDataSize > *pnDataSize)
	{
		xnLogError(XN_MASK_OPEN_NI, "Output buffer overflow");
		return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
	}

	xnOSMemCopy(pData, pCompressedData, nCompressedDataSize);
	*pnDataSize = nCompressedDataSize;
	return (XN_STATUS_OK);
}

ExportedUncompressedCodec::ExportedUncompressedCodec() : ExportedCodec(XN_CODEC_UNCOMPRESSED)
{

}

XnCodec* ExportedUncompressedCodec::CreateCodec()
{
	return XN_NEW(XnUncompressedCodec);
}