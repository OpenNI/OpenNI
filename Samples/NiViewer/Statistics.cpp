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
// --------------------------------
// Include
// --------------------------------
#include "Statistics.h"
#include "Device.h"
#include "Draw.h"
#include <math.h>

// --------------------------------
// Global Variables
// --------------------------------
XnPixelStatistics* g_PixelStatistics = NULL;

enum XnCollectionStatus
{
	NOT_COLLECTING,
	SHOULD_COLLECT,
	COLLECTING,
	COLLECTION_ENDED,
} g_StatisticsState;

// --------------------------------
// Code
// --------------------------------
void statisticsInit()
{
	g_PixelStatistics = NULL;
	g_StatisticsState = NOT_COLLECTING;

	const DepthMetaData* pDepthMD = getDepthMetaData();
	if (pDepthMD != NULL)
	{
		g_PixelStatistics = new XnPixelStatistics[pDepthMD->FullXRes() * pDepthMD->FullYRes()];
	}
}

void statisticsStart(int)
{
	const DepthMetaData* pDepthMD = getDepthMetaData();

	if (pDepthMD == NULL)
	{
		displayMessage("Can't collect statistics: Depth is turned off!");
		return;
	}

	int nCount = pDepthMD->FullXRes() * pDepthMD->FullYRes();

	XnDepthPixel nMaxDepth = getDepthGenerator()->GetDeviceMaxDepth();

	XnPixelStatistics* pPixel = g_PixelStatistics;
	for (int i = 0; i < nCount; ++i, ++pPixel)
	{
		xnOSMemSet(pPixel, 0, sizeof(XnPixelStatistics));
		pPixel->nMin = nMaxDepth;
	}

	g_StatisticsState = SHOULD_COLLECT;
}

void toggleStatistics(int)
{
	switch (g_StatisticsState)
	{
	case SHOULD_COLLECT:
	case COLLECTING:
		statisticsStop(0);
		break;
	case NOT_COLLECTING:
	case COLLECTION_ENDED:
		statisticsStart(0);
		break;
	}
}

void statisticsStop(int)
{
	g_StatisticsState = COLLECTION_ENDED;
}

void statisticsClear(int)
{
	g_StatisticsState = NOT_COLLECTING;
}

void statisticsAddFrame()
{
	if (g_StatisticsState == SHOULD_COLLECT)
	{
		g_StatisticsState = COLLECTING;
	}

	if (g_StatisticsState == COLLECTING)
	{
		const DepthMetaData* pDepthMD = getDepthMetaData();

		const XnDepthPixel* pDepth = pDepthMD->Data();

		for (XnUInt32 y = pDepthMD->YOffset(); y < pDepthMD->YRes() + pDepthMD->YOffset(); ++y)
		{
			XnPixelStatistics* pPixel = &g_PixelStatistics[y*pDepthMD->FullXRes() + pDepthMD->XOffset()];
			for (XnUInt32 x = pDepthMD->XOffset(); x < pDepthMD->XRes() + pDepthMD->XOffset(); ++x, ++pDepth, ++pPixel)
			{
				XnDepthPixel nDepth = *pDepth;

				if (nDepth > pPixel->nMax)
					pPixel->nMax = nDepth;

				if (nDepth < pPixel->nMin)
					pPixel->nMin = nDepth;

				pPixel->nSum += nDepth;
				pPixel->nSumSquare += (nDepth * nDepth);
				pPixel->nCount++;
				pPixel->dAverage = pPixel->nSum / pPixel->nCount;
				pPixel->dStdDev = sqrt( (1.0/pPixel->nCount) * (pPixel->nSumSquare) - (pPixel->dAverage * pPixel->dAverage) );
			}
		}
	}
}

bool isStatisticsActive()
{
	return (g_StatisticsState == COLLECTING || g_StatisticsState == COLLECTION_ENDED);
}

void getStatisticsMessage(char* csMessage)
{
	switch (g_StatisticsState)
	{
	case NOT_COLLECTING:
	case SHOULD_COLLECT:
		csMessage[0] = 0;
		break;
	case COLLECTING:
	case COLLECTION_ENDED:
		const DepthMetaData* pDepthMD = getDepthMetaData();
		sprintf(csMessage, "Collected Statistics for %u frames", g_PixelStatistics[pDepthMD->YOffset()*pDepthMD->FullXRes() + pDepthMD->XOffset()].nCount);
		break;
	}
}