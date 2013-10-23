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

#if (XN_PLATFORM == XN_PLATFORM_ANDROID_ARM)
#include <libusb.h>
#else
#include <libusb-1.0/libusb.h>
#endif

#include "XnUSBLinux.h"
#include "../XnUSBInternal.h"
#include <XnOS.h>
#include <XnLog.h>
#include <XnOSCpp.h>
#include "XnListT.h"


//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnUSBEventCallback
{
	XnUSBDeviceCallbackFunctionPtr pFunc;
	void* pCookie;
} XnUSBEventCallback;

typedef XnListT<XnUSBEventCallback*> XnUSBEventCallbackList;

XnUSBEventCallbackList g_connectivityEvent;

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define MAX_DEVPATH_LENGTH 256
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR			(0x02 << 5)
#define USB_ENDPOINT_IN			0x80

#define XN_MASK_USB "xnUSB"

#define XN_USB_HANDLE_EVENTS_TIMEOUT 1000

#define XN_VALIDATE_DEVICE_HANDLE(x)					\
	if (x == NULL)									\
		return (XN_STATUS_USB_DEVICE_NOT_VALID);
		
#define XN_VALIDATE_EP_HANDLE(x)						\
	if (x == NULL)									\
		return (XN_STATUS_USB_ENDPOINT_NOT_VALID);

struct xnUSBInitData
{
	libusb_context* pContext;
	XN_THREAD_HANDLE hThread;
	XnBool bShouldThreadRun;
	XnUInt32 nOpenDevices;
	XN_CRITICAL_SECTION_HANDLE hLock;
} g_InitData = {NULL, NULL, FALSE, 0, NULL};

XnStatus xnUSBPlatformSpecificShutdown();

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_THREAD_PROC xnUSBHandleEventsThread(XN_THREAD_PARAM pThreadParam)
{
	// init timeout
	struct timeval timeout;
	timeout.tv_sec = XN_USB_HANDLE_EVENTS_TIMEOUT / 1000;
	timeout.tv_usec = XN_USB_HANDLE_EVENTS_TIMEOUT % 1000;
	
	while (g_InitData.bShouldThreadRun)
	{
		// let libusb process its asynchronous events
		libusb_handle_events_timeout(g_InitData.pContext, &timeout);
	}
	
	XN_THREAD_PROC_RETURN(XN_STATUS_OK);
}

XnStatus xnUSBPlatformSpecificInit()
{
	xnLogVerbose(XN_MASK_USB, "Initializing USB...");

	// initialize the library
	int rc = libusb_init(&g_InitData.pContext);
	if (rc != 0)
	{
		return (XN_STATUS_USB_INIT_FAILED);
	}

	XnStatus nRetVal = xnOSCreateCriticalSection(&g_InitData.hLock);
	XN_IS_STATUS_OK(nRetVal);
	
	//libusb_set_debug(g_InitData.pContext, 3);
	
	xnLogInfo(XN_MASK_USB, "USB is initialized.");
	return (XN_STATUS_OK);	
}

XnStatus xnUSBAsynchThreadAddRef()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnAutoCSLocker locker(g_InitData.hLock);

	++g_InitData.nOpenDevices;
	
	if (g_InitData.hThread == NULL)
	{
		xnLogVerbose(XN_MASK_USB, "Starting libusb asynch thread...");
		
		// mark thread should run
		g_InitData.bShouldThreadRun = TRUE;
		
		// and start thread
		nRetVal = xnOSCreateThread(xnUSBHandleEventsThread, NULL, &g_InitData.hThread);
		if (nRetVal != XN_STATUS_OK)
		{
			// clean-up
			xnUSBPlatformSpecificShutdown();
			return nRetVal;
		}

		// set thread priority to critical
		nRetVal = xnOSSetThreadPriority(g_InitData.hThread, XN_PRIORITY_CRITICAL);
		if (nRetVal != 0)
		{
			xnLogWarning(XN_MASK_USB, "USB events thread: Failed to set thread priority to critical. This might cause loss of data...");
			printf("Warning: USB events thread - failed to set priority. This might cause loss of data...\n");
		}
	}
	
	return (XN_STATUS_OK);	
}

void xnUSBAsynchThreadStop()
{
	if (g_InitData.hThread != NULL)
	{
		// mark for thread to exit
		g_InitData.bShouldThreadRun = FALSE;
		
		// wait for it to exit
		xnLogWarning(XN_MASK_USB, "Shutting down USB events thread...");
		XnStatus nRetVal = xnOSWaitForThreadExit(g_InitData.hThread, XN_USB_HANDLE_EVENTS_TIMEOUT);
		if (nRetVal != XN_STATUS_OK)
		{
			xnOSTerminateThread(&g_InitData.hThread);
		}
		else
		{
			xnOSCloseThread(&g_InitData.hThread);
		}
		
		g_InitData.hThread = NULL;
	}
}

void xnUSBAsynchThreadRelease()
{
	XnAutoCSLocker locker(g_InitData.hLock);

	--g_InitData.nOpenDevices;

	if (g_InitData.nOpenDevices == 0)
	{
		xnUSBAsynchThreadStop();
	}
}

XnStatus xnUSBPlatformSpecificShutdown()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnUSBAsynchThreadStop();

	if (g_InitData.hLock != NULL)
	{
		xnOSCloseCriticalSection(&g_InitData.hLock);
		g_InitData.hLock = NULL;
	}
	
	if (g_InitData.pContext != NULL)
	{
		// close the library
		libusb_exit(g_InitData.pContext);
		g_InitData.pContext = NULL;
	}
	
	return (XN_STATUS_OK);
}

/*
* Finds a USB device.
* the returned device must be unreferenced when it is no longer needed using libusb_unref_device.
*/
XnStatus FindDevice(XnUInt16 nVendorID, XnUInt16 nProductID, void* pExtraParam, libusb_device** ppDevice)
{
	*ppDevice = NULL;

	// get device list
	libusb_device** ppDevices;
	ssize_t nDeviceCount = libusb_get_device_list(g_InitData.pContext, &ppDevices);
	
	// check for error
	if (nDeviceCount < 0)
	{
		return (XN_STATUS_USB_ENUMERATE_FAILED);
	}
	
	// enumerate over the devices
	for (ssize_t i = 0; i < nDeviceCount; ++i)
	{
		libusb_device* pDevice = ppDevices[i];
		
		// get device descriptor
		libusb_device_descriptor desc;
		int rc = libusb_get_device_descriptor(pDevice, &desc);
		if (rc != 0)
		{
			return (XN_STATUS_USB_ENUMERATE_FAILED);
		}
		
		// check if this is the requested device
		if (desc.idVendor == nVendorID && desc.idProduct == nProductID)
		{
			// add a reference to the device (so it won't be destroyed when list is freed)
			libusb_ref_device(pDevice);
			*ppDevice = pDevice;
			break;
		}
	}
	
	// free the list (also dereference each device)
	libusb_free_device_list(ppDevices, 1);
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBIsDevicePresent(XnUInt16 nVendorID, XnUInt16 nProductID, void* pExtraParam, XnBool* pbDevicePresent)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// make sure library was initialized
	XN_VALIDATE_USB_INIT();
	
	// Validate parameters
	XN_VALIDATE_OUTPUT_PTR(pbDevicePresent);

	*pbDevicePresent = FALSE;
	
	libusb_device* pDevice;
	nRetVal = FindDevice(nVendorID, nProductID, pExtraParam, &pDevice);
	XN_IS_STATUS_OK(nRetVal);
		
	if (pDevice != NULL)
	{
		*pbDevicePresent = TRUE;
		
		// unref device
		libusb_unref_device(pDevice);
	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBEnumerateDevices(XnUInt16 nVendorID, XnUInt16 nProductID, const XnUSBConnectionString** pastrDevicePaths, XnUInt32* pnCount)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// get device list
	libusb_device** ppDevices;
	ssize_t nDeviceCount = libusb_get_device_list(g_InitData.pContext, &ppDevices);
	
	// first enumeration - count
	XnUInt32 nCount = 0;
	
	for (ssize_t i = 0; i < nDeviceCount; ++i)
	{
		libusb_device* pDevice = ppDevices[i];
		
		// get device descriptor
		libusb_device_descriptor desc;
		int rc = libusb_get_device_descriptor(pDevice, &desc);
		if (rc != 0)
		{
			libusb_free_device_list(ppDevices, 1);
			return (XN_STATUS_USB_ENUMERATE_FAILED);
		}
		
		// check if this is the requested device
		if (desc.idVendor == nVendorID && desc.idProduct == nProductID)
		{
			++nCount;
		}
	}
	
	// allocate array
	XnUSBConnectionString* aResult = (XnUSBConnectionString*)xnOSCalloc(nCount, sizeof(XnUSBConnectionString));
	if (aResult == NULL)
	{
		libusb_free_device_list(ppDevices, 1);
		return XN_STATUS_ALLOC_FAILED;
	}
	
	// second enumeration - fill
	XnUInt32 nCurrent = 0;
	for (ssize_t i = 0; i < nDeviceCount; ++i)
	{
		libusb_device* pDevice = ppDevices[i];
		
		// get device descriptor
		libusb_device_descriptor desc;
		int rc = libusb_get_device_descriptor(pDevice, &desc);
		if (rc != 0)
		{
			libusb_free_device_list(ppDevices, 1);
			return (XN_STATUS_USB_ENUMERATE_FAILED);
		}
		
		// check if this is the requested device
		if (desc.idVendor == nVendorID && desc.idProduct == nProductID)
		{
			sprintf(aResult[nCurrent], "%04hx/%04hx@%hhu/%hhu", nVendorID, nProductID, libusb_get_bus_number(pDevice), libusb_get_device_address(pDevice));
			nCurrent++;
		}
	}
	
	*pastrDevicePaths = aResult;
	*pnCount = nCount;
		
	// free the list (also dereference each device)
	libusb_free_device_list(ppDevices, 1);
	
	return XN_STATUS_OK;
}

XN_C_API void xnUSBFreeDevicesList(const XnUSBConnectionString* astrDevicePaths)
{
	xnOSFree(astrDevicePaths);
}

XN_C_API XnStatus xnUSBOpenDeviceImpl(libusb_device* pDevice, XN_USB_DEV_HANDLE* pDevHandlePtr)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (pDevice == NULL)
	{
		return (XN_STATUS_USB_DEVICE_NOT_FOUND);
	}

	// allocate device handle
	libusb_device_handle* handle;
	
	// open device
	int rc = libusb_open(pDevice, &handle);
	
	// in any case, unref the device (we don't need it anymore)
	libusb_unref_device(pDevice);
	pDevice = NULL;
	
	// now check if open failed
	if (rc != 0)
	{
		return (XN_STATUS_USB_DEVICE_OPEN_FAILED);
	}
	
/*	
	// set for the first (and only) configuration (this will perform a light-weight reset)
	rc = libusb_set_configuration(handle, 1);
	if (rc != 0)
	{
		libusb_close(handle);
		return (XN_STATUS_USB_SET_CONFIG_FAILED);
	}
*/	
	// claim the interface (you cannot open any end point before claiming the interface)
	rc = libusb_claim_interface(handle, 0);
	if (rc != 0)
	{
		libusb_close(handle);
		return (XN_STATUS_USB_SET_INTERFACE_FAILED);
	}
	
/*	
	// set the alternate setting to default
	rc = libusb_set_interface_alt_setting(handle, 0, 0);
	if (rc != 0)
	{
		libusb_close(handle);
		return (XN_STATUS_USB_SET_INTERFACE_FAILED);
	}
*/	
	XN_VALIDATE_ALLOC(*pDevHandlePtr, XnUSBDeviceHandle);
	XN_USB_DEV_HANDLE pDevHandle = *pDevHandlePtr;
	pDevHandle->hDevice = handle;
	pDevHandle->nInterface = 0;
	pDevHandle->nAltSetting = 0;
	
	// mark the device is of high-speed
	pDevHandle->nDevSpeed = XN_USB_DEVICE_HIGH_SPEED;
	
	nRetVal = xnUSBAsynchThreadAddRef();
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(*pDevHandlePtr);
		return (nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBOpenDevice(XnUInt16 nVendorID, XnUInt16 nProductID, void* pExtraParam, void* pExtraParam2, XN_USB_DEV_HANDLE* pDevHandlePtr)
{
	XnStatus nRetVal = XN_STATUS_OK;
		
	// make sure library was initialized
	XN_VALIDATE_USB_INIT();
	
	// Validate parameters
	XN_VALIDATE_OUTPUT_PTR(pDevHandlePtr);

	libusb_device* pDevice;
	nRetVal = FindDevice(nVendorID, nProductID, pExtraParam, &pDevice);
	XN_IS_STATUS_OK(nRetVal);
		
	nRetVal = xnUSBOpenDeviceImpl(pDevice, pDevHandlePtr);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBOpenDeviceByPath(const XnUSBConnectionString strDevicePath, XN_USB_DEV_HANDLE* pDevHandlePtr)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// parse connection string
	XnUInt16 nVendorID = 0;
	XnUInt16 nProductID = 0;
	XnUInt8 nBus = 0;
	XnUInt8 nAddress = 0;
	sscanf(strDevicePath, "%hx/%hx@%hhu/%hhu", &nVendorID, &nProductID, &nBus, &nAddress);
	
	if (nVendorID == 0 || nProductID == 0 || nBus == 0 || nAddress == 0)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_USB_DEVICE_OPEN_FAILED, "Invalid connection string: %s", strDevicePath);
	}

	// find device	
	libusb_device** ppDevices;
	ssize_t nDeviceCount = libusb_get_device_list(g_InitData.pContext, &ppDevices);
	
	libusb_device* pRequestedDevice = NULL;
	
	for (ssize_t i = 0; i < nDeviceCount; ++i)
	{
		libusb_device* pDevice = ppDevices[i];
		
		// get device descriptor
		libusb_device_descriptor desc;
		int rc = libusb_get_device_descriptor(pDevice, &desc);
		if (rc != 0)
		{
			libusb_free_device_list(ppDevices, 1);
			return (XN_STATUS_USB_ENUMERATE_FAILED);
		}
		
		// check if this is the requested device
		if (desc.idVendor == nVendorID && desc.idProduct == nProductID && libusb_get_bus_number(pDevice) == nBus && libusb_get_device_address(pDevice) == nAddress)
		{
			// add a reference to the device (so it won't be destroyed when list is freed)
			libusb_ref_device(pDevice);
			pRequestedDevice = pDevice;
			break;	
		}
	}

	libusb_free_device_list(ppDevices, 1);
	
	nRetVal = xnUSBOpenDeviceImpl(pRequestedDevice, pDevHandlePtr);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBCloseDevice(XN_USB_DEV_HANDLE pDevHandle)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_DEVICE_HANDLE(pDevHandle);

	int rc = libusb_release_interface(pDevHandle->hDevice, pDevHandle->nInterface);
	if (0 != rc)
	{
		return (XN_STATUS_USB_DEVICE_CLOSE_FAILED);
	}

	libusb_close(pDevHandle->hDevice);

	XN_FREE_AND_NULL(pDevHandle);
	
	xnUSBAsynchThreadRelease();

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBGetDeviceSpeed(XN_USB_DEV_HANDLE pDevHandle, XnUSBDeviceSpeed* pDevSpeed)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_DEVICE_HANDLE(pDevHandle);
	XN_VALIDATE_OUTPUT_PTR(pDevSpeed);

	*pDevSpeed = pDevHandle->nDevSpeed;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBSetConfig(XN_USB_DEV_HANDLE pDevHandle, XnUInt8 nConfig)
{
	return (XN_STATUS_OS_UNSUPPORTED_FUNCTION);
}

XN_C_API XnStatus xnUSBGetConfig(XN_USB_DEV_HANDLE pDevHandle, XnUInt8* pnConfig)
{
	return (XN_STATUS_OS_UNSUPPORTED_FUNCTION);
}

XN_C_API XnStatus xnUSBSetInterface(XN_USB_DEV_HANDLE pDevHandle, XnUInt8 nInterface, XnUInt8 nAltInterface)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_DEVICE_HANDLE(pDevHandle);
	
	int rc = libusb_set_interface_alt_setting(pDevHandle->hDevice, nInterface, nAltInterface);
	if (rc != 0)
	{
		return (XN_STATUS_USB_SET_INTERFACE_FAILED);
	}
	
	pDevHandle->nInterface = nInterface;
	pDevHandle->nAltSetting = nAltInterface;
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBGetInterface(XN_USB_DEV_HANDLE pDevHandle, XnUInt8* pnInterface, XnUInt8* pnAltInterface)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_C_API XnStatus xnUSBOpenEndPoint(XN_USB_DEV_HANDLE pDevHandle, XnUInt16 nEndPointID, XnUSBEndPointType nEPType, XnUSBDirectionType nDirType, XN_USB_EP_HANDLE* pEPHandlePtr)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_DEVICE_HANDLE(pDevHandle);
	XN_VALIDATE_OUTPUT_PTR(pEPHandlePtr);

	// get the device from the handle
	libusb_device* pDevice = libusb_get_device(pDevHandle->hDevice);
	
	// get the configuration descriptor
	libusb_config_descriptor* pConfig;
	int rc = libusb_get_active_config_descriptor(pDevice, &pConfig);
	if (rc != 0)
	{
		return (XN_STATUS_USB_CONFIG_QUERY_FAILED);
	}
	
	// make sure configuration contains the interface we need
	if (pConfig->bNumInterfaces <= pDevHandle->nInterface)
	{
		libusb_free_config_descriptor(pConfig);
		return (XN_STATUS_USB_INTERFACE_QUERY_FAILED);
	}
	
	// take that interface
	const libusb_interface* pInterface = &pConfig->interface[pDevHandle->nInterface];
	
	// make sure interface contains the alternate setting we work with
	if (pInterface->num_altsetting <= pDevHandle->nAltSetting)
	{
		libusb_free_config_descriptor(pConfig);
		return (XN_STATUS_USB_INTERFACE_QUERY_FAILED);
	}
	
	// take that setting
	const libusb_interface_descriptor* pInterfaceDesc = &pInterface->altsetting[pDevHandle->nAltSetting];
	
	// search for the requested endpoint
	const libusb_endpoint_descriptor* pEndpointDesc = NULL;
	
	for (uint8_t i = 0; i < pInterfaceDesc->bNumEndpoints; ++i)
	{
		if (pInterfaceDesc->endpoint[i].bEndpointAddress == nEndPointID)
		{
			pEndpointDesc = &pInterfaceDesc->endpoint[i];
			break;
		}
	}
	
	if (pEndpointDesc == NULL)
	{
		libusb_free_config_descriptor(pConfig);
		return (XN_STATUS_USB_ENDPOINT_NOT_FOUND);
	}
	
	libusb_transfer_type transfer_type = (libusb_transfer_type)(pEndpointDesc->bmAttributes & 0x3); // lower 2-bits

    // calculate max packet size
	// NOTE: we do not use libusb functions (libusb_get_max_packet_size/libusb_get_max_iso_packet_size) because
	// they hace a bug and does not consider alternative interface
    XnUInt32 nMaxPacketSize = 0;
	
	if (transfer_type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)
	{
		XnUInt32 wMaxPacketSize = pEndpointDesc->wMaxPacketSize;
		// bits 11 and 12 mark the number of additional transactions, bits 0-10 mark the size
		XnUInt32 nAdditionalTransactions = wMaxPacketSize >> 11;
		XnUInt32 nPacketSize = wMaxPacketSize & 0x7FF;
		nMaxPacketSize = (nAdditionalTransactions + 1) * (nPacketSize);
	}
	else
	{
		nMaxPacketSize = pEndpointDesc->wMaxPacketSize;
	}

	// free the configuration descriptor. no need of it anymore
	libusb_free_config_descriptor(pConfig);
	pConfig = NULL;
	
	// Make sure the endpoint matches the required endpoint type
	if (nEPType == XN_USB_EP_BULK)
	{
		if (transfer_type != LIBUSB_TRANSFER_TYPE_BULK)
		{
			return (XN_STATUS_USB_WRONG_ENDPOINT_TYPE);
		}
	}
	else if (nEPType == XN_USB_EP_INTERRUPT)
	{
		if (transfer_type != LIBUSB_TRANSFER_TYPE_INTERRUPT)
		{
			return (XN_STATUS_USB_WRONG_ENDPOINT_TYPE);
		}
	}
	else if (nEPType == XN_USB_EP_ISOCHRONOUS)
	{
		if (transfer_type != LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)
		{
			return (XN_STATUS_USB_WRONG_ENDPOINT_TYPE);
		}
	}
	else
	{
		return (XN_STATUS_USB_UNKNOWN_ENDPOINT_TYPE);
	}

	// Make sure the endpoint matches the required direction
	libusb_endpoint_direction direction = (libusb_endpoint_direction)(nEndPointID & 0x80); // 8th bit
	if (nDirType == XN_USB_DIRECTION_IN)
	{
		if (direction != LIBUSB_ENDPOINT_IN)
		{
			return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
		}
	}
	else if (nDirType == XN_USB_DIRECTION_OUT)
	{
		if (direction != LIBUSB_ENDPOINT_OUT)
		{
			return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
		}
	}
	else
	{
		return (XN_STATUS_USB_UNKNOWN_ENDPOINT_DIRECTION);
	}
	
	// allocate handle
	XN_VALIDATE_ALIGNED_CALLOC(*pEPHandlePtr, XnUSBEPHandle, 1, XN_DEFAULT_MEM_ALIGN);
	XN_USB_EP_HANDLE pHandle = *pEPHandlePtr;
	pHandle->hDevice = pDevHandle->hDevice;
	pHandle->nAddress = nEndPointID;
	pHandle->nType = nEPType;
	pHandle->nDirection = nDirType;
	pHandle->nMaxPacketSize = nMaxPacketSize;

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnUSBCloseEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_EP_HANDLE(pEPHandle);
	
	XN_ALIGNED_FREE_AND_NULL(pEPHandle);
	
	return XN_STATUS_OK;
}

XN_C_API XnStatus xnUSBGetEndPointMaxPacketSize(XN_USB_EP_HANDLE pEPHandle, XnUInt32* pnMaxPacketSize)
{
	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_EP_HANDLE(pEPHandle);

	// Validate the input/output pointers
	XN_VALIDATE_INPUT_PTR(pEPHandle);
	XN_VALIDATE_OUTPUT_PTR(pnMaxPacketSize);

	*pnMaxPacketSize = pEPHandle->nMaxPacketSize;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBAbortEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_C_API XnStatus xnUSBFlushEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_C_API XnStatus xnUSBResetEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_C_API XnStatus xnUSBSendControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, XnUInt8 nRequest, XnUInt16 nValue, XnUInt16 nIndex, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_DEVICE_HANDLE(pDevHandle);
	
	if (nBufferSize != 0)
	{
		XN_VALIDATE_INPUT_PTR(pBuffer);
	}
	
	uint8_t bmRequestType;
	
	if (nType == XN_USB_CONTROL_TYPE_VENDOR )
	{
		bmRequestType = LIBUSB_REQUEST_TYPE_VENDOR;
	}
	else if (nType == XN_USB_CONTROL_TYPE_CLASS)
	{
		bmRequestType = LIBUSB_REQUEST_TYPE_CLASS;
	}
	else if (nType == XN_USB_CONTROL_TYPE_STANDARD)
	{
		bmRequestType = LIBUSB_REQUEST_TYPE_STANDARD;
	}
	else
	{
		return (XN_STATUS_USB_WRONG_CONTROL_TYPE);
	}
	
	bmRequestType |= LIBUSB_ENDPOINT_OUT;
	
	// send	
	int nBytesSent = libusb_control_transfer(pDevHandle->hDevice, bmRequestType, nRequest, nValue, nIndex, pBuffer, nBufferSize, nTimeOut);
	
	// check everything went OK
	if (nBytesSent == LIBUSB_ERROR_TIMEOUT)
	{
		return (XN_STATUS_USB_TRANSFER_TIMEOUT);
	}
	else if (nBytesSent < 0)
	{
		return (XN_STATUS_USB_CONTROL_SEND_FAILED);
	}
	
	if (nBytesSent != nBufferSize)
	{
		return (XN_STATUS_USB_GOT_UNEXPECTED_BYTES);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBReceiveControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, XnUInt8 nRequest, XnUInt16 nValue, XnUInt16 nIndex, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_DEVICE_HANDLE(pDevHandle);
	XN_VALIDATE_OUTPUT_PTR(pBuffer);
	XN_VALIDATE_OUTPUT_PTR(pnBytesReceived);
	
	if (nBufferSize == 0)
	{
		return (XN_STATUS_USB_BUFFER_TOO_SMALL);
	}
	
	*pnBytesReceived = 0;

	uint8_t bmRequestType;
	
	if (nType == XN_USB_CONTROL_TYPE_VENDOR )
	{
		bmRequestType = LIBUSB_REQUEST_TYPE_VENDOR;
	}
	else if (nType == XN_USB_CONTROL_TYPE_CLASS)
	{
		bmRequestType = LIBUSB_REQUEST_TYPE_CLASS;
	}
	else if (nType == XN_USB_CONTROL_TYPE_STANDARD)
	{
		bmRequestType = LIBUSB_REQUEST_TYPE_STANDARD;
	}
	else
	{
		return (XN_STATUS_USB_WRONG_CONTROL_TYPE);
	}
	
	bmRequestType |= LIBUSB_ENDPOINT_IN;
	
	// send	
	int nBytesReceived = libusb_control_transfer(pDevHandle->hDevice, bmRequestType, nRequest, nValue, nIndex, pBuffer, nBufferSize, nTimeOut);
	
	// check everything went OK
	if (nBytesReceived == LIBUSB_ERROR_TIMEOUT)
	{
		return (XN_STATUS_USB_TRANSFER_TIMEOUT);
	}
	else if (nBytesReceived < 0) // error
	{
		xnLogWarning(XN_MASK_USB, "Failed to receive from USB control endpoint (%d)", nBytesReceived);
		return (XN_STATUS_USB_CONTROL_RECV_FAILED);
	}
	else if (nBytesReceived == 0) // nothing received
	{
		// received empty message
		return (XN_STATUS_USB_NOT_ENOUGH_DATA);
	}
	else if (nBytesReceived > nBufferSize) // too much
	{
		xnLogWarning(XN_MASK_USB, "Too many bytes!!!");
		return (XN_STATUS_USB_TOO_MUCH_DATA);
	}
	
	// return number of bytes received
	*pnBytesReceived = nBytesReceived;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBWriteEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_EP_HANDLE(pEPHandle);
	XN_VALIDATE_INPUT_PTR(pBuffer);

	if (pEPHandle->nDirection != XN_USB_DIRECTION_OUT)
	{
		return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
	}
	
	if (nBufferSize == 0)
	{
		return (XN_STATUS_USB_BUFFER_TOO_SMALL);
	}

	// send (according to EP type)
	int nBytesSent = 0;
	int rc = 0;
	
	if (pEPHandle->nType == XN_USB_EP_BULK)
	{
		rc = libusb_bulk_transfer(pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, &nBytesSent, nTimeOut);
	}
	else if (pEPHandle->nType == XN_USB_EP_INTERRUPT)
	{
		rc = libusb_interrupt_transfer(pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, &nBytesSent, nTimeOut);
	}
	else
	{
		return (XN_STATUS_USB_UNSUPPORTED_ENDPOINT_TYPE);
	}
	
	// check result
	if (rc == LIBUSB_ERROR_TIMEOUT)
	{
		return (XN_STATUS_USB_TRANSFER_TIMEOUT);
	}
	else if (rc != 0)
	{
		return (XN_STATUS_USB_ENDPOINT_WRITE_FAILED);
	}
	
	if (nBytesSent != nBufferSize)
	{
		return (XN_STATUS_USB_GOT_UNEXPECTED_BYTES);
	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_EP_HANDLE(pEPHandle);
	XN_VALIDATE_OUTPUT_PTR(pBuffer);
	XN_VALIDATE_OUTPUT_PTR(pnBytesReceived);

	if (pEPHandle->nDirection != XN_USB_DIRECTION_IN)
	{
		return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
	}

	if (nBufferSize == 0)
	{
		return (XN_STATUS_USB_BUFFER_TOO_SMALL);
	}
	
	// receive (according to EP type)
	*pnBytesReceived = 0;

	int nBytesReceived = 0;
	int rc = 0;
	
	if (pEPHandle->nType == XN_USB_EP_BULK)
	{
		rc = libusb_bulk_transfer(pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, &nBytesReceived, nTimeOut);
	}
	else if (pEPHandle->nType == XN_USB_EP_INTERRUPT)
	{
		rc = libusb_interrupt_transfer(pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, &nBytesReceived, nTimeOut);
	}
	else
	{
		return (XN_STATUS_USB_UNSUPPORTED_ENDPOINT_TYPE);
	}
	
	// check result
	if (rc == LIBUSB_ERROR_TIMEOUT)
	{
		return (XN_STATUS_USB_TRANSFER_TIMEOUT);
	}
	else if (rc != 0)
	{
		return (XN_STATUS_USB_ENDPOINT_WRITE_FAILED);
	}
	
	if (nBytesReceived == 0)
	{
		return (XN_STATUS_USB_NOT_ENOUGH_DATA);
	}

	*pnBytesReceived = nBytesReceived;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBQueueReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_C_API XnStatus xnUSBFinishReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut)
{	
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

void xnCleanupThreadData(XnUSBReadThreadData* pThreadData)
{
	for (XnUInt32 i = 0; i < pThreadData->nNumBuffers; ++i)
	{
		if (pThreadData->pBuffersInfo[i].transfer != NULL)
		{
			XN_ALIGNED_FREE_AND_NULL(pThreadData->pBuffersInfo[i].transfer->buffer);
			libusb_free_transfer(pThreadData->pBuffersInfo[i].transfer);
			pThreadData->pBuffersInfo[i].transfer = NULL;
			xnOSCloseEvent(&pThreadData->pBuffersInfo[i].hEvent);
		}
	}
	
	XN_ALIGNED_FREE_AND_NULL(pThreadData->pBuffersInfo);
}

/** Checks if any transfer of the thread is queued. */
XnBool xnIsAnyTransferQueued(XnUSBReadThreadData* pThreadData)
{
	for (XnUInt32 i = 0; i < pThreadData->nNumBuffers; ++i)
	{
		if (pThreadData->pBuffersInfo[i].bIsQueued)
			return (TRUE);
	}
	
	return (FALSE);
}

XN_THREAD_PROC xnUSBReadThreadMain(XN_THREAD_PARAM pThreadParam)
{
	XnUSBReadThreadData* pThreadData = (XnUSBReadThreadData*)pThreadParam;

	// set thread priority to critical
	XnStatus nRetVal = xnOSSetThreadPriority(pThreadData->hReadThread, XN_PRIORITY_CRITICAL);
	if (nRetVal != 0)
	{
		xnLogWarning(XN_MASK_USB, "Failed to set thread priority to critical. This might cause loss of data...");
	}
	
	// first of all, submit all transfers
	for (XnUInt32 i = 0; i < pThreadData->nNumBuffers; ++i)
	{
		XnUSBBuffersInfo* pBufferInfo = &pThreadData->pBuffersInfo[i];
		libusb_transfer* pTransfer = pBufferInfo->transfer;
		
		// submit request
		pBufferInfo->bIsQueued = TRUE;
		int rc = libusb_submit_transfer(pTransfer);
		if (rc != 0)
		{
			xnLogError(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: Failed to submit asynch I/O transfer (err=%d)!", pTransfer->endpoint, pBufferInfo->nBufferID, rc);
		}
	}
	
	// now let libusb process asynchornous I/O
	
	while (TRUE)
	{
		for (XnUInt32 i = 0; i < pThreadData->nNumBuffers; ++i)
		{
			// check if thread can exit (only after kill was requested, and all transfers returned)
			if (pThreadData->bKillReadThread && !xnIsAnyTransferQueued(pThreadData))
			{
				XN_THREAD_PROC_RETURN(XN_STATUS_OK);
			}

			XnUSBBuffersInfo* pBufferInfo = &pThreadData->pBuffersInfo[i];
			libusb_transfer* pTransfer = pBufferInfo->transfer;

			// wait for the next transfer to be completed, and process it
			nRetVal = xnOSWaitEvent(pBufferInfo->hEvent, pThreadData->bKillReadThread ? 0 : pThreadData->nTimeOut);
			if (nRetVal == XN_STATUS_OS_EVENT_TIMEOUT)
			{
//				xnLogWarning(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: timeout. cancelling transfer...", pTransfer->endpoint, pBufferInfo->nBufferID);
				
				// cancel it
				int rc = libusb_cancel_transfer(pBufferInfo->transfer);
				if (rc != 0)
				{
					xnLogError(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: Failed to cancel asynch I/O transfer (err=%d)!", pTransfer->endpoint, pBufferInfo->nBufferID, rc);
				}
			
				// wait for it to cancel	
				nRetVal = xnOSWaitEvent(pBufferInfo->hEvent, XN_WAIT_INFINITE);
			}
			
			if (nRetVal != XN_STATUS_OK)
			{
				// not much we can do
				xnLogWarning(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: Failed waiting on asynch transfer event: %s", pTransfer->endpoint, pBufferInfo->nBufferID, xnGetStatusString(nRetVal));
			}
			
			// check the result of the transfer
			if (pBufferInfo->bIsQueued)
			{
				xnLogWarning(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: Transfer is still queued though event was raised!", pTransfer->endpoint, pBufferInfo->nBufferID);
				// TODO: cancel it?
			}
			else // transfer done
			{
				if (pBufferInfo->nLastStatus == LIBUSB_TRANSFER_COMPLETED || // read succeeded
					pBufferInfo->nLastStatus == LIBUSB_TRANSFER_CANCELLED)   // cancelled, but maybe some data arrived
				{
					if (pTransfer->type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)
					{
						XnUInt32 nTotalBytes = 0;
						
						// some packets may return empty, so we need to remove spaces, and make the buffer sequential
						for (XnUInt32 i = 0; i < pTransfer->num_iso_packets; ++i)
						{
							struct libusb_iso_packet_descriptor* pPacket = &pTransfer->iso_packet_desc[i];
							if (pPacket->status == LIBUSB_TRANSFER_COMPLETED && pPacket->actual_length != 0)
							{
								XnUChar* pBuffer = libusb_get_iso_packet_buffer_simple(pTransfer, i);
								// if buffer is not at same offset, move it
								if (pTransfer->buffer + nTotalBytes != pBuffer)
								{
//									printf("buffer %d has %d bytes. Moving to offset %d...\n", i, pPacket->actual_length, nTotalBytes);
									memmove(pTransfer->buffer + nTotalBytes, pBuffer, pPacket->actual_length);
								}
								nTotalBytes += pPacket->actual_length;
							}
							else if (pPacket->status != LIBUSB_TRANSFER_COMPLETED)
							{
								xnLogWarning(XN_MASK_USB, "Endpoint 0x%x, Buffer %d, packet %d Asynch transfer failed (status: %d)", pTransfer->endpoint, pBufferInfo->nBufferID, i, pPacket->status);
							}
						}
						
						if (nTotalBytes != 0)
						{
							// call callback method
							pBufferInfo->pThreadData->pCallbackFunction(pTransfer->buffer, nTotalBytes, pBufferInfo->pThreadData->pCallbackData);
						}
					}
					else
					{
						// call callback method
						pBufferInfo->pThreadData->pCallbackFunction(pTransfer->buffer, pTransfer->actual_length, pBufferInfo->pThreadData->pCallbackData);
					}
				}
				else if (pBufferInfo->nLastStatus == LIBUSB_TRANSFER_TIMED_OUT)
				{
					// no need to do anything.
				}
				else
				{
					xnLogWarning(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: Asynch transfer failed (status: %d)", pTransfer->endpoint, pBufferInfo->nBufferID, pTransfer->status);
				}

				// as long as running should continue, resubmit transfer
				if (!pBufferInfo->pThreadData->bKillReadThread)
				{
					pBufferInfo->bIsQueued = TRUE;
					int rc = libusb_submit_transfer(pTransfer);
					if (rc != 0)
					{
						xnLogError(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: Failed to re-submit asynch I/O transfer (err=%d)!", pTransfer->endpoint, pBufferInfo->nBufferID, rc);
						if (rc == LIBUSB_ERROR_NO_DEVICE)
						{
							for (XnUSBEventCallbackList::Iterator it = g_connectivityEvent.Begin(); it != g_connectivityEvent.End(); ++it)
							{
								XnUSBEventCallback* pCallback = *it;
								XnUSBEventArgs args;
								args.strDevicePath = NULL;
								args.eventType = XN_USB_EVENT_DEVICE_DISCONNECT;
								pCallback->pFunc(&args, pCallback->pCookie);
							}
						}

					}
				}
			}
		}
	}
	
	XN_THREAD_PROC_RETURN(XN_STATUS_OK);
}

/* This function is called whenever transfer is done (successfully or with an error). */
void xnTransferCallback(libusb_transfer *pTransfer)
{
	XnUSBBuffersInfo* pBufferInfo = (XnUSBBuffersInfo*)pTransfer->user_data;
	
	// mark that buffer is done
	pBufferInfo->bIsQueued = FALSE;
	
	// keep the status (according to libusb documentation, this field is invalid outside the callback method)
	pBufferInfo->nLastStatus = pTransfer->status;
	
	// notify endpoint thread this buffer is done
	XnStatus nRetVal = xnOSSetEvent(pBufferInfo->hEvent);
	if (nRetVal != XN_STATUS_OK)
	{
		// we don't have much to do if set event failed, log a warning
		xnLogWarning(XN_MASK_USB, "Failed to set event for buffer: %s", xnGetStatusString(nRetVal));
	}
}

XN_C_API XnStatus xnUSBInitReadThread(XN_USB_EP_HANDLE pEPHandle, XnUInt32 nBufferSize, XnUInt32 nNumBuffers, XnUInt32 nTimeOut, XnUSBReadCallbackFunctionPtr pCallbackFunction, void* pCallbackData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_EP_HANDLE(pEPHandle);
	XN_VALIDATE_INPUT_PTR(pCallbackFunction);
	
	xnLogVerbose(XN_MASK_USB, "Starting a USB read thread...");

	XnUSBReadThreadData* pThreadData = &pEPHandle->ThreadData;

	if (pThreadData->bIsRunning == TRUE)
	{
		return (XN_STATUS_USB_READTHREAD_ALREADY_INIT);
	}

	memset(pThreadData, 0, sizeof(XnUSBReadThreadData));
	pThreadData->nNumBuffers = nNumBuffers;
	pThreadData->pCallbackFunction = pCallbackFunction;
	pThreadData->pCallbackData = pCallbackData;
	pThreadData->bKillReadThread = FALSE;
	pThreadData->nTimeOut = nTimeOut;

	// allocate buffers
	pThreadData->pBuffersInfo = (XnUSBBuffersInfo*)xnOSCallocAligned(nNumBuffers, sizeof(XnUSBBuffersInfo), XN_DEFAULT_MEM_ALIGN);
	if (pThreadData->pBuffersInfo == NULL)
	{
		xnCleanupThreadData(pThreadData);
		return XN_STATUS_ALLOC_FAILED;
	}
	
	int nNumIsoPackets = 0;
	int nMaxPacketSize = 0;
	
	if (pEPHandle->nType == XN_USB_EP_ISOCHRONOUS)
	{
		// calculate how many packets can be set in this buffer
		nMaxPacketSize = pEPHandle->nMaxPacketSize;
		nNumIsoPackets = nBufferSize / nMaxPacketSize;
	}

	for (XnUInt32 i = 0; i < nNumBuffers; i++)
	{
		XnUSBBuffersInfo* pBufferInfo = &pThreadData->pBuffersInfo[i];
		pBufferInfo->nBufferID = i;
		pBufferInfo->pThreadData = pThreadData;
		
		// allocate transfer
		pBufferInfo->transfer = libusb_alloc_transfer(nNumIsoPackets);
		
		libusb_transfer* pTransfer = pBufferInfo->transfer;

		if (pTransfer == NULL)
		{
			xnCleanupThreadData(pThreadData);
			return (XN_STATUS_ALLOC_FAILED);
		}

		// allocate buffer
		XnUChar* pBuffer = (unsigned char*)xnOSCallocAligned(nBufferSize, sizeof(XnUChar), XN_DEFAULT_MEM_ALIGN);
		if (pBuffer == NULL)
		{
			xnCleanupThreadData(pThreadData);
			return (XN_STATUS_ALLOC_FAILED);
		}

		// fill transfer params
		if (pEPHandle->nType == XN_USB_EP_BULK)
		{
			libusb_fill_bulk_transfer(pTransfer, pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, xnTransferCallback, pBufferInfo, 0);
		}
		else if (pEPHandle->nType == XN_USB_EP_INTERRUPT)
		{
			libusb_fill_interrupt_transfer(pTransfer, pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, xnTransferCallback, pBufferInfo, 0);
		}
		else if (pEPHandle->nType == XN_USB_EP_ISOCHRONOUS)
		{
			libusb_fill_iso_transfer(pTransfer, pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, nNumIsoPackets, xnTransferCallback, pBufferInfo, 0);
			libusb_set_iso_packet_lengths(pTransfer, nMaxPacketSize);
		}
		else
		{
			return (XN_STATUS_USB_UNSUPPORTED_ENDPOINT_TYPE);
		}

		// create event
		nRetVal = xnOSCreateEvent(&pBufferInfo->hEvent,FALSE);
		if (nRetVal != XN_STATUS_OK)
		{
			xnCleanupThreadData(pThreadData);
			return (nRetVal);
		}
	}

	// create a thread to perform the asynchronous read operations
	nRetVal = xnOSCreateThread(xnUSBReadThreadMain, &pEPHandle->ThreadData, &pThreadData->hReadThread);
	if (nRetVal != XN_STATUS_OK)
	{
		xnCleanupThreadData(pThreadData);
		return (nRetVal);
	}

	pThreadData->bIsRunning = TRUE;
	
	xnLogInfo(XN_MASK_USB, "USB read thread was started.");

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBShutdownReadThread(XN_USB_EP_HANDLE pEPHandle)
{
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_EP_HANDLE(pEPHandle);

	XnUSBReadThreadData* pThreadData = &pEPHandle->ThreadData;

	if (pThreadData->bIsRunning == FALSE)
	{
		return (XN_STATUS_USB_READTHREAD_NOT_INIT);
	}

	if (pThreadData->hReadThread != NULL)
	{
		// mark thread should be killed
		pThreadData->bKillReadThread = TRUE;

		// PATCH: we don't cancel the requests, because there is a bug causing segmentation fault.
		// instead, we will just wait for all of them to return.
#if XN_PLATFORM == XN_PLATFORM_ANDROID_ARM
		// cancel all pending requests
		for (XnUInt32 i = 0; i < pThreadData->nNumBuffers; ++i)
		{
			if (pThreadData->pBuffersInfo[i].bIsQueued)
			{
				libusb_cancel_transfer(pThreadData->pBuffersInfo[i].transfer);
			}
		}
#endif
		
		// now wait for thread to exit (we wait the timeout of all buffers + an extra second)
		XnStatus nRetVal = xnOSWaitForThreadExit(pThreadData->hReadThread, pThreadData->nTimeOut * pThreadData->nNumBuffers + 1000);
		if (nRetVal != XN_STATUS_OK)
		{
			xnOSTerminateThread(&pThreadData->hReadThread);
		}
		else
		{
			xnOSCloseThread(&pThreadData->hReadThread);
		}
	}
	
	xnCleanupThreadData(pThreadData);

	pThreadData->bIsRunning = FALSE;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBSetCallbackHandler(XnUInt16 nVendorID, XnUInt16 /*nProductID*/, void* pExtraParam, XnUSBEventCallbackFunctionPtr pCallbackFunction, void* pCallbackData)
{
	return (XN_STATUS_OS_UNSUPPORTED_FUNCTION);
}

XN_C_API XnStatus XN_C_DECL xnUSBRegisterToConnectivityEvents(XnUInt16 nVendorID, XnUInt16 nProductID, XnUSBDeviceCallbackFunctionPtr pFunc, void* pCookie, XnRegistrationHandle* phRegistration)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pFunc);
	XN_VALIDATE_OUTPUT_PTR(phRegistration);

	XnUSBEventCallback* pCallback;
	XN_VALIDATE_NEW(pCallback, XnUSBEventCallback);
	pCallback->pFunc = pFunc;
	pCallback->pCookie = pCookie;

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
