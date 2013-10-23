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
#ifndef __DRAW_H__
#define __DRAW_H__

// --------------------------------
// Includes
// --------------------------------
#include <XnPlatform.h>

#pragma warning(push, 3)
#include <GL/gl.h>
#pragma warning(pop)

// --------------------------------
// Defines
// --------------------------------
#define WIN_SIZE_X 1280
#define WIN_SIZE_Y 1024

#define PRESET_COUNT 13

// --------------------------------
// Types
// --------------------------------
typedef enum
{
	OVERLAY,
	SIDE_BY_SIDE
} ScreenArrangementType;

typedef enum 
{
	DEPTH_OFF,
	LINEAR_HISTOGRAM,
	PSYCHEDELIC,
	PSYCHEDELIC_SHADES,
	RAINBOW,
	CYCLIC_RAINBOW,
	CYCLIC_RAINBOW_HISTOGRAM,
	STANDARD_DEVIATION,
	NUM_OF_DEPTH_TYPES,
} DepthColoringType;

extern const char* g_DepthColoring[NUM_OF_DEPTH_TYPES];

typedef enum
{
	IMAGE_OFF,
	IMAGE_NORMAL,
	DEPTH_MASKED_IMAGE,
	NUM_OF_IMAGE_TYPES,
} ImageColoringType;

extern const char* g_ImageColoring[NUM_OF_IMAGE_TYPES];

typedef struct  
{
	DepthColoringType Coloring;
	float fTransparency;
} DepthDrawConfig;

typedef struct
{
	ImageColoringType Coloring;
} ImageDrawConfig;

typedef struct
{
	bool bBackground;
	DepthDrawConfig Depth;
	ImageDrawConfig Image;
	ScreenArrangementType ScreenArrangement;
} StreamsDrawConfig;

// --------------------------------
// Drawing functions
// --------------------------------
void drawInit();
void displayMessage(const char* csFormat, ...);
void setPreset(int preset);
const char* getPresetName(int preset);
void setScreenLayout(int layout);
void toggleFullScreen(int);
void togglePointerMode(int);
void toggleHelpScreen(int);
void toggleBackground(int);
void drawFrame();
void windowReshaped(int width, int height);
void setDepthDrawing(int nColoring);
void setImageDrawing(int nColoring);
void setErrorState(const char* strMessage);
void resetIRHistogram(int dummy);

#endif //__DRAW_H__
