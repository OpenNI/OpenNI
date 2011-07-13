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
#include "XnXmlScriptNodeExporter.h"
#include "XnXmlScriptNode.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_OPEN_NI_NODE_NAME	"OpenNI"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
void GetOpenNIScriptNodeDescription(XnProductionNodeDescription* pDescription)
{
	pDescription->Type = XN_NODE_TYPE_SCRIPT;
	xnGetVersion(&pDescription->Version);
	strcpy(pDescription->strVendor, XN_VENDOR_OPEN_NI);
	strcpy(pDescription->strName, XN_OPEN_NI_NODE_NAME);
}

void XnXmlScriptNodeExporter::GetDescription(XnProductionNodeDescription* pDescription)
{
	GetOpenNIScriptNodeDescription(pDescription);
}

XnStatus XnXmlScriptNodeExporter::EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& TreesList, xn::EnumerationErrors* pErrors)
{
	// never return any results. This node is only created explicitly.
	return (XN_STATUS_OK);
}

XnStatus XnXmlScriptNodeExporter::Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance)
{
	XN_VALIDATE_NEW(*ppInstance, XnXmlScriptNode, context);
	return XN_STATUS_OK;
}

void XnXmlScriptNodeExporter::Destroy(xn::ModuleProductionNode* pInstance)
{
	xn::ModuleProductionNode* pNode = (xn::ModuleProductionNode*)pInstance;
	XN_DELETE(pNode);
}

