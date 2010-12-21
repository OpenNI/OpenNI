/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




// --------------------------------
// Includes
// --------------------------------
#include "Capture.h"
#include "Device.h"
#include "Draw.h"
#include <XnCppWrapper.h>
#include <XnCodecIDs.h>
using namespace xn;

#if (XN_PLATFORM == XN_PLATFORM_WIN32)
#include <Commdlg.h>
#endif

// --------------------------------
// Defines
// --------------------------------
#define CAPTURED_FRAMES_DIR_NAME "CapturedFrames"

// --------------------------------
// Types
// --------------------------------
typedef enum
{
	NOT_CAPTURING,
	SHOULD_CAPTURE,
	CAPTURING,
} CapturingState;

typedef struct CapturingData
{
	XnCodecID DepthFormat;
	XnCodecID ImageFormat;
	XnCodecID IRFormat;
	XnCodecID AudioFormat;

	Recorder* pRecorder;
	char csFileName[XN_FILE_MAX_PATH];
	XnUInt32 nStartOn; // time to start, in seconds
	CapturingState State;
	XnUInt32 nCapturedFrames;
	XnUInt32 nCapturedFrameUniqueID;
} CapturingData;

// --------------------------------
// Global Variables
// --------------------------------
CapturingData g_Capture;

NodeCodec g_DepthFormat;
NodeCodec g_ImageFormat;
NodeCodec g_IRFormat;
NodeCodec g_AudioFormat;

//#define XN_DONT_CAPTURE (MAX_STRINGS-1)

static const XnCodecID CODEC_DONT_CAPTURE = XN_CODEC_ID(0, 0, 0, 0);

// --------------------------------
// Code
// --------------------------------
void captureInit(const char* csINIFile)
{
	
	// Depth Formats
	int nIndex = 0;

	g_DepthFormat.pValues[nIndex] = XN_CODEC_16Z_EMB_TABLES;
	g_DepthFormat.pIndexToName[nIndex] = "PS Compression (16z ET)";
	nIndex++;

	g_DepthFormat.pValues[nIndex] = CODEC_DONT_CAPTURE;
	g_DepthFormat.pIndexToName[nIndex] = "Not Captured";
	nIndex++;

	g_DepthFormat.nValuesCount = nIndex;

	// Image Formats
	nIndex = 0;

	g_ImageFormat.pValues[nIndex] = XN_CODEC_JPEG;
	g_ImageFormat.pIndexToName[nIndex] = "JPEG";
	nIndex++;

	g_ImageFormat.pValues[nIndex] = XN_CODEC_UNCOMPRESSED;
	g_ImageFormat.pIndexToName[nIndex] = "Uncompressed";
	nIndex++;

 	g_ImageFormat.pValues[nIndex] = CODEC_DONT_CAPTURE;
 	g_ImageFormat.pIndexToName[nIndex] = "Not Captured";
	nIndex++;

	g_ImageFormat.nValuesCount = nIndex;

	// IR Formats
	nIndex = 0;

	g_IRFormat.pValues[nIndex] = XN_CODEC_UNCOMPRESSED;
	g_IRFormat.pIndexToName[nIndex] = "Uncompressed";
	nIndex++;

	g_IRFormat.pValues[nIndex] = CODEC_DONT_CAPTURE;
	g_IRFormat.pIndexToName[nIndex] = "Not Captured";
	nIndex++;

	g_IRFormat.nValuesCount = nIndex;

	// Audio Formats
	nIndex = 0;

	g_AudioFormat.pValues[nIndex] = XN_CODEC_UNCOMPRESSED;
	g_AudioFormat.pIndexToName[nIndex] = "Uncompressed";
	nIndex++;

 	g_AudioFormat.pValues[nIndex] = CODEC_DONT_CAPTURE;
 	g_AudioFormat.pIndexToName[nIndex] = "Not Captured";
	nIndex++;

	g_AudioFormat.nValuesCount = nIndex;

	// Init
	g_Capture.csFileName[0] = 0;
	g_Capture.State = NOT_CAPTURING;
	g_Capture.nCapturedFrameUniqueID = 0;

	g_Capture.DepthFormat = XN_CODEC_16Z_EMB_TABLES;
	g_Capture.ImageFormat = XN_CODEC_JPEG;
	g_Capture.IRFormat = XN_CODEC_UNCOMPRESSED;
	g_Capture.AudioFormat = CODEC_DONT_CAPTURE;

/*
	XnUInt32 nValue;
	if (xnOSReadIntFromINI(csINIFile, "RGBViewer", "DefaultDepthFormat", &nValue) == XN_STATUS_OK)
	{
		g_Capture.DepthFormat = (XnCompressionFormats)nValue;
	}

	if (xnOSReadIntFromINI(csINIFile, "RGBViewer", "DefaultImageFormat", &nValue) == XN_STATUS_OK)
	{
		g_Capture.ImageFormat = (XnCompressionFormats)nValue;
	}

	if (xnOSReadIntFromINI(csINIFile, "RGBViewer", "DefaultIRFormat", &nValue) == XN_STATUS_OK)
	{
		g_Capture.IRFormat = (XnCompressionFormats)nValue;
	}

	if (xnOSReadIntFromINI(csINIFile, "RGBViewer", "DefaultAudioFormat", &nValue) == XN_STATUS_OK)
	{
		g_Capture.AudioFormat = (XnCompressionFormats)nValue;
	}
	*/
}

bool isCapturing()
{
	return (g_Capture.State != NOT_CAPTURING);
}

#define START_CAPTURE_CHECK_RC(rc, what)												\
	if (nRetVal != XN_STATUS_OK)														\
	{																					\
		displayMessage("Failed to %s: %s\n", what, xnGetStatusString(rc));				\
		delete g_Capture.pRecorder;														\
		g_Capture.pRecorder = NULL;														\
		return false;																	\
	}

bool captureOpenWriteDevice()
{
	XnStatus nRetVal = XN_STATUS_OK;
	NodeInfoList recordersList;
	nRetVal = g_Context.EnumerateProductionTrees(XN_NODE_TYPE_RECORDER, NULL, recordersList);
	START_CAPTURE_CHECK_RC(nRetVal, "Enumerate recorders");
	// take first
	NodeInfo chosen = *recordersList.Begin();
	nRetVal = g_Context.CreateProductionTree(chosen);
	START_CAPTURE_CHECK_RC(nRetVal, "Create recorder");

	

	g_Capture.pRecorder = new Recorder;
	nRetVal = chosen.GetInstance(*g_Capture.pRecorder);
	START_CAPTURE_CHECK_RC(nRetVal, "Get recorder instance");

	nRetVal = g_Capture.pRecorder->SetDestination(XN_RECORD_MEDIUM_FILE, g_Capture.csFileName);
	START_CAPTURE_CHECK_RC(nRetVal, "Set output file");

	if (getDevice() != NULL)
	{
		nRetVal = g_Capture.pRecorder->AddNodeToRecording(*getDevice(), XN_CODEC_UNCOMPRESSED);
		START_CAPTURE_CHECK_RC(nRetVal, "add device node");
	}

	if (isDepthOn() && (g_Capture.DepthFormat != CODEC_DONT_CAPTURE))
	{
		nRetVal = g_Capture.pRecorder->AddNodeToRecording(*getDepthGenerator(), g_Capture.DepthFormat);
		START_CAPTURE_CHECK_RC(nRetVal, "add depth node");
	}

	if (isImageOn() && (g_Capture.ImageFormat != CODEC_DONT_CAPTURE))
	{
		nRetVal = g_Capture.pRecorder->AddNodeToRecording(*getImageGenerator(), g_Capture.ImageFormat);
		START_CAPTURE_CHECK_RC(nRetVal, "add image node");
	}

	if (isIROn() && (g_Capture.IRFormat != CODEC_DONT_CAPTURE))
	{
		nRetVal = g_Capture.pRecorder->AddNodeToRecording(*getIRGenerator(), g_Capture.IRFormat);
		START_CAPTURE_CHECK_RC(nRetVal, "add IR stream");
	}

	if (isAudioOn() && (g_Capture.AudioFormat != CODEC_DONT_CAPTURE))
	{
		nRetVal = g_Capture.pRecorder->AddNodeToRecording(*getAudioGenerator(), g_Capture.AudioFormat);
		START_CAPTURE_CHECK_RC(nRetVal, "add Audio stream");
	}

	return true;
}

void captureBrowse(int)
{
#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	OPENFILENAMEA ofn;
	TCHAR *szFilter = TEXT("ONI Files (*.oni)\0")
		TEXT("*.oni\0")
		TEXT("All Files (*.*)\0")
		TEXT("*.*\0");

	ZeroMemory(&ofn,sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = g_Capture.csFileName;
	ofn.nMaxFile = sizeof(g_Capture.csFileName);
	ofn.lpstrTitle = TEXT("Capture to...");
	ofn.nMaxFileTitle = sizeof (ofn.lpstrTitle);
	ofn.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR;

	GetSaveFileName(&ofn); 

	if (g_Capture.csFileName[0] != 0)
	{
		if (strstr(g_Capture.csFileName, ".oni") == NULL)
		{
			strcat(g_Capture.csFileName, ".oni");
		}
	}
#else // not Win32
	strcpy(g_Capture.csFileName, "./Captured.oni");
#endif

	captureOpenWriteDevice();
}

void captureStart(int nDelay)
{
	if (g_Capture.csFileName[0] == 0)
	{
		captureBrowse(0);
	}

	if (g_Capture.csFileName[0] == 0)
		return;

	if (g_Capture.pRecorder == NULL)
	{
		if (!captureOpenWriteDevice())
			return;
	}

	XnUInt64 nNow;
	xnOSGetTimeStamp(&nNow);
	nNow /= 1000;

	g_Capture.nStartOn = (XnUInt32)nNow + nDelay;
	g_Capture.State = SHOULD_CAPTURE;
}

void captureCloseWriteDevice()
{
	if (g_Capture.pRecorder != NULL)
	{
		g_Capture.pRecorder->Release();
		delete g_Capture.pRecorder;
		g_Capture.pRecorder = NULL;
	}
}

void captureRestart(int)
{
	captureCloseWriteDevice();
	if (captureOpenWriteDevice())
		captureStart(0);
}

void captureStop(int)
{
	if (g_Capture.State != NOT_CAPTURING)
	{
		g_Capture.State = NOT_CAPTURING;
		captureCloseWriteDevice();
	}
}

XnStatus captureFrame()
{
	XnStatus nRetVal = XN_STATUS_OK;
	if (g_Capture.State == SHOULD_CAPTURE)
	{
		XnUInt64 nNow;
		xnOSGetTimeStamp(&nNow);
		nNow /= 1000;

		if (nNow >= g_Capture.nStartOn)
		{
			g_Capture.nCapturedFrames = 0;
			g_Capture.State = CAPTURING;
		}
	}

	if (g_Capture.State == CAPTURING)
	{
		nRetVal = g_Capture.pRecorder->Record();
		XN_IS_STATUS_OK(nRetVal);
		g_Capture.nCapturedFrames++;
	}
	return XN_STATUS_OK;
}

void captureSetFormat(XnCodecID* pMember, XnCodecID newFormat, ProductionNode &node)
{
	if (*pMember == newFormat)
		return;

	if (g_Capture.pRecorder != NULL)
	{
		// check if it was off before
		if (*pMember == CODEC_DONT_CAPTURE)
		{
			g_Capture.pRecorder->AddNodeToRecording(node, newFormat);
		}
		// check if it is off now
		else if (newFormat == CODEC_DONT_CAPTURE)
		{
			g_Capture.pRecorder->RemoveNodeFromRecording(node);
		}
		else // just a change in compression
		{
			g_Capture.pRecorder->RemoveNodeFromRecording(node);
			g_Capture.pRecorder->AddNodeToRecording(node, newFormat);
		}
	}

	*pMember = newFormat;
}

void captureSetDepthFormat(int format)
{
	captureSetFormat(&g_Capture.DepthFormat, format, *getDepthGenerator());
}

void captureSetImageFormat(int format)
{
	captureSetFormat(&g_Capture.ImageFormat, format, *getImageGenerator());
}

void captureSetIRFormat(int format)
{
	captureSetFormat(&g_Capture.IRFormat, format, *getIRGenerator());
}

void captureSetAudioFormat(int format)
{
	captureSetFormat(&g_Capture.AudioFormat, format, *getAudioGenerator());
}

const char* getCodecName(NodeCodec *pNodeCodec, XnCodecID codecID)
{
	for (int i = 0; i < pNodeCodec->nValuesCount; i++)
	{
		if (pNodeCodec->pValues[i] == codecID)
		{
			return pNodeCodec->pIndexToName[i];
		}
	}
	return NULL;
}

const char* captureGetDepthFormatName()
{
	return getCodecName(&g_DepthFormat, g_Capture.DepthFormat);
}

const char* captureGetImageFormatName()
{
	return getCodecName(&g_ImageFormat, g_Capture.ImageFormat);
}

const char* captureGetIRFormatName()
{
	return getCodecName(&g_IRFormat, g_Capture.IRFormat);
}

const char* captureGetAudioFormatName()
{
	return getCodecName(&g_AudioFormat, g_Capture.AudioFormat);
}

void getCaptureMessage(char* pMessage)
{
	switch (g_Capture.State)
	{
	case SHOULD_CAPTURE:
		{
			XnUInt64 nNow;
			xnOSGetTimeStamp(&nNow);
			nNow /= 1000;
			sprintf(pMessage, "Capturing will start in %u seconds...", g_Capture.nStartOn - (XnUInt32)nNow);
		}
		break;
	case CAPTURING:
		sprintf(pMessage, "* Recording! * | Frames: %d | Press any key or use menu to stop...", g_Capture.nCapturedFrames); 
		break;
	default:
		pMessage[0] = 0;
	}
}

void getImageFileName(int num, char* csName)
{
	sprintf(csName, "%s/Image_%d.raw", CAPTURED_FRAMES_DIR_NAME, num);
}

void getDepthFileName(int num, char* csName)
{
	sprintf(csName, "%s/Depth_%d.raw", CAPTURED_FRAMES_DIR_NAME, num);
}

void getIRFileName(int num, char* csName)
{
	sprintf(csName, "%s/IR_%d.raw", CAPTURED_FRAMES_DIR_NAME, num);
}

int findUniqueFileName()
{
	xnOSCreateDirectory(CAPTURED_FRAMES_DIR_NAME);

	int num = g_Capture.nCapturedFrameUniqueID;

	XnBool bExist = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;
	XnChar csImageFileName[XN_FILE_MAX_PATH];
	XnChar csDepthFileName[XN_FILE_MAX_PATH];
	XnChar csIRFileName[XN_FILE_MAX_PATH];

	while (true)
	{
		// check image
		getImageFileName(num, csImageFileName);

		nRetVal = xnOSDoesFileExist(csImageFileName, &bExist);
		if (nRetVal != XN_STATUS_OK)
			break;

		if (!bExist)
		{
			// check depth
			getDepthFileName(num, csDepthFileName);

			nRetVal = xnOSDoesFileExist(csDepthFileName, &bExist);
			if (nRetVal != XN_STATUS_OK || !bExist)
				break;
		}

		if (!bExist)
		{
			// check IR
			getIRFileName(num, csIRFileName);

			nRetVal = xnOSDoesFileExist(csIRFileName, &bExist);
			if (nRetVal != XN_STATUS_OK || !bExist)
				break;
		}

		++num;
	}

	return num;
}

void captureSingleFrame(int)
{
	int num = findUniqueFileName();

	XnChar csImageFileName[XN_FILE_MAX_PATH];
	XnChar csDepthFileName[XN_FILE_MAX_PATH];
	XnChar csIRFileName[XN_FILE_MAX_PATH];
	getImageFileName(num, csImageFileName);
	getDepthFileName(num, csDepthFileName);
	getIRFileName(num, csIRFileName);

	const ImageMetaData* pImageMD = getImageMetaData();
	if (pImageMD != NULL)
	{
		xnOSSaveFile(csImageFileName, pImageMD->Data(), pImageMD->DataSize());
	}

	const IRMetaData* pIRMD = getIRMetaData();
	if (pIRMD != NULL)
	{
		xnOSSaveFile(csIRFileName, pIRMD->Data(), pIRMD->DataSize());
	}

	const DepthMetaData* pDepthMD = getDepthMetaData();
	if (pDepthMD != NULL)
	{
		xnOSSaveFile(csDepthFileName, pDepthMD->Data(), pDepthMD->DataSize());
	}
	
	g_Capture.nCapturedFrameUniqueID = num + 1;

	displayMessage("Frames saved with ID %d", num);
}
