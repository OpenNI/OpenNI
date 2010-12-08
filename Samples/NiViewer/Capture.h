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



#ifndef __CAPTURE_H__
#define __CAPTURE_H__

// --------------------------------
// Includes
// --------------------------------
#include "Device.h"

// --------------------------------
// Global Variables
// --------------------------------
extern NodeCodec g_DepthFormat;
extern NodeCodec g_ImageFormat;
extern NodeCodec g_IRFormat;
extern NodeCodec g_AudioFormat;

// --------------------------------
// Function Declarations
// --------------------------------
void captureInit(const char* csINIFile);
void captureBrowse(int);
void captureStart(int nDelay);
void captureRestart(int);
void captureStop(int);
XnStatus captureFrame();
void captureSetDepthFormat(int format);
void captureSetImageFormat(int format);
void captureSetIRFormat(int format);
void captureSetAudioFormat(int format);
const char* captureGetDepthFormatName();
const char* captureGetImageFormatName();
const char* captureGetIRFormatName();
const char* captureGetAudioFormatName();
void captureSingleFrame(int);
bool isCapturing();

void getCaptureMessage(char* pMessage);

#endif //__CAPTURE_H__