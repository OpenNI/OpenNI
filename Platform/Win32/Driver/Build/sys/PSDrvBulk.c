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

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
/*++
Routine Description:

This callback is invoked when the framework received  WdfRequestTypeRead or
WdfRequestTypeWrite request. This read/write is performed in stages of
MAX_TRANSFER_SIZE. Once a stage of transfer is complete, then the
request is circulated again, until the requested length of transfer is
performed.

Arguments:

Queue - Handle to the framework queue object that is associated
with the I/O request.

Request - Handle to a framework request object. This one represents
the WdfRequestTypeRead/WdfRequestTypeWrite IRP received by the framework.

Length - Length of the input/output buffer.

Return Value:

VOID
--*/
VOID ReadWriteBulkEndPoints(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN ULONG Length, IN WDF_REQUEST_TYPE RequestType)
{
    PMDL						newMdl = NULL;
	PMDL						requestMdl = NULL;
    PURB						urb = NULL;
    WDFMEMORY					urbMemory;
    ULONG						totalLength = Length;
    ULONG						stageLength = 0;
    ULONG						urbFlags = 0;
    NTSTATUS					status;
    ULONG_PTR					virtualAddress = 0;
    PREQUEST_CONTEXT			rwContext = NULL;
    PFILE_CONTEXT				fileContext = NULL;
    WDFUSBPIPE					pipe;
    WDF_USB_PIPE_INFORMATION	pipeInfo;
    WDF_OBJECT_ATTRIBUTES		objectAttribs;
    USBD_PIPE_HANDLE			usbdPipeHandle;
    PDEVICE_CONTEXT				deviceContext;
	WDF_REQUEST_SEND_OPTIONS	sendOptions;

    PSDrv_DbgPrint(3, ("ReadWriteBulkEndPoints - begins\n"));

    // First validate input parameters.
    deviceContext = GetDeviceContext(WdfIoQueueGetDevice(Queue));

    if (totalLength > deviceContext->MaximumTransferSize)
	{
        PSDrv_DbgPrint(1, ("Transfer length (%d) is bigger then MaximumTransferSize (%d)!\n", totalLength, deviceContext->MaximumTransferSize));
        status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    if ((RequestType != WdfRequestTypeRead) && (RequestType != WdfRequestTypeWrite))
	{
        PSDrv_DbgPrint(1, ("RequestType has to be either Read or Write! (RequestType = %d)\n", RequestType));
        status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    // Get the pipe associate with this request.
    fileContext = GetFileContext(WdfRequestGetFileObject(Request));
    pipe = fileContext->Pipe;
    WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);
    WdfUsbTargetPipeGetInformation(pipe, &pipeInfo);

    if((WdfUsbPipeTypeBulk != pipeInfo.PipeType) && (WdfUsbPipeTypeInterrupt != pipeInfo.PipeType))
	{
        PSDrv_DbgPrint(1, ("Usbd pipe type is not bulk or interrupt! (PipeType = %d)\n", pipeInfo.PipeType));
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto Exit;
    }

    rwContext = GetRequestContext(Request);

    if(RequestType == WdfRequestTypeRead)
	{
        status = WdfRequestRetrieveOutputWdmMdl(Request, &requestMdl);
        if(!NT_SUCCESS(status))
		{
            PSDrv_DbgPrint(1, ("WdfRequestRetrieveOutputWdmMdl failed! (Status = %x)\n", status));
            goto Exit;
        }

        urbFlags |= USBD_TRANSFER_DIRECTION_IN;
        rwContext->Read = TRUE;
        PSDrv_DbgPrint(3, ("This is a read operation...\n"));
    }
	else
	{
        status = WdfRequestRetrieveInputWdmMdl(Request, &requestMdl);
        if(!NT_SUCCESS(status))
		{
			PSDrv_DbgPrint(1, ("WdfRequestRetrieveInputWdmMdl failed! (Status = %x)\n", status));
            goto Exit;
        }

        urbFlags |= USBD_TRANSFER_DIRECTION_OUT;
        rwContext->Read = FALSE;
        PSDrv_DbgPrint(3, ("This is a write operation...\n"));
    }


    urbFlags |= USBD_SHORT_TRANSFER_OK;

    virtualAddress = (ULONG_PTR) MmGetMdlVirtualAddress(requestMdl);

    // The transfer request is for totalLength. We can perform a max of MAX_TRANSFER_SIZE in each stage.
    if (totalLength > MAX_TRANSFER_SIZE)
	{
        stageLength = MAX_TRANSFER_SIZE;
    }
    else
	{
        stageLength = totalLength;
    }

    newMdl = IoAllocateMdl((PVOID)virtualAddress, totalLength, FALSE, FALSE, NULL);
    if (newMdl == NULL)
	{
        PSDrv_DbgPrint(1, ("IoAllocateMdl failed! (newMdl is NULL)\n"));
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    // Map the portion of user-buffer described by an mdl to another mdl
    IoBuildPartialMdl(requestMdl, newMdl, (PVOID)virtualAddress, stageLength);

    WDF_OBJECT_ATTRIBUTES_INIT(&objectAttribs);
    objectAttribs.ParentObject = Request;

    status = WdfMemoryCreate(&objectAttribs, NonPagedPool, POOL_TAG, sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER), &urbMemory, (PVOID*)&urb);
    if (!NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(1, ("WdfMemoryCreate for urbMemory failed! (Status = %x)\n", status));
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    usbdPipeHandle = WdfUsbTargetPipeWdmGetPipeHandle(pipe);

    UsbBuildInterruptOrBulkTransferRequest(urb, sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER), usbdPipeHandle, NULL, newMdl, stageLength, urbFlags, NULL);

    status = WdfUsbTargetPipeFormatRequestForUrb(pipe, Request, urbMemory, NULL);
    if (!NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(1, ("WdfUsbTargetPipeFormatRequestForUrb failed! (Status = %x)\n", status));
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    WdfRequestSetCompletionRoutine(Request, ReadWriteCompletion, NULL);

    // Set REQUEST_CONTEXT parameters.
    rwContext->UrbMemory       = urbMemory;
    rwContext->Mdl             = newMdl;
    rwContext->Length          = totalLength - stageLength;
    rwContext->Numxfer         = 0;
    rwContext->VirtualAddress  = virtualAddress + stageLength;

	// Set the timeout
	if (fileContext->nTimeOut != 0)
	{
		WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions, WDF_REQUEST_SEND_OPTION_TIMEOUT);
		WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(&sendOptions, WDF_REL_TIMEOUT_IN_MS(fileContext->nTimeOut));

		PSDrv_DbgPrint(3, ("Pipe timeout is set to: %d\n", fileContext->nTimeOut));

		if (!WdfRequestSend(Request, WdfUsbTargetPipeGetIoTarget(pipe), &sendOptions))
		{
			status = WdfRequestGetStatus(Request);
			ASSERT(!NT_SUCCESS(status));
		}
	}
	else
	{
		if (!WdfRequestSend(Request, WdfUsbTargetPipeGetIoTarget(pipe), WDF_NO_SEND_OPTIONS))
		{
			status = WdfRequestGetStatus(Request);
			ASSERT(!NT_SUCCESS(status));
		}
	}

Exit:
    if (!NT_SUCCESS(status))
	{
        WdfRequestCompleteWithInformation(Request, status, 0);

        if (newMdl != NULL)
		{
            IoFreeMdl(newMdl);
        }
    }

    PSDrv_DbgPrint(3, ("ReadWriteBulkEndPoints - ends\n"));

    return;
}

/*++
Routine Description:

This is the completion routine for reads/writes
If the irp completes with success, we check if we
need to recirculate this irp for another stage of
transfer.

Arguments:

Context - Driver supplied context
Device - Device handle
Request - Request handle
Params - request completion params

Return Value:
None
--*/
VOID ReadWriteCompletion(IN WDFREQUEST Request, IN WDFIOTARGET Target, PWDF_REQUEST_COMPLETION_PARAMS CompletionParams, IN WDFCONTEXT Context)
{
    PMDL                    requestMdl;
    WDFUSBPIPE              pipe;
    ULONG                   stageLength;
    NTSTATUS                status;
    PREQUEST_CONTEXT        rwContext;
    PURB                    urb;
    PCHAR                   operation;
    ULONG                   bytesReadWritten;

    UNREFERENCED_PARAMETER(Context);
    rwContext = GetRequestContext(Request);

    PSDrv_DbgPrint(3, ("ReadWriteCompletion - begins\n"));

    if (rwContext->Read)
	{
        operation = "Read";
    }
	else
	{
        operation = "Write";
    }

    pipe = (WDFUSBPIPE)Target;
    status = CompletionParams->IoStatus.Status;

    if (!NT_SUCCESS(status))
	{
        goto End;
    }

    urb = (PURB)WdfMemoryGetBuffer(rwContext->UrbMemory, NULL);
    bytesReadWritten = urb->UrbBulkOrInterruptTransfer.TransferBufferLength;
    rwContext->Numxfer += bytesReadWritten;

    // If there is anything left to transfer.
    if (rwContext->Length == 0)
	{
        // this is the last transfer
        WdfRequestSetInformation(Request, rwContext->Numxfer);
        goto End;
    }

    // Start another transfer
    PSDrv_DbgPrint(3, ("Stage next %s transfer...\n", operation));

    if (rwContext->Length > MAX_TRANSFER_SIZE)
	{
        stageLength = MAX_TRANSFER_SIZE;
    }
    else
	{
        stageLength = rwContext->Length;
    }

    // Following call is required to free any mapping made on the partial MDL and reset internal MDL state.
    MmPrepareMdlForReuse(rwContext->Mdl);

    if (rwContext->Read)
	{
        status = WdfRequestRetrieveOutputWdmMdl(Request, &requestMdl);
        if(!NT_SUCCESS(status))
		{
            PSDrv_DbgPrint(1, ("WdfRequestRetrieveOutputWdmMdl for Read failed! (Status = %x)\n", status));
            goto End;
        }
    }
	else
	{
        status = WdfRequestRetrieveInputWdmMdl(Request, &requestMdl);
        if(!NT_SUCCESS(status))
		{
            PSDrv_DbgPrint(1, ("WdfRequestRetrieveInputWdmMdl for Write failed! (Status =  %x)\n", status));
            goto End;
        }
    }

    IoBuildPartialMdl(requestMdl, rwContext->Mdl, (PVOID)rwContext->VirtualAddress, stageLength);

    // Reinitialize the urb
    urb->UrbBulkOrInterruptTransfer.TransferBufferLength = stageLength;

    rwContext->VirtualAddress += stageLength;
    rwContext->Length -= stageLength;

    // Format the request to send a URB to a USB pipe.
    status = WdfUsbTargetPipeFormatRequestForUrb(pipe, Request, rwContext->UrbMemory, NULL);
    if (!NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(1, ("WdfUsbTargetPipeFormatRequestForUrb failed! (Status = %x)\n", status));
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto End;
    }

    WdfRequestSetCompletionRoutine(Request, ReadWriteCompletion, NULL);

    // Send the request asynchronously.
    if (!WdfRequestSend(Request, WdfUsbTargetPipeGetIoTarget(pipe), WDF_NO_SEND_OPTIONS))
	{
        PSDrv_DbgPrint(1, ("WdfRequestSend for %s failed!\n", operation));
        status = WdfRequestGetStatus(Request);
        goto End;
    }

    // Else when the request completes, this completion routine will be called again.
    PSDrv_DbgPrint(3, ("ReadWriteCompletion - ends 1\n"));

    return;

End:
    // Dump the request context, complete the request and return.
	PSDrv_DbgPrint(3, ("rwContext->UrbMemory      = %p\n", rwContext->UrbMemory));
	PSDrv_DbgPrint(3, ("rwContext->Mdl            = %p\n", rwContext->Mdl));
	PSDrv_DbgPrint(3, ("rwContext->Length         = %d\n", rwContext->Length));
	PSDrv_DbgPrint(3, ("rwContext->Numxfer        = %d\n", rwContext->Numxfer));
	PSDrv_DbgPrint(3, ("rwContext->VirtualAddress = %p\n", rwContext->VirtualAddress));

    IoFreeMdl(rwContext->Mdl);

    PSDrv_DbgPrint(3, ("Bulk or Interrupt %s request has finished. (Status = %x)\n", operation, status));

    WdfRequestComplete(Request, status);

    PSDrv_DbgPrint(3, ("ReadWriteCompletion - ends 2\n"));

    return;
}
