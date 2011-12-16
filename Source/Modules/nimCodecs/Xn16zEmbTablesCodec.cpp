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
#include "Xn16zEmbTablesCodec.h"
#include "XnStreamCompression.h"
#include <XnCodecIDs.h>
#include <XnLog.h>

/***********************/
/* Xn16zEmbTablesCodec */
/***********************/
Xn16zEmbTablesCodec::Xn16zEmbTablesCodec() : m_nMaxValue(0)
{
}

XnCodecID Xn16zEmbTablesCodec::GetCodecID() const 
{
	return XN_CODEC_16Z_EMB_TABLES;
}

XnStatus Xn16zEmbTablesCodec::Init(const ProductionNode& node)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = XnCodec::Init(node);
	XN_IS_STATUS_OK_LOG_ERROR("Init codec", nRetVal);
	
	if (node.GetInfo().GetDescription().Type != XN_NODE_TYPE_DEPTH)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_BAD_PARAM, XN_MASK_OPEN_NI, "Codec 16z with embedded tables requires a depth node!");
	}

	DepthGenerator depth(node);
	m_nMaxValue = depth.GetDeviceMaxDepth();

	return (XN_STATUS_OK);
}

XnFloat Xn16zEmbTablesCodec::GetWorseCompressionRatio() const 
{ 
	return XN_STREAM_COMPRESSION_DEPTH16Z_WORSE_RATIO; 
}

XnUInt32 Xn16zEmbTablesCodec::GetOverheadSize() const 
{ 
	return m_nMaxValue * sizeof(XnUInt16); 
}

XnStatus Xn16zEmbTablesCodec::CompressImpl(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize) const
{
	return XnStreamCompressDepth16ZWithEmbTable((XnUInt16*)pData, nDataSize, pCompressedData, pnCompressedDataSize, m_nMaxValue);
}

XnStatus Xn16zEmbTablesCodec::DecompressImpl(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize) const
{
	return XnStreamUncompressDepth16ZWithEmbTable(pCompressedData, nCompressedDataSize, (XnUInt16*)pData, pnDataSize);
}

/*****************************/
/* Exported16zEmbTablesCodec */
/*****************************/
Exported16zEmbTablesCodec::Exported16zEmbTablesCodec() : ExportedCodec(XN_CODEC_16Z_EMB_TABLES) 
{

}

XnCodec* Exported16zEmbTablesCodec::CreateCodec() 
{ 
	return XN_NEW(Xn16zEmbTablesCodec); 
}
