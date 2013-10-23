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
#ifndef __STATISTICS_H__
#define __STATISTICS_H__

// --------------------------------
// Includes
// --------------------------------
#include <XnCppWrapper.h>

// --------------------------------
// Defines
// --------------------------------
typedef struct
{
	int nCount;
	XnDepthPixel nMin;
	XnDepthPixel nMax;
	double nSum;
	double nSumSquare;
	double dAverage;
	double dStdDev;
} XnPixelStatistics;

// --------------------------------
// Global Variables
// --------------------------------
extern XnPixelStatistics* g_PixelStatistics;

// --------------------------------
// Function Declarations
// --------------------------------
void statisticsInit();
void statisticsStart(int);
void statisticsStop(int);
void toggleStatistics(int);
void statisticsClear(int);
void statisticsAddFrame();
bool isStatisticsActive();
void getStatisticsMessage(char* csMessage);

#endif //__STATISTICS_H__
