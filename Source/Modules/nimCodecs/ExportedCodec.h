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
#ifndef __EXPORTED_CODEC_H__
#define __EXPORTED_CODEC_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnCodec.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class ExportedCodec : virtual public ModuleExportedProductionNode
{
public:
	ExportedCodec(XnCodecID codecId);
	void GetDescription(XnProductionNodeDescription* pDescription);
	XnStatus EnumerateProductionTrees(Context& context, NodeInfoList& TreesList, xn::EnumerationErrors* pErrors);
	XnStatus Create(Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, NodeInfoList* pNeededTrees, const XnChar* strConfigurationDir, ModuleProductionNode** ppInstance);
	void Destroy(ModuleProductionNode* pInstance);

protected:
	virtual XnCodec* CreateCodec() = 0;

private:
	XnCodecID m_codecId;
};

#endif // __EXPORTED_CODEC_H__