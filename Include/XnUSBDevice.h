/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
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
#ifndef _XN_USB_DEVICE_H_
#define _XN_USB_DEVICE_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnPlatform.h"
#include "XnStatus.h"

#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	#include <usb100.h>
	typedef struct USB_ENDPOINT_DESCRIPTOR XnUSBEndpointDescriptor;
	typedef struct USB_INTERFACE_DESCRIPTOR XnUSBInterfaceDescriptor;
	typedef struct USB_CONFIGURATION_DESCRIPTOR XnUSBConfigDescriptor;
	typedef struct USB_DEVICE_DESCRIPTOR XnUSBDeviceDescriptor;
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

//---------------------------------------------------------------------------
// API
//---------------------------------------------------------------------------
XN_C_API XnStatus XN_C_DECL xnUSBDeviceInit(const XnUSBDeviceDescriptorHolder* pDeviceDescriptor, XnUInt32 nControlMessageMaxSize, XnUSBDevice** ppDevice);
XN_C_API void XN_C_DECL xnUSBDeviceShutdown(XnUSBDevice* pDevice);
XN_C_API XnBool XN_C_DECL xnUSBDeviceIsControlRequestPending(XnUSBDevice* pDevice);
XN_C_API XnStatus XN_C_DECL xnUSBDeviceReceiveControlRequest(XnUSBDevice* pDevice, XnUChar* pBuffer, XnUInt32* pnRequestSize);
XN_C_API XnStatus XN_C_DECL xnUSBDeviceSendControlReply(XnUSBDevice* pDevice, const XnUChar* pBuffer, XnUInt32 nReplySize);
XN_C_API XnStatus XN_C_DECL xnUSBDeviceSetNewControlRequestCallback(XnUSBDevice* pDevice, XnUSBDeviceNewControlRequestCallback pFunc, void* pCookie);
XN_C_API XnStatus XN_C_DECL xnUSBDeviceWriteEndpoint(XnUSBDevice* pDevice, XnUInt8 nAddress, XnUChar* pData, XnUInt32 nDataSize);

#endif