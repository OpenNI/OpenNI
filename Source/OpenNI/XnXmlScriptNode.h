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
#ifndef __XN_XML_SCRIPT_NODE_H__
#define __XN_XML_SCRIPT_NODE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnModuleCppInterface.h>
#include <XnCppWrapper.h>
#include "XnXml.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnXmlScriptNode : public xn::ModuleScriptNode
{
public:
	XnXmlScriptNode(xn::Context context);
	virtual ~XnXmlScriptNode();
	virtual const XnChar* GetSupportedFormat();
	virtual XnStatus LoadScriptFromFile(const XnChar* strFileName);
	virtual XnStatus LoadScriptFromString(const XnChar* strScript);
	virtual XnStatus Run(xn::NodeInfoList& createdNodes, xn::EnumerationErrors& errors);

private:
	TiXmlDocument m_doc;
	xn::Context m_context;
};

#endif // __XN_XML_SCRIPT_NODE_H__
