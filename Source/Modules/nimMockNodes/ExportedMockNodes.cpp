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
#include "ExportedMockNodes.h"
#include "MockDevice.h"
#include "MockDepthGenerator.h"
#include "MockImageGenerator.h"
#include "MockIRGenerator.h"
#include "MockAudioGenerator.h"
#include <XnInternalDefs.h>

/********************/
/* ExportedMockNodeBase */
/********************/
ExportedMockNodeBase::ExportedMockNodeBase(XnProductionNodeType type) :
	m_type(type)
{
}

ExportedMockNodeBase::~ExportedMockNodeBase()
{

}

void ExportedMockNodeBase::GetDescription(XnProductionNodeDescription* pDescription)
{
	strcpy(pDescription->strVendor, XN_VENDOR_OPEN_NI);
	strcpy(pDescription->strName, XN_MOCK_NODE_NAME);
	pDescription->Type = m_type;
	pDescription->Version.nMajor = XN_MAJOR_VERSION;
	pDescription->Version.nMinor = XN_MINOR_VERSION;
	pDescription->Version.nMaintenance = XN_MAINTENANCE_VERSION;
	pDescription->Version.nBuild = XN_BUILD_VERSION;
}

XnStatus ExportedMockNodeBase::EnumerateProductionTrees(xn::Context& /*context*/, xn::NodeInfoList& /*TreesList*/, xn::EnumerationErrors* /*pErrors*/)
{
	// Don't return this in a regular enumeration. Instead, mock nodes are created directly by OpenNI.
	return XN_STATUS_OK;
}

XnStatus ExportedMockNodeBase::Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* /*strCreationInfo*/, xn::NodeInfoList* /*pNeededTrees*/, const XnChar* /*strConfigurationDir*/, xn::ModuleProductionNode** ppInstance)
{
	XN_VALIDATE_OUTPUT_PTR(ppInstance);

	*ppInstance = CreateImpl(context, strInstanceName);
	XN_VALIDATE_ALLOC_PTR(*ppInstance);

	return XN_STATUS_OK;
}

void ExportedMockNodeBase::Destroy(xn::ModuleProductionNode* pInstance)
{
	XN_DELETE(pInstance);
}

/*********************/
/* ExportedMockDevice */
/*********************/
ExportedMockDevice::ExportedMockDevice() : 
	ExportedMockNodeBase(XN_NODE_TYPE_DEVICE)
{
}

xn::ModuleProductionNode* ExportedMockDevice::CreateImpl(xn::Context& context, const XnChar* strName)
{
	return XN_NEW(MockDevice, context, strName);
}

/*********************/
/* ExportedMockDepth */
/*********************/
ExportedMockDepth::ExportedMockDepth() : 
	ExportedMockNodeBase(XN_NODE_TYPE_DEPTH)
{
}

xn::ModuleProductionNode* ExportedMockDepth::CreateImpl(xn::Context& context, const XnChar* strName)
{
	return XN_NEW(MockDepthGenerator, context, strName);
}

/**********************/
/* ExportedMockImage  */
/**********************/

ExportedMockImage::ExportedMockImage() :
	ExportedMockNodeBase(XN_NODE_TYPE_IMAGE)
{
}

xn::ModuleProductionNode* ExportedMockImage::CreateImpl(xn::Context& context, const XnChar* strName)
{
	return XN_NEW(MockImageGenerator, context, strName);
}

/*******************/
/* ExportedMockIR */
/*******************/

ExportedMockIR::ExportedMockIR() :
	ExportedMockNodeBase(XN_NODE_TYPE_IR)
{
}

xn::ModuleProductionNode* ExportedMockIR::CreateImpl(xn::Context& context, const XnChar* strName)
{
	return XN_NEW(MockIRGenerator, context, strName);
}

/*********************/
/* ExportedMockAudio */
/*********************/
ExportedMockAudio::ExportedMockAudio() : 
	ExportedMockNodeBase(XN_NODE_TYPE_AUDIO)
{
}

xn::ModuleProductionNode* ExportedMockAudio::CreateImpl(xn::Context& context, const XnChar* strName)
{
	return XN_NEW(MockAudioGenerator, context, strName);
}

/*************************/
/* ExportedMockProductionNode */
/*************************/

ExportedMockProductionNode::ExportedMockProductionNode() :
	ExportedMockNodeBase(XN_NODE_TYPE_PRODUCTION_NODE)
{

}

xn::ModuleProductionNode* ExportedMockProductionNode::CreateImpl(xn::Context& context, const XnChar* strName)
{
	return XN_NEW(MockProductionNode, context, strName);
}

/*************************/
/* ExportedMockGenerator */
/*************************/

ExportedMockGenerator::ExportedMockGenerator() :
	ExportedMockNodeBase(XN_NODE_TYPE_GENERATOR)
{

}

xn::ModuleProductionNode* ExportedMockGenerator::CreateImpl(xn::Context& context, const XnChar* strName)
{
	return XN_NEW(MockGenerator, context, strName);
}

/*************************/
/* ExportedMockMapGenerator */
/*************************/

ExportedMockMapGenerator::ExportedMockMapGenerator() :
	ExportedMockNodeBase(XN_NODE_TYPE_MAP_GENERATOR)
{

}

xn::ModuleProductionNode* ExportedMockMapGenerator::CreateImpl(xn::Context& context, const XnChar* strName)
{
	return XN_NEW(MockMapGenerator, context, strName);
}
