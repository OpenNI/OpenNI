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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnModuleCppRegistratration.h>
#include "ExportedMockNodes.h"

//---------------------------------------------------------------------------
// Exporting
//---------------------------------------------------------------------------
XN_EXPORT_MODULE(Module)
XN_EXPORT_NODE(ExportedMockProductionNode, XN_NODE_TYPE_PRODUCTION_NODE)
XN_EXPORT_NODE(ExportedMockGenerator, XN_NODE_TYPE_GENERATOR)
XN_EXPORT_NODE(ExportedMockMapGenerator, XN_NODE_TYPE_MAP_GENERATOR)
XN_EXPORT_NODE(ExportedMockDevice, XN_NODE_TYPE_DEVICE)
XN_EXPORT_NODE(ExportedMockDepth, XN_NODE_TYPE_DEPTH)
XN_EXPORT_NODE(ExportedMockIR, XN_NODE_TYPE_IR)
XN_EXPORT_NODE(ExportedMockImage, XN_NODE_TYPE_IMAGE)
XN_EXPORT_NODE(ExportedMockAudio, XN_NODE_TYPE_AUDIO)
