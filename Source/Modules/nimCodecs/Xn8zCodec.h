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
#ifndef __XN_8Z_CODEC_H__
#define __XN_8Z_CODEC_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnCodec.h"
#include "ExportedCodec.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class Xn8zCodec : public XnCodec
{
public:
	virtual XnCodecID GetCodecID() const;

	virtual XnFloat GetWorseCompressionRatio() const;
	virtual XnUInt32 GetOverheadSize() const;

protected:
	virtual XnStatus CompressImpl(const XnUChar* pData, XnUInt32 nDataSize, XnUChar* pCompressedData, XnUInt32* pnCompressedDataSize) const;
	virtual XnStatus DecompressImpl(const XnUChar* pCompressedData, XnUInt32 nCompressedDataSize, XnUChar* pData, XnUInt32* pnDataSize) const;
};

class Exported8zCodec : public ExportedCodec
{
public:
	Exported8zCodec();
	virtual XnCodec* CreateCodec();
};


#endif //__XN_8Z_CODEC_H__