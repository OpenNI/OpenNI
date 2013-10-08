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
#ifndef __EXPORTED_PLAYER_H__
#define __EXPORTED_PLAYER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnModuleCppInterface.h>
#include <XnTypes.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class ExportedPlayer : public xn::ModuleExportedProductionNode
{
public:
	ExportedPlayer();
	virtual ~ExportedPlayer();

	virtual void GetDescription(XnProductionNodeDescription* pDescription);
	virtual XnStatus EnumerateProductionTrees(xn::Context& context, xn::NodeInfoList& TreesList, xn::EnumerationErrors* pErrors);
	virtual XnStatus Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, xn::ModuleProductionNode** ppInstance);
	virtual void Destroy(xn::ModuleProductionNode* pInstance);

private:
	//---------------------------------------------------------------------------
	// Constants
	//---------------------------------------------------------------------------
	static const char NAME[];
	static const char CREATION_INFO[];
};

#endif // __EXPORTED_PLAYER_H__