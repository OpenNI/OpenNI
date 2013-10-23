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
