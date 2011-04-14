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
#ifndef _XN_USBLINUX_X86_H_
#define _XN_USBLINUX_X86_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_USB_DEFAULT_EP_TIMEOUT 1000
#define XN_USB_READ_THREAD_KILL_TIMEOUT 10000

//---------------------------------------------------------------------------
// Structures & Enums
//---------------------------------------------------------------------------
typedef struct XnUSBDeviceHandle
{
//	XnBool bValid;
	libusb_device_handle* hDevice;
	XnUSBDeviceSpeed nDevSpeed;
	XnUInt8 nInterface;
	XnUInt8 nAltSetting;
} XnUSBDevHandle;

struct XnUSBReadThreadData; // Forward declaration

typedef struct XnUSBBuffersInfo
{
	/* A pointer back to the thread data. */
	XnUSBReadThreadData* pThreadData;
	/* transfer object (through which asynch operations take place). */
	libusb_transfer* transfer;
	/* TRUE when transfer is queued. */
	XnBool bIsQueued;
	/* An event to notify when buffer is ready. */
	XN_EVENT_HANDLE hEvent;
	XnUInt32 nBufferID;
	/* Holds the last status received. */
	libusb_transfer_status nLastStatus;
} XnUSBBuffersInfo;

/* Information about a thread reading from an endpoint. */
typedef struct XnUSBReadThreadData
{
	/* TRUE when thread is running. */
	XnBool bIsRunning;
	/* Number of buffers allocated. */
	XnUInt32 nNumBuffers;
	/* Array of buffers. */
	XnUSBBuffersInfo* pBuffersInfo;
	/* Size of each buffer. */
	XnUInt32 nBufferSize;
	/* Timeout value. */
	XnUInt32 nTimeOut;
	/* User callback function. */
	XnUSBReadCallbackFunctionPtr pCallbackFunction;
	/* User callback data. */
	void* pCallbackData;
	/* Handle to the read thread. */
	XN_THREAD_HANDLE hReadThread;
	/* When TRUE, signals the thread to exit. */
	XnBool bKillReadThread;
} XnUSBReadThreadData;

typedef struct XnUSBEndPointHandle
{
	libusb_device_handle* hDevice;
	unsigned char nAddress;
	XnUSBEndPointType  nType;
	XnUSBDirectionType nDirection;
	XnUSBReadThreadData ThreadData;
	XnUInt32 nMaxPacketSize;
} XnUSBEPHandle;

#endif //_XN_USBLINUX_X86_H_
