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
#include "XnVideoStream.h"
#include "XnVideoSource.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnVideoStream::XnVideoStream(HRESULT *phr, XnVideoSource *pParent, xn::ImageGenerator& imageGen, LPCWSTR pPinName) :
	CSourceStream(NAME("Video Stream"), phr, pParent, pPinName),
	m_imageGen(imageGen),
	m_bFlipVertically(FALSE),
	m_nPreferredMode(-1),
	m_Dump(pParent->m_Dump)
{
	ASSERT(phr);

	xnFPSInit(&m_FPS, 90);

	XnUInt32 nSupportedModes = m_imageGen.GetSupportedMapOutputModesCount();
	XnMapOutputMode* aOutputModes = new XnMapOutputMode[nSupportedModes];

	XnStatus nRetVal = m_imageGen.GetSupportedMapOutputModes(aOutputModes, nSupportedModes);
	if (nRetVal != XN_STATUS_OK)
	{
		*phr = E_UNEXPECTED;
		delete[] aOutputModes;
		return;
	}

	nRetVal = m_aSupportedModes.Reserve(nSupportedModes);
	if (nRetVal != XN_STATUS_OK)
	{
		*phr = E_UNEXPECTED;
		delete[] aOutputModes;
		return;
	}

	XnBool bRGB = m_imageGen.IsPixelFormatSupported(XN_PIXEL_FORMAT_RGB24);
	XnBool bMJPEG = m_imageGen.IsPixelFormatSupported(XN_PIXEL_FORMAT_MJPEG);
	Mode mode;

	for (XnUInt32 i = 0; i < nSupportedModes; ++i)
	{
		mode.OutputMode = aOutputModes[i];
		if (bRGB)
		{
			mode.Format = XN_PIXEL_FORMAT_RGB24;
			m_aSupportedModes.AddLast(mode);
		}

		if (bMJPEG)
		{
			mode.Format = XN_PIXEL_FORMAT_MJPEG;
			m_aSupportedModes.AddLast(mode);
		}
	}

	CMediaType mediaType;
	GetMediaType(0, &mediaType);
	SetMediaType(&mediaType);
}

XnVideoStream::~XnVideoStream()
{
	xnLogVerbose(XN_MASK_FILTER, "Destroying pin...");
	CAutoLock cAutoLock(m_pFilter->pStateLock());
	m_imageGen.Release();
	xnFPSFree(&m_FPS);
}

HRESULT XnVideoStream::FillBuffer(IMediaSample *pms)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pms);

	if (!m_imageGen.IsGenerating())
	{
		XN_METHOD_RETURN(E_UNEXPECTED);
	}

	VIDEOINFOHEADER* videoInfo = (VIDEOINFOHEADER*)m_mt.Format();
	bool bUpsideDown = videoInfo->bmiHeader.biHeight > 0;

	if (m_bFlipVertically)
	{
		bUpsideDown = !bUpsideDown;
	}

	BYTE *pData;
	long lDataLen;

	pms->GetPointer(&pData);
	lDataLen = pms->GetSize();

	{
		CAutoLock cAutoLock(m_pFilter->pStateLock());

		XnStatus nRetVal = XN_STATUS_OK;

		// ignore timeouts
		for(;;)
		{
			nRetVal = m_imageGen.WaitAndUpdateData();
			if (nRetVal != XN_STATUS_WAIT_DATA_TIMEOUT)
			{
				break;
			}
			else
			{
				xnDumpFileWriteString(m_Dump, "\tTimeout during FillBuffer\n");
			}
		}

		if (nRetVal != XN_STATUS_OK) XN_METHOD_RETURN(E_UNEXPECTED);
	}

	xn::ImageMetaData imageMD;
	m_imageGen.GetMetaData(imageMD);

	if (imageMD.PixelFormat() == XN_PIXEL_FORMAT_RGB24)
	{
		const XnRGB24Pixel* pImage = imageMD.RGB24Data();
		if (bUpsideDown)
		{
			// convert from left-to-right top-to-bottom RGB to left-to-right bottom-to-top BGR
			pImage += imageMD.XRes() * imageMD.YRes() - 1;

			for (XnUInt32 y = 0; y < imageMD.YRes(); ++y)
			{
				for (XnUInt32 x = 0; x < imageMD.XRes(); ++x, pImage -=1, pData += 3)
				{
					// translate RGB to BGR
					pData[0] = pImage->nBlue;
					pData[1] = pImage->nGreen;
					pData[2] = pImage->nRed;
				}
			}
		}
		else
		{
			for (XnUInt32 y = 0; y < imageMD.YRes(); ++y)
			{
				for (XnUInt32 x = 0; x < imageMD.XRes(); ++x, pImage += 1, pData += 3)
				{
					// translate RGB to BGR
					pData[0] = pImage->nBlue;
					pData[1] = pImage->nGreen;
					pData[2] = pImage->nRed;
				}
			}
		}
	}
	else if (imageMD.PixelFormat() == XN_PIXEL_FORMAT_MJPEG)
	{
		memcpy(pData, imageMD.Data(), imageMD.DataSize());
		pms->SetActualDataLength(imageMD.DataSize());
	}
	else
	{
		xnLogError(XN_MASK_FILTER, "Unsupported pixel format!");
		XN_METHOD_RETURN(E_UNEXPECTED);
	}

	// The current time is the sample's start
//		CRefTime rtStart = m_rtSampleTime;

	// Increment to find the finish time
//		m_rtSampleTime += (LONG)m_iRepeatTime;

//		pms->SetTime((REFERENCE_TIME *) &rtStart,(REFERENCE_TIME *) &m_rtSampleTime);

	pms->SetSyncPoint(TRUE);

	xnFPSMarkFrame(&m_FPS);

	XN_METHOD_RETURN(NOERROR);
}

//
// GetMediaType
//
// Preferred types should be ordered by quality, with zero as highest quality.
//
HRESULT XnVideoStream::GetMediaType(int iPosition, __inout CMediaType *pMediaType)
{
	XN_METHOD_START;
	HRESULT hr = S_OK;

	XN_METHOD_CHECK_POINTER(pMediaType);

	if(iPosition < 0)
	{
		XN_METHOD_RETURN(E_INVALIDARG);
	}

	if (m_nPreferredMode >= 0)
	{
		// Once a mode was set using IAMStreamConfig::SetFormat, this should be the only mode
		if (iPosition == 0)
		{
			VIDEO_STREAM_CONFIG_CAPS vscc;
			hr = GetStreamCapability(m_nPreferredMode, *pMediaType, vscc);
			XN_METHOD_RETURN(hr);
		}
		else
		{
			XN_METHOD_RETURN(VFW_S_NO_MORE_ITEMS);
		}
	}
	else
	{
		// Have we run off the end of types?
		if (iPosition > int(m_aSupportedModes.GetSize()))
		{
			XN_METHOD_RETURN(VFW_S_NO_MORE_ITEMS);
		}

		VIDEO_STREAM_CONFIG_CAPS vscc;
		hr = GetStreamCapability(iPosition, *pMediaType, vscc);
		XN_METHOD_RETURN(hr);
	}
}

HRESULT XnVideoStream::SetMediaType(const CMediaType* pMediaType)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pMediaType);

	Mode mode = MediaTypeToMode(*pMediaType);

	XnStatus nRetVal = m_imageGen.SetMapOutputMode(mode.OutputMode);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	nRetVal = m_imageGen.SetPixelFormat(mode.Format);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	HRESULT hr = CSourceStream::SetMediaType(pMediaType);
	XN_METHOD_RETURN(hr);
}

//
// CheckMediaType
//
// Returns E_INVALIDARG if the mediatype is not acceptable
//
HRESULT XnVideoStream::CheckMediaType(const CMediaType *pMediaType)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pMediaType);

	int index = FindCapability(*pMediaType);
	if (index == -1 || // not found
		m_nPreferredMode >= 0 && index != m_nPreferredMode)
	{
		XN_METHOD_RETURN(E_INVALIDARG);
	}

	XN_METHOD_RETURN(S_OK);
}

//
// DecideBufferSize
//
// This will always be called after the format has been successfully
// negotiated. So we have a look at m_mt to see what size image we agreed.
// Then we can ask for buffers of the correct size to contain them.
//
HRESULT XnVideoStream::DecideBufferSize(IMemAllocator *pIMemAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pIMemAlloc);
	XN_METHOD_CHECK_POINTER(pProperties);

	CAutoLock cAutoLock(m_pFilter->pStateLock());
	HRESULT hr = S_OK;

	VIDEOINFO *pvi = (VIDEOINFO*)m_mt.Format();
	pProperties->cBuffers = 3;
	pProperties->cbBuffer = pvi->bmiHeader.biSizeImage;

	ASSERT(pProperties->cbBuffer);

	// Ask the allocator to reserve us some sample memory. NOTE: the function
	// can succeed (that is return NOERROR) but still not have allocated the
	// memory that we requested, so we must check we got whatever we wanted

	ALLOCATOR_PROPERTIES Actual;
	hr = pIMemAlloc->SetProperties(pProperties,&Actual);
	if(FAILED(hr))
	{
		XN_METHOD_RETURN(hr);
	}

	// Is this allocator unsuitable
	if(Actual.cbBuffer < pProperties->cbBuffer)
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	// Make sure that we have only 3 buffers
	ASSERT(Actual.cBuffers == 3);
	XN_METHOD_RETURN(NOERROR);
}

HRESULT XnVideoStream::Active( void )
{
	XN_METHOD_START;

	XnStatus nRetVal = m_imageGen.StartGenerating();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_FILTER, "Can't start ImageGenerator: %s", xnGetStatusString(nRetVal));
		XN_METHOD_RETURN(E_UNEXPECTED);
	}

	HRESULT hr = CSourceStream::Active();

	XN_METHOD_RETURN(hr);
}

HRESULT XnVideoStream::Inactive( void )
{
	XN_METHOD_START;

	XnStatus nRetVal = m_imageGen.StopGenerating();
	if (nRetVal != XN_STATUS_OK)
	{
		XN_METHOD_RETURN(E_UNEXPECTED);
	}

	HRESULT hr = CSourceStream::Inactive();

	XN_METHOD_RETURN(hr);
}

STDMETHODIMP XnVideoStream::GetPages(CAUUID *pPages)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pPages);

	pPages->cElems = 1;
	pPages->pElems = reinterpret_cast<GUID*>(CoTaskMemAlloc(sizeof(GUID)*pPages->cElems));
	if (pPages->pElems == NULL) 
	{
		XN_METHOD_RETURN(E_OUTOFMEMORY);
	}
	pPages->pElems[0] = CLSID_VideoStreamConfigPropertyPage;
	XN_METHOD_RETURN(S_OK);
} 

//IUnknown
STDMETHODIMP XnVideoStream::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{   
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(ppv);

	HRESULT hr = S_OK;

	// Standard OLE stuff
	if(riid == IID_IAMStreamConfig) 
	{
		xnDumpFileWriteString(m_Dump, "\tPin query interface to IAMStreamConfig\n");
		hr = GetInterface(static_cast<IAMStreamConfig*>(this), ppv);
	}
	else if(riid == IID_IKsPropertySet)
	{
		xnDumpFileWriteString(m_Dump, "\tPin query interface to IKsPropertySet\n");
		hr = GetInterface(static_cast<IKsPropertySet*>(this), ppv);
	}
	else if(riid == IID_ISpecifyPropertyPages)
	{
		xnDumpFileWriteString(m_Dump, "\tPin query interface to ISpecifyPropertyPages\n");
		hr = GetInterface(static_cast<ISpecifyPropertyPages*>(this), ppv);
	}
	else 
	{
		OLECHAR strGuid[40];
		StringFromGUID2(riid, strGuid, 40);
		xnDumpFileWriteString(m_Dump, "\tPin query interface to %S\n", strGuid);
		hr = CSourceStream::NonDelegatingQueryInterface(riid, ppv);
	}

	XN_METHOD_RETURN(hr);
}

//////////////////////////////////////////////////////////////////////////
//  IAMStreamConfig (skype)
//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE XnVideoStream::GetNumberOfCapabilities(int *piCount, int *piSize)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(piCount);
	XN_METHOD_CHECK_POINTER(piSize);

	*piCount = m_aSupportedModes.GetSize();
	*piSize = sizeof(VIDEO_STREAM_CONFIG_CAPS);
	XN_METHOD_RETURN(S_OK);
}

HRESULT STDMETHODCALLTYPE XnVideoStream::GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pmt, BYTE *pSCC)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pmt);
	XN_METHOD_CHECK_POINTER(pSCC);

	xnDumpFileWriteString(m_Dump, "\tCalling %s for %d\n", __FUNCTION__, iIndex);

	CMediaType mediaType;
	VIDEO_STREAM_CONFIG_CAPS* pvscc = (VIDEO_STREAM_CONFIG_CAPS*)pSCC;
	HRESULT hr = GetStreamCapability(iIndex, mediaType, *pvscc);
	if (FAILED(hr)) XN_METHOD_RETURN(hr);

	xnDumpFileWriteString(m_Dump, "\tReturning %dx%d@%d using %s\n", m_aSupportedModes[iIndex].OutputMode.nXRes, m_aSupportedModes[iIndex].OutputMode.nYRes, m_aSupportedModes[iIndex].OutputMode.nFPS, xnPixelFormatToString(m_aSupportedModes[iIndex].Format));

	*pmt = CreateMediaType(&mediaType);
	XN_METHOD_RETURN(S_OK);
}

HRESULT STDMETHODCALLTYPE XnVideoStream::SetFormat(AM_MEDIA_TYPE *pmt)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(pmt);
	if (pmt == NULL)
	{
		XN_METHOD_RETURN(E_INVALIDARG);
	}

	xnLogVerbose(XN_MASK_FILTER, "SetFormat was called");

	// check if this format is supported
	CMediaType mediaType(*pmt);
	int index = FindCapability(mediaType);
	if (index == -1)
	{
		XN_METHOD_RETURN(VFW_E_INVALIDMEDIATYPE);
	}

	// keep previous one (so we can rollback)
	int prevPreferred = m_nPreferredMode;

	// set the preferred mode
	m_nPreferredMode = index;

	// try to reconnect (if needed)
	IPin* pin; 
	ConnectedTo(&pin);
	if (pin)
	{
		IFilterGraph *pGraph = ((XnVideoSource*)m_pFilter)->GetGraph();
		HRESULT hr = pGraph->Reconnect(this);
		if (FAILED(hr))
		{
			// rollback
			m_nPreferredMode = prevPreferred;
			XN_METHOD_RETURN(hr);
		}
	}

	XN_METHOD_RETURN(S_OK);
}

HRESULT STDMETHODCALLTYPE XnVideoStream::GetFormat(AM_MEDIA_TYPE **ppmt)
{
	XN_METHOD_START;

	XN_METHOD_CHECK_POINTER(ppmt);

	if (IsConnected())
	{
		*ppmt = CreateMediaType(&m_mt);
	}
	else
	{
		int iIndex = (m_nPreferredMode >= 0) ? m_nPreferredMode : 0;
		CMediaType mediaType;
		VIDEO_STREAM_CONFIG_CAPS vscc;
		GetStreamCapability(iIndex, mediaType, vscc);
		*ppmt = CreateMediaType(&mediaType);
	}

	XN_METHOD_RETURN(S_OK);
}

//////////////////////////////////////////////////////////////////////////
// IKsPropertySet
//////////////////////////////////////////////////////////////////////////


HRESULT XnVideoStream::Set(REFGUID guidPropSet, DWORD dwID, void *pInstanceData, 
						   DWORD cbInstanceData, void *pPropData, DWORD cbPropData)
{
	XN_METHOD_START;

	// Taken from MSDN
	XN_METHOD_RETURN(E_NOTIMPL);
}

// Get: Return the pin category (our only property). 
HRESULT XnVideoStream::Get(
						   REFGUID guidPropSet,   // Which property set.
						   DWORD dwPropID,        // Which property in that set.
						   void *pInstanceData,   // Instance data (ignore).
						   DWORD cbInstanceData,  // Size of the instance data (ignore).
						   void *pPropData,       // Buffer to receive the property data.
						   DWORD cbPropData,      // Size of the buffer.
						   DWORD *pcbReturned     // Return the size of the property.
						   )
{
	XN_METHOD_START;

	// Taken from MSDN
	if (guidPropSet != AMPROPSETID_Pin) 
		XN_METHOD_RETURN(E_PROP_SET_UNSUPPORTED);
	if (dwPropID != AMPROPERTY_PIN_CATEGORY)
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	if (pPropData == NULL && pcbReturned == NULL)
		XN_METHOD_RETURN(E_POINTER);
	if (pcbReturned)
		*pcbReturned = sizeof(GUID);
	if (pPropData == NULL)  // Caller just wants to know the size.
		XN_METHOD_RETURN(S_OK);
	if (cbPropData < sizeof(GUID)) // The buffer is too small.
		XN_METHOD_RETURN(E_UNEXPECTED);
	*(GUID *)pPropData = PIN_CATEGORY_CAPTURE;
	XN_METHOD_RETURN(S_OK);
}

// QuerySupported: Query whether the pin supports the specified property.
HRESULT XnVideoStream::QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport)
{
	XN_METHOD_START;

	// Taken from MSDN
	if (guidPropSet != AMPROPSETID_Pin)
		XN_METHOD_RETURN(E_PROP_SET_UNSUPPORTED);
	if (dwPropID != AMPROPERTY_PIN_CATEGORY)
		XN_METHOD_RETURN(E_PROP_ID_UNSUPPORTED);
	if (pTypeSupport)
		// We support getting this property, but not setting it.
		*pTypeSupport = KSPROPERTY_SUPPORT_GET; 
	XN_METHOD_RETURN(S_OK);
}

HRESULT XnVideoStream::SetMirror(BOOL bMirror)
{
	XN_METHOD_START;

	XnStatus nRetVal = m_imageGen.GetMirrorCap().SetMirror(bMirror);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_METHOD_RETURN(E_FAIL);
	}

	XN_METHOD_RETURN(S_OK);
}

BOOL XnVideoStream::GetMirror()
{
	return m_imageGen.GetMirrorCap().IsMirrored();
}

HRESULT XnVideoStream::SetVerticalFlip(BOOL bVerticalFlip)
{
	m_bFlipVertically = bVerticalFlip;
	return S_OK;
}

BOOL XnVideoStream::GetVerticalFlip()
{
	return m_bFlipVertically;
}

XnDouble XnVideoStream::GetCurrentFPS()
{
	return xnFPSCalc(&m_FPS);
}

HRESULT XnVideoStream::GetStreamCapability(int iIndex, CMediaType& mediaType, VIDEO_STREAM_CONFIG_CAPS& vscc)
{
	// check bounds
	if(iIndex < 0 || iIndex >= int(m_aSupportedModes.GetSize()))
	{
		xnLogVerbose(XN_MASK_FILTER, "GetStreamCapability() - Index %d is out of bounds!", iIndex);
		return S_FALSE;
	}

	VIDEOINFO *pvi = (VIDEOINFO*)mediaType.AllocFormatBuffer(sizeof(VIDEOINFO));
	if(NULL == pvi)
		return(E_OUTOFMEMORY);

	ZeroMemory(pvi, sizeof(VIDEOINFO));

	int xRes = m_aSupportedModes[iIndex].OutputMode.nXRes;
	int yRes = m_aSupportedModes[iIndex].OutputMode.nYRes;

	XnUInt64 nFrameTime = 10000000 / m_aSupportedModes[iIndex].OutputMode.nFPS;

	if (m_aSupportedModes[iIndex].Format == XN_PIXEL_FORMAT_RGB24)
	{
		pvi->bmiHeader.biCompression = BI_RGB;
	}
	else if (m_aSupportedModes[iIndex].Format == XN_PIXEL_FORMAT_MJPEG)
	{
		pvi->bmiHeader.biCompression = 'GPJM';
	}
	else
	{
		xnLogError(XN_MASK_FILTER, "Unknown format type!");
		return E_UNEXPECTED;
	}

	pvi->bmiHeader.biBitCount	= 24;
	pvi->bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
	pvi->bmiHeader.biWidth      = xRes;
	pvi->bmiHeader.biHeight     = yRes;
	pvi->bmiHeader.biPlanes     = 1;
	pvi->bmiHeader.biSizeImage  = GetBitmapSize(&pvi->bmiHeader);
	pvi->bmiHeader.biClrImportant = 0;

	SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.
	SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

	pvi->dwBitRate = 
		GetBitmapSize(&pvi->bmiHeader) * // bytes per frame
		m_aSupportedModes[iIndex].OutputMode.nFPS * // frames per second
		8; // bits per byte

	pvi->dwBitErrorRate = 0; // assume no errors
	pvi->AvgTimePerFrame = nFrameTime;

	mediaType.SetType(&MEDIATYPE_Video);
	mediaType.SetFormatType(&FORMAT_VideoInfo);
	mediaType.SetTemporalCompression(FALSE);

	// Work out the GUID for the subtype from the header info.
	const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
	mediaType.SetSubtype(&SubTypeGUID);
	mediaType.SetSampleSize(pvi->bmiHeader.biSizeImage);

	vscc.guid = FORMAT_VideoInfo;
	vscc.VideoStandard = AnalogVideo_None;
	vscc.InputSize.cx = xRes;
	vscc.InputSize.cy = yRes;
	vscc.MinCroppingSize.cx = xRes;
	vscc.MinCroppingSize.cy = yRes;
	vscc.MaxCroppingSize.cx = xRes;
	vscc.MaxCroppingSize.cy = yRes;
	vscc.CropGranularityX = 1;
	vscc.CropGranularityY = 1;
	vscc.CropAlignX = 1;
	vscc.CropAlignY = 1;

	vscc.MinOutputSize.cx = xRes;
	vscc.MinOutputSize.cy = yRes;
	vscc.MaxOutputSize.cx = xRes;
	vscc.MaxOutputSize.cy = yRes;
	vscc.OutputGranularityX = 1;
	vscc.OutputGranularityY = 1;
	vscc.StretchTapsX = 0;
	vscc.StretchTapsY = 0;
	vscc.ShrinkTapsX = 0;
	vscc.ShrinkTapsY = 0;
	// Frame interval is in 100 nanosecond units
	vscc.MinFrameInterval = nFrameTime;
	vscc.MaxFrameInterval = nFrameTime;
	vscc.MinBitsPerSecond = 
		mediaType.GetSampleSize() * // bytes in frame
		m_aSupportedModes[iIndex].OutputMode.nFPS * // frames per second
		8; // bits per byte
	vscc.MaxBitsPerSecond = vscc.MinBitsPerSecond;

	return S_OK;
}

int XnVideoStream::FindCapability(const CMediaType& mediaType)
{
	Mode mode = MediaTypeToMode(mediaType);
	if (mode.OutputMode.nFPS == 0)
	{
		return -1;
	}

	for (XnUInt32 i = 0; i < m_aSupportedModes.GetSize(); ++i)
	{
		if (memcmp(&mode, &m_aSupportedModes[i], sizeof(mode)) == 0)
		{
			return i;
		}
	}

	xnLogVerbose(XN_MASK_FILTER, "can't find a matching mode");
	return -1; // can't find a matching supported mode
}

XnVideoStream::Mode XnVideoStream::MediaTypeToMode(const CMediaType& mediaType)
{
	Mode result = {0};

	if (*mediaType.Type() != MEDIATYPE_Video)   // we only output video
	{                                                  
		xnLogError(XN_MASK_FILTER, "bad type");
		return result;
	}

	// Check for the subtypes we support
	const GUID *SubType = mediaType.Subtype();

	if (SubType && *SubType != GUID_NULL) 
	{
		if (*SubType == MEDIASUBTYPE_RGB24)
		{
			result.Format = XN_PIXEL_FORMAT_RGB24;
		}
		else if (*SubType == MEDIASUBTYPE_MJPG)
		{
			result.Format = XN_PIXEL_FORMAT_MJPEG;
		}
		else
		{
			xnLogVerbose(XN_MASK_FILTER, "bad subtype");
			return result;
		}
	}

	// Get the format area of the media type
	VIDEOINFO *pvi = (VIDEOINFO*)mediaType.Format();
	if (pvi == NULL)
	{
		return result;
	}

	result.OutputMode.nFPS = (XnUInt32)(10000000ULL / pvi->AvgTimePerFrame);
	result.OutputMode.nXRes = pvi->bmiHeader.biWidth;
	result.OutputMode.nYRes = pvi->bmiHeader.biHeight;
	return result;
}
