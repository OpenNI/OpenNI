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
