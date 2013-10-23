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

XnStatus XnXmlScriptNodeExporter::EnumerateProductionTrees(xn::Context& /*context*/, xn::NodeInfoList& /*TreesList*/, xn::EnumerationErrors* /*pErrors*/)
{
	// never return any results. This node is only created explicitly.
	return (XN_STATUS_OK);
}

XnStatus XnXmlScriptNodeExporter::Create(xn::Context& context, const XnChar* /*strInstanceName*/, const XnChar* /*strCreationInfo*/, xn::NodeInfoList* /*pNeededTrees*/, const XnChar* /*strConfigurationDir*/, xn::ModuleProductionNode** ppInstance)
{
	XN_VALIDATE_NEW(*ppInstance, XnXmlScriptNode, context);
	return XN_STATUS_OK;
}

void XnXmlScriptNodeExporter::Destroy(xn::ModuleProductionNode* pInstance)
{
	xn::ModuleProductionNode* pNode = (xn::ModuleProductionNode*)pInstance;
	XN_DELETE(pNode);
}

