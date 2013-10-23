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
#include <streams.h>
#include <olectl.h>
#include <initguid.h>
#include "XnVideoSource.h"
#include "Guids.h"
#include "XnVideoStream.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnVideoSource::XnVideoSource(LPUNKNOWN lpunk, HRESULT *phr) :
	CSource(g_videoName, lpunk, CLSID_OpenNIVideo),
	m_pVideoProcAmp(NULL),
	m_pCameraControl(NULL),
	m_Dump(xnDumpFileOpen(XN_MASK_FILTER, "FilterFlow.log"))
{
	ASSERT(phr != NULL);

	xnLogVerbose(XN_MASK_FILTER, "Creating video source filter");

	CAutoLock cAutoLock(&m_cStateLock);

	// initialize OpenNI
	XnStatus nRetVal = m_context.Init();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_FILTER, "Can't init context");
		*phr = E_UNEXPECTED;
	}

	// try to create an image generator
	nRetVal = m_image.Create(m_context);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_FILTER, "Can't create image generator");
		*phr = VFW_E_NO_CAPTURE_HARDWARE;
		return;
	}

	// create output pins. Every pin registers itself with the source object
	XnVideoStream* pStream = new XnVideoStream(phr, this, m_image, L"VideoOut");
	if (pStream == NULL)
	{
		*phr = E_OUTOFMEMORY;
	}

	*phr = NOERROR;
}

XnVideoSource::~XnVideoSource()
{
	xnLogVerbose(XN_MASK_FILTER, "Destroying filter...");

	if (m_pVideoProcAmp != NULL)
	{
		delete m_pVideoProcAmp;
	}

	if (m_pCameraControl != NULL)
	{
		delete m_pCameraControl;
	}

	for (int i = 0; i < GetPinCount(); ++i)
	{
		delete GetPin(i);
	}

	m_image.Release();
	m_context.Release();
}

//
// CreateInstance
//
// The only allowed way to create XnVideoSource!
//
CUnknown * WINAPI XnVideoSource::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
	ASSERT(phr);

	CUnknown *punk = new XnVideoSource(lpunk, phr);
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

STDMETHODIMP XnVideoSource::GetPages(CAUUID *pPages)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pPages);

	pPages->cElems = 3;
	pPages->pElems = reinterpret_cast<GUID*>(CoTaskMemAlloc(sizeof(GUID)*pPages->cElems));
	if (pPages->pElems == NULL) 
	{
		XN_METHOD_RETURN(E_OUTOFMEMORY);
	}
	pPages->pElems[0] = CLSID_VideoProcAmpPropertyPage;
	pPages->pElems[1] = CLSID_CameraControlPropertyPage;
	pPages->pElems[2] = CLSID_AdditionalOpenNIControlsPropertyPage;

	XN_METHOD_RETURN(S_OK);
} 

STDMETHODIMP XnVideoSource::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(ppv);

	HRESULT hr = S_OK;

	if (riid == IID_ISpecifyPropertyPages)
	{
		xnDumpFileWriteString(m_Dump, "\tFilter query interface to ISpecifyPropertyPages\n");
		hr = GetInterface(static_cast<ISpecifyPropertyPages*>(this), ppv);
	}
	else if (riid == IID_IAMVideoControl)
	{
		xnDumpFileWriteString(m_Dump, "\tFilter query interface to IAMVideoControl\n");
		hr = GetInterface(static_cast<IAMVideoControl*>(this), ppv);
	}
	else if (riid == IID_IAMVideoProcAmp)
	{
		xnDumpFileWriteString(m_Dump, "\tFilter query interface to IAMVideoProcAmp\n");
		if (m_pVideoProcAmp == NULL)
		{
			m_pVideoProcAmp = new VideoProcAmp(this);
			if (m_pVideoProcAmp == NULL)
			{
				XN_METHOD_RETURN(E_OUTOFMEMORY);
			}
		}

		hr = GetInterface(static_cast<IAMVideoProcAmp*>(m_pVideoProcAmp), ppv);
	}
	else if (riid == IID_IAMCameraControl)
	{
		xnDumpFileWriteString(m_Dump, "\tFilter query interface to IAMCameraControl\n");
		if (m_pCameraControl == NULL)
		{
			m_pCameraControl = new CameraControl(this);
			if (m_pCameraControl == NULL)
			{
				XN_METHOD_RETURN(E_OUTOFMEMORY);
			}
		}

		hr = GetInterface(static_cast<IAMCameraControl*>(m_pCameraControl), ppv);
	}
	else if (riid == IID_IAdditionalOpenNIControls)
	{
		xnDumpFileWriteString(m_Dump, "\tFilter query interface to IAdditionalControls\n");
		hr = GetInterface(static_cast<IAdditionalControls*>(this), ppv);
	}
	else
	{
		OLECHAR strGuid[40];
		StringFromGUID2(riid, strGuid, 40);
		xnDumpFileWriteString(m_Dump, "\tFilter query interface to %S\n", strGuid);

		hr = CSource::NonDelegatingQueryInterface(riid, ppv);
	}

	XN_METHOD_RETURN(hr);
}

HRESULT STDMETHODCALLTYPE XnVideoSource::GetCaps(IPin *pPin, long *pCapsFlags)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pPin);
	XN_METHOD_CHECK_POINTER(pCapsFlags);

	// we have only 1 pin, make sure this is it
	if (pPin != static_cast<IPin*>(GetPin(0)))
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	*pCapsFlags = VideoControlFlag_FlipHorizontal | VideoControlFlag_FlipVertical;
	XN_METHOD_RETURN(S_OK);
}

HRESULT STDMETHODCALLTYPE XnVideoSource::SetMode( IPin *pPin, long Mode )
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pPin);

	HRESULT hr = S_OK;

	// we have only 1 pin, make sure this is it
	XnVideoStream* pVideoStream = dynamic_cast<XnVideoStream*>(GetPin(0));
	if (pPin != static_cast<IPin*>(pVideoStream))
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	xnLogVerbose(XN_MASK_FILTER, "Setting flip mode to %d", Mode);

	hr = pVideoStream->SetMirror(Mode & VideoControlFlag_FlipHorizontal);
	if (FAILED(hr)) 
		XN_METHOD_RETURN(hr);

	hr = pVideoStream->SetVerticalFlip(Mode & VideoControlFlag_FlipVertical);
	if (FAILED(hr)) 
		XN_METHOD_RETURN(hr);

	XN_METHOD_RETURN(S_OK);
}

HRESULT STDMETHODCALLTYPE XnVideoSource::GetMode( IPin *pPin, __out long *Mode )
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pPin);
	XN_METHOD_CHECK_POINTER(Mode);

	// we have only 1 pin, make sure this is it
	XnVideoStream* pVideoStream = dynamic_cast<XnVideoStream*>(GetPin(0));
	if (pPin != static_cast<IPin*>(pVideoStream))
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	*Mode = 0;

	if (pVideoStream->GetMirror())
		*Mode |= VideoControlFlag_FlipHorizontal;

	if (pVideoStream->GetVerticalFlip())
		*Mode |= VideoControlFlag_FlipVertical;

	XN_METHOD_RETURN(S_OK);
}

HRESULT STDMETHODCALLTYPE XnVideoSource::GetCurrentActualFrameRate( IPin *pPin, __out LONGLONG *ActualFrameRate )
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pPin);
	XN_METHOD_CHECK_POINTER(ActualFrameRate);

	// we have only 1 pin, make sure this is it
	XnVideoStream* pVideoStream = dynamic_cast<XnVideoStream*>(GetPin(0));
	if (pPin != static_cast<IPin*>(pVideoStream))
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	*ActualFrameRate = (LONGLONG)(10000000.0 / pVideoStream->GetCurrentFPS() + 0.5);
	XN_METHOD_RETURN(S_OK);
}

HRESULT STDMETHODCALLTYPE XnVideoSource::GetMaxAvailableFrameRate( IPin *pPin, long iIndex, SIZE Dimensions, __out LONGLONG *MaxAvailableFrameRate )
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pPin);
	XN_METHOD_CHECK_POINTER(MaxAvailableFrameRate);

	HRESULT hr = S_OK;

	// we have only 1 pin, make sure this is it
	XnVideoStream* pVideoStream = dynamic_cast<XnVideoStream*>(GetPin(0));
	if (pPin != static_cast<IPin*>(pVideoStream))
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	AM_MEDIA_TYPE* pMediaType;
	VIDEO_STREAM_CONFIG_CAPS vscc;
	hr = pVideoStream->GetStreamCaps(iIndex, &pMediaType, (BYTE*)&vscc);
	if (FAILED(hr)) XN_METHOD_RETURN(hr);

	CoTaskMemFree(pMediaType);

	if (Dimensions.cx != vscc.MaxOutputSize.cx || Dimensions.cy != vscc.MaxOutputSize.cy)
		XN_METHOD_RETURN(E_FAIL);

	*MaxAvailableFrameRate = vscc.MaxFrameInterval;
	XN_METHOD_RETURN(S_OK);
}

HRESULT STDMETHODCALLTYPE XnVideoSource::GetFrameRateList( IPin *pPin, long iIndex, SIZE Dimensions, __out long *ListSize, __out LONGLONG **FrameRates )
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pPin);
	XN_METHOD_CHECK_POINTER(ListSize);

	HRESULT hr = S_OK;

	// we have only 1 pin, make sure this is it
	XnVideoStream* pVideoStream = dynamic_cast<XnVideoStream*>(GetPin(0));
	if (pPin != static_cast<IPin*>(pVideoStream))
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	AM_MEDIA_TYPE* pMediaType;
	VIDEO_STREAM_CONFIG_CAPS vscc;
	hr = pVideoStream->GetStreamCaps(iIndex, &pMediaType, (BYTE*)&vscc);
	if (FAILED(hr)) XN_METHOD_RETURN(hr);

	CoTaskMemFree(pMediaType);

	if (Dimensions.cx != vscc.MaxOutputSize.cx || Dimensions.cy != vscc.MaxOutputSize.cy)
		XN_METHOD_RETURN(E_FAIL);

	// we return 1 frame rate for each mode (this is the OpenNI way...)
	*ListSize = 1;

	if (FrameRates != NULL)
	{
		*FrameRates = (LONGLONG*)CoTaskMemAlloc(sizeof(LONGLONG)* (*ListSize));
		(*FrameRates)[0] = vscc.MaxFrameInterval;
	}

	XN_METHOD_RETURN(S_OK);
}

STDMETHODIMP XnVideoSource::GetPowerLineFrequencyDefault(XnPowerLineFrequency* pnValue)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pnValue);

	if (!m_image.IsCapabilitySupported(XN_CAPABILITY_ANTI_FLICKER))
	{
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	}

	*pnValue = m_image.GetAntiFlickerCap().GetPowerLineFrequency();

	XN_METHOD_RETURN(S_OK);
}

STDMETHODIMP XnVideoSource::GetPowerLineFrequency(XnPowerLineFrequency *pnValue)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pnValue);

	if (!m_image.IsCapabilitySupported(XN_CAPABILITY_ANTI_FLICKER))
	{
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	}

	*pnValue = m_image.GetAntiFlickerCap().GetPowerLineFrequency();
	XN_METHOD_RETURN(S_OK);
}

STDMETHODIMP XnVideoSource::SetPowerLineFrequency(XnPowerLineFrequency nValue)
{
	XN_METHOD_START;
	if (!m_image.IsCapabilitySupported(XN_CAPABILITY_ANTI_FLICKER))
	{
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	}

	XnStatus nRetVal = m_image.GetAntiFlickerCap().SetPowerLineFrequency(nValue);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_METHOD_RETURN(E_FAIL);
	}
	XN_METHOD_RETURN(S_OK);
}

STDMETHODIMP XnVideoSource::GetGainRange(XnInt32 *pnMin, XnInt32* pnMax, XnInt32* pnStep, XnInt32* pnDefault, XnBool* pbAutoSupported)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pnMin);
	XN_METHOD_CHECK_POINTER(pnMax);
	XN_METHOD_CHECK_POINTER(pnStep);
	XN_METHOD_CHECK_POINTER(pnDefault);
	XN_METHOD_CHECK_POINTER(pbAutoSupported);

	if (!m_image.IsCapabilitySupported(XN_CAPABILITY_GAIN))
	{
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	}

	xn::GeneralIntCapability cap = m_image.GetGainCap();
	cap.GetRange(*pnMin, *pnMax, *pnStep, *pnDefault, *pbAutoSupported);

	XN_METHOD_RETURN(S_OK);
}

STDMETHODIMP XnVideoSource::GetGain(XnInt32 *pnValue)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pnValue);

	if (!m_image.IsCapabilitySupported(XN_CAPABILITY_GAIN))
	{
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	}

	xn::GeneralIntCapability cap = m_image.GetGainCap();
	*pnValue = cap.Get();

	XN_METHOD_RETURN(S_OK);
}

STDMETHODIMP XnVideoSource::SetGain(XnInt32 nValue)
{
	XN_METHOD_START;

	if (!m_image.IsCapabilitySupported(XN_CAPABILITY_GAIN))
	{
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	}

	xn::GeneralIntCapability cap = m_image.GetGainCap();
	XnStatus nRetVal = cap.Set(nValue);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	XN_METHOD_RETURN(S_OK);
}

STDMETHODIMP XnVideoSource::GetLowLightCompensationDefault(XnBool* pbValue)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pbValue);

	if (!m_image.IsCapabilitySupported(XN_CAPABILITY_LOW_LIGHT_COMPENSATION))
	{
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	}

	XnInt32 nMin, nMax, nStep, nDefault;
	XnBool bAutoSupported;
	m_image.GetLowLightCompensationCap().GetRange(nMin, nMax, nStep, nDefault, bAutoSupported);

	*pbValue = nDefault;

	XN_METHOD_RETURN(S_OK);
}

STDMETHODIMP XnVideoSource::GetLowLightCompensation(XnBool *pbValue)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pbValue);

	if (!m_image.IsCapabilitySupported(XN_CAPABILITY_LOW_LIGHT_COMPENSATION))
	{
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	}

	xn::GeneralIntCapability cap = m_image.GetLowLightCompensationCap();
	*pbValue = (XnBool)cap.Get();

	XN_METHOD_RETURN(S_OK);
}

STDMETHODIMP XnVideoSource::SetLowLightCompensation(XnBool bValue)
{
	XN_METHOD_START;

	if (!m_image.IsCapabilitySupported(XN_CAPABILITY_LOW_LIGHT_COMPENSATION))
	{
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	}

	xn::GeneralIntCapability cap = m_image.GetLowLightCompensationCap();
	XnStatus nRetVal = cap.Set(bValue);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	XN_METHOD_RETURN(S_OK);
}

HRESULT XnVideoSource::GetCapRange(const XnChar* strCap, long *pMin, long *pMax, long *pSteppingDelta, long *pDefault, long *pCapsFlags)
{
	XN_METHOD_START;

	if (strCap == NULL || !m_image.IsCapabilitySupported(strCap))
	{
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	}

	XN_METHOD_CHECK_POINTER(pMin);
	XN_METHOD_CHECK_POINTER(pMax);
	XN_METHOD_CHECK_POINTER(pSteppingDelta);
	XN_METHOD_CHECK_POINTER(pDefault);
	XN_METHOD_CHECK_POINTER(pCapsFlags);

	xn::GeneralIntCapability cap = m_image.GetGeneralIntCap(strCap);
	XnInt32 nMin, nMax, nStep, nDefault;
	XnBool bIsAutoSupported;
	cap.GetRange(nMin, nMax, nStep, nDefault, bIsAutoSupported);

	*pMin = nMin;
	*pMax = nMax;
	*pSteppingDelta = nStep;
	*pDefault = nDefault;
	*pCapsFlags = bIsAutoSupported ? 0x01 : 0x02;

	XN_METHOD_RETURN(S_OK);
}

HRESULT XnVideoSource::GetCap(const XnChar* strCap, long *lValue, long *Flags)
{
	XN_METHOD_START;

	if (strCap == NULL || !m_image.IsCapabilitySupported(strCap))
	{
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	}

	XN_METHOD_CHECK_POINTER(lValue);
	XN_METHOD_CHECK_POINTER(Flags);

	xn::GeneralIntCapability cap = m_image.GetGeneralIntCap(strCap);
	XnInt32 nVal = cap.Get();
	if (nVal == XN_AUTO_CONTROL)
	{
		XnInt32 nMin, nMax, nStep, nDefault;
		XnBool bIsAutoSupported;
		cap.GetRange(nMin, nMax, nStep, nDefault, bIsAutoSupported);

		*Flags = 0x01;
		*lValue = nDefault;
	}
	else
	{
		*Flags = 0x02;
		*lValue = nVal;
	}

	XN_METHOD_RETURN(S_OK);
}

HRESULT XnVideoSource::SetCap(const XnChar* strCap, long lValue, long Flags)
{
	XN_METHOD_START;

	if (strCap == NULL || !m_image.IsCapabilitySupported(strCap))
	{
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	}

	xn::GeneralIntCapability cap = m_image.GetGeneralIntCap(strCap);

	if (Flags == 0x01)
	{
		lValue = XN_AUTO_CONTROL;
	}

	XnStatus nRetVal = cap.Set(lValue);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	XN_METHOD_RETURN(S_OK);
}

XnVideoSource::VideoProcAmp::VideoProcAmp(XnVideoSource* pSource) : 
	CUnknown(NAME("XnVideoSource::VideoProcAmp"), pSource->GetOwner()), 
	m_pSource(pSource),
	m_Dump(pSource->m_Dump)
{}

const XnChar* XnVideoSource::VideoProcAmp::GetPropertyCap(long Property)
{
	switch (Property)
	{
	case VideoProcAmp_Brightness:
		return XN_CAPABILITY_BRIGHTNESS;
	case VideoProcAmp_Contrast:
		return XN_CAPABILITY_CONTRAST;
	case VideoProcAmp_Hue:
		return XN_CAPABILITY_HUE;
	case VideoProcAmp_Saturation:
		return XN_CAPABILITY_SATURATION;
	case VideoProcAmp_Sharpness:
		return XN_CAPABILITY_SHARPNESS;
	case VideoProcAmp_Gamma:
		return XN_CAPABILITY_GAMMA;
	case VideoProcAmp_ColorEnable:
		return NULL;
	case VideoProcAmp_WhiteBalance:
		return XN_CAPABILITY_COLOR_TEMPERATURE;
	case VideoProcAmp_BacklightCompensation:
		return XN_CAPABILITY_BACKLIGHT_COMPENSATION;
	case VideoProcAmp_Gain:
		return XN_CAPABILITY_GAIN;
	default:
		xnLogWarning(XN_MASK_FILTER, "IAMVideoProcAmp: an unknown property (%d was requested)", Property);
		return NULL;
	}
}

STDMETHODIMP XnVideoSource::VideoProcAmp::GetRange(long Property, long *pMin, long *pMax, long *pSteppingDelta, long *pDefault, long *pCapsFlags)
{
	return m_pSource->GetCapRange(GetPropertyCap(Property), pMin, pMax, pSteppingDelta, pDefault, pCapsFlags);
}

STDMETHODIMP XnVideoSource::VideoProcAmp::Set(long Property, long lValue, long Flags)
{
	return m_pSource->SetCap(GetPropertyCap(Property), lValue, Flags);
}

STDMETHODIMP XnVideoSource::VideoProcAmp::Get(long Property, long *lValue, long *Flags)
{
	return m_pSource->GetCap(GetPropertyCap(Property), lValue, Flags);
}

XnVideoSource::CameraControl::CameraControl(XnVideoSource* pSource) : 
	CUnknown(NAME("XnVideoSource::CameraControl"), pSource->GetOwner()), 
	m_pSource(pSource),
	m_Dump(pSource->m_Dump)
{}

const XnChar* XnVideoSource::CameraControl::GetPropertyCap(long Property)
{
	switch (Property)
	{
	case CameraControl_Pan:
		return XN_CAPABILITY_PAN;
	case CameraControl_Tilt:
		return XN_CAPABILITY_TILT;
	case CameraControl_Roll:
		return XN_CAPABILITY_ROLL;
	case CameraControl_Zoom:
		return XN_CAPABILITY_ZOOM;
	case CameraControl_Exposure:
		return XN_CAPABILITY_EXPOSURE;
	case CameraControl_Iris:
		return XN_CAPABILITY_IRIS;
	case CameraControl_Focus:
		return XN_CAPABILITY_FOCUS;
	default:
		xnLogWarning(XN_MASK_FILTER, "IAMCameraControl: an unknown property (%d was requested)", Property);
		return NULL;
	}
}

STDMETHODIMP XnVideoSource::CameraControl::GetRange(long Property, long *pMin, long *pMax, long *pSteppingDelta, long *pDefault, long *pCapsFlags)
{
	return m_pSource->GetCapRange(GetPropertyCap(Property), pMin, pMax, pSteppingDelta, pDefault, pCapsFlags);
}

STDMETHODIMP XnVideoSource::CameraControl::Set(long Property, long lValue, long Flags)
{
	return m_pSource->SetCap(GetPropertyCap(Property), lValue, Flags);
}

STDMETHODIMP XnVideoSource::CameraControl::Get(long Property, long *lValue, long *Flags)
{
	return m_pSource->GetCap(GetPropertyCap(Property), lValue, Flags);
}
