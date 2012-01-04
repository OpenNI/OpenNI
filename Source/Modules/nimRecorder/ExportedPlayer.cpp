/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "ExportedPlayer.h"
#include "PlayerNode.h"
#include "DataRecords.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------
const char ExportedPlayer::NAME[] = "Player";
const char ExportedPlayer::CREATION_INFO[] = "Player";

//---------------------------------------------------------------------------
// ExportedPlayer class
//---------------------------------------------------------------------------
ExportedPlayer::ExportedPlayer()
{
}

ExportedPlayer::~ExportedPlayer()
{
}

void ExportedPlayer::GetDescription(XnProductionNodeDescription* pDescription)
{
	pDescription->Type = XN_NODE_TYPE_PLAYER;
	strcpy(pDescription->strName, NAME);
	strcpy(pDescription->strVendor, XN_VENDOR_OPEN_NI);
	pDescription->Version.nMajor = XN_MAJOR_VERSION;
	pDescription->Version.nMinor = XN_MINOR_VERSION;
	pDescription->Version.nMaintenance = XN_MAINTENANCE_VERSION;
	pDescription->Version.nBuild = XN_BUILD_VERSION;
}

XnStatus ExportedPlayer::EnumerateProductionTrees(xn::Context& /*context*/, xn::NodeInfoList& TreesList, xn::EnumerationErrors* /*pErrors*/)
{
	XnProductionNodeDescription description;
	XnStatus nRetVal = XN_STATUS_OK;

	GetDescription(&description);
	nRetVal = TreesList.Add(description, CREATION_INFO, NULL);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus ExportedPlayer::Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* /*pNeededTrees*/, const XnChar* /*strConfigurationDir*/, xn::ModuleProductionNode** ppInstance)
{
	XN_VALIDATE_INPUT_PTR(strInstanceName);
	XN_VALIDATE_INPUT_PTR(strCreationInfo);
	XN_VALIDATE_OUTPUT_PTR(ppInstance);

	if (strcmp(strCreationInfo, CREATION_INFO) != 0)
	{
		//This is not the creation info we gave in EnumerateProductionTrees
		XN_LOG_ERROR_RETURN(XN_STATUS_NO_MATCH, XN_MASK_OPEN_NI, "Invalid creation info");
	}
	PlayerNode *pPlayerNode;
	XN_VALIDATE_NEW_AND_INIT(pPlayerNode, PlayerNode, context, strInstanceName);

	*ppInstance = pPlayerNode;

	return XN_STATUS_OK;
}

void ExportedPlayer::Destroy(xn::ModuleProductionNode* pInstance)
{
	PlayerNode *pPlayerNode = dynamic_cast<PlayerNode*>(pInstance);
	if (pPlayerNode == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}
	XN_DELETE(pPlayerNode);
}
