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
#ifndef __XN_ADDITIONAL_CONTROLS_PROPERTY_PAGE_H__
#define __XN_ADDITIONAL_CONTROLS_PROPERTY_PAGE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <streams.h>
#include <XnPlatform.h>
#include "IAdditionalControls.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnAdditionalControlsPropertyPage : public CBasePropertyPage
{
public:
	XnAdditionalControlsPropertyPage(IUnknown* pUnk);

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

	virtual HRESULT OnConnect(IUnknown *pUnknown);
	virtual HRESULT OnDisconnect();
	virtual HRESULT OnActivate();
	virtual HRESULT OnApplyChanges();
	virtual INT_PTR OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

private:
	typedef struct Values
	{
		XnPowerLineFrequency nPowerLineFrequency;
		XnInt32 nGain;
		XnBool bAutoGain;
		XnBool bLowLightCompensation;
	} Values;

	void SetDirty();
	void SetDlgGainValue(XnInt32 nValue);
	void SetDlgPowerLineValue(XnPowerLineFrequency nValue);
	XnPowerLineFrequency GetDlgPowerLineValue();

	IAdditionalControls* m_pControls;
	Values m_defaultValues;
	Values m_oldValues;
	Values m_currentValues;

	HWND m_hGainSlide;
	HWND m_hAutoGainCheck;
	HWND m_hPowerLineComboBox;
	HWND m_hLowLightCheck;
};

#endif // __XN_ADDITIONAL_CONTROLS_PROPERTY_PAGE_H__