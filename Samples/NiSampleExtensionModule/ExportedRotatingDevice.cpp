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

XnStatus ExportedRotatingDevice::EnumerateProductionTrees( xn::Context& /*context*/, xn::NodeInfoList& TreesList, xn::EnumerationErrors* /*pErrors*/ )
{
	XnStatus nRetVal = XN_STATUS_OK;

	// return one option
	XnProductionNodeDescription desc;
	GetDescription(&desc);

	nRetVal = TreesList.Add(desc, NULL, NULL);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus ExportedRotatingDevice::Create( xn::Context& /*context*/, const XnChar* /*strInstanceName*/, const XnChar* /*strCreationInfo*/, xn::NodeInfoList* /*pNeededTrees*/, const XnChar* /*strConfigurationDir*/, xn::ModuleProductionNode** ppInstance )
{
	SampleRotatingDevice* pDepth = new SampleRotatingDevice();

	*ppInstance = pDepth;

	return (XN_STATUS_OK);
}

void ExportedRotatingDevice::Destroy( xn::ModuleProductionNode* pInstance )
{
	delete pInstance;
}