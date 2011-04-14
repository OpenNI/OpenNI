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