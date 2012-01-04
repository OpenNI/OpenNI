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
#include "XnCodec.h"
#include <XnLog.h>

XnCodec::XnCodec()
{

}

XnCodec::~XnCodec()
{

}

XnStatus XnCodec::Init(const ProductionNode& /*node*/)
{
	return XN_STATUS_OK;
}

XnStatus XnCodec::CompressData(const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten) const
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pSrc);
	XN_VALIDATE_INPUT_PTR(pDst);
	XN_VALIDATE_OUTPUT_PTR(pnBytesWritten);

	if ((nSrcSize * GetWorseCompressionRatio() + GetOverheadSize()) > nDstSize)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OUTPUT_BUFFER_OVERFLOW, XN_MASK_OPEN_NI, "Can't compress data - destination buffer is not large enough");
	}

	nRetVal = CompressImpl((const XnUChar*)pSrc, nSrcSize, (XnUChar*)pDst, &nDstSize);
	XN_IS_STATUS_OK_LOG_ERROR("Compress", nRetVal);

	*pnBytesWritten = nDstSize;

	return (XN_STATUS_OK);
}

XnStatus XnCodec::DecompressData(const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten) const
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pSrc);
	XN_VALIDATE_INPUT_PTR(pDst);
	XN_VALIDATE_OUTPUT_PTR(pnBytesWritten);

	nRetVal = DecompressImpl((const XnUChar*)pSrc, nSrcSize, (XnUChar*)pDst, &nDstSize);
	XN_IS_STATUS_OK_LOG_ERROR("Decompress", nRetVal);

	*pnBytesWritten = nDstSize;

	return (XN_STATUS_OK);
}