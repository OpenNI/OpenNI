/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
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
#include "MockAudioGenerator.h"
#include <XnPropNames.h>
#include <XnOS.h>
#include <XnLog.h>


MockAudioGenerator::MockAudioGenerator(const XnChar* strName) : 
	MockGenerator(strName, TRUE),
	m_pSupportedOutputModes(NULL),
	m_nSupportedOutputModesCount(0)
{
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
	return m_outputModeChangeEvent.Register(handler, pCookie, &hCallback);
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
