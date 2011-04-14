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
#include "XnUncompressedCodec.h"
#include "XnCodecIDs.h"

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