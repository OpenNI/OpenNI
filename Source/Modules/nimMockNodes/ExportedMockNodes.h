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
#ifndef __EXPORTED_MOCK_NODES_H__
#define __EXPORTED_MOCK_NODES_H__

#include "nimMockNodes.h"
#include <XnTypes.h>
#include <XnModuleCppInterface.h>

class ExportedMockNodeBase : public xn::ModuleExportedProductionNode
{
public:
	ExportedMockNodeBase(XnProductionNodeType type);
	virtual ~ExportedMockNodeBase();
	virtual void GetDescription(XnProductionNodeDescription* pDescription);
	virtual XnStatus EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& TreesList, xn::EnumerationErrors* pErrors);
	virtual XnStatus Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance);
	virtual void Destroy(xn::ModuleProductionNode* pInstance);

protected:
	virtual xn::ModuleProductionNode* CreateImpl(xn::Context& context, const XnChar* strName) = 0;

private:
	XnProductionNodeType m_type;
};

class ExportedMockDevice : public ExportedMockNodeBase
{
public:
	ExportedMockDevice();
	virtual ~ExportedMockDevice() {}
	virtual xn::ModuleProductionNode* CreateImpl(xn::Context& context, const XnChar* strName);
};

class ExportedMockDepth : public ExportedMockNodeBase
{
public:
	ExportedMockDepth();
	virtual ~ExportedMockDepth() {}
	virtual xn::ModuleProductionNode* CreateImpl(xn::Context& context, const XnChar* strName);
};

class ExportedMockImage : public ExportedMockNodeBase
{
public:
	ExportedMockImage();
	virtual ~ExportedMockImage() {}
	virtual xn::ModuleProductionNode* CreateImpl(xn::Context& context, const XnChar* strName);
};

class ExportedMockIR : public ExportedMockNodeBase
{
public:
	ExportedMockIR();
	virtual ~ExportedMockIR() {}
	virtual xn::ModuleProductionNode* CreateImpl(xn::Context& context, const XnChar* strName);
};

class ExportedMockAudio : public ExportedMockNodeBase
{
public:
	ExportedMockAudio();
	virtual ~ExportedMockAudio() {}
	virtual xn::ModuleProductionNode* CreateImpl(xn::Context& context, const XnChar* strName);
};

class ExportedMockProductionNode : public ExportedMockNodeBase
{
public:
	ExportedMockProductionNode();
	virtual ~ExportedMockProductionNode() {}
	virtual xn::ModuleProductionNode* CreateImpl(xn::Context& context, const XnChar* strName);
};

class ExportedMockGenerator : public ExportedMockNodeBase
{
public:
	ExportedMockGenerator();
	virtual ~ExportedMockGenerator() {}
	virtual xn::ModuleProductionNode* CreateImpl(xn::Context& context, const XnChar* strName);
};

class ExportedMockMapGenerator : public ExportedMockNodeBase
{
public:
	ExportedMockMapGenerator();
	virtual ~ExportedMockMapGenerator() {}
	virtual xn::ModuleProductionNode* CreateImpl(xn::Context& context, const XnChar* strName);
};

#endif //__EXPORTED_MOCK_NODES_H__