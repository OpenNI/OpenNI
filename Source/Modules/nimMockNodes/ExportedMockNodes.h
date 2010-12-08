/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/


#ifndef __EXPORTED_MOCK_NODES_H__
#define __EXPORTED_MOCK_NODES_H__

#include "nimMockNodes.h"
#include <XnTypes.h>
#include <XnModuleCppInterface.h>

class ExportedMockNode : public xn::ModuleExportedProductionNode
{
public:
	ExportedMockNode(XnProductionNodeType type);
	virtual ~ExportedMockNode();
	virtual void GetDescription(XnProductionNodeDescription* pDescription);
	virtual XnStatus EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& TreesList, xn::EnumerationErrors* pErrors);
	virtual XnStatus Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance);
	virtual void Destroy(xn::ModuleProductionNode* pInstance);

protected:
	virtual xn::ModuleProductionNode* CreateImpl(const XnChar* strName) = 0;

private:
	XnProductionNodeType m_type;
};

class ExportedMockDevice : public ExportedMockNode
{
public:
	ExportedMockDevice();
	virtual ~ExportedMockDevice() {}
	virtual xn::ModuleProductionNode* CreateImpl(const XnChar* strName);
};

class ExportedMockDepth : public ExportedMockNode
{
public:
	ExportedMockDepth();
	virtual ~ExportedMockDepth() {}
	virtual xn::ModuleProductionNode* CreateImpl(const XnChar* strName);
};

class ExportedMockImage : public ExportedMockNode
{
public:
	ExportedMockImage();
	virtual ~ExportedMockImage() {}
	virtual xn::ModuleProductionNode* CreateImpl(const XnChar* strName);
};

class ExportedMockIR : public ExportedMockNode
{
public:
	ExportedMockIR();
	virtual ~ExportedMockIR() {}
	virtual xn::ModuleProductionNode* CreateImpl(const XnChar* strName);
};

class ExportedMockAudio : public ExportedMockNode
{
public:
	ExportedMockAudio();
	virtual ~ExportedMockAudio() {}
	virtual xn::ModuleProductionNode* CreateImpl(const XnChar* strName);
};

#endif //__EXPORTED_MOCK_NODES_H__