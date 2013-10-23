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
#include "XnUtils.h"
#include "XnEnum.h"
#include "XnTypeManager.h"
#include "XnInternalTypes.h"
#include "XnModuleLoader.h"

//---------------------------------------------------------------------------
// Static Data
//---------------------------------------------------------------------------
XN_ENUM_MAP_START(XnPixelFormat)
	XN_ENUM_MAP_ENTRY(XN_PIXEL_FORMAT_RGB24, "RGB24")
	XN_ENUM_MAP_ENTRY(XN_PIXEL_FORMAT_YUV422, "YUV422")
	XN_ENUM_MAP_ENTRY(XN_PIXEL_FORMAT_GRAYSCALE_8_BIT, "Grayscale8")
	XN_ENUM_MAP_ENTRY(XN_PIXEL_FORMAT_GRAYSCALE_16_BIT, "Grayscale16")
	XN_ENUM_MAP_ENTRY(XN_PIXEL_FORMAT_MJPEG, "MJPEG")
XN_ENUM_MAP_END()

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XN_C_API XnPoint3D xnCreatePoint3D(XnFloat x, XnFloat y, XnFloat z)
{
	XnPoint3D point;
	point.X = x;
	point.Y = y;
	point.Z = z;

	return point;
}
XN_C_API XnPlane3D xnCreatePlane3D(XnPoint3D ptPoint, XnVector3D vNormal)
{
	XnPlane3D plane;
	plane.ptPoint = ptPoint;
	plane.vNormal = vNormal;

	return plane;
}

XN_C_API const XnChar* xnPixelFormatToString(XnPixelFormat format)
{
	XnStatus nRetVal = XN_STATUS_OK;
	const XnChar* strValue;
	XN_ENUM_TO_STRING(nRetVal, XnPixelFormat, format, strValue);
	if (nRetVal != XN_STATUS_OK)
	{
		strValue = "Unknown";
	}

	return strValue;
}

XN_C_API XnStatus xnPixelFormatFromString(const XnChar* strType, XnPixelFormat* pFormat)
{
	XN_VALIDATE_INPUT_PTR(strType);
	XN_VALIDATE_OUTPUT_PTR(pFormat);

	XN_ENUM_VALIDATE_FROM_STRING(XnPixelFormat, strType, *pFormat);

	return (XN_STATUS_OK);
}

XN_C_API XnUInt32 xnGetBytesPerPixelForPixelFormat(XnPixelFormat format)
{
	switch (format)
	{
	case XN_PIXEL_FORMAT_RGB24:
		return sizeof(XnRGB24Pixel);
	case XN_PIXEL_FORMAT_YUV422:
		return sizeof(XnYUV422DoublePixel) / 2;
	case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
		return sizeof(XnUInt8);
	case XN_PIXEL_FORMAT_GRAYSCALE_16_BIT:
		return sizeof(XnUInt16);
	case XN_PIXEL_FORMAT_MJPEG:
		return 2;
	default:
		XN_ASSERT(FALSE);
		return 0;
	}
}

XN_C_API XnInt32 xnVersionCompare(const XnVersion* pVersion1, const XnVersion* pVersion2)
{
	XnInt32 nResult = pVersion1->nMajor - pVersion2->nMajor;

	if (nResult == 0)
	{
		nResult = pVersion1->nMinor - pVersion2->nMinor;
	}

	if (nResult == 0)
	{
		nResult = pVersion1->nMaintenance - pVersion2->nMaintenance;
	}

	if (nResult == 0)
	{
		nResult = pVersion1->nBuild - pVersion2->nBuild;
	}

	return (nResult);
}

XN_C_API XnStatus xnVersionToString(const XnVersion* pVersion, XnChar* csResult, XnUInt32 nSize)
{
	XnUInt32 nWritten;
	return xnOSStrFormat(csResult, nSize, &nWritten, "%u.%u.%u.%u", pVersion->nMajor, pVersion->nMinor, pVersion->nMaintenance, pVersion->nBuild);
}

XN_C_API XnStatus xnProductionNodeDescriptionToString(const XnProductionNodeDescription* pDescription, XnChar* csResult, XnUInt32 nSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pDescription);
	XN_VALIDATE_INPUT_PTR(csResult);

	XnUInt32 nWritten = 0;
	nRetVal = xnOSStrFormat(csResult, nSize, &nWritten, "%s: %s/%s/", 
		xnProductionNodeTypeToString(pDescription->Type), 
		pDescription->strVendor, 
		pDescription->strName);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnVersionToString(&pDescription->Version, csResult + nWritten, nSize - nWritten);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_C_API XnOutputMetaData* xnAllocateOutputMetaData()
{
	return (XnOutputMetaData*)xnOSCalloc(1, sizeof(XnOutputMetaData));
}

XN_C_API void xnFreeOutputMetaData(const XnOutputMetaData* pMetaData)
{
	if (pMetaData != NULL)
	{
		xnOSFree(pMetaData);
	}
}

XN_C_API XnMapMetaData* xnAllocateMapMetaData()
{
	XnMapMetaData* pResult = (XnMapMetaData*)xnOSCalloc(1, sizeof(XnMapMetaData));
	if (pResult == NULL)
	{
		return (NULL);
	}

	pResult->pOutput = xnAllocateOutputMetaData();
	if (pResult->pOutput == NULL)
	{
		xnFreeMapMetaData(pResult);
		return (NULL);
	}

	return (pResult);
}

XN_C_API void xnFreeMapMetaData(const XnMapMetaData* pMetaData)
{
	if (pMetaData != NULL)
	{
		xnFreeOutputMetaData(pMetaData->pOutput);
		xnOSFree(pMetaData);
	}
}

XN_C_API XnDepthMetaData* xnAllocateDepthMetaData()
{
	XnDepthMetaData* pResult = (XnDepthMetaData*)xnOSCalloc(1, sizeof(XnDepthMetaData));
	if (pResult == NULL)
	{
		return (NULL);
	}

	pResult->pMap = xnAllocateMapMetaData();
	if (pResult->pMap == NULL)
	{
		xnFreeDepthMetaData(pResult);
		return (NULL);
	}

	return (pResult);
}

XN_C_API XnImageMetaData* xnAllocateImageMetaData()
{
	XnImageMetaData* pResult = (XnImageMetaData*)xnOSCalloc(1, sizeof(XnImageMetaData));
	if (pResult == NULL)
	{
		return (NULL);
	}

	pResult->pMap = xnAllocateMapMetaData();
	if (pResult->pMap == NULL)
	{
		xnFreeImageMetaData(pResult);
		return (NULL);
	}

	return (pResult);
}

XN_C_API XnIRMetaData* xnAllocateIRMetaData()
{
	XnIRMetaData* pResult = (XnIRMetaData*)xnOSCalloc(1, sizeof(XnIRMetaData));
	if (pResult == NULL)
	{
		return (NULL);
	}

	pResult->pMap = xnAllocateMapMetaData();
	if (pResult->pMap == NULL)
	{
		xnFreeIRMetaData(pResult);
		return (NULL);
	}

	return (pResult);
}

XN_C_API XnAudioMetaData* xnAllocateAudioMetaData()
{
	XnAudioMetaData* pResult = (XnAudioMetaData*)xnOSCalloc(1, sizeof(XnAudioMetaData));
	if (pResult == NULL)
	{
		return (NULL);
	}

	pResult->pOutput = xnAllocateOutputMetaData();
	if (pResult->pOutput == NULL)
	{
		xnFreeAudioMetaData(pResult);
		return (NULL);
	}

	return (pResult);
}

XN_C_API XnSceneMetaData* xnAllocateSceneMetaData()
{
	XnSceneMetaData* pResult = (XnSceneMetaData*)xnOSCalloc(1, sizeof(XnSceneMetaData));
	if (pResult == NULL)
	{
		return (NULL);
	}

	pResult->pMap = xnAllocateMapMetaData();
	if (pResult->pMap == NULL)
	{
		xnFreeSceneMetaData(pResult);
		return (NULL);
	}

	return (pResult);
}

XN_C_API void xnFreeDepthMetaData(const XnDepthMetaData* pMetaData)
{
	if (pMetaData != NULL)
	{
		xnFreeMapMetaData(pMetaData->pMap);
		xnOSFree(pMetaData);
	}
}

XN_C_API void xnFreeImageMetaData(const XnImageMetaData* pMetaData)
{
	if (pMetaData != NULL)
	{
		xnFreeMapMetaData(pMetaData->pMap);
		xnOSFree(pMetaData);
	}
}

XN_C_API void xnFreeIRMetaData(const XnIRMetaData* pMetaData)
{
	if (pMetaData != NULL)
	{
		xnFreeMapMetaData(pMetaData->pMap);
		xnOSFree(pMetaData);
	}
}

XN_C_API void xnFreeAudioMetaData(const XnAudioMetaData* pMetaData)
{
	if (pMetaData != NULL)
	{
		xnFreeOutputMetaData(pMetaData->pOutput);
		xnOSFree(pMetaData);
	}
}

XN_C_API void xnFreeSceneMetaData(const XnSceneMetaData* pMetaData)
{
	if (pMetaData != NULL)
	{
		xnFreeMapMetaData(pMetaData->pMap);
		xnOSFree(pMetaData);
	}
}

XN_C_API XnStatus xnCopyOutputMetaData(XnOutputMetaData* pDestination, const XnOutputMetaData* pSource)
{
	XN_VALIDATE_INPUT_PTR(pDestination);
	XN_VALIDATE_INPUT_PTR(pSource);
	xnOSMemCopy(pDestination, pSource, sizeof(XnOutputMetaData));
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnCopyMapMetaData(XnMapMetaData* pDestination, const XnMapMetaData* pSource)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pDestination);
	XN_VALIDATE_INPUT_PTR(pSource);

	nRetVal = xnCopyOutputMetaData(pDestination->pOutput, pSource->pOutput);
	XN_IS_STATUS_OK(nRetVal);

	// keep pointer (it will be lost in memcpy)
	XnOutputMetaData* pOutput = pDestination->pOutput;
	xnOSMemCopy(pDestination, pSource, sizeof(XnMapMetaData));
	pDestination->pOutput = pOutput;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnCopyDepthMetaData(XnDepthMetaData* pDestination, const XnDepthMetaData* pSource)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pDestination);
	XN_VALIDATE_INPUT_PTR(pSource);

	nRetVal = xnCopyMapMetaData(pDestination->pMap, pSource->pMap);
	XN_IS_STATUS_OK(nRetVal);

	// keep pointer (it will be lost in memcpy)
	XnMapMetaData* pMap = pDestination->pMap;
	xnOSMemCopy(pDestination, pSource, sizeof(XnDepthMetaData));
	pDestination->pMap = pMap;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnCopyImageMetaData(XnImageMetaData* pDestination, const XnImageMetaData* pSource)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pDestination);
	XN_VALIDATE_INPUT_PTR(pSource);

	nRetVal = xnCopyMapMetaData(pDestination->pMap, pSource->pMap);
	XN_IS_STATUS_OK(nRetVal);

	// keep pointer (it will be lost in memcpy)
	XnMapMetaData* pMap = pDestination->pMap;
	xnOSMemCopy(pDestination, pSource, sizeof(XnImageMetaData));
	pDestination->pMap = pMap;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnCopyIRMetaData(XnIRMetaData* pDestination, const XnIRMetaData* pSource)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pDestination);
	XN_VALIDATE_INPUT_PTR(pSource);

	nRetVal = xnCopyMapMetaData(pDestination->pMap, pSource->pMap);
	XN_IS_STATUS_OK(nRetVal);

	// keep pointer (it will be lost in memcpy)
	XnMapMetaData* pMap = pDestination->pMap;
	xnOSMemCopy(pDestination, pSource, sizeof(XnIRMetaData));
	pDestination->pMap = pMap;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnCopyAudioMetaData(XnAudioMetaData* pDestination, const XnAudioMetaData* pSource)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pDestination);
	XN_VALIDATE_INPUT_PTR(pSource);

	nRetVal = xnCopyOutputMetaData(pDestination->pOutput, pSource->pOutput);
	XN_IS_STATUS_OK(nRetVal);

	// keep pointer (it will be lost in memcpy)
	XnOutputMetaData* pOutput = pDestination->pOutput;
	xnOSMemCopy(pDestination, pSource, sizeof(XnAudioMetaData));
	pDestination->pOutput = pOutput;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnCopySceneMetaData(XnSceneMetaData* pDestination, const XnSceneMetaData* pSource)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pDestination);
	XN_VALIDATE_INPUT_PTR(pSource);

	nRetVal = xnCopyMapMetaData(pDestination->pMap, pSource->pMap);
	XN_IS_STATUS_OK(nRetVal);

	// keep pointer (it will be lost in memcpy)
	XnMapMetaData* pMap = pDestination->pMap;
	xnOSMemCopy(pDestination, pSource, sizeof(XnSceneMetaData));
	pDestination->pMap = pMap;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnRegisterExtensionNode(const XnChar* strTypeName, XnProductionNodeType baseType, XnProductionNodeType* pTypeID)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(strTypeName);
	XN_VALIDATE_OUTPUT_PTR(pTypeID);

	nRetVal = TypeManager::GetInstance().RegisterNewType(strTypeName, baseType, pTypeID);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_C_API XnModuleNodeHandle xnGetModuleNodeHandle(XnNodeHandle hNode)
{
	XN_VALIDATE_PTR(hNode, NULL);
	return hNode->pModuleInstance->hNode;
}
