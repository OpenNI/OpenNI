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

