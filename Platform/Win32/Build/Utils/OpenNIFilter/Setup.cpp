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
#ifndef __SETUP_H__
#define __SETUP_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <olectl.h>
#include <initguid.h>
#include "Guids.h"
#include "XnVideoSource.h"
#include <XnLog.h>
#include "XnAdditionalControlsPropertyPage.h"

//---------------------------------------------------------------------------
// Setup data
//---------------------------------------------------------------------------

const AMOVIESETUP_MEDIATYPE sudVideoPinTypes =
{
	&MEDIATYPE_Video,       // Major type
	&MEDIASUBTYPE_NULL      // Minor type
};

// Pin details
const AMOVIESETUP_PIN sudVideoPin =
{
	L"VideoOut",            // Pin string name
	FALSE,                  // Is it rendered
	TRUE,                   // Is it an output
	FALSE,                  // Can we have none
	FALSE,                  // Can we have many
	&CLSID_NULL,            // Connects to filter
	NULL,                   // Connects to pin
	1,                      // Number of types
	&sudVideoPinTypes 
};

const AMOVIESETUP_FILTER sudVideoFilter =
{
	&CLSID_OpenNIVideo,     // Filter CLSID
	L"OpenNI Video Filter", // String name
	MERIT_DO_NOT_USE,       // Filter merit
	1,                      // Number pins
	&sudVideoPin               // Pin details
};

// COM global table of objects in this dll

CFactoryTemplate g_Templates[] = 
{
	{ 
		g_videoName,
		&CLSID_OpenNIVideo,
		XnVideoSource::CreateInstance,
		NULL,
		&sudVideoFilter 
	},
	// This entry is for the property page.
	{ 
		g_propertyPageName,
		&CLSID_AdditionalOpenNIControlsPropertyPage,
		XnAdditionalControlsPropertyPage::CreateInstance, 
		NULL, 
		NULL
	}
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

////////////////////////////////////////////////////////////////////////
//
// Exported entry points for registration and unregistration 
// (in this case they only call through to default implementations).
//
////////////////////////////////////////////////////////////////////////

//
// DllRegisterServer
//
// Exported entry points for registration and unregistration
//
STDAPI DllRegisterServer()
{
	HRESULT hr = AMovieDllRegisterServer2( TRUE );
	if (FAILED(hr)) return hr;

	CoInitialize(0);

	IFilterMapper2 *fm = NULL;

	hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper2,(LPVOID*)&fm);

	if( FAILED(hr) ) {
		CoFreeUnusedLibraries();
		CoUninitialize();
		return hr;
	}

	IMoniker *pMoniker = 0;
	REGFILTER2 videoFilterReg;
	videoFilterReg.dwVersion = 1;
	videoFilterReg.dwMerit = MERIT_DO_NOT_USE;
	videoFilterReg.cPins = 1;
	videoFilterReg.rgPins = &sudVideoPin;
	hr = fm->RegisterFilter(CLSID_OpenNIVideo, g_videoName, &pMoniker, &CLSID_VideoInputDeviceCategory, NULL, &videoFilterReg);
	if( FAILED(hr) ) {
		CoFreeUnusedLibraries();
		CoUninitialize();
		return hr;
	}

	// release interface
	//
	if(fm)
		fm->Release();

	CoFreeUnusedLibraries();
	CoUninitialize();

	return hr;

} // DllRegisterServer


//
// DllUnregisterServer
//
STDAPI DllUnregisterServer()
{
	CoInitialize(0);

	IFilterMapper2 *fm = NULL;

	HRESULT hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper2,(LPVOID*)&fm);
	if( FAILED(hr) ) {
		CoFreeUnusedLibraries();
		CoUninitialize();
		return AMovieDllRegisterServer2(FALSE);
	}

	fm->UnregisterFilter(&CLSID_VideoInputDeviceCategory, g_videoName, CLSID_OpenNIVideo);

	return AMovieDllRegisterServer2(FALSE);
} // DllUnregisterServer


//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
					  DWORD  dwReason, 
					  LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_DETACH)
	{
		xnLogClose();
	}

	BOOL bRes = DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);

	// init log from local file (if it exists)
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		xnLogInitFromXmlFile("OpenNIConfig.xml");
	}

	return bRes;
}

#endif // __SETUP_H__
