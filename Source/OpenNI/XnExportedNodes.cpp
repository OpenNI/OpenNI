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
#include "XnXmlScriptNodeExporter.h"
#include <XnModuleCppRegistratration.h>
#include "xnInternalFuncs.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_EXPORT_MODULE(xn::Module);
XN_EXPORT_NODE(XnXmlScriptNodeExporter, XN_NODE_TYPE_SCRIPT);

XnOpenNIModuleInterface* GetOpenNIModuleInterface()
{
	static XnOpenNIModuleInterface moduleInterface = 
	{
		XN_MODULE_LOAD,
		XN_MODULE_UNLOAD,
		XN_MODULE_GET_EXPORTED_NODES_COUNT,
		XN_MODULE_GET_EXPORTED_NODES_ENTRY_POINTS,
		XN_MODULE_GET_OPEN_NI_VERSION
	};

	return &moduleInterface;
}
