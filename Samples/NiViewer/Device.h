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
#ifndef __DEVICE_H__
#define __DEVICE_H__

// --------------------------------
// Includes
// --------------------------------
#include <XnCppWrapper.h>
#include <XnTypes.h>
using namespace xn;

// --------------------------------
// Defines
// --------------------------------
#define MAX_STRINGS 20

// --------------------------------
// Types
// --------------------------------
typedef struct
{
	int nValuesCount;
	unsigned int pValues[MAX_STRINGS];
	const char* pValueToName[MAX_STRINGS];
} DeviceParameter;

typedef struct
{
	int nValuesCount;
	XnCodecID pValues[MAX_STRINGS];
	const char* pIndexToName[MAX_STRINGS];
} NodeCodec;

typedef struct
{
	int nValuesCount;
	const char* pValues[MAX_STRINGS];
} DeviceStringProperty;

// --------------------------------
// Global Variables
// --------------------------------
extern Context g_Context;

extern DeviceStringProperty g_PrimaryStream;
extern DeviceParameter g_Registration;

// --------------------------------
// Function Declarations
// --------------------------------
XnStatus openDeviceFile(const char* csFile);
XnStatus openDeviceFromXml(const char* csXmlFile, EnumerationErrors& errors);
XnStatus openDeviceFromXmlWithChoice(const char* csXmlFile, EnumerationErrors& errors);
void closeDevice();
void readFrame();
void changeRegistration(int nValue);
void changePrimaryStream(int nValue);
void toggleMirror(int);
void seekFrame(int nDiff);
void toggleDepthState(int nDummy);
void toggleImageState(int nDummy);
void toggleIRState(int nDummy);
void toggleAudioState(int nDummy);
void getDepthFormats(const char** pNames, unsigned int* pValues, int* pCount);
void getImageFormats(const char** pNames, unsigned int* pValues, int* pCount);
void getAudioFormats(const char** pNames, unsigned int* pValues, int* pCount);
void getPrimaryStreams(const char** pNames, unsigned int* pValues, int* pCount);
bool isDepthOn();
bool isImageOn();
bool isIROn();
bool isAudioOn();
bool isPlayerOn();
void setDepthResolution(int res);
void setDepthFPS(int fps);
void setImageResolution(int res);
void setImageFPS(int fps);
void setIRResolution(int res);
void setIRFPS(int fps);
void setStreamCropping(MapGenerator* pGenerator, XnCropping* pCropping);
void setPlaybackSpeed(int ratioDiff);
XnDouble getPlaybackSpeed();
bool getImageCoordinatesForDepthPixel(int x, int y, int& imageX, int& imageY);

Device* getDevice();
DepthGenerator* getDepthGenerator();
ImageGenerator* getImageGenerator();
IRGenerator* getIRGenerator();
AudioGenerator* getAudioGenerator();

const DepthMetaData* getDepthMetaData();
const ImageMetaData* getImageMetaData();
const IRMetaData* getIRMetaData();
const AudioMetaData* getAudioMetaData();

#endif //__DEVICE_H__
