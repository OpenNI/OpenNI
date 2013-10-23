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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnAdditionalControlsPropertyPage.h"
#include "resource.h"
#include "Guids.h"
#include <commctrl.h>
#include <XnTypes.h>

struct PowerLineInfo
{
	XnPowerLineFrequency nValue;
	const WCHAR* strName;
	int nIndex;
};

#define POWER_LINE_COUNT 3

PowerLineInfo g_powerLineInfo[POWER_LINE_COUNT] = 
{
	{ XN_POWER_LINE_FREQUENCY_OFF, L"Off", 0 },
	{ XN_POWER_LINE_FREQUENCY_50_HZ, L"50", 0 },
	{ XN_POWER_LINE_FREQUENCY_60_HZ, L"60", 0 },
};

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnAdditionalControlsPropertyPage::XnAdditionalControlsPropertyPage( IUnknown* pUnk ) :
	CBasePropertyPage(NAME("Additional Control PropPage"), pUnk, IDD_PROPPAGE, IDS_ADDITIONAL_CONTROLS_CAPTION),
	m_pControls(NULL)
{
}

CUnknown * WINAPI XnAdditionalControlsPropertyPage::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
	ASSERT(phr);

	*phr = S_OK;
	CUnknown *punk = new XnAdditionalControlsPropertyPage(lpunk);
	if(punk == NULL)
	{
		*phr = E_OUTOFMEMORY;
	}
	else if (*phr != NOERROR)
	{
		delete punk;
		punk = NULL;
	}

	return punk;
}

HRESULT XnAdditionalControlsPropertyPage::OnConnect( IUnknown *pUnknown )
{
	if (pUnknown == NULL)
	{
		return E_POINTER;
	}
	ASSERT(m_pControls == NULL);
	return pUnknown->QueryInterface(IID_IAdditionalOpenNIControls, (void**)&m_pControls);
}

HRESULT XnAdditionalControlsPropertyPage::OnActivate()
{
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_BAR_CLASSES;
	if (InitCommonControlsEx(&icc) == FALSE)
	{
		return E_FAIL;
	}

	ASSERT(m_pControls != NULL);

	m_hGainSlide = GetDlgItem(m_Dlg, IDC_SLIDER_GAIN);
	m_hAutoGainCheck = GetDlgItem(m_Dlg, IDC_CHECK_AUTO_GAIN);
	m_hLowLightCheck = GetDlgItem(m_Dlg, IDC_CHECK_LOW_LIGHT_COMPENSATION);
	m_hPowerLineComboBox = GetDlgItem(m_Dlg, IDC_COMBO_POWER_LINE);

	XnInt32 nMin, nMax, nStep, nDefault;
	XnBool bAutoSupported;
	HRESULT hr = m_pControls->GetGainRange(&nMin, &nMax, &nStep, &nDefault, &bAutoSupported);
	if (SUCCEEDED(hr))
	{
		m_defaultValues.bAutoGain = (nDefault == XN_AUTO_CONTROL);
		m_defaultValues.nGain = m_defaultValues.bAutoGain ? (nMax - nMin)/2 : nDefault;

		EnableWindow(m_hGainSlide, TRUE);

		SendDlgItemMessage(m_Dlg, IDC_SLIDER_GAIN, TBM_SETRANGE, 0,
			MAKELONG(nMin, nMax));

		SendDlgItemMessage(m_Dlg, IDC_SLIDER_GAIN, TBM_SETTICFREQ, 
			nStep, 0);

		EnableWindow(m_hAutoGainCheck, bAutoSupported);

		XnInt32 nCurrent;
		m_pControls->GetGain(&nCurrent);

		m_currentValues.bAutoGain = (nCurrent == XN_AUTO_CONTROL);
		m_currentValues.nGain = m_currentValues.bAutoGain ? m_defaultValues.nGain : nCurrent;

		Button_SetCheck(m_hAutoGainCheck, m_currentValues.bAutoGain ? BST_CHECKED : BST_UNCHECKED);
		SetDlgGainValue(nDefault);
	}
	else
	{
		EnableWindow(m_hGainSlide, FALSE);
		EnableWindow(m_hAutoGainCheck, FALSE);
	}

	hr = m_pControls->GetLowLightCompensationDefault(&m_defaultValues.bLowLightCompensation);
	if (SUCCEEDED(hr))
	{
		EnableWindow(m_hLowLightCheck, TRUE);
		m_pControls->GetLowLightCompensation(&m_currentValues.bLowLightCompensation);
		Button_SetCheck(m_hLowLightCheck, m_currentValues.bLowLightCompensation ? BST_CHECKED : BST_UNCHECKED);
	}
	else
	{
		EnableWindow(m_hLowLightCheck, FALSE);
		Button_SetCheck(m_hLowLightCheck, BST_UNCHECKED);
	}

	hr = m_pControls->GetPowerLineFrequencyDefault(&m_defaultValues.nPowerLineFrequency);
	if (SUCCEEDED(hr))
	{
		EnableWindow(m_hPowerLineComboBox, TRUE);

		for (int i = 0; i < POWER_LINE_COUNT; ++i)
		{
			g_powerLineInfo[i].nIndex = ComboBox_AddString(m_hPowerLineComboBox, g_powerLineInfo[i].strName);
		}

		m_pControls->GetPowerLineFrequency(&m_currentValues.nPowerLineFrequency);
		SetDlgPowerLineValue(m_currentValues.nPowerLineFrequency);
	}
	else
	{
		EnableWindow(m_hPowerLineComboBox, FALSE);
		SetDlgPowerLineValue(XN_POWER_LINE_FREQUENCY_OFF);
	}

	return hr;
}

void XnAdditionalControlsPropertyPage::SetDlgGainValue(XnInt32 nValue)
{
	SendDlgItemMessage(m_Dlg, IDC_SLIDER_GAIN, TBM_SETPOS, 1, nValue);

	SetDlgItemInt(m_Dlg, IDC_EDIT_GAIN, nValue, TRUE);
}

void XnAdditionalControlsPropertyPage::SetDlgPowerLineValue(XnPowerLineFrequency nValue)
{
	int index = 0;

	for (int i = 0; i < POWER_LINE_COUNT; ++i)
	{
		if (nValue == g_powerLineInfo[i].nValue)
		{
			index = g_powerLineInfo[i].nIndex;
			break;
		}
	}

	ComboBox_SetCurSel(m_hPowerLineComboBox, index);
}

XnPowerLineFrequency XnAdditionalControlsPropertyPage::GetDlgPowerLineValue()
{
	int index = ComboBox_GetCurSel(m_hPowerLineComboBox);

	for (int i = 0; i < POWER_LINE_COUNT; ++i)
	{
		if (index == g_powerLineInfo[i].nIndex)
		{
			return g_powerLineInfo[i].nValue;
		}
	}

	return XN_POWER_LINE_FREQUENCY_OFF;
}

INT_PTR XnAdditionalControlsPropertyPage::OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		{
			int idd = LOWORD(wParam);
			int notification = HIWORD(wParam);

			if (idd == IDC_BUTTON_DEFAULT && notification == BN_CLICKED)
			{
				// User clicked the 'Revert to Default' button.
				m_currentValues = m_defaultValues;

				m_pControls->SetGain(m_currentValues.nGain);
				// Update the slider control.
				SetDlgGainValue(m_currentValues.nGain);

				m_pControls->SetPowerLineFrequency(m_currentValues.nPowerLineFrequency);
				SetDlgPowerLineValue(m_currentValues.nPowerLineFrequency);

				m_pControls->SetLowLightCompensation(m_currentValues.bLowLightCompensation);
				Button_SetCheck(m_hLowLightCheck, m_currentValues.bLowLightCompensation);

				SetDirty();
				return (LRESULT) 1;
			}
			else if (idd == IDC_CHECK_AUTO_GAIN)
			{
				// User changed auto gain state
				m_pControls->SetGain(BST_CHECKED == Button_GetCheck(m_hAutoGainCheck) ? XN_AUTO_CONTROL : m_currentValues.nGain);
				SetDirty();
				return (LRESULT) 1;
			}
			else if (idd == IDC_CHECK_LOW_LIGHT_COMPENSATION)
			{
				// User changed low light compensation state
				m_pControls->SetLowLightCompensation(BST_CHECKED == Button_GetCheck(m_hLowLightCheck));
				SetDirty();
				return (LRESULT) 1;
			}
			else if (idd == IDC_COMBO_POWER_LINE && notification == CBN_SELENDOK)
			{
				m_currentValues.nPowerLineFrequency = GetDlgPowerLineValue();
				m_pControls->SetPowerLineFrequency(m_currentValues.nPowerLineFrequency);
				SetDirty();
				return (LRESULT) 1;
			}
		}
		break;

	case WM_HSCROLL:
		{
			// User moved the slider.
			switch(LOWORD(wParam))
			{
			case TB_PAGEDOWN:
			case SB_THUMBTRACK:
			case TB_PAGEUP:
				m_currentValues.nGain = SendDlgItemMessage(m_Dlg, IDC_SLIDER_GAIN,
					TBM_GETPOS, 0, 0);

				if (!m_currentValues.bAutoGain)
				{
					m_pControls->SetGain(m_currentValues.nGain);
				}

				SetDlgGainValue(m_currentValues.nGain);

				SetDirty();
			}
			return (LRESULT) 1;
		}
	} // Switch.

	// Let the parent class handle the message.
	return CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

HRESULT XnAdditionalControlsPropertyPage::OnApplyChanges(void)
{
	m_oldValues = m_currentValues;
	return S_OK;
} 

HRESULT XnAdditionalControlsPropertyPage::OnDisconnect(void)
{
	if (m_pControls)
	{
		// If the user clicked OK, m_currentValues holds the new values.
		// Otherwise, if the user clicked Cancel, m_currentValues is the old values.
		m_pControls->SetPowerLineFrequency(m_currentValues.nPowerLineFrequency);  
		m_pControls->SetGain(m_currentValues.nGain);
		m_pControls->SetLowLightCompensation(m_currentValues.bLowLightCompensation);
		m_pControls->Release();
		m_pControls = NULL;
	}
	return S_OK;
}

void XnAdditionalControlsPropertyPage::SetDirty()
{
	m_bDirty = TRUE;
	if (m_pPageSite)
	{
		m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
	}
}
