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