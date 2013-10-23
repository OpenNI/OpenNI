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
#include "XnXmlScriptNode.h"
#include <XnLog.h>
#include "XnLicensingInternal.h"
#include <XnOSCpp.h>
#include "XnInternalTypes.h"
#include "XnTypeManager.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus xnXmlReadMapOutputMode(const TiXmlElement* pOpcode, XnMapOutputMode* pMapOutputMode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnXmlReadUInt32Attribute(pOpcode, "xRes", &pMapOutputMode->nXRes);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnXmlReadUInt32Attribute(pOpcode, "yRes", &pMapOutputMode->nYRes);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnXmlReadUInt32Attribute(pOpcode, "FPS", &pMapOutputMode->nFPS);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnXmlReadWaveOutputMode(const TiXmlElement* pOpcode, XnWaveOutputMode* pWaveOutputMode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnXmlReadUInt32Attribute(pOpcode, "sampleRate", &pWaveOutputMode->nSampleRate);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnXmlReadUInt16Attribute(pOpcode, "bitsPerSample", &pWaveOutputMode->nBitsPerSample);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnXmlReadUInt8Attribute(pOpcode, "channels", &pWaveOutputMode->nChannels);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnXmlReadCropping(const TiXmlElement* pOpcode, XnCropping* pCropping)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnXmlReadBoolAttribute(pOpcode, "enabled", &pCropping->bEnabled);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnXmlReadUInt16Attribute(pOpcode, "xOffset", &pCropping->nXOffset);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnXmlReadUInt16Attribute(pOpcode, "yOffset", &pCropping->nYOffset);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnXmlReadUInt16Attribute(pOpcode, "xSize", &pCropping->nXSize);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnXmlReadUInt16Attribute(pOpcode, "ySize", &pCropping->nYSize);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnXmlReadVecotr3D(const TiXmlElement* pOpcode, XnVector3D* pVector)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDouble dValue;
	nRetVal = xnXmlReadRealAttribute(pOpcode, "x", &dValue);
	XN_IS_STATUS_OK(nRetVal);
	pVector->X = (XnFloat)dValue;

	nRetVal = xnXmlReadRealAttribute(pOpcode, "y", &dValue);
	XN_IS_STATUS_OK(nRetVal);
	pVector->Y = (XnFloat)dValue;

	nRetVal = xnXmlReadRealAttribute(pOpcode, "z", &dValue);
	XN_IS_STATUS_OK(nRetVal);
	pVector->Z = (XnFloat)dValue;

	return (XN_STATUS_OK);
}

XnStatus xnXmlReadBoundingBox3D(const TiXmlElement* pElem, XnBoundingBox3D* pBox)
{
	XnStatus nRetVal = XN_STATUS_OK;

	const TiXmlElement* pMin;
	nRetVal = xnXmlGetChildElement(pElem, "Min", &pMin);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnXmlReadVecotr3D(pMin, &pBox->LeftBottomNear);
	XN_IS_STATUS_OK(nRetVal);

	const TiXmlElement* pMax;
	nRetVal = xnXmlGetChildElement(pElem, "Max", &pMax);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnXmlReadVecotr3D(pMax, &pBox->RightTopFar);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnXmlReadUserPosition(const TiXmlElement* pOpcode, XnInt* pnIndex, XnBoundingBox3D* pUserPosition)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnXmlReadIntAttribute(pOpcode, "index", pnIndex);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnXmlReadBoundingBox3D(pOpcode, pUserPosition);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnConfigureMirror(XnNodeHandle hNode, const TiXmlElement* pOpcode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnBool bOn;
	nRetVal = xnXmlReadBoolAttribute(pOpcode, "on", &bOn);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetMirror(hNode, bOn);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnConfigureMapOutputMode(XnNodeHandle hNode, const TiXmlElement* pOpcode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnMapOutputMode Mode;
	nRetVal = xnXmlReadMapOutputMode(pOpcode, &Mode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetMapOutputMode(hNode, &Mode);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnConfigureWaveOutputMode(XnNodeHandle hNode, const TiXmlElement* pOpcode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnWaveOutputMode Mode;
	nRetVal = xnXmlReadWaveOutputMode(pOpcode, &Mode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetWaveOutputMode(hNode, &Mode);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnConfigureCropping(XnNodeHandle hNode, const TiXmlElement* pOpcode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnCropping Cropping;
	nRetVal = xnXmlReadCropping(pOpcode, &Cropping);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetCropping(hNode, &Cropping);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnConfigurePixelFormat(XnNodeHandle hNode, const TiXmlElement* pOpcode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnPixelFormat format;
	nRetVal = xnPixelFormatFromString(pOpcode->GetText(), &format);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetPixelFormat(hNode, format);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnConfigureUserPosition(XnNodeHandle hNode, const TiXmlElement* pOpcode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnInt nIndex;
	XnBoundingBox3D UserPosition;
	nRetVal = xnXmlReadUserPosition(pOpcode, &nIndex, &UserPosition);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetUserPosition(hNode, nIndex, &UserPosition);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnConfigureProperty(XnNodeHandle hNode, const TiXmlElement* pOpcode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	const XnChar* strName;
	nRetVal = xnXmlReadStringAttribute(pOpcode, "name", &strName);
	XN_IS_STATUS_OK(nRetVal);

	const XnChar* strType;
	nRetVal = xnXmlReadStringAttribute(pOpcode, "type", &strType);
	XN_IS_STATUS_OK(nRetVal);

	if (strcmp(strType, "int") == 0)
	{
		XnInt nValue;
		nRetVal = xnXmlReadIntAttribute(pOpcode, "value", &nValue);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnSetIntProperty(hNode, strName, nValue);
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogError(XN_MASK_OPEN_NI, "Failed to set property '%s' from xml: %s", strName, xnGetStatusString(nRetVal));
			return nRetVal;
		}
	}
	else if (strcmp(strType, "real") == 0)
	{
		XnDouble dValue;
		nRetVal = xnXmlReadRealAttribute(pOpcode, "value", &dValue);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnSetRealProperty(hNode, strName, dValue);
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogError(XN_MASK_OPEN_NI, "Failed to set property '%s' from xml: %s", strName, xnGetStatusString(nRetVal));
			return nRetVal;
		}
	}
	else if (strcmp(strType, "string") == 0)
	{
		const XnChar* strValue;
		nRetVal = xnXmlReadStringAttribute(pOpcode, "value", &strValue);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnSetStringProperty(hNode, strName, strValue);
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogError(XN_MASK_OPEN_NI, "Failed to set property '%s' from xml: %s", strName, xnGetStatusString(nRetVal));
			return nRetVal;
		}
	}
	else
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Invalid property type: %s", strType);
	}

	return (XN_STATUS_OK);
}

XnStatus xnConfigureFrameSync(XnNodeHandle hNode, const TiXmlElement* pOpcode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnContext* pContext = hNode->pContext;
	XnNodeHandle hOther = NULL;

	nRetVal = xnGetRefNodeHandleByName(pContext, pOpcode->GetText(), &hOther);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnFrameSyncWith(hNode, hOther);
	xnProductionNodeRelease(hOther);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnConfigureAlternativeViewPoint(XnNodeHandle hNode, const TiXmlElement* pOpcode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (!xnIsCapabilitySupported(hNode, XN_CAPABILITY_ALTERNATIVE_VIEW_POINT))
	{
		return XN_STATUS_INVALID_OPERATION;
	}

	XnContext* pContext = hNode->pContext;
	XnNodeHandle hOther = NULL;

	nRetVal = xnGetRefNodeHandleByName(pContext, pOpcode->GetText(), &hOther);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetViewPoint(hNode, hOther);
	xnProductionNodeRelease(hOther);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnConfigureRecorderDestination(XnNodeHandle hNode, const TiXmlElement* pOpcode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnRecordMedium medium = XN_RECORD_MEDIUM_FILE; // default

	if (pOpcode->Attribute("medium") != NULL)
	{
		const XnChar* strMedium;
		nRetVal = xnXmlReadStringAttribute(pOpcode, "medium", &strMedium);
		XN_IS_STATUS_OK(nRetVal);

		if (strcmp(strMedium, "File") == 0)
		{
			medium = XN_RECORD_MEDIUM_FILE;
		}
		else
		{
			XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Unknown recording medium: '%s'", strMedium);
		}
	}

	const XnChar* strName;
	nRetVal = xnXmlReadStringAttribute(pOpcode, "name", &strName);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnSetRecorderDestination(hNode, medium, strName);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnConfigureAddNodeToRecording(XnNodeHandle hNode, const TiXmlElement* pOpcode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	const XnChar* strName;
	nRetVal = xnXmlReadStringAttribute(pOpcode, "name", &strName);
	XN_IS_STATUS_OK(nRetVal);

	const XnChar* strCodec;
	nRetVal = xnXmlReadStringAttribute(pOpcode, "codec", &strCodec);
	XN_IS_STATUS_OK(nRetVal);

	// find node
	XnContext* pContext = hNode->pContext;
	XnNodeHandle hOther = NULL;
	nRetVal = xnGetRefNodeHandleByName(pContext, strName, &hOther);
	XN_IS_STATUS_OK(nRetVal);

	// find codec
	if (strlen(strCodec) != sizeof(XnCodecID))
	{
		xnProductionNodeRelease(hOther);
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "'%s' is not a valid codec ID!", strCodec);
	}

	XnCodecID codecID;
	xnOSMemCopy(&codecID, strCodec, sizeof(codecID));

	nRetVal = xnAddNodeToRecording(hNode, hOther, codecID);
	xnProductionNodeRelease(hOther);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus xnConfigureSetOpcode(XnNodeHandle hNode, const TiXmlElement* pOpcode)
{
	const XnChar* strOpcode = pOpcode->Value();

	if (strcmp(strOpcode, "Mirror") == 0)
	{
		return xnConfigureMirror(hNode, pOpcode);
	}
	else if (strcmp(strOpcode, "MapOutputMode") == 0)
	{
		return xnConfigureMapOutputMode(hNode, pOpcode);
	}
	else if (strcmp(strOpcode, "WaveOutputMode") == 0)
	{
		return xnConfigureWaveOutputMode(hNode, pOpcode);
	}
	else if (strcmp(strOpcode, "Cropping") == 0)
	{
		return xnConfigureCropping(hNode, pOpcode);
	}
	else if (strcmp(strOpcode, "PixelFormat") == 0)
	{
		return xnConfigurePixelFormat(hNode, pOpcode);
	}
	else if (strcmp(strOpcode, "UserPosition") == 0)
	{
		return xnConfigureUserPosition(hNode, pOpcode);
	}
	else if (strcmp(strOpcode, "Property") == 0)
	{
		return xnConfigureProperty(hNode, pOpcode);
	}
	else if (strcmp(strOpcode, "FrameSync") == 0)
	{
		return xnConfigureFrameSync(hNode, pOpcode);
	}
	else if (strcmp(strOpcode, "AlternativeViewPoint") == 0)
	{
		return xnConfigureAlternativeViewPoint(hNode, pOpcode);
	}
	else if (strcmp(strOpcode, "RecorderDestination") == 0)
	{
		return xnConfigureRecorderDestination(hNode, pOpcode);
	}
	else if (strcmp(strOpcode, "AddNodeToRecording") == 0)
	{
		return xnConfigureAddNodeToRecording(hNode, pOpcode);
	}
	else
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Invalid configuration option: %s", strOpcode);
	}
}

XnStatus xnConfigureNodeFromXml(XnNodeHandle hNode, const TiXmlElement* pNode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	const TiXmlElement* pConfig = pNode->FirstChildElement("Configuration");
	if (pConfig == NULL)
	{
		return (XN_STATUS_OK);
	}

	XnBool bLock = FALSE;
	const XnChar* strLock = pConfig->Attribute("lock");
	if (strLock != NULL)
	{
		xnXmlReadBoolAttribute(pConfig, "lock", &bLock);
	}

	XnLockHandle hLock = 0;

	if (bLock)
	{
		nRetVal = xnLockNodeForChanges(hNode, &hLock);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnLockedNodeStartChanges(hNode, hLock);
		XN_IS_STATUS_OK(nRetVal);
	}

	const TiXmlElement* pOpcode = pConfig->FirstChildElement();
	while (pOpcode != NULL)
	{
		nRetVal = xnConfigureSetOpcode(hNode, pOpcode);
		XN_IS_STATUS_OK(nRetVal);

		pOpcode = pOpcode->NextSiblingElement();
	}

	if (bLock)
	{
		nRetVal = xnLockedNodeEndChanges(hNode, hLock);
	}

	return (XN_STATUS_OK);
}

static XnBool xnVersionGetNext(XnChar* strCurrent, XnChar** pstrNext)
{
	XnChar* strPoint = strchr(strCurrent, '.');
	if (strPoint == NULL)
	{
		return FALSE;
	}

	*strPoint = '\0';
	*pstrNext = strPoint + 1;

	return TRUE;
}

XnBool xnReadVersionFromString(const XnChar* strVersion, XnVersion* pVersion)
{
	XnChar csVersion[XN_MAX_NAME_LENGTH];
	strcpy(csVersion, strVersion);

	XnChar* strMajor = csVersion;
	XnChar* strMinor;
	XnChar* strMaintenance;
	XnChar* strBuild;

	if (xnVersionGetNext(strMajor, &strMinor) &&
		xnVersionGetNext(strMinor, &strMaintenance) &&
		xnVersionGetNext(strMaintenance, &strBuild))
	{
		if (0 == sscanf(strMajor, "%hhu", &pVersion->nMajor) ||
			0 == sscanf(strMinor, "%hhu", &pVersion->nMinor) ||
			0 == sscanf(strMaintenance, "%hu", &pVersion->nMaintenance) ||
			0 == sscanf(strBuild, "%u", &pVersion->nBuild))
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

XnStatus xnReadVersionFromXml(const TiXmlElement* pElem, XnVersion* pVersion)
{
	if (!xnReadVersionFromString(pElem->GetText(), pVersion))
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI,
			"Invalid '%s' xml entry - version string is not legal (line %u, col %u)!",
			pElem->Value(), pElem->Row(), pElem->Column());
	}

	return (XN_STATUS_OK);
}

XnStatus xnXmlReadQuery(const TiXmlElement* pQueryElem, XnNodeQuery* pQuery)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// vendor
	const TiXmlElement* pVendor = pQueryElem->FirstChildElement("Vendor");
	if (pVendor != NULL)
	{
		xnNodeQuerySetVendor(pQuery, pVendor->GetText());
	}

	// name
	const TiXmlElement* pName = pQueryElem->FirstChildElement("Name");
	if (pName != NULL)
	{
		xnNodeQuerySetName(pQuery, pName->GetText());
	}

	// Min version
	const TiXmlElement* pMinVersion = pQueryElem->FirstChildElement("MinVersion");
	if (pMinVersion != NULL)
	{
		XnVersion minVersion;
		nRetVal = xnReadVersionFromXml(pMinVersion, &minVersion);
		XN_IS_STATUS_OK(nRetVal);

		xnNodeQuerySetMinVersion(pQuery, &minVersion);
	}

	// Max version
	const TiXmlElement* pMaxVersion = pQueryElem->FirstChildElement("MaxVersion");
	if (pMaxVersion != NULL)
	{
		XnVersion maxVersion;
		nRetVal = xnReadVersionFromXml(pMaxVersion, &maxVersion);
		XN_IS_STATUS_OK(nRetVal);

		xnNodeQuerySetMaxVersion(pQuery, &maxVersion);
	}

	// Capabilities
	const TiXmlElement* pCapabilities = pQueryElem->FirstChildElement("Capabilities");
	if (pCapabilities != NULL)
	{
		const TiXmlElement* pCap = pCapabilities->FirstChildElement("Capability");
		while (pCap != NULL)
		{
			xnNodeQueryAddSupportedCapability(pQuery, pCap->GetText());
			pCap = pCap->NextSiblingElement("Capability");
		}
	}

	// Map Output Modes
	const TiXmlElement* pOutputModes = pQueryElem->FirstChildElement("MapOutputModes");
	if (pOutputModes != NULL)
	{
		const TiXmlElement* pMode = pOutputModes->FirstChildElement("MapOutputMode");
		while (pMode != NULL)
		{
			XnMapOutputMode Mode;
			nRetVal = xnXmlReadMapOutputMode(pMode, &Mode);
			XN_IS_STATUS_OK(nRetVal);

			xnNodeQueryAddSupportedMapOutputMode(pQuery, &Mode);

			pMode = pMode->NextSiblingElement("MapOutputMode");
		}
	}

	// Min User Position
	const TiXmlElement* pMinUserPositions = pQueryElem->FirstChildElement("MinUserPositions");
	if (pMinUserPositions != NULL)
	{
		XnInt nMinUserPositions;
		nRetVal = xnXmlReadTextAsInt(pMinUserPositions, &nMinUserPositions);
		XN_IS_STATUS_OK(nRetVal);

		xnNodeQuerySetSupportedMinUserPositions(pQuery, nMinUserPositions);
	}

	// Existing Node Only
	XnBool bExistingOnly = FALSE;
	const TiXmlElement* pExistingOnly = pQueryElem->FirstChildElement("ExistingNodeOnly");
	if (pExistingOnly != NULL)
	{
		xnNodeQuerySetExistingNodeOnly(pQuery, TRUE);
		bExistingOnly = TRUE;
	}

	// Non Existing Node Only
	const TiXmlElement* pNonExistingOnly = pQueryElem->FirstChildElement("NonExistingNodeOnly");
	if (pNonExistingOnly != NULL)
	{
		if (bExistingOnly)
		{
			xnLogError(XN_MASK_OPEN_NI, "Cannot specify both <ExistingNodeOnly> and <NonExistingNodeOnly> in query");
			XN_ASSERT(FALSE);
			return XN_STATUS_INVALID_OPERATION;
		}
		xnNodeQuerySetNonExistingNodeOnly(pQuery, TRUE);
	}

	// Needed Nodes
	const TiXmlElement* pNeededNodes = pQueryElem->FirstChildElement("NeededNodes");
	if (pNeededNodes != NULL)
	{
		const TiXmlElement* pNode = pNeededNodes->FirstChildElement("Node");
		while (pNode != NULL)
		{
			xnNodeQueryAddNeededNode(pQuery, pNode->GetText());

			pNode = pNode->NextSiblingElement("Node");
		}
	}

	// Creation info
	const TiXmlElement* pCreationInfo = pQueryElem->FirstChildElement("CreationInfo");
	if (pCreationInfo != NULL)
	{
		xnNodeQuerySetCreationInfo(pQuery, pCreationInfo->GetText());
	}

	return (XN_STATUS_OK);
}

XnStatus xnConfigureCreateNodes(XnContext* pContext, const TiXmlElement* pRootElem, XnNodeInfoList* pCreatedNodes, XnEnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;

	const TiXmlElement* pProudctionNodes = pRootElem->FirstChildElement("ProductionNodes");
	if (pProudctionNodes == NULL)
	{
		return (XN_STATUS_OK);
	}

	// global mirror
	const TiXmlElement* pGlobalMirror = pProudctionNodes->FirstChildElement("GlobalMirror");
	if (pGlobalMirror != NULL)
	{
		XnBool bOn;
		nRetVal = xnXmlReadBoolAttribute(pGlobalMirror, "on", &bOn);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnSetGlobalMirror(pContext, bOn);
		XN_IS_STATUS_OK(nRetVal);
	}

	// file recordings
	const TiXmlElement* pRecording = pProudctionNodes->FirstChildElement("Recording");
	if (pRecording != NULL)
	{
		const XnChar* strFileName;
		nRetVal = xnXmlReadStringAttribute(pRecording, "file", &strFileName);
		XN_IS_STATUS_OK(nRetVal);

		xnLogVerbose(XN_MASK_OPEN_NI, "Opening file recording '%s'...", strFileName);

		XnNodeHandle hPlayer;
		nRetVal = xnContextOpenFileRecordingEx(pContext, strFileName, &hPlayer);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnNodeInfoListAddNode(pCreatedNodes, hPlayer->pNodeInfo);
		if (nRetVal != XN_STATUS_OK)
		{
			xnProductionNodeRelease(hPlayer);
			return (nRetVal);
		}

		XnDouble dSpeed = 1.0;
		if (NULL != pRecording->Attribute("playbackSpeed", &dSpeed))
		{
			nRetVal = xnSetPlaybackSpeed(hPlayer, dSpeed);
			XN_IS_STATUS_OK(nRetVal);
		}

		const XnChar* REPEAT = "repeat";
		if (NULL != pRecording->Attribute(REPEAT))
		{
			XnBool bRepeat;
			nRetVal = xnXmlReadBoolAttribute(pRecording, REPEAT, &bRepeat);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = xnSetPlayerRepeat(hPlayer, bRepeat);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	const XnChar* strNodeTagName = "Node";
	const XnChar* strStartGeneratingAttr = "startGenerating";

	XnBool bStartGeneratingAll = TRUE;
	if (NULL != pProudctionNodes->Attribute(strStartGeneratingAttr))
	{
		nRetVal = xnXmlReadBoolAttribute(pProudctionNodes, strStartGeneratingAttr, &bStartGeneratingAll);
		XN_IS_STATUS_OK(nRetVal);
	}

	// new nodes
	const TiXmlElement* pNode = pProudctionNodes->FirstChildElement(strNodeTagName);
	while (pNode != NULL)
	{
		// get type
		const XnChar* strType;
		nRetVal = xnXmlReadStringAttribute(pNode, "type", &strType);
		XN_IS_STATUS_OK(nRetVal);

		// check stopOnError status
		XnBool bStopOnError = TRUE;
		if (NULL != pNode->Attribute("stopOnError"))
		{
			nRetVal = xnXmlReadBoolAttribute(pNode, "stopOnError", &bStopOnError);
			XN_IS_STATUS_OK(nRetVal);
		}

		xnLogVerbose(XN_MASK_OPEN_NI, "Requested to create a node of type %s%s...", strType, bStopOnError ? "" : " (StopOnError=FALSE)");

		XnProductionNodeType Type;
		nRetVal = xnProductionNodeTypeFromString(strType, &Type);
		XN_IS_STATUS_OK(nRetVal);

		// check if there is a query
		XnNodeQuery* pQuery = NULL;
		const TiXmlElement* pQueryElem = pNode->FirstChildElement("Query");
		if (pQueryElem != NULL)
		{
			nRetVal = xnNodeQueryAllocate(&pQuery);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = xnXmlReadQuery(pQueryElem, pQuery);
			XN_IS_STATUS_OK(nRetVal);
		}

		// enumerate
		XnNodeInfoList* pTrees;
		nRetVal = xnEnumerateProductionTrees(pContext, Type, pQuery, &pTrees, pErrors);
		if (nRetVal == XN_STATUS_NO_NODE_PRESENT && !bStopOnError)
		{
			// go to next one
			pNode = pNode->NextSiblingElement(strNodeTagName);
			continue;
		}
		XN_IS_STATUS_OK(nRetVal);

		if (pQuery != NULL)
		{
			xnNodeQueryFree(pQuery);
			pQuery = NULL;
		}

		// choose first one
		XnNodeInfoListIterator itChosen = xnNodeInfoListGetFirst(pTrees);
		XnNodeInfo* pChosenInfo = xnNodeInfoListGetCurrent(itChosen);

		// check if a name was requested
		if (NULL != pNode->Attribute("name"))
		{
			const XnChar* strName = NULL;
			nRetVal = xnXmlReadStringAttribute(pNode, "name", &strName);
			if (nRetVal != XN_STATUS_OK)
			{
				xnNodeInfoListFree(pTrees);
				return (nRetVal);
			}

			nRetVal = xnNodeInfoSetInstanceName(pChosenInfo, strName);
			if (nRetVal != XN_STATUS_OK)
			{
				xnNodeInfoListFree(pTrees);
				return (nRetVal);
			}
		}

		// create it
		XnNodeHandle hNode;
		nRetVal = xnCreateProductionTree(pContext, pChosenInfo, &hNode);
		if (nRetVal != XN_STATUS_OK)
		{
			xnNodeInfoListFree(pTrees);
			return (nRetVal);
		}

		// free the list
		xnNodeInfoListFree(pTrees);

		// add it to the list of created nodes
		nRetVal = xnNodeInfoListAddNode(pCreatedNodes, pChosenInfo);
		if (nRetVal != XN_STATUS_OK)
		{
			xnProductionNodeRelease(hNode);
			return (nRetVal);
		}

		// config it
		nRetVal = xnConfigureNodeFromXml(hNode, pNode);
		if (nRetVal != XN_STATUS_OK)
		{
			xnProductionNodeRelease(hNode);
			return (nRetVal);
		}

		// check if we need to start it (if start generating all is on, it will be started at the end)
		XnBool bStart = FALSE;
		if (!bStartGeneratingAll)
		{
			if (NULL != pNode->Attribute(strStartGeneratingAttr))
			{
				nRetVal = xnXmlReadBoolAttribute(pNode, strStartGeneratingAttr, &bStart);
				if (nRetVal != XN_STATUS_OK)
				{
					xnProductionNodeRelease(hNode);
					return (nRetVal);
				}
			}

			if (bStart)
			{
				nRetVal = xnStartGenerating(hNode);
				if (nRetVal != XN_STATUS_OK)
				{
					xnProductionNodeRelease(hNode);
					return (nRetVal);
				}
			}
		}

		pNode = pNode->NextSiblingElement(strNodeTagName);
	}

	// start generating all created nodes (by the order they were created in)
	if (bStartGeneratingAll)
	{
		XnBool bIsGenerator;

		for (XnNodeInfoListIterator it = xnNodeInfoListGetFirst(pCreatedNodes);
			xnNodeInfoListIteratorIsValid(it);
			it = xnNodeInfoListGetNext(it))
		{
			XnNodeInfo* pNodeInfo = xnNodeInfoListGetCurrent(it);
			nRetVal = TypeManager::GetInstance().IsTypeDerivedFrom(pNodeInfo->Description.Type, XN_NODE_TYPE_GENERATOR, &bIsGenerator);
			XN_IS_STATUS_OK(nRetVal);

			if (bIsGenerator)
			{
				XN_ASSERT(pNodeInfo->hNode != NULL);
				nRetVal = xnStartGenerating(pNodeInfo->hNode);
				XN_IS_STATUS_OK(nRetVal);
			}
		}
	}

	return (XN_STATUS_OK);
}

XnStatus RunXmlScriptImpl(XnContext* pContext, TiXmlDocument* pDoc, XnNodeInfoList* pCreatedNodes, XnEnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;

	TiXmlElement* pRootElem = pDoc->RootElement();
	if (pRootElem != NULL)
	{
		nRetVal = xnLoadLicensesFromXml(pContext, pRootElem);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnConfigureCreateNodes(pContext, pRootElem, pCreatedNodes, pErrors);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnXmlScriptNode::XnXmlScriptNode(xn::Context context) : m_context(context)
{
}

XnXmlScriptNode::~XnXmlScriptNode()
{
}

const XnChar* XnXmlScriptNode::GetSupportedFormat()
{
	return XN_SCRIPT_FORMAT_XML;
}

XnStatus XnXmlScriptNode::LoadScriptFromFile(const XnChar* strFileName)
{
	return xnXmlLoadDocument(m_doc, strFileName);
}

XnStatus XnXmlScriptNode::LoadScriptFromString(const XnChar* strScript)
{
	if (!m_doc.Parse(strScript))
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI,
			"Failed loading xml: %s [row %d, column %d]",
			m_doc.ErrorDesc(), m_doc.ErrorRow(), m_doc.ErrorCol());
	}

	return XN_STATUS_OK;
}

XnStatus XnXmlScriptNode::Run(xn::NodeInfoList& createdNodes, xn::EnumerationErrors& errors)
{
	return RunXmlScriptImpl(m_context.GetUnderlyingObject(), &m_doc, createdNodes.GetUnderlyingObject(), errors.GetUnderlying());
}