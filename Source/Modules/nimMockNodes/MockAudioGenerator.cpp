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
#include "MockAudioGenerator.h"
#include <XnPropNames.h>
#include <XnOS.h>
#include <XnLog.h>


MockAudioGenerator::MockAudioGenerator(xn::Context& context, const XnChar* strName) : 
	MockGenerator(context, strName, TRUE),
	m_pSupportedOutputModes(NULL),
	m_nSupportedOutputModesCount(0),
	m_bSupportedOutputModesCountReceived(FALSE)
{
	xnOSMemSet(&m_waveOutputMode, 0, sizeof(m_waveOutputMode));
}

MockAudioGenerator::~MockAudioGenerator()
{
}

XnUChar* MockAudioGenerator::GetAudioBuffer()
{
	return (XnUChar*)GetData();
}

XnUInt32 MockAudioGenerator::GetSupportedWaveOutputModesCount()
{
	return m_nSupportedOutputModesCount;
}

XnStatus MockAudioGenerator::GetSupportedWaveOutputModes(XnWaveOutputMode aSupportedModes[], XnUInt32& nCount)
{
	XN_VALIDATE_PTR(m_pSupportedOutputModes, XN_STATUS_PROPERTY_NOT_SET);
	nCount = XN_MIN(nCount, m_nSupportedOutputModesCount);
	xnOSMemCopy(aSupportedModes, m_pSupportedOutputModes, nCount * sizeof(m_pSupportedOutputModes[0]));
	return XN_STATUS_OK;
}

XnStatus MockAudioGenerator::SetWaveOutputMode(const XnWaveOutputMode& OutputMode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (xnOSMemCmp(&OutputMode, &m_waveOutputMode, sizeof(OutputMode)) != 0)
	{
		m_waveOutputMode = OutputMode;
		nRetVal = m_outputModeChangeEvent.Raise();
		XN_IS_STATUS_OK(nRetVal);
	}
	return XN_STATUS_OK;
}

XnStatus MockAudioGenerator::GetWaveOutputMode(XnWaveOutputMode& OutputMode)
{
	OutputMode = m_waveOutputMode;
	return XN_STATUS_OK;
}

XnStatus MockAudioGenerator::RegisterToWaveOutputModeChanges(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return m_outputModeChangeEvent.Register(handler, pCookie, hCallback);
}

void MockAudioGenerator::UnregisterFromWaveOutputModeChanges(XnCallbackHandle hCallback)
{
	m_outputModeChangeEvent.Unregister(hCallback);
}

XnStatus MockAudioGenerator::SetIntProperty( const XnChar* strName, XnUInt64 nValue )
{
	if (strcmp(strName, XN_PROP_WAVE_SUPPORTED_OUTPUT_MODES_COUNT) == 0 ||
		strcmp(strName, XN_PROP_SUPPORTED_USER_POSITIONS_COUNT) == 0) // Bug workaround: old files wrote down bad name
	{
		m_nSupportedOutputModesCount = (XnUInt32)nValue;
		m_bSupportedOutputModesCountReceived = TRUE;
	}
	else
	{
		return MockGenerator::SetIntProperty(strName, nValue);
	}

	return XN_STATUS_OK;
}

XnStatus MockAudioGenerator::SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer)
{
	XN_VALIDATE_INPUT_PTR(strName);
	XN_VALIDATE_INPUT_PTR(pBuffer);
	XnStatus nRetVal = XN_STATUS_OK;
	if (strcmp(strName, XN_PROP_WAVE_OUTPUT_MODE) == 0)
	{
		if (nBufferSize != sizeof(m_waveOutputMode))
		{
			XN_LOG_ERROR_RETURN(XN_STATUS_INVALID_BUFFER_SIZE, XN_MASK_OPEN_NI, "Cannot set XN_PROP_WAVE_OUTPUT_MODE - buffer size is incorrect");
		}
		const XnWaveOutputMode* pOutputMode = (const XnWaveOutputMode*)pBuffer;
		nRetVal = SetWaveOutputMode(*pOutputMode);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (strcmp(strName, XN_PROP_WAVE_SUPPORTED_OUTPUT_MODES) == 0)
	{
		if (m_bSupportedOutputModesCountReceived)		
		{
			m_bSupportedOutputModesCountReceived = FALSE; //For next time
			if (nBufferSize != m_nSupportedOutputModesCount * sizeof(XnWaveOutputMode))
			{
				XN_LOG_ERROR_RETURN(XN_STATUS_INVALID_BUFFER_SIZE, XN_MASK_OPEN_NI, "Cannot set XN_PROP_SUPPORTED_WAVE_OUTPUT_MODES - buffer size is incorrect");
			}

			XN_DELETE_ARR(m_pSupportedOutputModes);
			m_pSupportedOutputModes = XN_NEW_ARR(XnWaveOutputMode, m_nSupportedOutputModesCount);
			XN_VALIDATE_ALLOC_PTR(m_pSupportedOutputModes);
			xnOSMemCopy(m_pSupportedOutputModes, pBuffer, nBufferSize);
		}
		else
		{
			XN_ASSERT(FALSE);
			XN_LOG_ERROR_RETURN(XN_STATUS_CORRUPT_FILE, XN_MASK_OPEN_NI, "Got XN_PROP_SUPPORTED_WAVE_OUTPUT_MODES without XN_PROP_SUPPORTED_WAVE_OUTPUT_MODES_COUNT before it");
		}
	}
	else if (strcmp(strName, XN_PROP_WAVE_SUPPORTED_OUTPUT_MODES_COUNT) == 0)
	{
		// Bug workaround: old files wrote down bad name with invalid value. Just ignore
	}
	else
	{
		nRetVal = MockGenerator::SetGeneralProperty(strName, nBufferSize, pBuffer);
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
}
