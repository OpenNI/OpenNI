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

XnStatus ExportedMockNodeBase::EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& TreesList, xn::EnumerationErrors* pErrors)
{
	// Don't return this in a regular enumeration. Instead, mock nodes are created directly by OpenNI.
	return XN_STATUS_OK;
}

XnStatus ExportedMockNodeBase::Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_VALIDATE_OUTPUT_PTR(ppInstance);

	*ppInstance = CreateImpl(strInstanceName);
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

xn::ModuleProductionNode* ExportedMockDevice::CreateImpl(const XnChar* strName)
{
	return XN_NEW(MockDevice, strName);
}

/*********************/
/* ExportedMockDepth */
/*********************/
ExportedMockDepth::ExportedMockDepth() : 
	ExportedMockNodeBase(XN_NODE_TYPE_DEPTH)
{
}

xn::ModuleProductionNode* ExportedMockDepth::CreateImpl(const XnChar* strName)
{
	return XN_NEW(MockDepthGenerator, strName);
}

/**********************/
/* ExportedMockImage  */
/**********************/

ExportedMockImage::ExportedMockImage() :
	ExportedMockNodeBase(XN_NODE_TYPE_IMAGE)
{
}

xn::ModuleProductionNode* ExportedMockImage::CreateImpl(const XnChar* strName)
{
	return XN_NEW(MockImageGenerator, strName);
}

/*******************/
/* ExportedMockIR */
/*******************/

ExportedMockIR::ExportedMockIR() :
	ExportedMockNodeBase(XN_NODE_TYPE_IR)
{
}

xn::ModuleProductionNode* ExportedMockIR::CreateImpl(const XnChar* strName)
{
	return XN_NEW(MockIRGenerator, strName);
}

/*********************/
/* ExportedMockAudio */
/*********************/
ExportedMockAudio::ExportedMockAudio() : 
	ExportedMockNodeBase(XN_NODE_TYPE_AUDIO)
{
}

xn::ModuleProductionNode* ExportedMockAudio::CreateImpl(const XnChar* strName)
{
	return XN_NEW(MockAudioGenerator, strName);
}

/*************************/
/* ExportedMockProductionNode */
/*************************/

ExportedMockProductionNode::ExportedMockProductionNode() :
	ExportedMockNodeBase(XN_NODE_TYPE_PRODUCTION_NODE)
{

}

xn::ModuleProductionNode* ExportedMockProductionNode::CreateImpl(const XnChar* strName)
{
	return XN_NEW(MockProductionNode, strName);
}

/*************************/
/* ExportedMockGenerator */
/*************************/

ExportedMockGenerator::ExportedMockGenerator() :
	ExportedMockNodeBase(XN_NODE_TYPE_GENERATOR)
{

}

xn::ModuleProductionNode* ExportedMockGenerator::CreateImpl(const XnChar* strName)
{
	return XN_NEW(MockGenerator, strName);
}

/*************************/
/* ExportedMockMapGenerator */
/*************************/

ExportedMockMapGenerator::ExportedMockMapGenerator() :
	ExportedMockNodeBase(XN_NODE_TYPE_MAP_GENERATOR)
{

}

xn::ModuleProductionNode* ExportedMockMapGenerator::CreateImpl(const XnChar* strName)
{
	return XN_NEW(MockMapGenerator, strName);
}
