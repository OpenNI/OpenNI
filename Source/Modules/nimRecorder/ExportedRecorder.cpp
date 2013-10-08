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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "ExportedRecorder.h"
#include "RecorderNode.h"
#include "DataRecords.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------
const char ExportedRecorder::NAME[] = "Recorder";
const char ExportedRecorder::CREATION_INFO[] = "Recorder";

//---------------------------------------------------------------------------
// ExportedRecorder class
//---------------------------------------------------------------------------
ExportedRecorder::ExportedRecorder()
{
	xnOSMemSet(m_strInstanceName, '\0', sizeof(m_strInstanceName));
}

ExportedRecorder::~ExportedRecorder()
{

}

void ExportedRecorder::GetDescription(XnProductionNodeDescription* pDescription)
{
	pDescription->Type = XN_NODE_TYPE_RECORDER;
	strcpy(pDescription->strName, NAME);
	strcpy(pDescription->strVendor, XN_VENDOR_OPEN_NI);
	pDescription->Version.nMajor = XN_MAJOR_VERSION;
	pDescription->Version.nMinor = XN_MINOR_VERSION;
	pDescription->Version.nMaintenance = XN_MAINTENANCE_VERSION;
	pDescription->Version.nBuild = XN_BUILD_VERSION;
}

XnStatus ExportedRecorder::EnumerateProductionTrees(xn::Context& /*context*/, xn::NodeInfoList& TreesList, xn::EnumerationErrors* /*pErrors*/)
{
	XnProductionNodeDescription description;
	XnStatus nRetVal = XN_STATUS_OK;
	
	GetDescription(&description);
	nRetVal = TreesList.Add(description, CREATION_INFO, NULL);
	XN_IS_STATUS_OK(nRetVal);
		
	return XN_STATUS_OK;
}

XnStatus ExportedRecorder::Create(xn::Context& context, const XnChar* strInstanceName, const XnChar* strCreationInfo, xn::NodeInfoList* /*pNeededTrees*/, const XnChar* /*strConfigurationDir*/, xn::ModuleProductionNode** ppInstance)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_VALIDATE_INPUT_PTR(strInstanceName);
	XN_VALIDATE_INPUT_PTR(strCreationInfo);
	XN_VALIDATE_OUTPUT_PTR(ppInstance);

	nRetVal = xnOSStrCopy(m_strInstanceName, strInstanceName, sizeof(m_strInstanceName));
	XN_IS_STATUS_OK(nRetVal);
	if (strcmp(strCreationInfo, CREATION_INFO) != 0)
	{
		//This is not the creation info we gave in EnumerateProductionTrees
		XN_LOG_ERROR_RETURN(XN_STATUS_NO_MATCH, XN_MASK_OPEN_NI, "Invalid creation info");
	}
	RecorderNode *pRecorderNode;
	XN_VALIDATE_NEW_AND_INIT(pRecorderNode, RecorderNode, context);

	*ppInstance = pRecorderNode;

	return XN_STATUS_OK;
}

void ExportedRecorder::Destroy(xn::ModuleProductionNode* pInstance)
{
	RecorderNode *pRecorderNode = dynamic_cast<RecorderNode*>(pInstance);
	if (pRecorderNode == NULL)
	{
		XN_ASSERT(FALSE);
		return;
	}
	pRecorderNode->Destroy();
	XN_DELETE(pRecorderNode);
}
