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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnUncompressedCodec.h"
#include "Xn16zCodec.h"
#include "Xn16zEmbTablesCodec.h"
#include "Xn8zCodec.h"
#include "XnJpegCodec.h"
#include <XnModuleCppRegistratration.h>

XN_EXPORT_MODULE(xn::Module)
XN_EXPORT_CODEC(Exported16zCodec)
XN_EXPORT_CODEC(Exported16zEmbTablesCodec)
XN_EXPORT_CODEC(Exported8zCodec)
XN_EXPORT_CODEC(ExportedJpegCodec)
XN_EXPORT_CODEC(ExportedUncompressedCodec)
