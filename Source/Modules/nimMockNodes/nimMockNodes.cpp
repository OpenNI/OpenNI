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
