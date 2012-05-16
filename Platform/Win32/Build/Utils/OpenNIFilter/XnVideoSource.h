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
#ifndef __OPEN_NI_VIDEO_H__
#define __OPEN_NI_VIDEO_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <streams.h>
#include <XnCppWrapper.h>
#include "Guids.h"
#include <XnLog.h>
#include "IAdditionalControls.h"

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define XN_METHOD_START				xnDumpFileWriteString(m_Dump, "%s was called\n", __FUNCTION__)

#define XN_METHOD_RETURN(hr)		\
	{								\
		xnDumpFileWriteString(m_Dump, "\t%s returned %s (%d)\n", __FUNCTION__, XN_STRINGIFY(hr), hr);	\
		return hr;					\
	}

#define XN_METHOD_CHECK_POINTER(p)								\
	if (p == NULL)												\
	{															\
		xnDumpFileWriteString(m_Dump, "\t%s returned %s (%d) (%s)\n", \
			__FUNCTION__, "E_POINTER",							\
			E_POINTER, XN_STRINGIFY(p));						\
	}

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnVideoSource : 
	public CSource, 
	public ISpecifyPropertyPages,
	public IAMVideoControl,
	public IAdditionalControls
{
public:
	~XnVideoSource();

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

	xn::Context& GetContext() { return m_context; }
	IFilterGraph *GetGraph() {return m_pGraph;}

	// IUnknown
	DECLARE_IUNKNOWN;

	// INonDelegatingUnknown
	STDMETHOD(NonDelegatingQueryInterface)(REFIID riid, void **ppv);

	// ISpecifyPropertyPages
	STDMETHOD(GetPages)(CAUUID *pPages);

	// IAMVideoControl
	STDMETHOD(GetCaps)(IPin *pPin, long *pCapsFlags);
	STDMETHOD(SetMode)( IPin *pPin, long Mode );
	STDMETHOD(GetMode)( IPin *pPin, __out long *Mode );
	STDMETHOD(GetCurrentActualFrameRate)( IPin *pPin, __out LONGLONG *ActualFrameRate );
	STDMETHOD(GetMaxAvailableFrameRate)( IPin *pPin, long iIndex, SIZE Dimensions, __out LONGLONG *MaxAvailableFrameRate );
	STDMETHOD(GetFrameRateList)( IPin *pPin, long iIndex, SIZE Dimensions, __out long *ListSize, __out LONGLONG **FrameRates );

	// IAdditionalControls
	STDMETHOD(GetPowerLineFrequencyDefault)(XnPowerLineFrequency* pnValue);
	STDMETHOD(GetPowerLineFrequency)(XnPowerLineFrequency *pnValue);
	STDMETHOD(SetPowerLineFrequency)(XnPowerLineFrequency nValue);
	STDMETHOD(GetGainRange)(XnInt32 *pnMin, XnInt32* pnMax, XnInt32* pnStep, XnInt32* pnDefault, XnBool* pbAutoSupported);
	STDMETHOD(GetGain)(XnInt32 *pnValue);
	STDMETHOD(SetGain)(XnInt32 nValue);
	STDMETHOD(GetLowLightCompensationDefault)(XnBool* pbValue);
	STDMETHOD(GetLowLightCompensation)(XnBool *pbValue);
	STDMETHOD(SetLowLightCompensation)(XnBool bValue);

	XnDumpFile* m_Dump;

private:
	class VideoProcAmp : public CUnknown, public IAMVideoProcAmp
	{
	public:
		VideoProcAmp(XnVideoSource* pSource);

		DECLARE_IUNKNOWN;

		STDMETHOD(GetRange)(long Property, long *pMin, long *pMax, long *pSteppingDelta, long *pDefault, long *pCapsFlags);
		STDMETHOD(Set)(long Property, long lValue, long Flags);
		STDMETHOD(Get)(long Property, long *lValue, long *Flags);

	private:
		const XnChar* GetPropertyCap(long Property);

		XnVideoSource* m_pSource;
		XnDumpFile*& m_Dump;
	};

	class CameraControl : public CUnknown, public IAMCameraControl
	{
	public:
		CameraControl(XnVideoSource* pSource);

		DECLARE_IUNKNOWN;

		STDMETHOD(GetRange)(long Property, long *pMin, long *pMax, long *pSteppingDelta, long *pDefault, long *pCapsFlags);
		STDMETHOD(Set)(long Property, long lValue, long Flags);
		STDMETHOD(Get)(long Property, long *lValue, long *Flags);

	private:
		const XnChar* GetPropertyCap(long Property);

		XnVideoSource* m_pSource;
		XnDumpFile*& m_Dump;
	};

	HRESULT GetCapRange(const XnChar* strCap, long *pMin, long *pMax, long *pSteppingDelta, long *pDefault, long *pCapsFlags);
	HRESULT GetCap(const XnChar* strCap, long *lValue, long *Flags);
	HRESULT SetCap(const XnChar* strCap, long lValue, long Flags);

	// It is only allowed to to create this object with CreateInstance
	XnVideoSource(LPUNKNOWN lpunk, HRESULT *phr);

	xn::Context m_context;
	xn::ImageGenerator m_image;
	VideoProcAmp* m_pVideoProcAmp;
	CameraControl* m_pCameraControl;
};

#endif // __OPEN_NI_VIDEO_H__