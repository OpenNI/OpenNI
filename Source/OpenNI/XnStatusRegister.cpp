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
#include <XnLog.h>
#include <XnHashT.h>
#include <XnStatusRegister.h>

#define XN_MASK_STATUS "XnStatus"
#define XN_OK 0

//---------------------------------------------------------------------------
// Data Types
//---------------------------------------------------------------------------
typedef XnHashT<XnUInt16, XnErrorCodeData> XnStatusHash;
typedef XnHashT<XnUInt16, XnStatusHash*> XnGroupsHash;

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------

/** An array holding all groups' errors */
static XnGroupsHash* g_pErrorGroups = NULL;

static XnErrorCodeData s_OK_Data = { XN_STATUS_OK, XN_STRINGIFY(XN_STATUS_OK), "OK" };
static XnStatus s_XN_OK_result = xnRegisterErrorCodeMessages(0, 0, 1, &s_OK_Data);

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnErrorCodeData* xnGetErrorCodeData(const XnStatus Status)
{
	// search for it
	XnUInt16 nGroup = XN_STATUS_GROUP(Status);
	XnUInt16 nCode = XN_STATUS_CODE(Status);

	if (g_pErrorGroups == NULL)
	{
		return NULL;
	}

	XnStatusHash* pStatusHash = NULL;
	if (g_pErrorGroups->Get(nGroup, pStatusHash) != XN_STATUS_OK)
	{
		// unregistered group
		return NULL;
	}

	XnErrorCodeData* pResult = NULL;
	pStatusHash->Get(nCode, pResult);
	return pResult;
}

XN_C_API XnStatus xnRegisterErrorCodeMessages(XnUInt16 nGroup, XnUInt16 nFirst, XnUInt16 nCount, XnErrorCodeData* pErrorCodeData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_VALIDATE_OUTPUT_PTR(pErrorCodeData);

	if (g_pErrorGroups == NULL)
	{
		g_pErrorGroups = XN_NEW(XnGroupsHash);
		XN_VALIDATE_ALLOC_PTR(g_pErrorGroups);
	}

	XnStatusHash* pStatusHash = NULL;
	if (g_pErrorGroups->Get(nGroup, pStatusHash) != XN_STATUS_OK)
	{
		XN_VALIDATE_NEW(pStatusHash, XnStatusHash);
		nRetVal = g_pErrorGroups->Set(nGroup, pStatusHash);
		XN_IS_STATUS_OK(nRetVal);
	}

	for (XnUInt16 nIndex = 0; nIndex < nCount; ++nIndex)
	{
		XnErrorCodeData data = {0};
		data.nCode = nFirst + nIndex;
		data.csMessage = xnOSStrDup(pErrorCodeData[nIndex].csMessage);
		data.csName = xnOSStrDup(pErrorCodeData[nIndex].csName);

		XnErrorCodeData prevData = {0};
		pStatusHash->Get((XnUInt16)data.nCode, prevData);

		nRetVal = pStatusHash->Set((XnUInt16)data.nCode, data);
		XN_IS_STATUS_OK(nRetVal);

		// if prev contained anything, free it
		xnOSFree(prevData.csName);
		xnOSFree(prevData.csMessage);
	}

	return XN_STATUS_OK;
}

XN_C_API const XnChar* xnGetStatusString(const XnStatus Status)
{
	XnErrorCodeData* pErrorData = xnGetErrorCodeData(Status);
	if (pErrorData == NULL)
	{
		// unregistered error
		return "Unknown Xiron Status!";
	}
	else
	{
		return pErrorData->csMessage;
	}
}

XN_C_API const XnChar* xnGetStatusName(const XnStatus Status)
{
	XnErrorCodeData* pErrorData = xnGetErrorCodeData(Status);
	if (pErrorData == NULL)
	{
		// unregistered error
		return "Unknown Xiron Status!";
	}
	else
	{
		return pErrorData->csName;
	}
}

XN_C_API void xnPrintError(const XnStatus Status, const XnChar* csUserMessage)
{
	printf("%s: %s\n", csUserMessage, xnGetStatusString(Status));
}
