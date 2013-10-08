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
#include "ExportedCodec.h"
#include <XnLog.h>

ExportedCodec::ExportedCodec(XnCodecID codecId) : m_codecId(codecId)
{

}

void ExportedCodec::GetDescription(XnProductionNodeDescription* pDescription)
{
	strcpy(pDescription->strVendor, XN_VENDOR_OPEN_NI);
	memcpy(pDescription->strName, &m_codecId, sizeof(m_codecId));
	pDescription->strName[sizeof(m_codecId)] = '\0';
	pDescription->Type = XN_NODE_TYPE_CODEC;
	pDescription->Version.nMajor = XN_MAJOR_VERSION;
	pDescription->Version.nMinor = XN_MINOR_VERSION;
	pDescription->Version.nMaintenance = XN_MAINTENANCE_VERSION;
	pDescription->Version.nBuild = XN_BUILD_VERSION;
}

XnStatus ExportedCodec::EnumerateProductionTrees(Context& /*context*/, NodeInfoList& TreesList, xn::EnumerationErrors* /*pErrors*/)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnProductionNodeDescription desc;
	GetDescription(&desc);

	nRetVal = TreesList.Add(desc, NULL, NULL);
	XN_IS_STATUS_OK_LOG_ERROR("Add to trees list", nRetVal);

	return (XN_STATUS_OK);
}

XnStatus ExportedCodec::Create(Context& /*context*/, const XnChar* /*strInstanceName*/, const XnChar* /*strCreationInfo*/, NodeInfoList* /*pNeededTrees*/, const XnChar* /*strConfigurationDir*/, ModuleProductionNode** ppInstance)
{
	XnCodec* pCodec = CreateCodec();
	XN_VALIDATE_ALLOC_PTR(pCodec);

	*ppInstance = pCodec;

	return (XN_STATUS_OK);
}

void ExportedCodec::Destroy(ModuleProductionNode* pInstance)
{
	XnCodec* pCodec = dynamic_cast<XnCodec*>(pInstance);
	XN_DELETE(pCodec);
}