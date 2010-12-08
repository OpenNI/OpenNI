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


#include "MockAudioGenerator.h"


MockAudioGenerator::MockAudioGenerator(const XnChar* strName) : MockGenerator(strName)
{

}

MockAudioGenerator::~MockAudioGenerator()
{

}

XnUChar* MockAudioGenerator::GetAudioBuffer()
{
	return (XnUChar*)GetCurrentData();
}

XnUInt32 MockAudioGenerator::GetSupportedWaveOutputModesCount()
{
	return 1;
}

XnStatus MockAudioGenerator::GetSupportedWaveOutputModes(XnWaveOutputMode aSupportedModes[], XnUInt32& nCount)
{
	aSupportedModes[0].nBitsPerSample = 16;
	aSupportedModes[0].nChannels = 2;
	aSupportedModes[0].nSampleRate = XN_SAMPLE_RATE_44K;

	nCount = 1;
	return XN_STATUS_OK;
}

XnStatus MockAudioGenerator::SetWaveOutputMode(const XnWaveOutputMode& OutputMode)
{
	return XN_STATUS_OK;
}

XnStatus MockAudioGenerator::GetWaveOutputMode(XnWaveOutputMode& OutputMode)
{
	OutputMode.nBitsPerSample = 16;
	OutputMode.nChannels = 2;
	OutputMode.nSampleRate = XN_SAMPLE_RATE_44K;
	return XN_STATUS_OK;
}

XnStatus MockAudioGenerator::RegisterToWaveOutputModeChanges(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback)
{
	return XN_STATUS_OK;
}

void MockAudioGenerator::UnregisterFromWaveOutputModeChanges(XnCallbackHandle hCallback)
{
}