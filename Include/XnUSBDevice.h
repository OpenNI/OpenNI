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
#ifndef _XN_USB_DEVICE_H_
#define _XN_USB_DEVICE_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnPlatform.h"
#include "XnStatus.h"

#if (XN_PLATFORM == XN_PLATFORM_WIN32)

	#include <Win32/usb100.h>
	typedef USB_ENDPOINT_DESCRIPTOR XnUSBEndpointDescriptor;
	typedef USB_INTERFACE_DESCRIPTOR XnUSBInterfaceDescriptor;
	typedef USB_CONFIGURATION_DESCRIPTOR XnUSBConfigDescriptor;
	typedef USB_DEVICE_DESCRIPTOR XnUSBDeviceDescriptor;

	#define USB_DT_CONFIG_SIZE 0
	#define USB_DT_CONFIG 0
	#define USB_CONFIG_ATT_ONE 0
	#define USB_DT_ENDPOINT_SIZE 0
	#define USB_DT_ENDPOINT 0
	#define USB_ENDPOINT_XFER_BULK 0
	#define USB_DT_INTERFACE_SIZE 0
	#define USB_DT_INTERFACE 0
	#define USB_CLASS_VENDOR_SPEC 0
	#define USB_DT_DEVICE_SIZE 0
	#define USB_DT_DEVICE 0

#elif (XN_PLATFORM == XN_PLATFORM_LINUX_X86 || XN_PLATFORM == XN_PLATFORM_LINUX_ARM)
	#include <linux/usb/ch9.h>
	typedef struct usb_endpoint_descriptor XnUSBEndpointDescriptor;
	typedef struct usb_interface_descriptor XnUSBInterfaceDescriptor;
	typedef struct usb_config_descriptor XnUSBConfigDescriptor;
	typedef struct usb_device_descriptor XnUSBDeviceDescriptor;
#else
	#error "Unsupported Platform!"
#endif

//---------------------------------------------------------------------------
// Structures & Enums
//---------------------------------------------------------------------------
typedef enum XnUSBDeviceConnectionState
{
	XN_USB_DEVICE_DISCONNECTED,
	XN_USB_DEVICE_CONNECTED,
	XN_USB_DEVICE_SUSPENDED,
} XnUSBDeviceConnectionState;

typedef struct XnUSBStringDescriptor
{
	XnUInt8 nID;
	const XnChar* strString;
} XnUSBStringDescriptor;

typedef struct XnUSBInterfaceDescriptorHolder
{
	XnUSBInterfaceDescriptor descriptor;
	XnUSBEndpointDescriptor** aEndpoints;	
} XnUSBInterfaceDescriptorHolder;

typedef struct XnUSBConfigDescriptorHolder
{
	XnUSBConfigDescriptor descriptor;
	XnUSBInterfaceDescriptorHolder** aInterfaces;
} XnUSBConfigDescriptorHolder;

typedef struct XnUSBDeviceDescriptorHolder
{
	XnUSBDeviceDescriptor descriptor;
	XnUSBConfigDescriptorHolder** aConfigurations;
	XnUSBStringDescriptor* aStrings;
	XnUInt8 nStrings;
} XnUSBDeviceDescriptorHolder;

struct XnUSBDevice;
typedef struct XnUSBDevice XnUSBDevice;

typedef void (*XnUSBDeviceNewControlRequestCallback)(XnUSBDevice* pDevice, void* pCookie);
typedef void (*XnUSBDeviceConnectivityChangedCallback)(XnUSBDevice* pDevice, XnUSBDeviceConnectionState state, void* pCookie);

//---------------------------------------------------------------------------
// API
//---------------------------------------------------------------------------
XN_C_API XnStatus XN_C_DECL xnUSBDeviceInit(const XnUSBDeviceDescriptorHolder* pDeviceDescriptor, XnUInt32 nControlMessageMaxSize, XnUSBDevice** ppDevice);
XN_C_API void XN_C_DECL xnUSBDeviceShutdown(XnUSBDevice* pDevice);
XN_C_API XnBool XN_C_DECL xnUSBDeviceIsControlRequestPending(XnUSBDevice* pDevice);

//pnRequestSize is max size on input, actual size on output
XN_C_API XnStatus XN_C_DECL xnUSBDeviceReceiveControlRequest(XnUSBDevice* pDevice, XnUChar* pBuffer, XnUInt32* pnRequestSize);
XN_C_API XnStatus XN_C_DECL xnUSBDeviceSendControlReply(XnUSBDevice* pDevice, const XnUChar* pBuffer, XnUInt32 nReplySize);
XN_C_API XnStatus XN_C_DECL xnUSBDeviceSetNewControlRequestCallback(XnUSBDevice* pDevice, XnUSBDeviceNewControlRequestCallback pFunc, void* pCookie);
XN_C_API XnStatus XN_C_DECL xnUSBDeviceSetConnectivityChangedCallback(XnUSBDevice* pDevice, XnUSBDeviceConnectivityChangedCallback pFunc, void* pCookie);
XN_C_API XnStatus XN_C_DECL xnUSBDeviceWriteEndpoint(XnUSBDevice* pDevice, XnUInt8 nAddress, const XnUChar* pData, XnUInt32 nDataSize);
XN_C_API XnStatus XN_C_DECL xnUSBDeviceResetEndpoint(XnUSBDevice* pDevice, XnUInt8 nAddress);

#endif
