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
#include <XnUSB.h>
#include "../xnUSBInternal.h"
#include "XnUSBWin32.h"
#include <XnLog.h>
#include <setupapi.h>
#include <strsafe.h>
#include <process.h>
#include <dbt.h>
#include "devioctl.h"
#include "usb.h"
#include "XnListT.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnUSBEventCallback
{
	XnUSBDeviceCallbackFunctionPtr pFunc;
	void* pCookie;
	XnChar strVidPid[50];
} XnUSBEventCallback;

typedef XnListT<XnUSBEventCallback*> XnUSBEventCallbackList;

//---------------------------------------------------------------------------
// Global Vars
//---------------------------------------------------------------------------

HANDLE g_xnUsbhModule = NULL;
HWND g_xnUsbhDevDetectWnd = NULL;
HANDLE g_xnUsbhPnThread = NULL;

XnUSBEventCallbackList g_connectivityEvent;

//---------------------------------------------------------------------------
// PNP Functions
//---------------------------------------------------------------------------
void _cdecl DevDetectThread(PVOID Arg)
{
	g_xnUsbhDevDetectWnd = CreateWindow("xnUsbDeviceDetector", "", WS_POPUP, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

	SetEvent((HANDLE)Arg);

	if(g_xnUsbhDevDetectWnd)
	{
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0) > 0)
		{
			DispatchMessage(&msg);
		}
	}
}

LRESULT CALLBACK DevDetectWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_DEVICECHANGE:
			{
				PDEV_BROADCAST_HDR pBroadcastHdr = (PDEV_BROADCAST_HDR)lParam;
				PDEV_BROADCAST_DEVICEINTERFACE pDevIF = (PDEV_BROADCAST_DEVICEINTERFACE)pBroadcastHdr;

				if(pBroadcastHdr)
				{
					DWORD nEventType = (DWORD)wParam; 
					if (pBroadcastHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) 
					{
						_strlwr(pDevIF->dbcc_name);

						XnUSBEventArgs args;
						args.strDevicePath = pDevIF->dbcc_name;

						switch (nEventType)
						{
						case DBT_DEVICEARRIVAL:
							args.eventType = XN_USB_EVENT_DEVICE_CONNECT;
							break;
						case DBT_DEVICEREMOVECOMPLETE:
							args.eventType = XN_USB_EVENT_DEVICE_DISCONNECT;
							break;
						default:
							return 0;
						}

						for (XnUSBEventCallbackList::Iterator it = g_connectivityEvent.Begin(); it != g_connectivityEvent.End(); ++it)
						{
							XnUSBEventCallback* pCallback = *it;
							if (strstr(args.strDevicePath, pCallback->strVidPid) != NULL)
							{
								pCallback->pFunc(&args, pCallback->pCookie);
							}
						}
					}
				}
			}
			break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);	
	}

	return 0;
}


//---------------------------------------------------------------------------
// Helper Functions
//---------------------------------------------------------------------------
XnStatus xnUSBInitOvlp(XN_USB_EP_HANDLE pEPHandle)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (pEPHandle->hEPHandleOvlp != NULL)
	{
		CloseHandle(pEPHandle->hEPHandleOvlp);
	}

	if (pEPHandle->ovlpIO.hEvent != NULL)
	{
		xnOSCloseEvent(&pEPHandle->ovlpIO.hEvent);
	}

	// Open the overlapped pipe handle
	pEPHandle->hEPHandleOvlp = CreateFile(pEPHandle->cpPipeName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (pEPHandle->hEPHandleOvlp == INVALID_HANDLE_VALUE)
	{
		return (XN_STATUS_ERROR);
	}

	// Init the overlapped variables
	xnOSMemSet(&pEPHandle->ovlpIO, 0, sizeof(OVERLAPPED));
	nRetVal = xnOSCreateEvent(&pEPHandle->ovlpIO.hEvent, FALSE);
	if (nRetVal != XN_STATUS_OK)
	{
		return (XN_STATUS_ERROR);
	}

	return (XN_STATUS_OK);
}


HANDLE xnUSBOpenOneDevice(HDEVINFO hDevInfo, PSP_DEVICE_INTERFACE_DATA pDevInterfaceData, PSTR pDevName, const XnChar* cpWantedDevPath)
{
	// Local variables
	PSP_DEVICE_INTERFACE_DETAIL_DATA pDevInterfaceDetailData = NULL;
	ULONG nPredictedLength = 0;
	ULONG nRequiredLength = 0;
	HANDLE hDevice = INVALID_HANDLE_VALUE;

	// Probe how much memory is needed to read the device info
	SetupDiGetDeviceInterfaceDetail(hDevInfo, pDevInterfaceData, NULL, 0, &nRequiredLength, NULL);

	// Allocate memory for the device info
	nPredictedLength = nRequiredLength;

	pDevInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(nPredictedLength);
	if(pDevInterfaceDetailData == NULL)
	{
		// Not enough memory...
		return INVALID_HANDLE_VALUE;
	}

	// Read the device info
	pDevInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
	if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, pDevInterfaceData, pDevInterfaceDetailData, nPredictedLength, &nRequiredLength, NULL))
	{
		// Something bad has happened...
		free(pDevInterfaceDetailData);
		return INVALID_HANDLE_VALUE;
	}

	// Construct the device file name
	StringCchCopy(pDevName, MAX_DEVICE_STR_LENGTH, pDevInterfaceDetailData->DevicePath);

	// Check if it's the exact device the user wanted
	if (cpWantedDevPath != NULL)
	{
		if (strcmp(pDevName, cpWantedDevPath) != 0)
		{
			free(pDevInterfaceDetailData);
			return INVALID_HANDLE_VALUE;
		}
	}

	// Open the device
	hDevice = CreateFile(pDevInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);

	// Clean up memory
	free(pDevInterfaceDetailData);

	// Return the device (doesn't mean we have succeeded...)
	return hDevice;
}

XnStatus xnUSBSetPipeProperty(XN_USB_EP_HANDLE pEPHandle, XnUInt32 nIndex, XnUInt32 nValue)
{
	// Local variables
	XnBool bResult = FALSE;
	ULONG nRetBytes = 0;
	PSUSBDRV_PIPE_PROPERTY PipeProp;

	// Init the pipe property structure
	PipeProp.nIndex = nIndex;
	PipeProp.nValue = nValue;

	// Set pipe property (regular handle)
	bResult = DeviceIoControl(pEPHandle->hEPHandle, IOCTL_PSDRV_SET_PIPE_PROPERTY, &PipeProp, sizeof(PSUSBDRV_PIPE_PROPERTY), NULL, NULL, &nRetBytes, NULL);
	if (bResult == FALSE)
	{
		return (XN_STATUS_USB_SET_ENDPOINT_POLICY_FAILED);
	}

	// Set pipe property (ovlp handle)
	bResult = DeviceIoControl(pEPHandle->hEPHandleOvlp, IOCTL_PSDRV_SET_PIPE_PROPERTY, &PipeProp, sizeof(PSUSBDRV_PIPE_PROPERTY), NULL, NULL, &nRetBytes, NULL);
	if (bResult == FALSE)
	{
		return (XN_STATUS_USB_SET_ENDPOINT_POLICY_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus xnUSBGetDriverVersion(XN_USB_DEV_HANDLE pDevHandle, PSUSBDRV_DRIVER_VERSION* pUsbDriverVersion)
{
	// Local variables
	XnBool bResult = FALSE;
	ULONG nRetBytes = 0;

	// Do the get driver version
	bResult = DeviceIoControl(pDevHandle->hUSBDevHandle, IOCTL_PSDRV_GET_DRIVER_VERSION, NULL, 0, pUsbDriverVersion, sizeof(PSUSBDRV_DRIVER_VERSION), &nRetBytes, NULL);
	if (bResult == FALSE)
	{
		return (XN_STATUS_USB_GET_DRIVER_VERSION);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus xnUSBGetDeviceSpeedInternal(XN_USB_DEV_HANDLE pDevHandle, XnUSBDeviceSpeed* pUSBDeviceSpeed)
{
	// Local variables
	XnBool bResult = FALSE;
	ULONG nRetBytes = 0;

	// Do the device get speed
	bResult = DeviceIoControl(pDevHandle->hUSBDevHandle, IOCTL_PSDRV_GET_DEVICE_SPEED, NULL, 0, pUSBDeviceSpeed, sizeof(XnUSBDeviceSpeed), &nRetBytes, NULL);
	if (bResult == FALSE)
	{
		return (XN_STATUS_USB_GET_SPEED_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus xnUSBPlatformSpecificInit()
{
	WNDCLASS wc;
	wc.style = 0;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = (HINSTANCE)g_xnUsbhModule;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "xnUsbDeviceDetector";
	wc.lpfnWndProc = DevDetectWndProc;
	wc.hIcon = NULL;
	wc.hCursor = NULL;

	BOOL ret = RegisterClass(&wc);
	if(ret)
	{
		HANDLE hWaitEv = CreateEvent(NULL,FALSE,FALSE,NULL);
		g_xnUsbhPnThread = (HANDLE)_beginthread(DevDetectThread,0,(PVOID)hWaitEv);
		WaitForSingleObject(hWaitEv,INFINITE);
		CloseHandle(hWaitEv);
	}
	else
	{
		return (XN_STATUS_USB_FAILED_TO_REGISTER_CALLBACK);
	}

	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
	NotificationFilter.dbcc_size =  sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_classguid = GUID_CLASS_PSDRV_USB;
	RegisterDeviceNotification(g_xnUsbhDevDetectWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

	return (XN_STATUS_OK);
}

XnStatus xnUSBPlatformSpecificShutdown()
{
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBIsDevicePresent(XnUInt16 /*nVendorID*/, XnUInt16 /*nProductID*/, void* pExtraParam, XnBool* pbDevicePresent)
{
	// Local variables
	LPGUID pInterfaceGuid = NULL;
	HDEVINFO deviceInfo;
	SP_DEVICE_INTERFACE_DATA interfaceData;
	BOOL bResult = FALSE;

	// Validate the input/output pointers
	XN_VALIDATE_INPUT_PTR(pExtraParam);
	XN_VALIDATE_OUTPUT_PTR(pbDevicePresent);

	// Init the driver GUID
	pInterfaceGuid = (LPGUID)pExtraParam;

	// Let's assume the device is not present for now
	*pbDevicePresent = FALSE;

	// See if the driver is installed
	deviceInfo = SetupDiGetClassDevs(pInterfaceGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (deviceInfo == INVALID_HANDLE_VALUE)
	{
		// No devices are present...
		return (XN_STATUS_OK);
	}

	// See if any devices are attached
	interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	bResult = SetupDiEnumDeviceInterfaces(deviceInfo, NULL, pInterfaceGuid, 0, &interfaceData);
	if (bResult == FALSE)
	{
		SetupDiDestroyDeviceInfoList(deviceInfo);

		// No devices are present...
		return (XN_STATUS_OK);
	}

	SetupDiDestroyDeviceInfoList(deviceInfo);

	// Yay! We found at least one device
	*pbDevicePresent = TRUE;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBEnumerateDevices(XnUInt16 nVendorID, XnUInt16 nProductID, const XnUSBConnectionString** pastrDevicePaths, XnUInt32* pnCount)
{
	// support up to 30 devices
	XnUSBConnectionString cpUSBID;
	XnUSBConnectionString cpUSBPathCmp;
	XnUSBConnectionString aNames[MAX_POTENTIAL_DEVICES];
	HDEVINFO hDevInfo = NULL;
	ULONG nDevices = 0;
	XnUInt32 nFoundDevices = 0;
	SP_DEVICE_INTERFACE_DATA devInterfaceData;
	XnBool bReachedEnd = FALSE;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();

	LPCGUID pInterfaceGuid = &GUID_CLASS_PSDRV_USB;

	// See if the driver is installed
	hDevInfo = SetupDiGetClassDevs (pInterfaceGuid, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		// No devices are present...
		return (XN_STATUS_USB_DRIVER_NOT_FOUND);
	}

	// Scan the hardware for any devices that are attached to our driver.
	devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	while (nDevices < MAX_POTENTIAL_DEVICES)
	{
		// Get information about the device
		if (SetupDiEnumDeviceInterfaces(hDevInfo, 0, pInterfaceGuid, nDevices, &devInterfaceData))
		{
			PSP_DEVICE_INTERFACE_DETAIL_DATA pDevInterfaceDetailData = NULL;
			ULONG nPredictedLength = 0;
			ULONG nRequiredLength = 0;

			// Probe how much memory is needed to read the device info
			SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, NULL, 0, &nRequiredLength, NULL);

			// Allocate memory for the device info
			nPredictedLength = nRequiredLength;

			pDevInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(nPredictedLength);
			if(pDevInterfaceDetailData == NULL)
			{
				// Not enough memory...
				return XN_STATUS_ALLOC_FAILED;
			}

			// Read the device info
			pDevInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, pDevInterfaceDetailData, nPredictedLength, &nRequiredLength, NULL))
			{
				// Something bad has happened...
				free(pDevInterfaceDetailData);
				return XN_STATUS_ERROR;
			}

			// Make sure we have the right VID/PID
			cpUSBID[0] = 0;
			sprintf_s (cpUSBID, "vid_%04x&pid_%04x", nVendorID, nProductID);
			
			cpUSBPathCmp[0] = 0;
			StringCchCopy(cpUSBPathCmp, MAX_DEVICE_STR_LENGTH, pDevInterfaceDetailData->DevicePath);

			if (strstr(_strlwr(cpUSBPathCmp), cpUSBID) != 0)
			{
				StringCchCopy(aNames[nFoundDevices], MAX_DEVICE_STR_LENGTH, pDevInterfaceDetailData->DevicePath);

				++nFoundDevices;
			}

			++nDevices;
		}
		else if (ERROR_NO_MORE_ITEMS == GetLastError())
		{
			// no more devices
			bReachedEnd = TRUE;
			break;
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	if (!bReachedEnd)
	{
		// we probably passed our limit
		XN_LOG_ERROR_RETURN(XN_STATUS_ERROR, XN_MASK_USB, "Found more than %d devices! This is not supported.", MAX_POTENTIAL_DEVICES);
	}

	XnUSBConnectionString* pNames;
	XN_VALIDATE_CALLOC(pNames, XnUSBConnectionString, nFoundDevices);
	xnOSMemCopy(pNames, aNames, sizeof(XnUSBConnectionString) * nFoundDevices);

	*pastrDevicePaths = pNames;
	*pnCount = nFoundDevices;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API void xnUSBFreeDevicesList(const XnUSBConnectionString* astrDevicePaths)
{
	xnOSFree(astrDevicePaths);
}

XnStatus xnUSBOpenDeviceImpl(const XnChar* strDevicePath, XN_USB_DEV_HANDLE* pDevHandlePtr)
{
	// Local variables
	XnStatus nRetVal = XN_STATUS_OK;
	LPCGUID pInterfaceGuid = NULL;
	XN_USB_DEV_HANDLE pDevHandle = NULL;
	ULONG nNumberDevices = 0;
	HDEVINFO hDevInfo = NULL;
	BOOLEAN bDone = FALSE;
	SP_DEVICE_INTERFACE_DATA devInterfaceData;
	PUSB_DEVICE_DESCRIPTOR  pUsbDeviceInst = NULL;
	PUSB_DEVICE_DESCRIPTOR* ppUsbDevices = &pUsbDeviceInst;
	PUSB_DEVICE_DESCRIPTOR  pTempDevDesc = NULL;
	ULONG nIdx = 0;
	HANDLE hSelectedDevice = INVALID_HANDLE_VALUE;
	PSUSBDRV_DRIVER_VERSION UsbDriverVersion;
	PSUSBDRV_INTERFACE_PROPERTY pInterfaceProp;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();

	// Validate the input/output pointers
	XN_VALIDATE_OUTPUT_PTR(pDevHandlePtr);

	// Allocate a new xnUSB Device handle
	XN_VALIDATE_ALIGNED_CALLOC(*pDevHandlePtr, XnUSBDeviceHandle, 1, XN_DEFAULT_MEM_ALIGN);
	pDevHandle = *pDevHandlePtr;

	// Get Device Path
	pInterfaceGuid = &GUID_CLASS_PSDRV_USB;

	// See if the driver is installed
	hDevInfo =  SetupDiGetClassDevs (pInterfaceGuid, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		// No devices are present...
		XN_ALIGNED_FREE_AND_NULL(pDevHandle);
		return (XN_STATUS_USB_DRIVER_NOT_FOUND);
	}

	// Guess how much devices we're going to have. If it's too little, we'll realloc it soon.
	nNumberDevices = 4;

	// Scan the hardware for any devices that are attached to our driver. Stop only after we have successfully opened a device.
	devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	nIdx = 0;
	bDone = FALSE;
	while (!bDone)
	{
		// Allocate enough memory for all the devices that are attached to the driver
		nNumberDevices *= 2;

		if (*ppUsbDevices)
		{
			pTempDevDesc = (USB_DEVICE_DESCRIPTOR*)realloc(*ppUsbDevices, (nNumberDevices * sizeof (USB_DEVICE_DESCRIPTOR)));
			if(pTempDevDesc)
			{
				// We have enough memory!
				*ppUsbDevices = pTempDevDesc;
				pTempDevDesc = NULL;
			}
			else
			{
				// Out of memory... realloc failed...
				free(*ppUsbDevices);
				*ppUsbDevices = NULL;
			}
		}
		else
		{
			*ppUsbDevices = (USB_DEVICE_DESCRIPTOR*)calloc (nNumberDevices, sizeof (USB_DEVICE_DESCRIPTOR));
		}

		// Make sure we have found some devices
		if (NULL == *ppUsbDevices)
		{
			SetupDiDestroyDeviceInfoList(hDevInfo);
			XN_ALIGNED_FREE_AND_NULL(pDevHandle);
			return (XN_STATUS_USB_DEVICE_GETINFO_FAILED);
		}

		pUsbDeviceInst = *ppUsbDevices + nIdx;

		// Scan all the devices that are attached to the driver. Stop when one of them open successfully.
		for (; nIdx < nNumberDevices; nIdx++)
		{
			// Get information about the device
			if (SetupDiEnumDeviceInterfaces (hDevInfo, 0, pInterfaceGuid, nIdx, &devInterfaceData))
			{
				// Try to open this device
				hSelectedDevice = xnUSBOpenOneDevice(hDevInfo, &devInterfaceData, pDevHandle->cpDeviceName, strDevicePath);
				if (hSelectedDevice != INVALID_HANDLE_VALUE)
				{
					// Success! We have a valid device handle.
					bDone = TRUE;
					break;
				}
			}
			else
			{
				// Did we reach the end?
				if (ERROR_NO_MORE_ITEMS == GetLastError())
				{
					// Yup... and we didn't find any devices...
					bDone = TRUE;
					break;
				}
			}
		}
	}

	// Save the number of devices
	nNumberDevices = nIdx;

	// Cleanup memory
	SetupDiDestroyDeviceInfoList (hDevInfo);
	free (*ppUsbDevices);

	// Do we have a valid device?
	if (hSelectedDevice == INVALID_HANDLE_VALUE)
	{
		// Nope...
		XN_ALIGNED_FREE_AND_NULL(pDevHandle);
		return (XN_STATUS_USB_DEVICE_NOT_FOUND);
	}

	// Save the device handle
	pDevHandle->hUSBDevHandle = hSelectedDevice;

	// Get the device speed
	nRetVal = xnUSBGetDeviceSpeedInternal(pDevHandle, &pDevHandle->nDevSpeed);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_ALIGNED_FREE_AND_NULL(pDevHandle);
		return (nRetVal);
	}

	// Get the driver version
	nRetVal = xnUSBGetDriverVersion(pDevHandle, &UsbDriverVersion);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_ALIGNED_FREE_AND_NULL(pDevHandle);
		return (nRetVal);
	}

	xnLogVerbose(XN_MASK_USB, "USB Driver Version is: %d.%d.%d.%d", UsbDriverVersion.nMajor, UsbDriverVersion.nMinor, UsbDriverVersion.nMaintenance, UsbDriverVersion.nBuild);

	// Mark the device handle as valid
	pDevHandle->bValid = TRUE;

	// Read the current alt-if settings
	nRetVal = xnUSBGetInterface(pDevHandle, &pInterfaceProp.nIF, &pInterfaceProp.nAltIF);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_ALIGNED_FREE_AND_NULL(pDevHandle);
		return (nRetVal);
	}

	pDevHandle->nAltInterface = pInterfaceProp.nAltIF;

	xnLogVerbose(XN_MASK_USB, "USB Driver Current Alt Setting is: %d", pDevHandle->nAltInterface);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBOpenDevice(XnUInt16 /*nVendorID*/, XnUInt16 /*nProductID*/, void* /*pExtraParam*/, void* pExtraParam2, XN_USB_DEV_HANDLE* pDevHandlePtr)
{
	return xnUSBOpenDeviceImpl((const XnChar*)pExtraParam2, pDevHandlePtr);
}

XN_C_API XnStatus xnUSBOpenDeviceByPath(const XnUSBConnectionString strDevicePath, XN_USB_DEV_HANDLE* pDevHandlePtr)
{
	return xnUSBOpenDeviceImpl(strDevicePath, pDevHandlePtr);
}

XN_C_API XnStatus xnUSBCloseDevice(XN_USB_DEV_HANDLE pDevHandle)
{
	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PDEV_HANDLE(pDevHandle);

	// Close the device handle
	CloseHandle(pDevHandle->hUSBDevHandle);

	// Free the device handle
	xnOSFreeAligned(pDevHandle);

	// All is good...
	return (XN_STATUS_OK);	
}

XN_C_API XnStatus xnUSBGetDeviceSpeed(XN_USB_DEV_HANDLE pDevHandle, XnUSBDeviceSpeed* pDevSpeed)
{
	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PDEV_HANDLE(pDevHandle);

	// Validate the input/output pointers
	XN_VALIDATE_OUTPUT_PTR(pDevSpeed);

	// Update the data...
	*pDevSpeed = pDevHandle->nDevSpeed;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBSetConfig(XN_USB_DEV_HANDLE /*pDevHandle*/, XnUInt8 /*nConfig*/)
{
	return (XN_STATUS_OS_UNSUPPORTED_FUNCTION);
}

XN_C_API XnStatus xnUSBGetConfig(XN_USB_DEV_HANDLE /*pDevHandle*/, XnUInt8* /*pnConfig*/)
{
	return (XN_STATUS_OS_UNSUPPORTED_FUNCTION);
}

XN_C_API XnStatus xnUSBSetInterface(XN_USB_DEV_HANDLE pDevHandle, XnUInt8 /*nInterface*/, XnUInt8 nAltInterface)
{
	// Local variables
	XnBool bResult = FALSE;
	ULONG nRetBytes = 0;
	PSUSBDRV_INTERFACE_PROPERTY InterfaceProp;

	// Init the pipe property structure
	InterfaceProp.nIF = 0;
	InterfaceProp.nAltIF = nAltInterface;

	// Do the set interface
	bResult = DeviceIoControl(pDevHandle->hUSBDevHandle, IOCTL_PSDRV_SET_INTERFACE, &InterfaceProp, sizeof(PSUSBDRV_INTERFACE_PROPERTY), NULL, NULL, &nRetBytes, NULL);
	if (bResult == FALSE)
	{
		return (XN_STATUS_USB_SET_INTERFACE_FAILED);
	}
	else
	{
		xnLogVerbose(XN_MASK_USB, "USB altIF was set to %d", nAltInterface);
	}

	pDevHandle->nAltInterface = nAltInterface;

	return (XN_STATUS_OK);	
}

XN_C_API XnStatus xnUSBGetInterface(XN_USB_DEV_HANDLE pDevHandle, XnUInt8* pnInterface, XnUInt8* pnAltInterface)
{
	// Local variables
	XnBool bResult = FALSE;
	ULONG nRetBytes = 0;
	PSUSBDRV_INTERFACE_PROPERTY InterfaceProp;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PDEV_HANDLE(pDevHandle);

	// Validate the input/output pointers
	XN_VALIDATE_OUTPUT_PTR(pnInterface);
	XN_VALIDATE_OUTPUT_PTR(pnAltInterface);

	// Do the get interface
	bResult = DeviceIoControl(pDevHandle->hUSBDevHandle, IOCTL_PSDRV_GET_INTERFACE, NULL, 0, &InterfaceProp, sizeof(PSUSBDRV_INTERFACE_PROPERTY), &nRetBytes, NULL);
	if (bResult == FALSE)
	{
		return (XN_STATUS_USB_GET_INTERFACE_FAILED);
	}

	// Update the data...
	*pnInterface = InterfaceProp.nIF;
	*pnAltInterface = InterfaceProp.nAltIF;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBOpenEndPoint(XN_USB_DEV_HANDLE pDevHandle, XnUInt16 nEndPointID, XnUSBEndPointType nEPType, XnUSBDirectionType nDirType, XN_USB_EP_HANDLE* pEPHandlePtr)
{
	// Local variables
	XnBool bResult = TRUE;
	XnStatus nRetVal = XN_STATUS_OK;
	XnInt32 nRetBytes = 0;
	XnChar pConfigDescBuf[MAX_CONFIG_DESC_SIZE];
	XnChar* pBuf = NULL;
	PUSB_CONFIGURATION_DESCRIPTOR pUSBConfigDesc = NULL;
	PUSB_INTERFACE_DESCRIPTOR pUSBInterfaceDesc = NULL;
	PUSB_ENDPOINT_DESCRIPTOR pUSBEndPointDesc = NULL;
	XN_USB_EP_HANDLE pEPHandle = NULL;	
	XnChar cpPipeID[3];

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PDEV_HANDLE(pDevHandle);

	// Validate the input/output pointers
	XN_VALIDATE_OUTPUT_PTR(pEPHandlePtr);

	// Allocate a new xnUSB EP handle
	XN_VALIDATE_ALIGNED_CALLOC(*pEPHandlePtr, xnUSBEPHandle, 1, XN_DEFAULT_MEM_ALIGN);
	pEPHandle = *pEPHandlePtr;

	// Read the config descriptor
	bResult = DeviceIoControl(pDevHandle->hUSBDevHandle, IOCTL_PSDRV_GET_CONFIG_DESCRIPTOR, pConfigDescBuf, sizeof(pConfigDescBuf), pConfigDescBuf, sizeof(pConfigDescBuf), (PULONG)&nRetBytes, NULL);
	if (bResult)
	{
		XnUInt32 nIFIdx = 0;
		UCHAR nEPIdx = 0;
		XnUInt32 nUBBEPType = 0;
		XnUInt32 nCurrIF = 0;

		pBuf = pConfigDescBuf;

		pUSBConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)pBuf;

		pBuf += pUSBConfigDesc->bLength;

		// Scan all the interfaces
		do {
			pUSBInterfaceDesc = (PUSB_INTERFACE_DESCRIPTOR)pBuf;

			pBuf += pUSBInterfaceDesc->bLength;

			// Scan all the endpoints
			for (nEPIdx = 0; nEPIdx < pUSBInterfaceDesc->bNumEndpoints; nEPIdx++)
			{
				pUSBEndPointDesc = (PUSB_ENDPOINT_DESCRIPTOR)pBuf;

				// Is this the EP we're looking for?
				if ((pUSBEndPointDesc->bEndpointAddress == nEndPointID) && (pDevHandle->nAltInterface == nCurrIF))
				{
					// Get the EP type
					nUBBEPType = pUSBEndPointDesc->bmAttributes & USB_ENDPOINT_TYPE_MASK;

					// Verify that the EP type matches the requested EP
					if (nEPType == XN_USB_EP_BULK)
					{
						if (nUBBEPType != USB_ENDPOINT_TYPE_BULK)
						{
							XN_ALIGNED_FREE_AND_NULL(pEPHandle);
							return (XN_STATUS_USB_WRONG_ENDPOINT_TYPE);
						}
					}
					else if (nEPType == XN_USB_EP_INTERRUPT)
					{
						if (nUBBEPType != USB_ENDPOINT_TYPE_INTERRUPT)
						{
							XN_ALIGNED_FREE_AND_NULL(pEPHandle);
							return (XN_STATUS_USB_WRONG_ENDPOINT_TYPE);
						}
					}
					else if (nEPType == XN_USB_EP_ISOCHRONOUS)
					{
						if (nUBBEPType != USB_ENDPOINT_TYPE_ISOCHRONOUS)
						{
							XN_ALIGNED_FREE_AND_NULL(pEPHandle);
							return (XN_STATUS_USB_WRONG_ENDPOINT_TYPE);
						}
					}
					else
					{
						XN_ALIGNED_FREE_AND_NULL(pEPHandle);
						return (XN_STATUS_USB_UNKNOWN_ENDPOINT_TYPE);
					}

					// Verify that the EP direction matches the requested direction
					if (nDirType == XN_USB_DIRECTION_IN)
					{
						if (USB_ENDPOINT_DIRECTION_IN(pUSBEndPointDesc->bEndpointAddress) == FALSE)
						{
							XN_ALIGNED_FREE_AND_NULL(pEPHandle);
							return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
						}
					}
					else if (nDirType == XN_USB_DIRECTION_OUT)
					{
						if (USB_ENDPOINT_DIRECTION_OUT(pUSBEndPointDesc->bEndpointAddress) == FALSE)
						{
							XN_ALIGNED_FREE_AND_NULL(pEPHandle);
							return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
						}
					}
					else
					{
						XN_ALIGNED_FREE_AND_NULL(pEPHandle);
						return (XN_STATUS_USB_UNKNOWN_ENDPOINT_DIRECTION);
					}

					// Construct the pipe file name
					pEPHandle->cpPipeName[0] = 0;

					cpPipeID[0] = '0';
					cpPipeID[1] = '0' + nEPIdx;
					cpPipeID[2] = 0;
			
					StringCchCopy(pEPHandle->cpPipeName, MAX_DEVICE_STR_LENGTH, pDevHandle->cpDeviceName);
					StringCchCat(pEPHandle->cpPipeName, MAX_DEVICE_STR_LENGTH, PSDRV_PIPE_PREFIX);
					StringCchCat(pEPHandle->cpPipeName, MAX_DEVICE_STR_LENGTH, cpPipeID);

					// Open the regular pipe handle
					pEPHandle->hEPHandle = CreateFile(pEPHandle->cpPipeName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
					if (pEPHandle->hEPHandle == INVALID_HANDLE_VALUE)
					{
						XN_ALIGNED_FREE_AND_NULL(pEPHandle);
						return (XN_STATUS_USB_OPEN_ENDPOINT_FAILED);
					}

					// Init the overlapped I/O structs
					nRetVal = xnUSBInitOvlp(pEPHandle);
					if (nRetVal != XN_STATUS_OK)
					{
						XN_ALIGNED_FREE_AND_NULL(pEPHandle);
						return (XN_STATUS_USB_OPEN_ENDPOINT_FAILED);						
					}

					// Init the ThreadData variables
					xnOSMemSet(&pEPHandle->ThreadData, 0, sizeof(xnUSBReadThreadData));
					pEPHandle->ThreadData.bInUse = FALSE;

					// Init the default endpoint properties
					pEPHandle->nTimeOut = XN_USB_DEFAULT_EP_TIMEOUT;
					pEPHandle->nEPType = nEPType;
					pEPHandle->nEPDir = nDirType;
					pEPHandle->nEndPointID = nEndPointID;

					// Set the default endpoint timeout
					nRetVal = xnUSBSetPipeProperty(pEPHandle, PSUSBDRV_PIPE_PROPERTY_TIMEOUT, XN_USB_DEFAULT_EP_TIMEOUT);
					if (nRetVal != XN_STATUS_OK)
					{
						XN_ALIGNED_FREE_AND_NULL(pEPHandle);
						return (nRetVal);
					}

					if (nUBBEPType == USB_ENDPOINT_TYPE_ISOCHRONOUS)
					{
						// bits 11 and 12 mark the number of additional transactions, bits 0-10 mark the size
						XnUInt32 nAdditionalTransactions = pUSBEndPointDesc->wMaxPacketSize >> 11;
						XnUInt32 nPacketSize = pUSBEndPointDesc->wMaxPacketSize & 0x7FF;
						pEPHandle->nMaxPacketSize = (nAdditionalTransactions + 1) * (nPacketSize);
					}
					else
					{
						pEPHandle->nMaxPacketSize = pUSBEndPointDesc->wMaxPacketSize;
					}

					// Mark the endpoint as valid
					pEPHandle->bValid = TRUE;

					// The end... (Happy)
					return (XN_STATUS_OK);
				}

				pBuf += pUSBEndPointDesc->bLength;
			}

			nCurrIF++;
			nIFIdx = (ULONG)(pBuf - pConfigDescBuf);

		} while (nIFIdx < pUSBConfigDesc->wTotalLength);
	}

	// The end... (Sad)
	XN_ALIGNED_FREE_AND_NULL(pEPHandle);
	return (XN_STATUS_USB_ENDPOINT_NOT_FOUND);
}

XN_C_API XnStatus xnUSBCloseEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PEP_HANDLE(pEPHandle);

	// Validate the input/output pointers
	XN_VALIDATE_OUTPUT_PTR(pEPHandle);

	// Close the EP handles
	CloseHandle(pEPHandle->hEPHandle);
	CloseHandle(pEPHandle->hEPHandleOvlp);

	// Close the overlapped events
	xnOSCloseEvent(&pEPHandle->ovlpIO.hEvent);

	// Free the EP handle
	XN_ALIGNED_FREE_AND_NULL(pEPHandle);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBGetEndPointMaxPacketSize(XN_USB_EP_HANDLE pEPHandle, XnUInt32* pnMaxPacketSize)
{
	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PEP_HANDLE(pEPHandle);

	// Validate the input/output pointers
	XN_VALIDATE_INPUT_PTR(pEPHandle);
	XN_VALIDATE_OUTPUT_PTR(pnMaxPacketSize);

	*pnMaxPacketSize = pEPHandle->nMaxPacketSize;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBAbortEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	// Local variables
	BOOL bResult = FALSE;
	ULONG nRetBytes = 0;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PEP_HANDLE(pEPHandle);

	// Abort the pipe
	bResult = DeviceIoControl(pEPHandle->hEPHandle, IOCTL_PSDRV_ABORT_PIPE, NULL, NULL, NULL, NULL, &nRetBytes, NULL);
	if (bResult != TRUE)
	{
		return (XN_STATUS_USB_ABORT_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBFlushEndPoint(XN_USB_EP_HANDLE /*pEPHandle*/)
{
	return (XN_STATUS_OS_UNSUPPORTED_FUNCTION);
}

XN_C_API XnStatus xnUSBResetEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	// Local variables
	BOOL bResult = FALSE;
	ULONG nRetBytes = 0;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PEP_HANDLE(pEPHandle);

	// Reset the pipe
	bResult = DeviceIoControl(pEPHandle->hEPHandle, IOCTL_PSDRV_RESET_PIPE, NULL, NULL, NULL, NULL, &nRetBytes, NULL);
	if (bResult != TRUE)
	{
		return (XN_STATUS_USB_RESET_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBSendControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, XnUInt8 nRequest, XnUInt16 nValue, XnUInt16 nIndex, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut)
{
	// Local variables
	XnBool bResult = FALSE;
	ULONG nRetBytes = 0;
	PSUSBDRV_CONTROL_TRANSFER ControlTransfer;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PDEV_HANDLE(pDevHandle);

	// Init the control transfer structure
	ControlTransfer.cDirection = RequestHostToDevice;
	ControlTransfer.cRequestType = (UCHAR)nType; // cast is safe. enum was defined exactly according to standard
	ControlTransfer.cRequest = nRequest;
	ControlTransfer.nValue = nValue;
	ControlTransfer.nIndex = nIndex;
	ControlTransfer.nTimeout = nTimeOut;

	// Do the control transfer
	bResult = DeviceIoControl(pDevHandle->hUSBDevHandle, IOCTL_PSDRV_CONTROL_TRANSFER, &ControlTransfer, sizeof(ControlTransfer), pBuffer, nBufferSize, &nRetBytes, NULL);
	if (bResult != TRUE)
	{
		// Something failed... let's get the last error
		DWORD nLastErr = GetLastError();

		// Was it a timeout?
		if (nLastErr == ERROR_SEM_TIMEOUT)
		{
			return (XN_STATUS_USB_TRANSFER_TIMEOUT);
		}
		
		// Nope, return a generic error
		return (XN_STATUS_USB_CONTROL_SEND_FAILED);
	}

	// Make sure we have sent all the bytes in the buffer
	if (nRetBytes != nBufferSize)
	{
		return (XN_STATUS_USB_GOT_UNEXPECTED_BYTES);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBReceiveControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, XnUInt8 nRequest, XnUInt16 nValue, XnUInt16 nIndex, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut)
{
	// Local variables
	XnBool bResult = FALSE;
	PSUSBDRV_CONTROL_TRANSFER ControlTransfer;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PDEV_HANDLE(pDevHandle);

	// Validate the input/output pointers
	XN_VALIDATE_OUTPUT_PTR(pBuffer);
	XN_VALIDATE_OUTPUT_PTR(pnBytesReceived);

	// Make sure we are not sending an empty buffer
	if (nBufferSize == 0)
	{
		return (XN_STATUS_USB_BUFFER_TOO_SMALL);
	}

	// Init the control transfer structure
	ControlTransfer.cDirection = RequestDeviceToHost;
	ControlTransfer.cRequestType = (UCHAR)nType; // cast is safe. enum was defined exactly according to standard
	ControlTransfer.cRequest = nRequest;
	ControlTransfer.nValue = nValue;
	ControlTransfer.nIndex = nIndex;
	ControlTransfer.nTimeout = nTimeOut;

	// Do the control transfer
	bResult = DeviceIoControl(pDevHandle->hUSBDevHandle, IOCTL_PSDRV_CONTROL_TRANSFER, &ControlTransfer, sizeof(ControlTransfer), pBuffer, nBufferSize, (DWORD*)pnBytesReceived, NULL);
	if (bResult != TRUE)
	{
		// Something failed... let's get the last error
		DWORD nLastErr = GetLastError();

		// Was it a timeout?
		if (nLastErr == ERROR_SEM_TIMEOUT)
		{
			return (XN_STATUS_USB_TRANSFER_TIMEOUT);
		}

		// Nope, log and return a generic error
		xnLogError(XN_MASK_USB, "Got error receiving data: %u", nLastErr);
		return (XN_STATUS_USB_CONTROL_RECV_FAILED);
	}

	// Did we receive an empty buffer?
	if (*pnBytesReceived == 0)
	{
		return (XN_STATUS_USB_NOT_ENOUGH_DATA);
	} // Make sure we didn't get more then we asked for
	else if (*pnBytesReceived > nBufferSize)
	{
		return (XN_STATUS_USB_TOO_MUCH_DATA);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut)
{
	// Local variables
	BOOL bResult = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PEP_HANDLE(pEPHandle);

	// Validate the input/output pointers
	XN_VALIDATE_OUTPUT_PTR(pBuffer);
	XN_VALIDATE_OUTPUT_PTR(pnBytesReceived);

	// Zero the amount of the received bytes
	*pnBytesReceived = 0;

	// Is this an IN endpoint? It must be if we're going to read from it!
	if (pEPHandle->nEPDir != XN_USB_DIRECTION_IN)
	{
		return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
	}

	// Make sure the receive buffer is big enough to hold some data
	if (nBufferSize == 0)
	{
		return (XN_STATUS_USB_BUFFER_TOO_SMALL);
	}

	// Update the EP timeout
	if (nTimeOut != pEPHandle->nTimeOut)
	{
		nRetVal = xnUSBSetPipeProperty(pEPHandle, PSUSBDRV_PIPE_PROPERTY_TIMEOUT, nTimeOut);
		XN_IS_STATUS_OK(nRetVal);

		pEPHandle->nTimeOut = nTimeOut;
	}

	// Reset the endpoint
	if (pEPHandle->nEPType == XN_USB_EP_ISOCHRONOUS)
	{
		nRetVal = xnUSBResetEndPoint(pEPHandle);
		XN_IS_STATUS_OK(nRetVal);
	}

	// Read from the EP
	bResult = ReadFile(pEPHandle->hEPHandle, pBuffer, nBufferSize, (PULONG)pnBytesReceived, NULL);
	if (bResult == FALSE)
	{
		// Something failed... let's get the last error
		DWORD nLastErr = GetLastError();

		// Was it a timeout?
		if (nLastErr == ERROR_SEM_TIMEOUT)
		{
			return (XN_STATUS_USB_TRANSFER_TIMEOUT);
		}

		// Nope, return a generic error
		return (XN_STATUS_USB_ENDPOINT_READ_FAILED);
	}

	// Did we get any data at all?
	if (*pnBytesReceived == 0)
	{
		return (XN_STATUS_USB_NOT_ENOUGH_DATA);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBWriteEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut)
{
	// Local variables
	BOOL bResult = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;
	ULONG nBytesSent = 0;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PEP_HANDLE(pEPHandle);

	// Validate the input/output pointers
	XN_VALIDATE_INPUT_PTR(pBuffer);

	// Is this an OUT endpoint? It must be if we're going to write into it!
	if (pEPHandle->nEPDir != XN_USB_DIRECTION_OUT)
	{
		return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
	}

	// Make sure we are not sending an empty buffer
	if (nBufferSize == 0)
	{
		return (XN_STATUS_USB_BUFFER_TOO_SMALL);
	}

	// Update the EP timeout
	if (nTimeOut != pEPHandle->nTimeOut)
	{
		nRetVal = xnUSBSetPipeProperty(pEPHandle, PSUSBDRV_PIPE_PROPERTY_TIMEOUT, nTimeOut);
		XN_IS_STATUS_OK(nRetVal);

		pEPHandle->nTimeOut = nTimeOut;
	}

	// Reset the endpoint
	if (pEPHandle->nEPType == XN_USB_EP_ISOCHRONOUS)
	{
		nRetVal = xnUSBResetEndPoint(pEPHandle);
		XN_IS_STATUS_OK(nRetVal);
	}

	// Write into the EP
	bResult = WriteFile(pEPHandle->hEPHandle, pBuffer, nBufferSize,  (PULONG) &nBytesSent, NULL);
	if (bResult == FALSE)
	{
		// Something failed... let's get the last error
		DWORD nLastErr = GetLastError();

		// Was it a timeout? 
		if (nLastErr == ERROR_SEM_TIMEOUT)
		{
			return (XN_STATUS_USB_TRANSFER_TIMEOUT);
		}

		// Nope, return a generic error
		return (XN_STATUS_USB_ENDPOINT_WRITE_FAILED);
	}

	// Make sure we didn't miss any bytes from the buffer
	if (nBytesSent != nBufferSize)
	{
		return (XN_STATUS_USB_GOT_UNEXPECTED_BYTES);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBQueueReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut)
{
	// Local variables
	BOOL bResult = FALSE;
	XnStatus nRetVal = XN_STATUS_OK;
	ULONG nBytesRcvd = 0;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PEP_HANDLE(pEPHandle);

	// Validate the input/output pointers
	XN_VALIDATE_OUTPUT_PTR(pBuffer);

	// Is this an IN endpoint? It must be if we're going to read from it!
	if (pEPHandle->nEPDir != XN_USB_DIRECTION_IN)
	{
		return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
	}

	// Make sure the receive buffer is big enough to hold some data
	if (nBufferSize == 0)
	{
		return (XN_STATUS_USB_BUFFER_TOO_SMALL);
	}

	// Update the EP timeout
	if (nTimeOut != pEPHandle->nTimeOut)
	{
		nRetVal = xnUSBSetPipeProperty(pEPHandle, PSUSBDRV_PIPE_PROPERTY_TIMEOUT, nTimeOut);
		XN_IS_STATUS_OK(nRetVal);

		pEPHandle->nTimeOut = nTimeOut;
	}

	// Reset the endpoint
	if (pEPHandle->nEPType == XN_USB_EP_ISOCHRONOUS)
	{
		nRetVal = xnUSBResetEndPoint(pEPHandle);
		XN_IS_STATUS_OK(nRetVal);
	}

	// Queue the read via overlapped I/O
	bResult = ReadFile(pEPHandle->hEPHandleOvlp, pBuffer, nBufferSize, (PULONG)&nBytesRcvd, &pEPHandle->ovlpIO);
	if (bResult == FALSE)
	{
		// If everything is OK, we're expecting to get an ERROR_IO_PENDING error
		DWORD nLastErr = GetLastError();
		if (nLastErr != ERROR_IO_PENDING)
		{
			// That's not what we expected, so return a generic error
			return (XN_STATUS_USB_ENDPOINT_READ_FAILED);
		}
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBFinishReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut)
{	
	// Local variables
	XnBool bRetVal = TRUE;
	DWORD nWaitRetVal = 0;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PEP_HANDLE(pEPHandle);

	// Validate the input/output pointers
	XN_VALIDATE_OUTPUT_PTR(pnBytesReceived);

	// Did the read command finish yet? if not, wait for it to complete or to timeout	
	nWaitRetVal = WaitForSingleObject(pEPHandle->ovlpIO.hEvent, nTimeOut);
	if (nWaitRetVal != 0)
	{
		// Did it timeout?
		if (nWaitRetVal == WAIT_TIMEOUT)
		{
			return (XN_STATUS_USB_TRANSFER_TIMEOUT);
		}
		else // Nope, so return a generic error
		{
			return (XN_STATUS_USB_OVERLAPIO_FAILED);
		}
	}

	// Get the operation status and the amount of received bytes
	bRetVal = GetOverlappedResult(pEPHandle->hEPHandleOvlp, &pEPHandle->ovlpIO, (DWORD*)pnBytesReceived, FALSE);
	XN_IS_BOOL_OK_RET(bRetVal, XN_STATUS_USB_OVERLAPIO_FAILED);

	// All is good...
	return (XN_STATUS_OK);
}

XN_THREAD_PROC xnUSBReadThreadMain(XN_THREAD_PARAM pThreadParam)
{
	// Local variables
	xnUSBReadThreadData* pThreadData = (xnUSBReadThreadData*)pThreadParam;
	XnUInt32 nBufIdx = 0;
	XnBool bResult = FALSE;
	ULONG nBytesRead = 0;
	HANDLE hCompletionPort = NULL;
	HANDLE hEPOvlp = NULL;
	OVERLAPPED* pOvlpIO = NULL;
	xnUSBBuffersInfo* pBuffersInfo = NULL;
	OVERLAPPED* pFinishedOV = NULL;
	ULONG_PTR nFinishedEP = 0;
	XnUInt32 nTimeOut = 0;
	XnUInt32 nBufferSize = 0;
	ULONG_PTR nOVIdx = 0;
	XnUSBReadCallbackFunctionPtr pCallbackFunction = NULL;
	PVOID pCallbackData = NULL;

	// Verify that we have a valid ThreadData pointer
	if (pThreadData == NULL)
	{
		return (XN_STATUS_ERROR);
	}

	// Dereference common variables
	hEPOvlp = pThreadData->pEPHandle->hEPHandleOvlp;
	pOvlpIO = pThreadData->pOvlpIO;
	pBuffersInfo = pThreadData->pBuffersInfo;
	nTimeOut = pThreadData->nTimeOut;
	nBufferSize = pThreadData->nBufferSize;
	pCallbackFunction = pThreadData->pCallbackFunction;
	pCallbackData = pThreadData->pCallbackData;

	// Raise the thread priority to real time. We don't want to miss any USB data and we don't want anyone to interrupt us.
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	// Create the overlapped I/O completion port
	hCompletionPort = CreateIoCompletionPort(hEPOvlp, NULL, pThreadData->pEPHandle->nEndPointID, 0);
	if (hCompletionPort == NULL)
	{
		//printf ("xnUSBReadThreadMain: CreateIoCompletionPort error!\n");
		return (XN_STATUS_ERROR);	
	}

	// Queue the initial overlapped reads
	for (nBufIdx=0; nBufIdx<pThreadData->nNumBuffers; nBufIdx++)
	{
		bResult = ReadFile(hEPOvlp, pBuffersInfo[nBufIdx].pBuffer, nBufferSize, (PULONG)&nBytesRead, &pOvlpIO[nBufIdx]);
/*
		if (bResult == FALSE)
		{
			nLastErr = GetLastError();
			if (nLastErr != ERROR_IO_PENDING)
			{
				printf ("xnUSBReadThreadMain: pre-loop read error! (%d)\n", nLastErr);
				//return (XN_STATUS_ERROR);
			}
		}
*/
	}

	// Loop forever... (unless signaled to die)
	for (;;)
	{
		// Time to die?
		if(pThreadData->bKillReadThread)
		{
			if (hCompletionPort != NULL)
			{
				CloseHandle(hCompletionPort);
			}

			return (XN_STATUS_OK);
		}	

		// Wait for any of our requests to complete
		bResult = GetQueuedCompletionStatus(hCompletionPort, &nBytesRead, &nFinishedEP, &pFinishedOV, nTimeOut);
		if (bResult == FALSE)
		{
//			printf ("xnUSBReadThreadMain: GetQueuedCompletionStatus error! (%d)\n", GetLastError());

			// if the error is SEM_TIMEOUT, the overlapped I/O operation has ended with a timeout, 
			// and we still need to requeue it. Other errors does not return the Finished OV, and so
			// we should just try again.
			if (GetLastError() != ERROR_SEM_TIMEOUT)
				continue;

			//return (XN_STATUS_ERROR);
		}

		// Calculate which request came back
		nOVIdx = pFinishedOV - pOvlpIO;

//		printf("GetQueuedCompletionStatus: Buffer:%d Bytes:%d EP:0x%x Status:%d\n", nOVIdx, nBytesRead, nFinishedEP, bResult);

		// If the request was completed successfully, call the callback function
		if (bResult == TRUE)
		{
			pCallbackFunction(pBuffersInfo[nOVIdx].pBuffer, nBytesRead, pCallbackData);
		}

		// Re-queue the request
		bResult = ReadFile(hEPOvlp, pBuffersInfo[nOVIdx].pBuffer, nBufferSize, NULL, pFinishedOV);
/*
		if (bResult == FALSE)
		{
			nLastErr = GetLastError();
			if (nLastErr != ERROR_IO_PENDING)
			{
				printf ("xnUSBReadThreadMain: loop read error! (%d)\n", nLastErr);
				//return (XN_STATUS_ERROR);
			}
		}
*/
	}
}

XN_C_API XnStatus xnUSBInitReadThread(XN_USB_EP_HANDLE pEPHandle, XnUInt32 nBufferSize, XnUInt32 nNumBuffers, XnUInt32 nTimeOut, XnUSBReadCallbackFunctionPtr pCallbackFunction, PVOID pCallbackData)
{
	// Local variables
	XnStatus nRetVal = XN_STATUS_OK;
	xnUSBReadThreadData* pThreadData = NULL;
	XnUInt32 nBufIdx = 0;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PEP_HANDLE(pEPHandle);

	// Validate the input/output pointers
	XN_VALIDATE_INPUT_PTR(pCallbackFunction);

	// Dereference the ThreadData
	pThreadData = &pEPHandle->ThreadData;

	// Does this EP already have a read thread?
	if (pThreadData->bInUse == TRUE)
	{
		return (XN_STATUS_USB_READTHREAD_ALREADY_INIT);
	}

	// Wipe the ThreadData memory
	xnOSMemSet(pThreadData, 0, sizeof(xnUSBReadThreadData));

	// Init the ThreadData structure
	pThreadData->pEPHandle = pEPHandle;
	pThreadData->nBufferSize = nBufferSize;
	pThreadData->nNumBuffers = nNumBuffers;
	pThreadData->nTimeOut = nTimeOut;
	pThreadData->pCallbackFunction = pCallbackFunction;
	pThreadData->pCallbackData = pCallbackData;
	pThreadData->bKillReadThread = FALSE;

	// Update the EP timeout
	if (nTimeOut != pEPHandle->nTimeOut)
	{
		nRetVal = xnUSBSetPipeProperty(pEPHandle, PSUSBDRV_PIPE_PROPERTY_TIMEOUT, nTimeOut);
		XN_IS_STATUS_OK(nRetVal);

		pEPHandle->nTimeOut = nTimeOut;
	}	

	// Allocate the read buffers and create the overlapped I/O events
	XN_VALIDATE_ALIGNED_CALLOC(pThreadData->pBuffersInfo, xnUSBBuffersInfo, nNumBuffers, XN_DEFAULT_MEM_ALIGN);
	XN_VALIDATE_ALIGNED_CALLOC(pThreadData->pOvlpIO, OVERLAPPED, nNumBuffers, XN_DEFAULT_MEM_ALIGN);

	for (nBufIdx=0; nBufIdx<nNumBuffers; nBufIdx++)
	{
		nRetVal = xnOSCreateEvent(&pThreadData->pOvlpIO[nBufIdx].hEvent, FALSE);
		XN_IS_STATUS_OK(nRetVal); // Add cleanup memory!

		XN_VALIDATE_ALIGNED_CALLOC(pThreadData->pBuffersInfo[nBufIdx].pBuffer, XnUChar, nBufferSize, XN_DEFAULT_MEM_ALIGN); // Add cleanup memory!
	}

	// Create the read thread
	nRetVal = xnOSCreateThread(xnUSBReadThreadMain, &pEPHandle->ThreadData, &pThreadData->hReadThread);
	XN_IS_STATUS_OK(nRetVal); // Add cleanup memory!

	// Mark that this EP has a valid read thread
	pThreadData->bInUse = TRUE;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBShutdownReadThread(XN_USB_EP_HANDLE pEPHandle)
{
	// Local variables
	XnStatus nRetVal = XN_STATUS_OK;
	xnUSBReadThreadData* pThreadData = NULL;

	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_USB_PEP_HANDLE(pEPHandle);

	// Dereference the ThreadData
	pThreadData = &pEPHandle->ThreadData;

	// Make sure that this EP has a read thread active
	if (pThreadData->bInUse == FALSE)
	{
		return (XN_STATUS_USB_READTHREAD_NOT_INIT);
	}

	// Make sure that the read thread is alive
	if (pThreadData->hReadThread != NULL)
	{
		// Request for the read thread to die
		pThreadData->bKillReadThread = TRUE;
		
		// close thread
		xnOSWaitAndTerminateThread(&pThreadData->hReadThread, XN_USB_READ_THREAD_KILL_TIMEOUT);

		pThreadData->hReadThread = NULL;
	}

	// Free any buffers and events if necessary...
	if (pThreadData->pBuffersInfo != NULL)
	{
		for (XnUInt32 nBufIdx = 0; nBufIdx < pThreadData->nNumBuffers; nBufIdx++)
		{
			if (pThreadData->pOvlpIO[nBufIdx].hEvent != NULL)
			{
				xnOSCloseEvent(&pThreadData->pOvlpIO[nBufIdx].hEvent);
			}

			if (pThreadData->pBuffersInfo[nBufIdx].pBuffer != NULL)
			{
				XN_ALIGNED_FREE_AND_NULL(pThreadData->pBuffersInfo[nBufIdx].pBuffer);
			}
		}

		XN_ALIGNED_FREE_AND_NULL(pThreadData->pBuffersInfo);
		XN_ALIGNED_FREE_AND_NULL(pThreadData->pOvlpIO);
	}

	// Re-init the overlapped I/O structs (they cannot be reused because of completion I/O port)
	nRetVal = xnUSBInitOvlp(pEPHandle);
	if (nRetVal != XN_STATUS_OK)
	{
		return (XN_STATUS_USB_READTHREAD_SHUTDOWN_FAILED);						
	}

	// Mark that this EP doesn't have a read thread anymore
	pThreadData->bInUse = FALSE;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XN_C_DECL xnUSBRegisterToConnectivityEvents(XnUInt16 nVendorID, XnUInt16 nProductID, XnUSBDeviceCallbackFunctionPtr pFunc, void* pCookie, XnRegistrationHandle* phRegistration)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pFunc);
	XN_VALIDATE_OUTPUT_PTR(phRegistration);

	XnUInt32 nDummy;

	XnUSBEventCallback* pCallback;
	XN_VALIDATE_NEW(pCallback, XnUSBEventCallback);
	pCallback->pFunc = pFunc;
	pCallback->pCookie = pCookie;
	xnOSStrFormat(pCallback->strVidPid, sizeof(pCallback->strVidPid), &nDummy, "vid_%04x&pid_%04x", nVendorID, nProductID);

	nRetVal = g_connectivityEvent.AddLast(pCallback);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pCallback);
		return (nRetVal);
	}

	*phRegistration = (XnRegistrationHandle)pCallback;

	return XN_STATUS_OK;
}

XN_C_API void XN_C_DECL xnUSBUnregisterFromConnectivityEvents(XnRegistrationHandle hRegistration)
{
	XnUSBEventCallback* pCallback = reinterpret_cast<XnUSBEventCallback*>(hRegistration);
	XnUSBEventCallbackList::Iterator it = g_connectivityEvent.Find(pCallback);
	if (it != g_connectivityEvent.End())
	{
		g_connectivityEvent.Remove(it);
		XN_DELETE(pCallback);
	}
}

//---------------------------------------------------------------------------
// Deprecated stuff
//---------------------------------------------------------------------------
static XnRegistrationHandle g_hDeprecatedCallback = NULL;
static XnUSBEventCallbackFunctionPtr g_pDeprecatedCallbackFunc = NULL;
static void* g_pDeprecatedCallbackCookie = NULL;

static void XN_CALLBACK_TYPE XnUSBDeviceCallbackBC(XnUSBEventArgs* pArgs, void* /*pCookie*/)
{
	if (g_pDeprecatedCallbackFunc != NULL)
	{
		g_pDeprecatedCallbackFunc(pArgs->eventType, const_cast<XnChar*>(pArgs->strDevicePath), g_pDeprecatedCallbackCookie);
	}
}

XN_C_API XnStatus xnUSBSetCallbackHandler(XnUInt16 nVendorID, XnUInt16 nProductID, void* /*pExtraParam*/, XnUSBEventCallbackFunctionPtr pCallbackFunction, void* pCallbackData)
{
	if (g_hDeprecatedCallback == NULL)
	{
		XnStatus nRetVal = xnUSBRegisterToConnectivityEvents(nVendorID, nProductID, XnUSBDeviceCallbackBC, NULL, &g_hDeprecatedCallback);
		XN_IS_STATUS_OK(nRetVal);

		// Ugly hack: previous behavior was to ignore vendor and product, and call callback for every event
		// we place a string that is always there.
		XnUSBEventCallback* pCallback = (XnUSBEventCallback*)g_hDeprecatedCallback;
		xnOSStrCopy(pCallback->strVidPid, "vid", sizeof(pCallback->strVidPid));
	}

	g_pDeprecatedCallbackFunc = pCallbackFunction;
	g_pDeprecatedCallbackCookie = pCallbackData;

	return XN_STATUS_OK;
}

