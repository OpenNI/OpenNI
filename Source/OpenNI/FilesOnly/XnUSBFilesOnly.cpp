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

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus xnUSBPlatformSpecificInit()
{
	return (XN_STATUS_OK);
}

XnStatus xnUSBPlatformSpecificShutdown()
{
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBIsDevicePresent(XnUInt16 nVendorID, XnUInt16 nProductID, void* pExtraParam, XnBool* pbDevicePresent)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBOpenDevice(XnUInt16 nVendorID, XnUInt16 nProductID, void* pExtraParam, XN_USB_DEV_HANDLE* pDevHandlePtr)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBCloseDevice(XN_USB_DEV_HANDLE pDevHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBGetDeviceSpeed(XN_USB_DEV_HANDLE pDevHandle, XnUSBDeviceSpeed* pDevSpeed)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBSetConfig(XN_USB_DEV_HANDLE pDevHandle, XnUInt8 nConfig)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBGetConfig(XN_USB_DEV_HANDLE pDevHandle, XnUInt8* pnConfig)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBSetInterface(XN_USB_DEV_HANDLE pDevHandle, XnUInt8 nInterface, XnUInt8 nAltInterface)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBGetInterface(XN_USB_DEV_HANDLE pDevHandle, XnUInt8* pnInterface, XnUInt8* pnAltInterface)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBOpenEndPoint(XN_USB_DEV_HANDLE pDevHandle, XnUInt16 nEndPointID, XnUSBEndPointType nEPType, XnUSBDirectionType nDirType, XN_USB_EP_HANDLE* pEPHandlePtr)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBCloseEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBAbortEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBFlushEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBResetEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBSendControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, XnUInt8 nRequest, XnUInt16 nValue, XnUInt16 nIndex, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBReceiveControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, XnUInt8 nRequest, XnUInt16 nValue, XnUInt16 nIndex, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBWriteEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBQueueReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, XnUInt32 nBufferSize, XnUInt32 nTimeOut)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBFinishReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUInt32* pnBytesReceived, XnUInt32 nTimeOut)
{	
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBInitReadThread(XN_USB_EP_HANDLE pEPHandle, XnUInt32 nBufferSize, XnUInt32 nNumBuffers, XnUInt32 nTimeOut, XnUSBReadCallbackFunctionPtr pCallbackFunction, void* pCallbackData)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XnUSBShutdownReadThread(XN_USB_EP_HANDLE pEPHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}
