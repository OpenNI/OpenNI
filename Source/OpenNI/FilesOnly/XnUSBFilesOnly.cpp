/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnUSB.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnUSBPlatformSpecificInit()
{
	return (XN_STATUS_OK);
}

XnStatus XnUSBPlatformSpecificShutdown()
{
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBIsDevicePresent(XN_UINT16 nVendorID, XN_UINT16 nProductID, void* pExtraParam, XN_BOOL* pbDevicePresent)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBOpenDevice(XN_UINT16 nVendorID, XN_UINT16 nProductID, void* pExtraParam, XN_USB_DEV_HANDLE* pDevHandlePtr)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBCloseDevice(XN_USB_DEV_HANDLE pDevHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBGetDeviceSpeed(XN_USB_DEV_HANDLE pDevHandle, XnUSBDeviceSpeed* pDevSpeed)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBSetConfig(XN_USB_DEV_HANDLE pDevHandle, XN_UINT8 nConfig)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBGetConfig(XN_USB_DEV_HANDLE pDevHandle, XN_UINT8* pnConfig)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBSetInterface(XN_USB_DEV_HANDLE pDevHandle, XN_UINT8 nInterface, XN_UINT8 nAltInterface)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBGetInterface(XN_USB_DEV_HANDLE pDevHandle, XN_UINT8* pnInterface, XN_UINT8* pnAltInterface)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBOpenEndPoint(XN_USB_DEV_HANDLE pDevHandle, XN_UINT16 nEndPointID, XnUSBEndPointType nEPType, XnUSBDirectionType nDirType, XN_USB_EP_HANDLE* pEPHandlePtr)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBCloseEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBAbortEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBFlushEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBResetEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBSendControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, XN_UINT8 nRequest, XN_UINT16 nValue, XN_UINT16 nIndex, XN_UCHAR* pBuffer, XN_UINT32 nBufferSize, XN_UINT32 nTimeOut)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBReceiveControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, XN_UINT8 nRequest, XN_UINT16 nValue, XN_UINT16 nIndex, XN_UCHAR* pBuffer, XN_UINT32 nBufferSize, XN_UINT32* pnBytesReceived, XN_UINT32 nTimeOut)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XN_UCHAR* pBuffer, XN_UINT32 nBufferSize, XN_UINT32* pnBytesReceived, XN_UINT32 nTimeOut)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBWriteEndPoint(XN_USB_EP_HANDLE pEPHandle, XN_UCHAR* pBuffer, XN_UINT32 nBufferSize, XN_UINT32 nTimeOut)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBQueueReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XN_UCHAR* pBuffer, XN_UINT32 nBufferSize, XN_UINT32 nTimeOut)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBFinishReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XN_UINT32* pnBytesReceived, XN_UINT32 nTimeOut)
{	
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBInitReadThread(XN_USB_EP_HANDLE pEPHandle, XN_UINT32 nBufferSize, XN_UINT32 nNumBuffers, XN_UINT32 nTimeOut, XnUSBReadCallbackFunctionPtr pCallbackFunction, void* pCallbackData)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnUSBShutdownReadThread(XN_USB_EP_HANDLE pEPHandle)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}
