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
#ifndef _XN_USB_H_
#define _XN_USB_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnOS.h"

//---------------------------------------------------------------------------
// Structures & Enums
//---------------------------------------------------------------------------
typedef enum {
	XN_USB_DEVICE_LOW_SPEED = 0,
	XN_USB_DEVICE_FULL_SPEED,
	XN_USB_DEVICE_HIGH_SPEED,
} XnUSBDeviceSpeed;

typedef enum {
	XN_USB_EP_BULK = 0,
	XN_USB_EP_ISOCHRONOUS,
	XN_USB_EP_INTERRUPT,
} XnUSBEndPointType;

typedef enum {
	XN_USB_DIRECTION_IN = 0,
	XN_USB_DIRECTION_OUT,
} XnUSBDirectionType;

typedef enum {
	XN_USB_CONTROL_TYPE_STANDARD = 0,
	XN_USB_CONTROL_TYPE_CLASS,
	XN_USB_CONTROL_TYPE_VENDOR,
} XnUSBControlType;

typedef enum {
	XN_USB_EVENT_DEVICE_CONNECT = 0,
	XN_USB_EVENT_DEVICE_DISCONNECT,
} XnUSBEventType;

struct XnUSBDeviceHandle;
struct XnUSBEndPointHandle;

typedef XnChar XnUSBConnectionString[XN_FILE_MAX_PATH];

typedef XnUSBDeviceHandle*  XN_USB_DEV_HANDLE;
typedef XnUSBEndPointHandle* XN_USB_EP_HANDLE;

typedef XnBool (XN_CALLBACK_TYPE* XnUSBReadCallbackFunctionPtr)(XnUChar* pBuffer, XnUInt32 nBufferSize, void* pCallbackData);
typedef XnBool (XN_CALLBACK_TYPE* XnUSBEventCallbackFunctionPtr)(XnUSBEventType USBEventType, XnChar* cpDevPath, void* pCallbackData);

typedef struct XnUSBEventArgs
{
	const XnChar* strDevicePath;
	XnUSBEventType eventType;
} XnUSBEventArgs;

typedef void (XN_CALLBACK_TYPE* XnUSBDeviceCallbackFunctionPtr)(XnUSBEventArgs* pArgs, void* pCookie);

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------
XN_C_API XnStatus XN_C_DECL xnUSBInit();
XN_C_API XnStatus XN_C_DECL xnUSBShutdown();

XN_C_API XnStatus XN_API_DEPRECATED("Use xnUSBEnumerateDevices() instead") XN_C_DECL xnUSBIsDevicePresent(XnUInt16 nVendorID, XnUInt16 nProductID, void* pExtraParam, XnBool* pbDevicePresent);

XN_C_API XnStatus XN_C_DECL xnUSBEnumerateDevices(XnUInt16 nVendorID, XnUInt16 nProductID, const XnUSBConnectionString** pastrDevicePaths, XnUInt32* pnCount);
XN_C_API void XN_C_DECL xnUSBFreeDevicesList(const XnUSBConnectionString* astrDevicePaths);

XN_C_API XnStatus XN_API_DEPRECATED("Use xnUSBOpenDeviceByPath() instead") XN_C_DECL xnUSBOpenDevice(XnUInt16 nVendorID, XnUInt16 nProductID, void* pExtraParam, void* pExtraParam2, XN_USB_DEV_HANDLE* pDevHandlePtr);
XN_C_API XnStatus XN_C_DECL xnUSBOpenDeviceByPath(const XnUSBConnectionString strDevicePath, XN_USB_DEV_HANDLE* pDevHandlePtr);
XN_C_API XnStatus XN_C_DECL xnUSBCloseDevice(XN_USB_DEV_HANDLE pDevHandle);

XN_C_API XnStatus XN_C_DECL xnUSBGetDeviceSpeed(XN_USB_DEV_HANDLE pDevHandle, XnUSBDeviceSpeed* pDevSpeed);

XN_C_API XnStatus XN_C_DECL xnUSBSetConfig(XN_USB_DEV_HANDLE pDevHandle, XnUInt8 nConfig);
XN_C_API XnStatus XN_C_DECL xnUSBGetConfig(XN_USB_DEV_HANDLE pDevHandle, XnUInt8* pnConfig);

XN_C_API XnStatus XN_C_DECL xnUSBSetInterface(XN_USB_DEV_HANDLE pDevHandle, XnUInt8 nInterface, XnUInt8 nAltInterface);
XN_C_API XnStatus XN_C_DECL xnUSBGetInterface(XN_USB_DEV_HANDLE pDevHandle, XnUInt8* pnInterface, XnUInt8* pnAltInterface);

XN_C_API XnStatus XN_C_DECL xnUSBOpenEndPoint(XN_USB_DEV_HANDLE pDevHandle, XnUInt16 nEndPointID, XnUSBEndPointType nEPType, XnUSBDirectionType nDirType, XN_USB_EP_HANDLE* pEPHandlePtr);
XN_C_API XnStatus XN_C_DECL xnUSBCloseEndPoint(XN_USB_EP_HANDLE pEPHandle);
XN_C_API XnStatus XN_C_DECL xnUSBGetEndPointMaxPacketSize(XN_USB_EP_HANDLE pEPHandle, XnUInt32* pnMaxPacketSize);

XN_C_API XnStatus XN_C_DECL xnUSBAbortEndPoint(XN_USB_EP_HANDLE pEPHandle);
XN_C_API XnStatus XN_C_DECL xnUSBFlushEndPoint(XN_USB_EP_HANDLE pEPHandle);
XN_C_API XnStatus XN_C_DECL xnUSBResetEndPoint(XN_USB_EP_HANDLE pEPHandle);

XN_C_API XnStatus XN_C_DECL xnUSBSendControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, XnUInt8 nRequest, XnUInt16 nValue, XnUInt16 nIndex, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut);
XN_C_API XnStatus XN_C_DECL xnUSBReceiveControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, XnUInt8 nRequest, XnUInt16 nValue, XnUInt16 nIndex, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut);

XN_C_API XnStatus XN_C_DECL xnUSBReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut);
XN_C_API XnStatus XN_C_DECL xnUSBWriteEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut);

XN_C_API XnStatus XN_C_DECL xnUSBQueueReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut);
XN_C_API XnStatus XN_C_DECL xnUSBFinishReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut);

XN_C_API XnStatus XN_C_DECL xnUSBInitReadThread(XN_USB_EP_HANDLE pEPHandle, XnUInt32 nBufferSize, XnUInt32 nNumBuffers, XnUInt32 nTimeOut, XnUSBReadCallbackFunctionPtr pCallbackFunction, void* pCallbackData);
XN_C_API XnStatus XN_C_DECL xnUSBShutdownReadThread(XN_USB_EP_HANDLE pEPHandle);

XN_C_API XnStatus XN_API_DEPRECATED("Use xnUSBRegisterToConnectivityEvents() instead") XN_C_DECL xnUSBSetCallbackHandler(XnUInt16 nVendorID, XnUInt16 nProductID, void* pExtraParam, XnUSBEventCallbackFunctionPtr pCallbackFunction, void* pCallbackData);

XN_C_API XnStatus XN_C_DECL xnUSBRegisterToConnectivityEvents(XnUInt16 nVendorID, XnUInt16 nProductID, XnUSBDeviceCallbackFunctionPtr pFunc, void* pCookie, XnRegistrationHandle* phRegistration);
XN_C_API void XN_C_DECL xnUSBUnregisterFromConnectivityEvents(XnRegistrationHandle hRegistration);

#endif //_XN_USB_H_
