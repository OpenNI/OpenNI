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

#define PSDRV_PIPE_PREFIX "\\PIPE"

#define XN_MASK_USB "XnUSB"

//---------------------------------------------------------------------------
// Structures & Enums
//---------------------------------------------------------------------------
typedef struct XnUSBDeviceHandle
{
	XN_BOOL bValid;

	XN_CHAR cpDeviceName[MAX_DEVICE_STR_LENGTH]; 
	HANDLE hUSBDevHandle;

	XN_UINT8 nAltInterface;

	XnUSBDeviceSpeed nDevSpeed;
} XnUSBDevHandle;

typedef struct XnUSBBuffersInfo
{
	XN_UCHAR* pBuffer;
} XnUSBBuffersInfo;

typedef struct XnUSBReadThreadData
{
	XN_BOOL bInUse;

	XN_USB_EP_HANDLE pEPHandle;

	XN_UINT32 nBufferSize;
	XN_UINT32 nNumBuffers;

	XN_UINT32 nTimeOut;

	XnUSBReadCallbackFunctionPtr pCallbackFunction;
	PVOID pCallbackData;

	XN_THREAD_HANDLE  hReadThread;
	XN_BOOL			  bKillReadThread;

	XnUSBBuffersInfo* pBuffersInfo;
	OVERLAPPED* pOvlpIO;
} XnUSBReadThreadData;

typedef struct XnUSBEndPointHandle
{
	XN_BOOL bValid;

	HANDLE hEPHandle;
	HANDLE hEPHandleOvlp;

	XN_UINT32 nTimeOut;

	OVERLAPPED ovlpIO;
	XnUSBReadThreadData ThreadData;

	XN_UINT16 nEndPointID;
	XnUSBEndPointType  nEPType;
	XnUSBDirectionType nEPDir;
} XnUSBEPHandle;

#endif //_XN_USBWIN32_H_