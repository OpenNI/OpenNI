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

