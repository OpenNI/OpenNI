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


#ifndef __MOCKAUDIOGENERATOR_H__
#define __MOCKAUDIOGENERATOR_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include "MockGenerator.h"

class MockAudioGenerator : 
	public MockGenerator,
	virtual public xn::ModuleAudioGenerator
{
public:
	MockAudioGenerator(const XnChar* strName);
	virtual ~MockAudioGenerator();
	virtual XnUChar* GetAudioBuffer();
	virtual XnUInt32 GetSupportedWaveOutputModesCount();
	virtual XnStatus GetSupportedWaveOutputModes(XnWaveOutputMode aSupportedModes[], XnUInt32& nCount);
	virtual XnStatus SetWaveOutputMode(const XnWaveOutputMode& OutputMode);
	virtual XnStatus GetWaveOutputMode(XnWaveOutputMode& OutputMode);
	virtual XnStatus RegisterToWaveOutputModeChanges(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromWaveOutputModeChanges(XnCallbackHandle hCallback);

protected:
	virtual XnUInt32 GetRequiredBufferSize() { return 0; }
};

#endif // __MOCKAUDIOGENERATOR_H__
