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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "SampleDepth.h"


#define SUPPORTED_X_RES 400
#define SUPPORTED_Y_RES 300
#define SUPPORTED_FPS 30
#define MAX_DEPTH_VALUE	15000

SampleDepth::SampleDepth() : 
	m_bGenerating(FALSE),
	m_bDataAvailable(FALSE),
	m_pDepthMap(NULL),
	m_nFrameID(0),
	m_nTimestamp(0),
	m_hScheduler(NULL),
	m_bMirror(FALSE)
{
}

SampleDepth::~SampleDepth()
{
	delete[] m_pDepthMap;
}

XnStatus SampleDepth::Init()
{
	m_pDepthMap = new XnDepthPixel[SUPPORTED_X_RES * SUPPORTED_Y_RES];
	if (m_pDepthMap == NULL)
	{
		return XN_STATUS_ALLOC_FAILED;
	}

	return (XN_STATUS_OK);
}

XnBool SampleDepth::IsCapabilitySupported( const XnChar* strCapabilityName )
{
	// we only support the mirror capability
	return (strcmp(strCapabilityName, XN_CAPABILITY_MIRROR) == 0);
}

XnStatus SampleDepth::StartGenerating()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	m_bGenerating = TRUE;

	// start scheduler thread
	nRetVal = xnOSCreateThread(SchedulerThread, this, &m_hScheduler);
	if (nRetVal != XN_STATUS_OK)
	{
		m_bGenerating = FALSE;
		return (nRetVal);
	}

	m_generatingEvent.Raise();

	return (XN_STATUS_OK);
}

XnBool SampleDepth::IsGenerating()
{
	return m_bGenerating;
}

void SampleDepth::StopGenerating()
{
	m_bGenerating = FALSE;

	// wait for thread to exit
	xnOSWaitForThreadExit(m_hScheduler, 100);

	m_generatingEvent.Raise();
}

XnStatus SampleDepth::RegisterToGenerationRunningChange( XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback )
{
	return m_generatingEvent.Register(handler, pCookie, hCallback);
}

void SampleDepth::UnregisterFromGenerationRunningChange( XnCallbackHandle hCallback )
{
	m_generatingEvent.Unregister(hCallback);
}

XnStatus SampleDepth::RegisterToNewDataAvailable( XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback )
{
	return m_dataAvailableEvent.Register(handler, pCookie, hCallback);
}

void SampleDepth::UnregisterFromNewDataAvailable( XnCallbackHandle hCallback )
{
	m_dataAvailableEvent.Unregister(hCallback);
}

XnBool SampleDepth::IsNewDataAvailable( XnUInt64& nTimestamp )
{
	// return next timestamp
	nTimestamp = 1000000 / SUPPORTED_FPS;
	return m_bDataAvailable;
}

XnStatus SampleDepth::UpdateData()
{
	XnDepthPixel* pPixel = m_pDepthMap;

	// change our internal data, so that pixels go from frameID incrementally in both axes.
	for (XnUInt y = 0; y < SUPPORTED_Y_RES; ++y)
	{
		for (XnUInt x = 0; x < SUPPORTED_X_RES; ++x, ++pPixel)
		{
			*pPixel = (m_nFrameID + x + y) % MAX_DEPTH_VALUE;
		}
	}

	// if needed, mirror the map
	if (m_bMirror)
	{
		XnDepthPixel temp;

		for (XnUInt y = 0; y < SUPPORTED_Y_RES; ++y)
		{
			XnDepthPixel* pUp = &m_pDepthMap[y * SUPPORTED_X_RES];
			XnDepthPixel* pDown = &m_pDepthMap[(y+1) * SUPPORTED_X_RES - 1];

			for (XnUInt x = 0; x < SUPPORTED_X_RES/2; ++x, ++pUp, --pDown)
			{
				temp = *pUp;
				*pUp = *pDown;
				*pDown = temp;
			}
		}
	}

	m_nFrameID++;
	m_nTimestamp += 1000000 / SUPPORTED_FPS;

	// mark that data is old
	m_bDataAvailable = FALSE;
	
	return (XN_STATUS_OK);
}


const void* SampleDepth::GetData()
{
	return m_pDepthMap;
}

XnUInt32 SampleDepth::GetDataSize()
{
	return (SUPPORTED_X_RES * SUPPORTED_Y_RES * sizeof(XnDepthPixel));
}

XnUInt64 SampleDepth::GetTimestamp()
{
	return m_nTimestamp;
}

XnUInt32 SampleDepth::GetFrameID()
{
	return m_nFrameID;
}

xn::ModuleMirrorInterface* SampleDepth::GetMirrorInterface()
{
	return this;
}

XnStatus SampleDepth::SetMirror( XnBool bMirror )
{
	m_bMirror = bMirror;
	m_mirrorEvent.Raise();
	return (XN_STATUS_OK);
}

XnBool SampleDepth::IsMirrored()
{
	return m_bMirror;
}

XnStatus SampleDepth::RegisterToMirrorChange( XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback )
{
	return m_mirrorEvent.Register(handler, pCookie, hCallback);
}

void SampleDepth::UnregisterFromMirrorChange( XnCallbackHandle hCallback )
{
	m_mirrorEvent.Unregister(hCallback);
}

XnUInt32 SampleDepth::GetSupportedMapOutputModesCount()
{
	// we only support a single mode
	return 1;
}

XnStatus SampleDepth::GetSupportedMapOutputModes( XnMapOutputMode aModes[], XnUInt32& nCount )
{
	if (nCount < 1)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	aModes[0].nXRes = SUPPORTED_X_RES;
	aModes[0].nYRes = SUPPORTED_Y_RES;
	aModes[0].nFPS = SUPPORTED_FPS;

	return (XN_STATUS_OK);
}

XnStatus SampleDepth::SetMapOutputMode( const XnMapOutputMode& Mode )
{
	// make sure this is our supported mode
	if (Mode.nXRes != SUPPORTED_X_RES ||
		Mode.nYRes != SUPPORTED_Y_RES ||
		Mode.nFPS != SUPPORTED_FPS)
	{
		return (XN_STATUS_BAD_PARAM);
	}

	return (XN_STATUS_OK);
}

XnStatus SampleDepth::GetMapOutputMode( XnMapOutputMode& Mode )
{
	Mode.nXRes = SUPPORTED_X_RES;
	Mode.nYRes = SUPPORTED_Y_RES;
	Mode.nFPS = SUPPORTED_FPS;

	return (XN_STATUS_OK);
}

XnStatus SampleDepth::RegisterToMapOutputModeChange( XnModuleStateChangedHandler /*handler*/, void* /*pCookie*/, XnCallbackHandle& hCallback )
{
	// no need. we only allow one mode
	hCallback = this;
	return XN_STATUS_OK;
}

void SampleDepth::UnregisterFromMapOutputModeChange( XnCallbackHandle /*hCallback*/ )
{
	// do nothing (we didn't really register)	
}

XnDepthPixel* SampleDepth::GetDepthMap()
{
	return m_pDepthMap;
}

XnDepthPixel SampleDepth::GetDeviceMaxDepth()
{
	return MAX_DEPTH_VALUE;
}

void SampleDepth::GetFieldOfView( XnFieldOfView& FOV )
{
	// some numbers
	FOV.fHFOV = 1.35;
	FOV.fVFOV = 1.35;
}

XnStatus SampleDepth::RegisterToFieldOfViewChange( XnModuleStateChangedHandler /*handler*/, void* /*pCookie*/, XnCallbackHandle& hCallback )
{
	// no need. it never changes
	hCallback = this;
	return XN_STATUS_OK;
}

void SampleDepth::UnregisterFromFieldOfViewChange( XnCallbackHandle /*hCallback*/ )
{
	// do nothing (we didn't really register)	
}

XN_THREAD_PROC SampleDepth::SchedulerThread( void* pCookie )
{
	SampleDepth* pThis = (SampleDepth*)pCookie;

	while (pThis->m_bGenerating)
	{
		// wait 33 ms (to produce 30 FPS)
		xnOSSleep(1000000/SUPPORTED_FPS/1000);

		pThis->OnNewFrame();
	}

	XN_THREAD_PROC_RETURN(0);
}

void SampleDepth::OnNewFrame()
{
	m_bDataAvailable = TRUE;
	m_dataAvailableEvent.Raise();
}
