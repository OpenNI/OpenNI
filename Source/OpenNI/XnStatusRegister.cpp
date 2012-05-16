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
