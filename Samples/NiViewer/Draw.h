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



#ifndef __DRAW_H__
#define __DRAW_H__

// --------------------------------
// Includes
// --------------------------------
#include <XnPlatform.h>
#include <GL/gl.h>

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

#endif //__DRAW_H__