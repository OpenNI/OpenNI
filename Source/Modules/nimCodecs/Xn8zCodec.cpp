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
