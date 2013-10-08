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
#ifndef __XN_PROP_NAMES_H__
#define __XN_PROP_NAMES_H__

//ProductionNode
#define XN_PROP_STATE_READY "xnStateReady" // int. Meant only for mock nodes.

//Generator
#define XN_PROP_IS_GENERATING "xnIsGenerating" //int
#define XN_PROP_MIRROR "xnMirror" //int
#define XN_PROP_TIMESTAMP "xnTimeStamp" //int
#define XN_PROP_FRAME_ID "xnFrameID" //int
#define XN_PROP_NEWDATA "xnNewData" //general. Meant only for mock nodes.
#define XN_PROP_FRAME_SYNCED_WITH "xnFrameSyncedWith" // String. name of the frame synced

//MapGenerator
#define XN_PROP_MAP_OUTPUT_MODE "xnMapOutputMode" //general
#define XN_PROP_SUPPORTED_MAP_OUTPUT_MODES_COUNT "xnSupportedMapOutputModesCount" //int
#define XN_PROP_SUPPORTED_MAP_OUTPUT_MODES "xnSupportedMapOutputModes" //general
#define XN_PROP_CROPPING "xnCropping" //general
#define XN_PROP_BYTES_PER_PIXEL "xnBytesPerPixel" //int

//ImageGenerator
#define XN_PROP_SUPPORTED_PIXEL_FORMATS "xnSupportedPixelFormats" //general
#define XN_PROP_PIXEL_FORMAT "xnPixelFormat" //int

//GestureGenerator
#define XN_PROP_GESTURE_RECOGNIZED "xnGestureRecognized" //general
#define XN_PROP_GESTURE_PROGRESS "xnGestureProgress" //general

//DepthGenerator
#define XN_PROP_DEVICE_MAX_DEPTH "xnDeviceMaxDepth" //int
#define XN_PROP_SUPPORTED_USER_POSITIONS_COUNT "xnSupportedUserPositionsCount" //int
#define XN_PROP_USER_POSITIONS "xnUserPositions" //general
#define XN_PROP_FIELD_OF_VIEW "xnFOV" // general (XnFieldOfView)

//AudioGenerator
#define XN_PROP_WAVE_OUTPUT_MODE "xnWaveOutputMode" //general
#define XN_PROP_WAVE_SUPPORTED_OUTPUT_MODES_COUNT "xnWaveSupportedOutputModesCount" //int
#define XN_PROP_WAVE_SUPPORTED_OUTPUT_MODES "xnWaveSupportedOutputModes" //general

#endif //__XN_PROP_NAMES_H__
