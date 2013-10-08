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
#ifndef __MOCKAUDIOGENERATOR_H__
#define __MOCKAUDIOGENERATOR_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include "MockGenerator.h"

XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_INHERITS_VIA_DOMINANCE_WARNING_ID)

class MockAudioGenerator : 
	public MockGenerator,
	virtual public xn::ModuleAudioGenerator
{
public:
	MockAudioGenerator(xn::Context& context, const XnChar* strName);
	virtual ~MockAudioGenerator();

	/*Generator*/
	virtual const void* GetData() { return MockGenerator::GetData(); }
	XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue);
	XnStatus SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer);

	/*Audio Generator*/
	virtual XnUChar* GetAudioBuffer();
	virtual XnUInt32 GetSupportedWaveOutputModesCount();
	virtual XnStatus GetSupportedWaveOutputModes(XnWaveOutputMode aSupportedModes[], XnUInt32& nCount);
	virtual XnStatus SetWaveOutputMode(const XnWaveOutputMode& OutputMode);
	virtual XnStatus GetWaveOutputMode(XnWaveOutputMode& OutputMode);
	virtual XnStatus RegisterToWaveOutputModeChanges(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromWaveOutputModeChanges(XnCallbackHandle hCallback);

protected:
	virtual XnUInt32 GetRequiredBufferSize() { return 0; }

	PropChangeEvent m_outputModeChangeEvent;

	XnUInt32 m_nSupportedOutputModesCount;
	XnBool m_bSupportedOutputModesCountReceived;
	XnWaveOutputMode m_waveOutputMode;
	XnWaveOutputMode* m_pSupportedOutputModes;
};

XN_PRAGMA_STOP_DISABLED_WARNING_SECTION

#endif // __MOCKAUDIOGENERATOR_H__
