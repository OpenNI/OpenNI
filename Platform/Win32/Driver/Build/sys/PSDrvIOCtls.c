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
#include "PSDrvPrivate.h"
#include "PSDrvVersion.h"

//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PSDrv_EvtDeviceFileCreate)
#pragma alloc_text(PAGE, PSDrv_EvtIoDeviceControl)
#pragma alloc_text(PAGE, PSDrv_EvtIoRead)
#pragma alloc_text(PAGE, PSDrv_EvtIoWrite)
#pragma alloc_text(PAGE, GetPipeFromName)
#pragma alloc_text(PAGE, ResetPipe)
#pragma alloc_text(PAGE, AbortPipe)
#pragma alloc_text(PAGE, ResetDevice)
#pragma alloc_text(PAGE, SetPipeProperty)
#pragma alloc_text(PAGE, ControlTransfer)
#endif

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
/*++
Routine Description:

The framework calls a driver's EvtDeviceFileCreate callback
when the framework receives an IRP_MJ_CREATE request.
The system sends this request when a user application opens the
device to perform an I/O operation, such as reading or writing a file.
This callback is called synchronously, in the context of the thread
that created the IRP_MJ_CREATE request.

Arguments:

Device - Handle to a framework device object.
FileObject - Pointer to fileobject that represents the open handle.
CreateParams - copy of the create IO_STACK_LOCATION

Return Value:

NT status code
--*/
VOID PSDrv_EvtDeviceFileCreate(IN WDFDEVICE Device, IN WDFREQUEST Request, IN WDFFILEOBJECT FileObject)
{
    NTSTATUS                    status = STATUS_UNSUCCESSFUL;
    PUNICODE_STRING             fileName;
    PFILE_CONTEXT               pFileContext;
    PDEVICE_CONTEXT             pDevContext;
    WDFUSBPIPE                  pipe;

    PSDrv_DbgPrint(3, ("PSDrv_EvtDeviceFileCreate - begins\n"));

    PAGED_CODE();

    // initialize variables
    pDevContext = GetDeviceContext(Device);
    pFileContext = GetFileContext(FileObject);
    fileName = WdfFileObjectGetFileName(FileObject);

    if (0 == fileName->Length)
	{
        // opening a device as opposed to pipe.
        status = STATUS_SUCCESS;
    }
    else {
        pipe = GetPipeFromName(pDevContext, fileName);

		// Does the pipe exists?
        if (pipe != NULL)
		{
            pFileContext->Pipe = pipe;

            WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(pipe);

			// Set the default timeout
			pFileContext->nTimeOut = PSUSBDRV_DEFAULT_TIMEOUT;

            status = STATUS_SUCCESS;
        }
		else
		{
            status = STATUS_INVALID_DEVICE_REQUEST;
        }
    }

    WdfRequestComplete(Request, status);

    PSDrv_DbgPrint(3, ("PSDrv_EvtDeviceFileCreate - ends\n"));

    return;
}

/*++
Routine Description:

This event is called when the framework receives IRP_MJ_DEVICE_CONTROL
requests from the system.

Arguments:

Queue - Handle to the framework queue object that is associated
with the I/O request.
Request - Handle to a framework request object.

OutputBufferLength - length of the request's output buffer,
if an output buffer is available.
InputBufferLength - length of the request's input buffer,
if an input buffer is available.

IoControlCode - the driver-defined or system-defined I/O control code
(IOCTL) that is associated with the request.
Return Value:

VOID
--*/
VOID PSDrv_EvtIoDeviceControl(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t OutputBufferLength, IN size_t InputBufferLength, IN ULONG IoControlCode)
{
    WDFDEVICE					 device;
    PVOID						 ioBuffer;
    size_t						 bufLength;
    NTSTATUS					 status;
    PDEVICE_CONTEXT				 pDevContext;
    PFILE_CONTEXT				 pFileContext;
    ULONG						 length = 0;
	PSUSBDRV_PIPE_PROPERTY*		 pPipeProp;
	PSUSBDRV_CONTROL_TRANSFER*	 pControlTransfer;
	PSUSBDRV_DRIVER_VERSION*	 pDriverVersion;
	PSUSBDRV_INTERFACE_PROPERTY* pInterfaceProperty;
	unsigned int*				 pnDeviceSpeed;
	WDFMEMORY					 WdfMem = NULL; 
	PUCHAR                       pControlBuffer;
	WDFMEMORY					 WdfMemOut = NULL;

	
	WDF_USB_INTERFACE_SELECT_SETTING_PARAMS  selectSettingParams;

    UNREFERENCED_PARAMETER(InputBufferLength);

    PSDrv_DbgPrint(3, ("PSDrv_EvtIoDeviceControl - begins\n"));

    PAGED_CODE();

    // initialize variables
    device = WdfIoQueueGetDevice(Queue);
    pDevContext = GetDeviceContext(device);

    switch(IoControlCode)
	{
		case IOCTL_PSDRV_RESET_PIPE:
			PSDrv_DbgPrint(3, ("IOControl: ResetPipe\n"));

			pFileContext = GetFileContext(WdfRequestGetFileObject(Request));

			if (pFileContext->Pipe == NULL)
			{
				PSDrv_DbgPrint(3, ("Invalid pipe!\n"));
				status = STATUS_INVALID_PARAMETER;
			}
			else
			{
				status = ResetPipe(pFileContext->Pipe);
			}

			break;

		case IOCTL_PSDRV_ABORT_PIPE:
			PSDrv_DbgPrint(3, ("IOControl: AbortPipe\n"));

			pFileContext = GetFileContext(WdfRequestGetFileObject(Request));

			if (pFileContext->Pipe == NULL)
			{
				PSDrv_DbgPrint(3, ("Invalid pipe!\n"));
				status = STATUS_INVALID_PARAMETER;
			}
			else
			{
				status = AbortPipe(pFileContext->Pipe);
			}

			break;

		case IOCTL_PSDRV_GET_CONFIG_DESCRIPTOR:
			PSDrv_DbgPrint(3, ("IOControl: GetConfigDescriptor\n"));

			if (pDevContext->UsbConfigurationDescriptor)
			{
				length = pDevContext->UsbConfigurationDescriptor->wTotalLength;

				status = WdfRequestRetrieveOutputBuffer(Request, length, &ioBuffer, &bufLength);
				if(!NT_SUCCESS(status))
				{
					PSDrv_DbgPrint(1, ("WdfRequestRetrieveOutputBuffer failed! (Status = %x)\n", status));
					status = STATUS_INVALID_PARAMETER;
					break;
				}

				RtlCopyMemory(ioBuffer, pDevContext->UsbConfigurationDescriptor, length);

				status = STATUS_SUCCESS;
			}
			else
			{
				PSDrv_DbgPrint(3, ("UsbConfigurationDescriptor is NULL!\n"));
				status = STATUS_INVALID_DEVICE_STATE;
			}

			break;

		case IOCTL_PSDRV_RESET_DEVICE:
			PSDrv_DbgPrint(3, ("IOControl: ResetDevice\n"));

			status = ResetDevice(device);

			break;

		case IOCTL_PSDRV_CONTROL_TRANSFER:
			PSDrv_DbgPrint(3, ("IOControl: ControlTransfer\n"));

			//Get a pointer to the input buffer
			status = WdfRequestRetrieveInputMemory(Request, &WdfMem);
			if(!NT_SUCCESS(status))
			{
				PSDrv_DbgPrint(1, ("WdfRequestRetrieveInputMemory failed! (Status = %x)\n", status));
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			if (WdfMem == NULL)
			{
				PSDrv_DbgPrint(1, ("WdfMem is NULL!\n"));
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			pControlTransfer = WdfMemoryGetBuffer(WdfMem, NULL);
			if (pControlTransfer == NULL)
			{
				PSDrv_DbgPrint(1, ("pControlTransfer is NULL!\n"));
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			//Get a pointer to the output buffer
			if (OutputBufferLength != 0)
			{
				status = WdfRequestRetrieveOutputMemory(Request, &WdfMemOut);
				if(!NT_SUCCESS(status))
				{
					PSDrv_DbgPrint(1, ("WdfRequestRetrieveOutputMemory failed! (Status = %x)\n", status));
					status = STATUS_INVALID_PARAMETER;
					break;
				}

				if (WdfMemOut == NULL)
				{
					PSDrv_DbgPrint(1, ("WdfMemOut is NULL!\n"));
					status = STATUS_INVALID_PARAMETER;
					break;
				}

				pControlBuffer = WdfMemoryGetBuffer(WdfMemOut, NULL);
				if (pControlBuffer == NULL)
				{
					PSDrv_DbgPrint(1, ("pControlBuffer is NULL!\n"));
					status = STATUS_INVALID_PARAMETER;
					break;
				}
			}
			else
			{
				PSDrv_DbgPrint(1, ("This control request has no buffer...\n"));
				pControlBuffer = NULL;
			}

			// Call the control transfer function
			status = ControlTransfer(pDevContext, pControlTransfer, pControlBuffer, OutputBufferLength, &length);

			break;

		case IOCTL_PSDRV_SET_PIPE_PROPERTY:
			PSDrv_DbgPrint(3, ("IOControl: SetPipeProperty\n"));

			status = WdfRequestRetrieveInputMemory(Request, &WdfMem); 
			if(!NT_SUCCESS(status))
			{
				PSDrv_DbgPrint(1, ("WdfRequestRetrieveInputMemory failed! (Status = %x)\n", status));
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			if (WdfMem == NULL)
			{
				PSDrv_DbgPrint(1, ("WdfMem is NULL!\n"));
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			pPipeProp = WdfMemoryGetBuffer(WdfMem, NULL);
			if (pPipeProp == NULL)
			{
				PSDrv_DbgPrint(1, ("pPipeProp is NULL!\n"));
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			pFileContext = GetFileContext(WdfRequestGetFileObject(Request));
			if (pFileContext->Pipe == NULL)
			{
				PSDrv_DbgPrint(3, ("Invalid pipe!\n"));
				status = STATUS_INVALID_PARAMETER;
			}
			else
			{
				status = SetPipeProperty(pFileContext, pPipeProp);
			}

			break;

		case IOCTL_PSDRV_SET_INTERFACE:
			PSDrv_DbgPrint(3, ("IOControl: SetInterface\n"));

			status = WdfRequestRetrieveInputMemory(Request, &WdfMem); 
			if(!NT_SUCCESS(status))
			{
				PSDrv_DbgPrint(1, ("WdfRequestRetrieveInputMemory failed! (Status = %x)\n", status));
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			if (WdfMem == NULL)
			{
				PSDrv_DbgPrint(1, ("WdfMem is NULL!\n"));
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			pInterfaceProperty = WdfMemoryGetBuffer(WdfMem, NULL);
			if (pInterfaceProperty == NULL)
			{
				PSDrv_DbgPrint(1, ("pInterfaceProperty is NULL!\n"));
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			PSDrv_DbgPrint(3, ("SetInterface: Going to change AltIF to %d...\n", pInterfaceProperty->nAltIF));

			WDF_USB_INTERFACE_SELECT_SETTING_PARAMS_INIT_SETTING(&selectSettingParams, pInterfaceProperty->nAltIF);

			status = WdfUsbInterfaceSelectSetting(pDevContext->UsbInterface, WDF_NO_OBJECT_ATTRIBUTES, &selectSettingParams);

			if (status == STATUS_SUCCESS)
			{
				pDevContext->nCurrIf = 0;
				pDevContext->nCurrAltIf = pInterfaceProperty->nAltIF;

				PSDrv_DbgPrint(3, ("SetInterface: AltIF is now %d...\n", pInterfaceProperty->nAltIF));
			}

			break;

		case IOCTL_PSDRV_GET_INTERFACE:
			PSDrv_DbgPrint(3, ("IOControl: GetInterface\n"));

			length = sizeof(PSUSBDRV_INTERFACE_PROPERTY);

			status = WdfRequestRetrieveOutputBuffer(Request, length, &pInterfaceProperty, &bufLength);
			if(!NT_SUCCESS(status))
			{
				PSDrv_DbgPrint(1, ("WdfRequestRetrieveOutputBuffer failed! (Status = %x)\n", status));
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			pInterfaceProperty->nIF = pDevContext->nCurrIf;
			pInterfaceProperty->nAltIF = pDevContext->nCurrAltIf;

			status = STATUS_SUCCESS;

			break;

		case IOCTL_PSDRV_GET_DRIVER_VERSION:
			PSDrv_DbgPrint(3, ("IOControl: GetDriverVersion\n"));

			length = sizeof(PSUSBDRV_DRIVER_VERSION);

			status = WdfRequestRetrieveOutputBuffer(Request, length, &pDriverVersion, &bufLength);
			if(!NT_SUCCESS(status))
			{
				PSDrv_DbgPrint(1, ("WdfRequestRetrieveOutputBuffer failed! (Status = %x)\n", status));
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			pDriverVersion->nMajor = PSUSBDRV_MAJOR_VERSION;
			pDriverVersion->nMinor = PSUSBDRV_MINOR_VERSION;
			pDriverVersion->nMaintenance = PSUSBDRV_MAINTENANCE_VERSION;
			pDriverVersion->nBuild = PSUSBDRV_BUILD_VERSION;
			
			status = STATUS_SUCCESS;

			break;

		case IOCTL_PSDRV_GET_DEVICE_SPEED:
			PSDrv_DbgPrint(3, ("IOControl: GetDeviceSpeed\n"));

			length = sizeof(unsigned int);

			status = WdfRequestRetrieveOutputBuffer(Request, length, &pnDeviceSpeed, &bufLength);
			if(!NT_SUCCESS(status))
			{
				PSDrv_DbgPrint(1, ("WdfRequestRetrieveOutputBuffer failed! (Status = %x)\n", status));
				status = STATUS_INVALID_PARAMETER;
				break;
			}

			if (pDevContext->IsDeviceHighSpeed == TRUE)
			{
				*pnDeviceSpeed = PSUSBDRV_DEVICE_HIGH_SPEED;
			}
			else
			{
				*pnDeviceSpeed = PSUSBDRV_DEVICE_FULL_SPEED;
			}

			break;

		default:
			PSDrv_DbgPrint(3, ("Unknown IOControl! (ControlCode = %x)\n", IoControlCode));

			status = STATUS_INVALID_DEVICE_REQUEST;

			break;
    }

    WdfRequestCompleteWithInformation(Request, status, length);

	PSDrv_DbgPrint(3, ("PSDrv_EvtIoDeviceControl - ends\n"));

    return;
}

/*++
Routine Description:

Called by the framework when it receives Read requests.

Arguments:

Queue - Default queue handle
Request - Handle to the read/write request
Length - Length of the data buffer associated with the request.
The default property of the queue is to not dispatch
zero length read & write requests to the driver and
complete is with status success. So we will never get
a zero length request.
--*/
VOID PSDrv_EvtIoRead(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t Length)
{
    PFILE_CONTEXT				fileContext = NULL;
    WDFUSBPIPE					pipe;
    WDF_USB_PIPE_INFORMATION	pipeInfo;

    PAGED_CODE();

    // Get the pipe associate with this request.
    fileContext = GetFileContext(WdfRequestGetFileObject(Request));

    pipe = fileContext->Pipe;
    if (pipe == NULL)
	{
		PSDrv_DbgPrint(1, ("PSDrv_EvtIoRead: Invalid pipe!\n"));
        WdfRequestCompleteWithInformation(Request, STATUS_INVALID_PARAMETER, 0);
        return;
    }

    WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);
    WdfUsbTargetPipeGetInformation(pipe, &pipeInfo);

	if((WdfUsbPipeTypeBulk == pipeInfo.PipeType) || (WdfUsbPipeTypeInterrupt == pipeInfo.PipeType))
	{
        ReadWriteBulkEndPoints(Queue, Request, (ULONG)Length, WdfRequestTypeRead);
        return;
    }
	else if(WdfUsbPipeTypeIsochronous == pipeInfo.PipeType)
	{
        ReadWriteIsochEndPoints(Queue, Request, (ULONG)Length, WdfRequestTypeRead);
        return;
    }

    return;
}

/*++
Routine Description:

Called by the framework when it receives Write requests.

Arguments:

Queue - Default queue handle
Request - Handle to the read/write request
Length - Length of the data buffer associated with the request.
The default property of the queue is to not dispatch
zero length read & write requests to the driver and
complete is with status success. So we will never get
a zero length request.
--*/
VOID PSDrv_EvtIoWrite(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t Length)
{
    PFILE_CONTEXT				fileContext = NULL;
    WDFUSBPIPE					pipe;
    WDF_USB_PIPE_INFORMATION	pipeInfo;

    PAGED_CODE();

    // Get the pipe associate with this request.
    fileContext = GetFileContext(WdfRequestGetFileObject(Request));

    pipe = fileContext->Pipe;
    if (pipe == NULL) 
	{
   		PSDrv_DbgPrint(1, ("PSDrv_EvtIoWrite: Invalid pipe!\n"));
        WdfRequestCompleteWithInformation(Request, STATUS_INVALID_PARAMETER, 0);
        return;
    }

    WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);
    WdfUsbTargetPipeGetInformation(pipe, &pipeInfo);

    if((WdfUsbPipeTypeBulk == pipeInfo.PipeType) || (WdfUsbPipeTypeInterrupt == pipeInfo.PipeType))
	{
        ReadWriteBulkEndPoints(Queue, Request, (ULONG)Length, WdfRequestTypeWrite);
        return;
    }
	else if(WdfUsbPipeTypeIsochronous == pipeInfo.PipeType)
	{
        ReadWriteIsochEndPoints(Queue, Request, (ULONG)Length, WdfRequestTypeWrite);
        return;
    }

    return;
}

/*++
Routine Description:

This routine will pass the string pipe name and
fetch the pipe handle.

Arguments:

DeviceContext - pointer to Device Context

FileName - string pipe name

--*/
WDFUSBPIPE GetPipeFromName(IN PDEVICE_CONTEXT DeviceContext, IN PUNICODE_STRING FileName)
{
    LONG                  ix;
    ULONG                 uval;
    ULONG                 nameLength;
    ULONG                 umultiplier;
    WDFUSBPIPE            pipe = NULL;

    PAGED_CODE();

    nameLength = (FileName->Length / sizeof(WCHAR));

    if(nameLength != 0)
	{
        // Parse the pipe#
        ix = nameLength - 1;

        // if last char isn't digit, decrement it.
        while((ix > -1) && ((FileName->Buffer[ix] < (WCHAR)'0') || (FileName->Buffer[ix] > (WCHAR)'9')))
		{
            ix--;
        }

        if (ix > -1)
		{
            uval = 0;
            umultiplier = 1;

            // traversing least to most significant digits.
            while((ix > -1) && (FileName->Buffer[ix] >= (WCHAR)'0') && (FileName->Buffer[ix] <= (WCHAR)'9'))
			{
                uval += (umultiplier * (ULONG)(FileName->Buffer[ix] - (WCHAR)'0'));

                ix--;
                umultiplier *= 10;
            }

            pipe = WdfUsbInterfaceGetConfiguredPipe(DeviceContext->UsbInterface, (UCHAR)uval, NULL);
        }
    }

    return pipe;
}

/*++
Routine Description:

This routine resets the pipe.

Arguments:

Pipe - framework pipe handle

Return Value:

NT status value
--*/
NTSTATUS ResetPipe(IN WDFUSBPIPE Pipe)
{
    NTSTATUS status;

    PAGED_CODE();

    // This routine synchronously submits a URB_FUNCTION_RESET_PIPE request down the stack.
    status = WdfUsbTargetPipeResetSynchronously(Pipe, WDF_NO_HANDLE, NULL);
    if (NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(3, ("ResetPipe success!\n"));
        status = STATUS_SUCCESS;
    }
    else
	{
        PSDrv_DbgPrint(1, ("ResetPipe failed! (Status = %x)\n", status));
    }

    return status;
}

/*++
Routine Description:

This routine aborts the pipe.

Arguments:

Pipe - framework pipe handle

Return Value:

NT status value
--*/
NTSTATUS AbortPipe(IN WDFUSBPIPE Pipe)
{
	NTSTATUS status;

	PAGED_CODE();

	//  This routine synchronously submits a URB_FUNCTION_RESET_PIPE request down the stack.
	status = WdfUsbTargetPipeAbortSynchronously(Pipe, WDF_NO_HANDLE, NULL);

	if (NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(3, ("AbortPipe success!\n"));
		status = STATUS_SUCCESS;
	}
	else
	{
		PSDrv_DbgPrint(1, ("AbortPipe failed! (Status = %x)\n", status));
	}

	return status;
}

VOID StopAllPipes(IN PDEVICE_CONTEXT DeviceContext)
{
    UCHAR count;
	UCHAR i;

    count = DeviceContext->NumberConfiguredPipes;
    for (i = 0; i < count; i++)
	{
        WDFUSBPIPE pipe;

        pipe = WdfUsbInterfaceGetConfiguredPipe(DeviceContext->UsbInterface, i, NULL);

        WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(pipe), WdfIoTargetCancelSentIo);
    }
}

VOID StartAllPipes(IN PDEVICE_CONTEXT DeviceContext)
{
    NTSTATUS status;
    UCHAR count;
	UCHAR i;

    count = DeviceContext->NumberConfiguredPipes;
    for (i = 0; i < count; i++)
	{
        WDFUSBPIPE pipe;

        pipe = WdfUsbInterfaceGetConfiguredPipe(DeviceContext->UsbInterface, i, NULL);

        status = WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(pipe));

        if (!NT_SUCCESS(status))
		{
            PSDrv_DbgPrint(1, ("StartAllPipes failed on pipe %d! (Status = %x)\n", i, status));
        }
    }
}

/*++
Routine Description:

This routine calls WdfUsbTargetDeviceResetPortSynchronously to reset the device if it's still
connected.

Arguments:

Device - Handle to a framework device

Return Value:

NT status value
--*/
NTSTATUS ResetDevice(IN WDFDEVICE Device)

{
    PDEVICE_CONTEXT pDeviceContext;
    NTSTATUS status;

    PSDrv_DbgPrint(3, ("ResetDevice - begins\n"));

    PAGED_CODE();

    pDeviceContext = GetDeviceContext(Device);
    
    // A reset-device
    // request will be stuck in the USB until the pending transactions
    // have been canceled. Similarly, if there are pending transfers on the BULK
    // IN/OUT pipe cancel them.
    // To work around this issue, the driver should stop the continuous reader
    // (by calling WdfIoTargetStop) before resetting the device, and restart the
    // continuous reader (by calling WdfIoTargetStart) after the request completes.
    StopAllPipes(pDeviceContext);
    
    // It may not be necessary to check whether device is connected before
    // resetting the port.
    status = WdfUsbTargetDeviceIsConnectedSynchronous(pDeviceContext->WdfUsbTargetDevice);

    if(NT_SUCCESS(status))
	{
        status = WdfUsbTargetDeviceResetPortSynchronously(pDeviceContext->WdfUsbTargetDevice);
    }

    StartAllPipes(pDeviceContext);
    
    PSDrv_DbgPrint(3, ("ResetDevice - ends\n"));

    return status;
}

/*++
Routine Description:

This routine sets a property on a pipe.

Arguments:

pFileContext - Pipe (which is actually a file) context handle
pPipeProp - The pipe property struct

Return Value:

NT status value
--*/
NTSTATUS SetPipeProperty(IN OUT PFILE_CONTEXT pFileContext, IN PSUSBDRV_PIPE_PROPERTY* pPipeProp)
{
	NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

	switch(pPipeProp->nIndex)
	{
		case PSUSBDRV_PIPE_PROPERTY_TIMEOUT:
			pFileContext->nTimeOut = pPipeProp->nValue;

			PSDrv_DbgPrint(3, ("SetPipeProperty: Timeout is set to: %d.\n", pPipeProp->nValue));

			break;
		default:
			status = STATUS_INVALID_PARAMETER;

			PSDrv_DbgPrint(3, ("SetPipeProperty: Invalid Property! (Property = %d)\n", pPipeProp->nIndex));

			break;
	}

	return status;
}

/*++
Routine Description:

This routine sets a property on a pipe.

Arguments:

pDeviceContext - Device context handle
pControlTransfer - The control transfer property struct

Return Value:

NT status value
--*/
NTSTATUS ControlTransfer(IN PDEVICE_CONTEXT pDeviceContext, IN PSUSBDRV_CONTROL_TRANSFER* pControlTransfer, IN OUT PUCHAR pControlBuffer, IN size_t nControlBufferSize, OUT ULONG* pLength)
{
	NTSTATUS						status = STATUS_SUCCESS;
	unsigned int					nTimeout = PSUSBDRV_DEFAULT_CONTROL_TIMEOUT;
	WDF_REQUEST_SEND_OPTIONS		sendOptions;
	WDF_USB_CONTROL_SETUP_PACKET	controlSetupPacket;
	WDF_MEMORY_DESCRIPTOR			memDesc;

	PAGED_CODE();

	RtlZeroMemory(&controlSetupPacket, sizeof(WDF_USB_CONTROL_SETUP_PACKET));

	controlSetupPacket.Packet.bm.Request.Dir = pControlTransfer->cDirection;
	controlSetupPacket.Packet.bm.Request.Type = pControlTransfer->cRequestType;
	controlSetupPacket.Packet.bm.Request.Recipient = BmRequestToDevice;
	controlSetupPacket.Packet.bRequest = pControlTransfer->cRequest;
	controlSetupPacket.Packet.wValue.Value = pControlTransfer->nValue;
	controlSetupPacket.Packet.wIndex.Value = pControlTransfer->nIndex;

	PSDrv_DbgPrint(3, ("ControlTransfer: Dir:%d Type:%d Request:%d Value:%d Index:%d.\n", pControlTransfer->cDirection, pControlTransfer->cRequestType,  pControlTransfer->cRequest, pControlTransfer->nValue, pControlTransfer->nIndex));

	if (pControlTransfer->nTimeout != 0)
	{
		if (pControlTransfer->nTimeout > PSUSBDRV_DEFAULT_CONTROL_TIMEOUT)
		{
			PSDrv_DbgPrint(3, ("ControlTransfer: Timeout was truncated from %d to %d!\n", pControlTransfer->nTimeout, PSUSBDRV_DEFAULT_CONTROL_TIMEOUT));

			nTimeout = PSUSBDRV_DEFAULT_CONTROL_TIMEOUT;
		}
		else
		{
			nTimeout = pControlTransfer->nTimeout;
		}
	}

	WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions, WDF_REQUEST_SEND_OPTION_TIMEOUT);
	WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(&sendOptions, WDF_REL_TIMEOUT_IN_MS(nTimeout));	

	PSDrv_DbgPrint(3, ("ControlTransfer: Timeout is set to: %d.\n", nTimeout));

	PSDrv_DbgPrint(3, ("ControlTransfer: Performing the control transfer...\n"));

	if (pControlBuffer != NULL)
	{
		PSDrv_DbgPrint(3, ("ControlTransfer: Buffer:%x Length:%d\n", pControlBuffer, nControlBufferSize));

		if (nControlBufferSize == 0)
		{
			PSDrv_DbgPrint(3, ("ControlTransfer: pControlBuffer is not NULL but nControlBufferSize is 0!\n"));

			return (STATUS_INVALID_PARAMETER);
		}

		WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDesc, pControlBuffer, (ULONG)nControlBufferSize);

		status = WdfUsbTargetDeviceSendControlTransferSynchronously(pDeviceContext->WdfUsbTargetDevice, WDF_NO_HANDLE, &sendOptions, &controlSetupPacket, &memDesc, pLength);
	}
	else
	{
		status = WdfUsbTargetDeviceSendControlTransferSynchronously(pDeviceContext->WdfUsbTargetDevice, WDF_NO_HANDLE, &sendOptions, &controlSetupPacket, NULL, pLength);
	}

	PSDrv_DbgPrint(3, ("ControlTransfer: Finished! Status=%x BytesTransferred=%d\n", status, *pLength));

	return status;
}