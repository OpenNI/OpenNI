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
#include "ExportedCodec.h"

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

XnStatus ExportedCodec::EnumerateProductionTrees(Context& context, NodeInfoList& TreesList, xn::EnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnProductionNodeDescription desc;
	GetDescription(&desc);

	nRetVal = TreesList.Add(desc, NULL, NULL);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus ExportedCodec::Create(Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, ModuleProductionNode** ppInstance)
{
	XnStatus nRetVal = XN_STATUS_OK;

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