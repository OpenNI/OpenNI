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
