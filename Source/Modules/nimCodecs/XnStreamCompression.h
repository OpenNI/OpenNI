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
#ifndef _XN_STREAMCOMPRESSION_H_
#define _XN_STREAMCOMPRESSION_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include <jpeglib.h>
#include <setjmp.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_STREAM_COMPRESSION_DEPTH16Z_WORSE_RATIO 1.333F
#define XN_STREAM_COMPRESSION_IMAGE8Z_WORSE_RATIO 1.333F
#define XN_STREAM_COMPRESSION_IMAGEJ_WORSE_RATIO 1.2F
#define XN_STREAM_COMPRESSION_CONF4_WORSE_RATIO 0.51F
#define XN_STREAM_COMPRESSION_JPEG_DEFAULT_QUALITY 90

#define XN_STREAM_STRING_BAD_FORMAT -1

#define XN_MASK_JPEG "JPEG"

//---------------------------------------------------------------------------
// Structs
//---------------------------------------------------------------------------

XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_STRUCT_PADDED_WARNING_ID);

typedef struct XnLibJpegErrorMgr
{
	struct jpeg_error_mgr pub;

	jmp_buf setjmpBuffer;
} XnLibJpegErrorMgr; 

XN_PRAGMA_STOP_DISABLED_WARNING_SECTION;

typedef struct XnStreamCompJPEGContext
{
	jpeg_compress_struct		jCompStruct;
	XnLibJpegErrorMgr			jErrMgr;
	struct jpeg_destination_mgr	jDestMgr;
} XnStreamCompJPEGContext;

typedef struct XnStreamUncompJPEGContext
{
	jpeg_decompress_struct	jDecompStruct;
	XnLibJpegErrorMgr		jErrMgr;
	struct jpeg_source_mgr	jSrcMgr;
} XnStreamUncompJPEGContext;

//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
XnStatus XnStreamCompressDepth16Z(const XnUInt16* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize);
XnStatus XnStreamCompressDepth16ZWithEmbTable(const XnUInt16* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize, XnUInt16 nMaxValue);
XnStatus XnStreamUncompressDepth16Z(const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt16* pOutput, XnUInt32* pnOutputSize);
XnStatus XnStreamUncompressDepth16ZWithEmbTable(const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt16* pOutput, XnUInt32* pnOutputSize);

XnStatus XnStreamCompressImage8Z(const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize);
XnStatus XnStreamUncompressImage8Z(const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize);

XnStatus XnStreamCompressConf4(const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize);
XnStatus XnStreamUncompressConf4(const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize);

void		   XnStreamJPEGCompDummyFunction(struct jpeg_compress_struct* pjCompStruct);
boolean		   XnStreamJPEGCompDummyFailFunction(struct jpeg_compress_struct* pjCompStruct);
XnStatus XnStreamInitCompressImageJ(XnStreamCompJPEGContext* pStreamCompJPEGContext);
XnStatus XnStreamFreeCompressImageJ(XnStreamCompJPEGContext* pStreamCompJPEGContext);
XnStatus XnStreamCompressImage8J(XnStreamCompJPEGContext* pStreamCompJPEGContext, const XnUInt8* pInput, XnUInt8* pOutput, XnUInt32* pnOutputSize, const XnUInt32 nXRes, const XnUInt32 nYRes, const XnUInt32 nQuality);
XnStatus XnStreamCompressImage24J(XnStreamCompJPEGContext* pStreamCompJPEGContext, const XnUInt8* pInput, XnUInt8* pOutput, XnUInt32* pnOutputSize, const XnUInt32 nXRes, const XnUInt32 nYRes, const XnUInt32 nQuality);

void			   XnStreamJPEGDecompDummyFunction(struct jpeg_decompress_struct* pjDecompStruct);
boolean			   XnStreamJPEGDecompDummyFailFunction(struct jpeg_decompress_struct* pjDecompStruct);
void			   XnStreamJPEGDecompSkipFunction(struct jpeg_decompress_struct* pjDecompStruct, XnInt nNumBytes);
XnStatus XnStreamInitUncompressImageJ(XnStreamUncompJPEGContext* pStreamUncompJPEGContext);
XnStatus XnStreamFreeUncompressImageJ(XnStreamUncompJPEGContext* pStreamUncompJPEGContext);
XnStatus XnStreamUncompressImageJ(XnStreamUncompJPEGContext* pStreamUncompJPEGContext, const XnUInt8* pInput, const XnUInt32 nInputSize, XnUInt8* pOutput, XnUInt32* pnOutputSize);

#endif //_XN_STREAMCOMPRESSION_H_
