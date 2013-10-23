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
#pragma warning(disable:4200)  
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int

#include <initguid.h>
#include <ntddk.h>
#include <ntintsafe.h>
#include "usbdi.h"
#include "usbdlib.h"

#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)

#include <wdf.h>
#include <wdfusb.h>
#include "PSDrvPublic.h"

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#if DBG
	#define PSDrv_DbgPrint(level, _x_)				\
			if ((level) <= g_DebugLevel)			\
			{										\
				DbgPrint("PSDrv3: ");				\
				DbgPrint _x_;						\
			}
#else
	#define PSDrv_DbgPrint(level, _x_)
#endif

#undef ExAllocatePool
#define ExAllocatePool(type, size)	\
		ExAllocatePoolWithTag(type, size, POOL_TAG);

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#ifndef _H_PSDRV_PRIVATE_H_
#define _H_PSDRV_PRIVATE_H_

#define GLOBAL_DEBUG_LEVEL 3

#define POOL_TAG (ULONG) '03SP'

#define MAX_TRANSFER_SIZE 65535
#define DEFAULT_REGISTRY_TRANSFER_SIZE 65536

#define REMOTE_WAKEUP_MASK 0x20

#define GetListHeadEntry(ListHead)  ((ListHead)->Flink)

#define IDLE_CAPS_TYPE IdleUsbSelectiveSuspend

//---------------------------------------------------------------------------
// Structs & Enums
//---------------------------------------------------------------------------

// A structure representing the instance information associated with this particular device.
typedef struct _DEVICE_CONTEXT
{
    USB_DEVICE_DESCRIPTOR           UsbDeviceDescriptor;
    PUSB_CONFIGURATION_DESCRIPTOR   UsbConfigurationDescriptor;
    WDFUSBDEVICE                    WdfUsbTargetDevice;
    ULONG                           WaitWakeEnable;
    BOOLEAN                         IsDeviceHighSpeed;
    WDFUSBINTERFACE                 UsbInterface;
    UCHAR                           NumberConfiguredPipes;
    ULONG                           MaximumTransferSize;
	UCHAR							nCurrIf;
	UCHAR							nCurrAltIf;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext)

// This context is associated with every open handle.
typedef struct _FILE_CONTEXT
{
    WDFUSBPIPE Pipe;
	ULONG	nTimeOut;
} FILE_CONTEXT, *PFILE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FILE_CONTEXT, GetFileContext)

// This context is associated with every request received by the driver from the app.
typedef struct _REQUEST_CONTEXT
{
    WDFMEMORY         UrbMemory;
    PMDL              Mdl;
    ULONG             Length;					// remaining to xfer
    ULONG             Numxfer;					// cumulate xfer
    ULONG_PTR         VirtualAddress;			// va for next segment of xfer.
    BOOLEAN           Read;						// TRUE if Read
	ULONG			  TotalLength;				// The original total transfer length
} REQUEST_CONTEXT, * PREQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(REQUEST_CONTEXT, GetRequestContext)

// This context is associated with every work item
typedef struct _WORKITEM_CONTEXT
{
    WDFDEVICE       Device;
    WDFUSBPIPE      Pipe;
} WORKITEM_CONTEXT, *PWORKITEM_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WORKITEM_CONTEXT, GetWorkItemContext)

// Required for doing ISOCH transfers. This context is associated with every subrequest created by the driver to do ISOCH transfer.
typedef struct _SUB_REQUEST_CONTEXT
{
    WDFREQUEST  UserRequest;
    PURB        SubUrb;
    PMDL        SubMdl;
    LIST_ENTRY  ListEntry; // used in CancelRoutine
} SUB_REQUEST_CONTEXT, *PSUB_REQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(SUB_REQUEST_CONTEXT, GetSubRequestContext)

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
extern ULONG g_DebugLevel;

//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD PSDrv_EvtDeviceAdd;
EVT_WDF_DEVICE_PREPARE_HARDWARE PSDrv_EvtDevicePrepareHardware;
EVT_WDF_DEVICE_FILE_CREATE PSDrv_EvtDeviceFileCreate;

EVT_WDF_IO_QUEUE_IO_READ PSDrv_EvtIoRead;
EVT_WDF_IO_QUEUE_IO_WRITE PSDrv_EvtIoWrite;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL PSDrv_EvtIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_STOP PSDrvEvtIoStop;
EVT_WDF_IO_QUEUE_IO_RESUME PSDrvEvtIoResume;

EVT_WDF_REQUEST_COMPLETION_ROUTINE SubRequestCompletionRoutine;
EVT_WDF_REQUEST_COMPLETION_ROUTINE ReadWriteCompletion;

// Device
NTSTATUS ReadAndSelectDescriptors(IN WDFDEVICE Device);
NTSTATUS ConfigureDevice(IN WDFDEVICE Device);
NTSTATUS SelectInterfaces(IN WDFDEVICE Device);
NTSTATUS SetPowerPolicy(__in WDFDEVICE Device);
NTSTATUS AbortPipes(IN WDFDEVICE Device);
NTSTATUS ReadFdoRegistryKeyValue(__in WDFDRIVER Driver, __in LPWSTR Name, __out PULONG Value);

// Queue
WDFUSBPIPE GetPipeFromName(IN PDEVICE_CONTEXT DeviceContext, IN PUNICODE_STRING FileName);
NTSTATUS ResetPipe(IN WDFUSBPIPE Pipe);
NTSTATUS AbortPipe(IN WDFUSBPIPE Pipe);
NTSTATUS ResetDevice(IN WDFDEVICE Device);
NTSTATUS SetPipeProperty(IN OUT PFILE_CONTEXT pFileContext, IN PSUSBDRV_PIPE_PROPERTY* pPipeProp);
NTSTATUS ControlTransfer(IN PDEVICE_CONTEXT pDeviceContext, IN PSUSBDRV_CONTROL_TRANSFER* pControlTransfer, IN OUT PUCHAR pControlBuffer, IN size_t nControlBufferSize, OUT ULONG* pLength);

// Bulk
VOID ReadWriteIsochEndPoints(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN ULONG Length, IN WDF_REQUEST_TYPE RequestType);
VOID PerformHighSpeedIsochTransfer(IN WDFDEVICE Device, IN WDFQUEUE Queue, IN WDFREQUEST Request, IN ULONG TotalLength, IN WDF_REQUEST_TYPE RequestType);

// Isoch
VOID ReadWriteBulkEndPoints(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN ULONG Length, IN WDF_REQUEST_TYPE RequestType);

#endif //_H_PSDRV_PRIVATE_H_
