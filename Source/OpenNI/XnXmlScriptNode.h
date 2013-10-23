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
