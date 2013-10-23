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
#ifndef __XN_VIDEO_STREAM_H__
#define __XN_VIDEO_STREAM_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnVideoSource.h"
#include <XnFPSCalculator.h>
#include <XnArray.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnVideoStream : 
	public CSourceStream, 
	public IAMStreamConfig, 
	public IKsPropertySet, 
	public ISpecifyPropertyPages
{
public:
	XnVideoStream(HRESULT *phr, XnVideoSource *pParent, xn::ImageGenerator& imageGen, LPCWSTR pPinName);
	~XnVideoStream();

	// Gets next frame
	HRESULT FillBuffer(IMediaSample *pms);

	// Ask for buffers of the size appropriate to the agreed media type
	HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc,
		ALLOCATOR_PROPERTIES *pProperties);

	virtual HRESULT CheckMediaType(const CMediaType *pMediaType);
	virtual HRESULT GetMediaType(int iPosition, __inout CMediaType *pMediaType);  // List pos. 0-n
	virtual HRESULT SetMediaType(const CMediaType* pMediaType);

	virtual HRESULT Active(void);
	virtual HRESULT Inactive(void);

	// IQualityControl
	STDMETHODIMP Notify(IBaseFilter * pSender, Quality q) { return E_FAIL; }

	//  IAMStreamConfig interface
	HRESULT STDMETHODCALLTYPE SetFormat(AM_MEDIA_TYPE *pmt);
	HRESULT STDMETHODCALLTYPE GetFormat(AM_MEDIA_TYPE **ppmt);
	HRESULT STDMETHODCALLTYPE GetNumberOfCapabilities(int *piCount, int *piSize);
	HRESULT STDMETHODCALLTYPE GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pmt, BYTE *pSCC);

	//  IKsPropertySet
	HRESULT STDMETHODCALLTYPE Set(REFGUID guidPropSet, DWORD dwID, void *pInstanceData, DWORD cbInstanceData, void *pPropData, DWORD cbPropData);
	HRESULT STDMETHODCALLTYPE Get(REFGUID guidPropSet, DWORD dwPropID, void *pInstanceData,DWORD cbInstanceData, void *pPropData, DWORD cbPropData, DWORD *pcbReturned);
	HRESULT STDMETHODCALLTYPE QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport);

	// ISpecifyPropertyPages
	STDMETHOD(GetPages)(CAUUID *pPages);

	// IUnknown
	DECLARE_IUNKNOWN;
	// INonDelegatingUnknown
	STDMETHOD(NonDelegatingQueryInterface)(REFIID riid, void **ppv);

	HRESULT SetMirror(BOOL bMirror);
	BOOL GetMirror();
	HRESULT SetVerticalFlip(BOOL bVerticalFlip);
	BOOL GetVerticalFlip();

	XnDouble GetCurrentFPS();

private:
	typedef struct Mode
	{
		XnMapOutputMode OutputMode;
		XnPixelFormat Format;
	} Mode;

	HRESULT GetStreamCapability(int iIndex, CMediaType& mediaType, VIDEO_STREAM_CONFIG_CAPS& vscc);
	int FindCapability(const CMediaType& mediaType);

	Mode MediaTypeToMode(const CMediaType& mediaType);

	xn::ImageGenerator& m_imageGen;
	XnArray<Mode> m_aSupportedModes;
	BOOL m_bFlipVertically;
	XnFPSData m_FPS;
	int m_nPreferredMode;
	XnDumpFile*& m_Dump;
};

#endif // __XN_VIDEO_STREAM_H__