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
#include "ExportedRotatingDevice.h"
#include "RotatingDeviceExtension.h"
#include <XnModuleCppRegistratration.h>

// create a module class which also registers extension types in this module
class SampleModule : public ExtensionModule
{
public:
	virtual XnStatus RegisterNewTypes() 
	{
		XnStatus nRetVal = XN_STATUS_OK;
		
		nRetVal = xnRegisterExtensionNode("RotatingDevice", XN_NODE_TYPE_DEVICE, &XN_NODE_TYPE_ROTATING_DEVICE);
		XN_IS_STATUS_OK(nRetVal);
		
		return (XN_STATUS_OK);
	}
};

// tell OpenNI this shared library is a module
XN_EXPORT_MODULE(SampleModule)

// tell OpenNI we export a SampleDevice node (extension of device), and give it its exporter class
XN_EXPORT_NODE(ExportedRotatingDevice, XN_NODE_TYPE_ROTATING_DEVICE)
