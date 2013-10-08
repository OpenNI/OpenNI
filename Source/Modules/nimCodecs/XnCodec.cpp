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