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
#include "SampleRotatingDevice.h"

void ExportedRotatingDevice::GetDescription( XnProductionNodeDescription* pDescription )
{
	pDescription->Type = XN_NODE_TYPE_ROTATING_DEVICE;
	strcpy(pDescription->strVendor, "OpenNI");
	strcpy(pDescription->strName, "SampleDevice");
	pDescription->Version.nMajor = XN_MAJOR_VERSION;
	pDescription->Version.nMinor = XN_MINOR_VERSION;
	pDescription->Version.nMaintenance = XN_MAINTENANCE_VERSION;
	pDescription->Version.nBuild = XN_BUILD_VERSION;
}

XnStatus ExportedRotatingDevice::EnumerateProductionTrees( xn::Context& context, xn::NodeInfoList& TreesList, xn::EnumerationErrors* pErrors )
{
	XnStatus nRetVal = XN_STATUS_OK;

	// return one option
	XnProductionNodeDescription desc;
	GetDescription(&desc);

	nRetVal = TreesList.Add(desc, NULL, NULL);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus ExportedRotatingDevice::Create( xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance )
{
	XnStatus nRetVal = XN_STATUS_OK;

	SampleRotatingDevice* pDepth = new SampleRotatingDevice();

	*ppInstance = pDepth;

	return (XN_STATUS_OK);
}

void ExportedRotatingDevice::Destroy( xn::ModuleProductionNode* pInstance )
{
	delete pInstance;
}