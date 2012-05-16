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
