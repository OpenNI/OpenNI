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
#ifndef _XN_USBWIN32_H_
#define _XN_USBWIN32_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include <PSDrvPublic.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_USB_DEFAULT_EP_TIMEOUT 1000
#define XN_USB_READ_THREAD_KILL_TIMEOUT 10000

#define MAX_DEVICE_STR_LENGTH 256
#define MAX_CONFIG_DESC_SIZE 2048

#define MAX_POTENTIAL_DEVICES 30

#define PSDRV_PIPE_PREFIX "\\PIPE"

#define XN_MASK_USB "xnUSB"

//---------------------------------------------------------------------------
// Structures & Enums
//---------------------------------------------------------------------------
typedef struct XnUSBDeviceHandle
{
	XnBool bValid;

	XnChar cpDeviceName[MAX_DEVICE_STR_LENGTH]; 
	HANDLE hUSBDevHandle;

	XnUInt8 nAltInterface;

	XnUSBDeviceSpeed nDevSpeed;
} xnUSBDevHandle;

typedef struct xnUSBBuffersInfo
{
	XnUChar* pBuffer;
} xnUSBBuffersInfo;

typedef struct xnUSBReadThreadData
{
	XnBool bInUse;

	XN_USB_EP_HANDLE pEPHandle;

	XnUInt32 nBufferSize;
	XnUInt32 nNumBuffers;

	XnUInt32 nTimeOut;

	XnUSBReadCallbackFunctionPtr pCallbackFunction;
	PVOID pCallbackData;

	XN_THREAD_HANDLE  hReadThread;
	XnBool			  bKillReadThread;

	xnUSBBuffersInfo* pBuffersInfo;
	OVERLAPPED* pOvlpIO;
} xnUSBReadThreadData;

typedef struct XnUSBEndPointHandle
{
	XnChar cpPipeName[MAX_DEVICE_STR_LENGTH];

	XnBool bValid;

	HANDLE hEPHandle;
	HANDLE hEPHandleOvlp;

	XnUInt32 nTimeOut;

	OVERLAPPED ovlpIO;
	xnUSBReadThreadData ThreadData;

	XnUInt16 nEndPointID;
	XnUSBEndPointType  nEPType;
	XnUSBDirectionType nEPDir;
	XnUInt32 nMaxPacketSize;
} xnUSBEPHandle;

#endif //_XN_USBWIN32_H_
