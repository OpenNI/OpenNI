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
#ifndef __I_ADDITIONAL_CONTROLS_H__
#define __I_ADDITIONAL_CONTROLS_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnTypes.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class IAdditionalControls : public IUnknown
{
public:
	STDMETHOD(GetPowerLineFrequencyDefault)(XnPowerLineFrequency* pnValue) = 0;
	STDMETHOD(GetPowerLineFrequency)(XnPowerLineFrequency *pnValue) = 0;
	STDMETHOD(SetPowerLineFrequency)(XnPowerLineFrequency nValue) = 0;
	STDMETHOD(GetGainRange)(XnInt32 *pnMin, XnInt32* pnMax, XnInt32* pnStep, XnInt32* pnDefault, XnBool* pbAutoSupported) = 0;
	STDMETHOD(GetGain)(XnInt32 *pnValue) = 0;
	STDMETHOD(SetGain)(XnInt32 nValue) = 0;
	STDMETHOD(GetLowLightCompensationDefault)(XnBool* pbValue) = 0;
	STDMETHOD(GetLowLightCompensation)(XnBool *pbValue) = 0;
	STDMETHOD(SetLowLightCompensation)(XnBool bValue) = 0;
};

#endif // __I_ADDITIONAL_CONTROLS_H__

