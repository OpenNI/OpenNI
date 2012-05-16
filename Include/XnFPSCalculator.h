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
#ifndef __XN_FPS_CALCULATOR_H__
#define __XN_FPS_CALCULATOR_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnStatus.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
struct XnFPSDataImpl;
typedef struct XnFPSDataImpl* XnFPSData; 

//---------------------------------------------------------------------------
// Exported Functions
//---------------------------------------------------------------------------
/**
* This function initializes the FPS calculator.
* 
* @param	pFPS			[in]	A pointer to an XnFPSData struct.
* @param	nSamplesCount	[in]	The number of last frames to be kept.
*/
XN_C_API XnStatus XN_C_DECL xnFPSInit(XnFPSData* pFPS, XnUInt32 nSamplesCount);

/**
* This function marks that another frame was processed.
* 
* @param	pFPS	[in]	A pointer to an XnFPSData struct.
* @param	nNow	[in]	Optional. Current time. When not provided, function will take it itself.
*/
XN_C_API XnStatus XN_C_DECL xnFPSMarkFrame(XnFPSData* pFPS, XnUInt64 nNow = 0);
	
/**
* This function calculates the average FPS over the last frames.
* 
* @param	pFPS			[in]	A pointer to an XnFPSData struct.
* @param	nAverageOver	[in]	The number of milliseconds to average over.
* @param	nNow			[in]	Optional. Current time. When not provided, function will take it itself.
*/
XN_C_API XnDouble XN_C_DECL xnFPSCalc(XnFPSData* pFPS, XnUInt32 nAverageOver = 3000, XnUInt64 nNow = 0);

/**
* This function frees the FPS calculator.
* 
* @param	pFPS	[in]	A pointer to an XnFPSData struct.
*/
XN_C_API XnStatus XN_C_DECL xnFPSFree(XnFPSData* pFPS);

#endif //__XN_FPS_CALCULATOR_H__
