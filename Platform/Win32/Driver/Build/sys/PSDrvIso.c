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

This routine does some validation and
invokes appropriate function to perform
Isoch transfer

Arguments:

DeviceObject - pointer to device object
Irp - I/O request packet

Return Value:

NT status value
--*/
VOID ReadWriteIsochEndPoints(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN ULONG Length, IN WDF_REQUEST_TYPE RequestType)
{
    ULONG                   totalLength;
    ULONG                   packetSize;
    NTSTATUS               status;
    PDEVICE_CONTEXT         deviceContext;
    WDF_USB_PIPE_INFORMATION   pipeInfo;
    PFILE_CONTEXT           fileContext;
    WDFUSBPIPE              pipe;

    PSDrv_DbgPrint(3, ("ReadWriteIsochEndPoints - begins\n"));

    // initialize vars
    totalLength = Length;

    deviceContext = GetDeviceContext(WdfIoQueueGetDevice(Queue));

    // Get the pipe associate with this request.
    fileContext = GetFileContext(WdfRequestGetFileObject(Request));
    pipe = fileContext->Pipe;
    WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);
    WdfUsbTargetPipeGetInformation(pipe, &pipeInfo);

    if((WdfUsbPipeTypeIsochronous != pipeInfo.PipeType)) {
        PSDrv_DbgPrint(1, ("Usbd pipe type is not Isochronous\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto Exit;
    }

    // each packet can hold this much info
    packetSize = pipeInfo.MaximumPacketSize;

    if(packetSize == 0) {
        PSDrv_DbgPrint(1, ("Invalid parameter\n"));
        status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    // atleast packet worth of data to be transferred.
    if(totalLength < packetSize) {

        PSDrv_DbgPrint(1, ("Atleast packet worth of data..\n"));
        status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    // Perform reset. if there are some active transfers queued up
    // for this endpoint then the reset pipe will fail.
	// DevNote: On second thought, reset is very bad here!!! it will break overlapping I/Os...
    //ResetPipe(pipe);

    if(deviceContext->IsDeviceHighSpeed) {
        PerformHighSpeedIsochTransfer(WdfIoQueueGetDevice(Queue),
                                      Queue,
                                      Request,
                                      Length,
                                      RequestType);
        return;
    }
    else {
		PSDrv_DbgPrint(1, ("Full speed isochronous endpoints are not supported!\n"));
		status = STATUS_INVALID_DEVICE_REQUEST;
		goto Exit;
    }

Exit:
    WdfRequestCompleteWithInformation(Request, status, 0);
    return;
}

/*++
Routine Description:

High Speed Isoch Transfer requires packets in multiples of 8.
(Argument: 8 micro-frames per ms frame)
Another restriction is that each Irp/Urb pair can be associated
with a max of 1024 packets.

Here is one of the ways of creating Irp/Urb pairs.
Depending on the characteristics of real-world device,
the algorithm may be different

This algorithm will distribute data evenly among all the packets.

Input:
TotalLength - no. of bytes to be transferred.

Other parameters:
packetSize - max size of each packet for this pipe.

Implementation Details:

Step 1:
ASSERT(TotalLength >= 8)

Step 2:
Find the exact number of packets required to transfer all of this data

numberOfPackets = (TotalLength + packetSize - 1) / packetSize

Step 3:
Number of packets in multiples of 8.

if(0 == (numberOfPackets % 8)) {

actualPackets = numberOfPackets;
}
else {

actualPackets = numberOfPackets +
(8 - (numberOfPackets % 8));
}

Step 4:
Determine the min. data in each packet.

minDataInEachPacket = TotalLength / actualPackets;

Step 5:
After placing min data in each packet,
determine how much data is left to be distributed.

dataLeftToBeDistributed = TotalLength -
(minDataInEachPacket * actualPackets);

Step 6:
Start placing the left over data in the packets
(above the min data already placed)

numberOfPacketsFilledToBrim = dataLeftToBeDistributed /
(packetSize - minDataInEachPacket);

Step 7:
determine if there is any more data left.

dataLeftToBeDistributed -= (numberOfPacketsFilledToBrim *
(packetSize - minDataInEachPacket));

Step 8:
The "dataLeftToBeDistributed" is placed in the packet at index
"numberOfPacketsFilledToBrim"

Algorithm at play:

TotalLength  = 8193
packetSize   = 8
Step 1

Step 2
numberOfPackets = (8193 + 8 - 1) / 8 = 1025

Step 3
actualPackets = 1025 + 7 = 1032

Step 4
minDataInEachPacket = 8193 / 1032 = 7 bytes

Step 5
dataLeftToBeDistributed = 8193 - (7 * 1032) = 969.

Step 6
numberOfPacketsFilledToBrim = 969 / (8 - 7) = 969.

Step 7
dataLeftToBeDistributed = 969 - (969 * 1) = 0.

Step 8
Done :)

Another algorithm
Completely fill up (as far as possible) the early packets.
Place 1 byte each in the rest of them.
Ensure that the total number of packets is multiple of 8.

This routine then
1. Creates a Sub Request for each irp/urb pair.
(Each irp/urb pair can transfer a max of 1024 packets.)
2. All the irp/urb pairs are initialized
3. The subsidiary irps (of the irp/urb pair) are passed
down the stack at once.
4. The main Read/Write is completed in the SubRequestCompletionRoutine
even if one SubRequest is sent successfully.

Arguments:

Device - Device handle
Request - Default queue handle
Request - Read/Write Request received from the user app.
TotalLength - Length of the user buffer.

Return Value:

VOID
--*/
VOID PerformHighSpeedIsochTransfer(IN WDFDEVICE Device, IN WDFQUEUE Queue, IN WDFREQUEST Request, IN ULONG TotalLength, IN WDF_REQUEST_TYPE RequestType)
{
    ULONG                   numberOfPackets;
    ULONG                   actualPackets;
    ULONG                   minDataInEachPacket;
    ULONG                   dataLeftToBeDistributed;
    ULONG                   numberOfPacketsFilledToBrim;
    ULONG                   packetSize;
    ULONG                   stageSize;
    PUCHAR                  virtualAddress;
    NTSTATUS                status;
    PDEVICE_CONTEXT         deviceContext;
    PREQUEST_CONTEXT        rwContext;
    PMDL                    requestMdl;
    WDF_USB_PIPE_INFORMATION   pipeInfo;
    WDFUSBPIPE              pipe;
    PFILE_CONTEXT           fileContext;
    WDF_OBJECT_ATTRIBUTES   attributes;
    WDFREQUEST              subRequest;
    PSUB_REQUEST_CONTEXT    subReqContext;
    ULONG                   j;
    USBD_PIPE_HANDLE        usbdPipeHandle;

    UNREFERENCED_PARAMETER(Device);

    rwContext = GetRequestContext(Request);

    if(RequestType == WdfRequestTypeRead) {

        rwContext->Read = TRUE;
    } else {
        rwContext->Read = FALSE;
    }

    PSDrv_DbgPrint(3, ("PerformHighSpeedIsochTransfer %s - begins\n",
                                rwContext->Read ? "Read":"Write"));
    if(TotalLength < 8) {

        status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    deviceContext = GetDeviceContext(WdfIoQueueGetDevice(Queue));

    // Get the pipe associate with this request.
    fileContext = GetFileContext(WdfRequestGetFileObject(Request));
    pipe = fileContext->Pipe;
    WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);
    WdfUsbTargetPipeGetInformation(pipe, &pipeInfo);


    // each packet can hold this much info
    packetSize = pipeInfo.MaximumPacketSize;

    numberOfPackets = (TotalLength + packetSize - 1) / packetSize;

    if(0 == (numberOfPackets % 8)) {

        actualPackets = numberOfPackets;
    }
    else {
        // we need multiple of 8 packets only.
        actualPackets = numberOfPackets +
                        (8 - (numberOfPackets % 8));
    }

    minDataInEachPacket = TotalLength / actualPackets;

    if(minDataInEachPacket == packetSize) {

        numberOfPacketsFilledToBrim = actualPackets;
        dataLeftToBeDistributed     = 0;

        PSDrv_DbgPrint(1, ("TotalLength = %d\n", TotalLength));
        PSDrv_DbgPrint(1, ("PacketSize  = %d\n", packetSize));
        PSDrv_DbgPrint(1, ("Each of %d packets has %d bytes\n",
                            numberOfPacketsFilledToBrim,
                            packetSize));
    }
    else {

        dataLeftToBeDistributed = TotalLength -
                              (minDataInEachPacket * actualPackets);

        numberOfPacketsFilledToBrim = dataLeftToBeDistributed /
                                  (packetSize - minDataInEachPacket);

        dataLeftToBeDistributed -= (numberOfPacketsFilledToBrim *
                                (packetSize - minDataInEachPacket));


        PSDrv_DbgPrint(1, ("TotalLength = %d\n", TotalLength));
        PSDrv_DbgPrint(1, ("PacketSize  = %d\n", packetSize));
        PSDrv_DbgPrint(1, ("Each of %d packets has %d bytes\n",
                            numberOfPacketsFilledToBrim,
                            packetSize));
        if(dataLeftToBeDistributed) {

            PSDrv_DbgPrint(1, ("One packet has %d bytes\n",
                                minDataInEachPacket + dataLeftToBeDistributed));
            PSDrv_DbgPrint(1, ("Each of %d packets has %d bytes\n",
                                actualPackets - (numberOfPacketsFilledToBrim + 1),
                                minDataInEachPacket));
        }
        else {
            PSDrv_DbgPrint(1, ("Each of %d packets has %d bytes\n",
                                actualPackets - numberOfPacketsFilledToBrim,
                                minDataInEachPacket));
        }
    }

	rwContext->TotalLength = TotalLength;

    if(RequestType == WdfRequestTypeRead) {
        status = WdfRequestRetrieveOutputWdmMdl(Request, &requestMdl);
        if(!NT_SUCCESS(status)){
            PSDrv_DbgPrint(1, ("WdfRequestRetrieveOutputWdmMdl failed %x\n", status));
            goto Exit;
        }
    } else {
        status = WdfRequestRetrieveInputWdmMdl(Request, &requestMdl);
        if(!NT_SUCCESS(status)){
            PSDrv_DbgPrint(1, ("WdfRequestRetrieveInputWdmMdl failed %x\n", status));
            goto Exit;
        }
    }

    virtualAddress = (PUCHAR) MmGetMdlVirtualAddress(requestMdl);


	// ZZZZZZ (This used to loop the subrequests)
	{

        WDFMEMORY               subUrbMemory;
        PURB                    subUrb;
        PMDL                    subMdl;
        ULONG                   nPackets;
        ULONG                   siz;
        ULONG                   offset;


        // For every stage of transfer we need to do the following
        // tasks
        // 1. allocate a request
        // 2. allocate an urb
        // 3. allocate a mdl.
        // 4. Format the request for transferring URB
        // 5. Send the Request.
        if(actualPackets <= 1024) {

            nPackets = actualPackets;
            actualPackets = 0;
        }
        else {

            nPackets = 1024;
            actualPackets -= 1024;
        }

        PSDrv_DbgPrint(1, ("nPackets = %d for Irp/URB\n", nPackets));

        ASSERT(nPackets <= 1024);

        siz = GET_ISO_URB_SIZE(nPackets);


        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&attributes, SUB_REQUEST_CONTEXT            );

        status = WdfRequestCreate(
            &attributes,
            WdfUsbTargetDeviceGetIoTarget(deviceContext->WdfUsbTargetDevice),
            &subRequest);

        if(!NT_SUCCESS(status)) {

            PSDrv_DbgPrint(1, ("WdfRequestCreate failed 0x%x\n", status));
            goto Exit;
        }

        subReqContext = GetSubRequestContext(subRequest);
        subReqContext->UserRequest = Request;

        // Allocate memory for URB.
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = subRequest;
        status = WdfMemoryCreate(
                            &attributes,
                            NonPagedPool,
                            POOL_TAG,
                            siz,
                            &subUrbMemory,
                            (PVOID*) &subUrb);

        if (!NT_SUCCESS(status)) {
            WdfObjectDelete(subRequest);
            PSDrv_DbgPrint(1, ("Failed to alloc MemoryBuffer for suburb\n"));
            goto Exit;
        }

        subReqContext->SubUrb = subUrb;


        if(nPackets > numberOfPacketsFilledToBrim) {

            stageSize =  packetSize * numberOfPacketsFilledToBrim;
            stageSize += (minDataInEachPacket *
                          (nPackets - numberOfPacketsFilledToBrim));
            stageSize += dataLeftToBeDistributed;
        }
        else {

            stageSize = packetSize * nPackets;
        }

        // allocate a mdl.
        subMdl = IoAllocateMdl((PVOID) virtualAddress,
                               stageSize,
                               FALSE,
                               FALSE,
                               NULL);

        if(subMdl == NULL) {

            PSDrv_DbgPrint(1, ("failed to alloc mem for sub context mdl\n"));
            WdfObjectDelete(subRequest);
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }

        IoBuildPartialMdl(requestMdl,
                          subMdl,
                          (PVOID) virtualAddress,
                          stageSize);

        subReqContext->SubMdl = subMdl;

        virtualAddress += stageSize;
        TotalLength -= stageSize;

        usbdPipeHandle = WdfUsbTargetPipeWdmGetPipeHandle(pipe);
        subUrb->UrbIsochronousTransfer.Hdr.Length = (USHORT) siz;
        subUrb->UrbIsochronousTransfer.Hdr.Function = URB_FUNCTION_ISOCH_TRANSFER;
        subUrb->UrbIsochronousTransfer.PipeHandle = usbdPipeHandle;

        if(rwContext->Read) {

            subUrb->UrbIsochronousTransfer.TransferFlags =
                                                     USBD_TRANSFER_DIRECTION_IN;
        }
        else {

            subUrb->UrbIsochronousTransfer.TransferFlags =
                                                     USBD_TRANSFER_DIRECTION_OUT;
        }

        subUrb->UrbIsochronousTransfer.TransferBufferLength = stageSize;
        subUrb->UrbIsochronousTransfer.TransferBufferMDL = subMdl;
		subUrb->UrbIsochronousTransfer.TransferBuffer = NULL;

/*
//        This is a way to set the start frame and NOT specify ASAP flag.

        WdfUsbTargetDeviceRetrieveCurrentFrameNumber(deviceContext->WdfUsbTargetDevice, &frameNumber);
        subUrb->UrbIsochronousTransfer.StartFrame = frameNumber  + 20;
*/

        subUrb->UrbIsochronousTransfer.TransferFlags |=
                                        USBD_START_ISO_TRANSFER_ASAP;

        subUrb->UrbIsochronousTransfer.NumberOfPackets = nPackets;
        subUrb->UrbIsochronousTransfer.UrbLink = NULL;

        // set the offsets for every packet for reads/writes
        if(rwContext->Read) {

            offset = 0;

            for(j = 0; j < nPackets; j++) {

                subUrb->UrbIsochronousTransfer.IsoPacket[j].Offset = offset;
                subUrb->UrbIsochronousTransfer.IsoPacket[j].Length = 0;

                if(numberOfPacketsFilledToBrim) {

                    offset += packetSize;
                    numberOfPacketsFilledToBrim--;
                    status = RtlULongSub(stageSize, packetSize, &stageSize);
                    ASSERT(NT_SUCCESS(status));
                }
                else if(dataLeftToBeDistributed) {

                    offset += (minDataInEachPacket + dataLeftToBeDistributed);
                    stageSize -= (minDataInEachPacket + dataLeftToBeDistributed);
                    dataLeftToBeDistributed = 0;
                }
                else {

                    offset += minDataInEachPacket;
                    stageSize -= minDataInEachPacket;
                }
            }

            ASSERT(stageSize == 0);
        }
        else {

            offset = 0;

            for(j = 0; j < nPackets; j++) {

                subUrb->UrbIsochronousTransfer.IsoPacket[j].Offset = offset;

                if(numberOfPacketsFilledToBrim) {

                    subUrb->UrbIsochronousTransfer.IsoPacket[j].Length = packetSize;
                    offset += packetSize;
                    numberOfPacketsFilledToBrim--;
                    stageSize -= packetSize;
                }
                else if(dataLeftToBeDistributed) {

                    subUrb->UrbIsochronousTransfer.IsoPacket[j].Length =
                                        minDataInEachPacket + dataLeftToBeDistributed;
                    offset += (minDataInEachPacket + dataLeftToBeDistributed);
                    stageSize -= (minDataInEachPacket + dataLeftToBeDistributed);
                    dataLeftToBeDistributed = 0;

                }
                else {
                    subUrb->UrbIsochronousTransfer.IsoPacket[j].Length = minDataInEachPacket;
                    offset += minDataInEachPacket;
                    stageSize -= minDataInEachPacket;
                }
            }

            ASSERT(stageSize == 0);
        }

        // Associate the URB with the request.
        status = WdfUsbTargetPipeFormatRequestForUrb(pipe,
                                          subRequest,
                                          subUrbMemory,
                                          NULL );
        if (!NT_SUCCESS(status)) {
            PSDrv_DbgPrint(1, ("Failed to format requset for urb\n"));
            WdfObjectDelete(subRequest);
            IoFreeMdl(subMdl);
            goto Exit;
        }

        WdfRequestSetCompletionRoutine(subRequest,
                                        SubRequestCompletionRoutine,
                                        rwContext);
  
        if (WdfRequestSend(subRequest, WdfUsbTargetPipeGetIoTarget(pipe), WDF_NO_SEND_OPTIONS) == FALSE) {
            status = WdfRequestGetStatus(subRequest);
            PSDrv_DbgPrint(1, ("WdfRequestSend failed with status code 0x%x\n", status));
            WdfVerifierDbgBreakPoint(); // break into the debugger if the registry value is set.
            goto Exit;
        }
    }

Exit:
    // Checking the status besides the number of list entries will help differentiate
    // failures where everything succeeded vs where there were failures before adding
    // list entries.
    if(NT_SUCCESS(status)) {
        // We will let the completion routine to cleanup and complete the
        // main request.
		PSDrv_DbgPrint(3, ("PerformHighSpeedIsochTransfer -- ends 1\n"));

        return;
    }
    else {
        BOOLEAN  completeRequest;
        NTSTATUS tempStatus;

        completeRequest = TRUE;
        tempStatus = STATUS_SUCCESS;

        if (completeRequest) {
            if (rwContext->Numxfer > 0 ) {
                WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, rwContext->Numxfer);
            }
            else {
                WdfRequestCompleteWithInformation(Request, status, rwContext->Numxfer); 
            }
            
        }

    }

    PSDrv_DbgPrint(3, ("PerformHighSpeedIsochTransfer -- ends 2\n"));
    return;
}

/*++
Routine Description:

Completion Routine

Arguments:

Context - Driver supplied context
Target - Target handle
Request - Request handle
Params - request completion params

Return Value:
VOID
--*/
#define ISO_COMPLETE_EXTRA_DEBUG 1
VOID SubRequestCompletionRoutine(IN WDFREQUEST Request, IN WDFIOTARGET Target, PWDF_REQUEST_COMPLETION_PARAMS CompletionParams, IN WDFCONTEXT Context)
{
    PURB                    urb;
    ULONG                   i;
    NTSTATUS                status;
    PREQUEST_CONTEXT        rwContext;
    WDFREQUEST              mainRequest;
    PSUB_REQUEST_CONTEXT    subReqContext;
	PUCHAR					pDataBuffer;
	ULONG					nBufferPos = 0;
	ULONG					nPacketLength;
	ULONG					nPacketOffset;
#ifdef ISO_COMPLETE_EXTRA_DEBUG
	ULONG*					pnPktHeader0 = NULL;
	ULONG*					pnPktHeader1 = NULL;
#endif

    UNREFERENCED_PARAMETER(Target);

    PSDrv_DbgPrint(3, ("SubRequestCompletionRoutine - begins\n"));

    subReqContext = GetSubRequestContext(Request);

    urb = (PURB) subReqContext->SubUrb;

	rwContext = (PREQUEST_CONTEXT) Context;
	ASSERT(rwContext);

    status = CompletionParams->IoStatus.Status;

    if(NT_SUCCESS(status) && USBD_SUCCESS(urb->UrbHeader.Status))
	{
        rwContext->Numxfer += urb->UrbIsochronousTransfer.TransferBufferLength;

        PSDrv_DbgPrint(1, ("rwContext->Numxfer = %d\n", rwContext->Numxfer));
    }
    else
	{
        PSDrv_DbgPrint(1, ("read-write irp failed with status %X\n", status));
        PSDrv_DbgPrint(1, ("urb header status %X\n", urb->UrbHeader.Status));
    }

	// If we transfered some data, check if need to "repack" the buffer... (if the requested length is different then the actual returned length).
	if ((rwContext->TotalLength != rwContext->Numxfer) && (rwContext->Numxfer > 0) && (rwContext->Read == TRUE))
	{
		// Get a pointer to the data buffer
		pDataBuffer = (PUCHAR)MmGetSystemAddressForMdlSafe(subReqContext->SubMdl,NormalPagePriority);

		// Make sure it's a valid pointer...
		if(pDataBuffer!=NULL) 
		{
			PSDrv_DbgPrint(3, ("Repacking the data buffer:\n"));

			// Scan the packets
			for(i = 0; i < urb->UrbIsochronousTransfer.NumberOfPackets; i++)
			{

#ifdef ISO_COMPLETE_EXTRA_DEBUG
				// Check for packet errors
				if (urb->UrbIsochronousTransfer.IsoPacket[i].Status != 0)
				{
					PSDrv_DbgPrint(3, ("IsoPacketFailure[%d]: Length = %d, Offset = %d Status = %d\n", i, urb->UrbIsochronousTransfer.IsoPacket[i].Length, urb->UrbIsochronousTransfer.IsoPacket[i].Offset, urb->UrbIsochronousTransfer.IsoPacket[i].Status));
				}
#endif

				// Check if the packet has any data in it...
				nPacketLength = urb->UrbIsochronousTransfer.IsoPacket[i].Length;

				if (nPacketLength != 0)
				{
					nPacketOffset = urb->UrbIsochronousTransfer.IsoPacket[i].Offset;

#ifdef ISO_COMPLETE_EXTRA_DEBUG
					if (nPacketLength > 8)
					{
						pnPktHeader0 = (ULONG*)&pDataBuffer[nPacketOffset];
						pnPktHeader1 = (ULONG*)&pDataBuffer[nPacketOffset] + 1;
						PSDrv_DbgPrint(3, ("IsoPacket[%d]: Length = %d, Offset = %d -> %d (0x%08x 0x%08x)\n", i, nPacketLength, nPacketOffset, nBufferPos, *pnPktHeader0, *pnPktHeader1));
					}
					else
					{
						PSDrv_DbgPrint(3, ("IsoPacket[%d]: Length = %d, Offset = %d -> %d\n", i, nPacketLength, nPacketOffset, nBufferPos));
					}
#endif

					// Check that the we aren't copying into the exact same offset...
					if (nPacketOffset != nBufferPos)
					{
						// Repack the packet...
						RtlCopyMemory(&pDataBuffer[nBufferPos], &pDataBuffer[nPacketOffset], nPacketLength);
					}
#ifdef ISO_COMPLETE_EXTRA_DEBUG
					else
					{
						PSDrv_DbgPrint(3, ("Skipping RtlCopyMemory (same offset)...\n"));
					}
#endif

					nBufferPos += nPacketLength;
				}
			}	
		}
		else
		{
			PSDrv_DbgPrint(3, ("pDataBuffer is NULL...\n"));
		}
	}
	else
	{
#ifdef ISO_COMPLETE_EXTRA_DEBUG	
		for(i = 0; i < urb->UrbIsochronousTransfer.NumberOfPackets; i++) {

			PSDrv_DbgPrint(3, ("IsoPacket[%d].Length = %X IsoPacket[%d].Status = %X\n",
								i,
								urb->UrbIsochronousTransfer.IsoPacket[i].Length,
								i,
								urb->UrbIsochronousTransfer.IsoPacket[i].Status));
		}	
#endif	
	
		PSDrv_DbgPrint(3, ("No need to repack the data buffer...\n"));
	}

	// Free the MDL
	IoFreeMdl(subReqContext->SubMdl);
	subReqContext->SubMdl = NULL;


	// ZZZZZZZZ (Used to check if num pending requests is 0
	{
        PSDrv_DbgPrint(3, ("no more pending sub requests\n"));

        if(NT_SUCCESS(status))
		{
            PSDrv_DbgPrint(1, ("urb start frame %X\n", urb->UrbIsochronousTransfer.StartFrame));
        }

        mainRequest = WdfObjectContextGetObject(rwContext);

        // if we transferred some data, main Irp completes with success
        PSDrv_DbgPrint(1, ("Total data transferred = %X\n", rwContext->Numxfer));

        PSDrv_DbgPrint(3, ("SubRequestCompletionRoutine %s completed\n", rwContext->Read?"Read":"Write"));
        
        if (rwContext->Numxfer > 0 )
		{
            WdfRequestCompleteWithInformation(mainRequest, STATUS_SUCCESS, rwContext->Numxfer);
        }
        else
		{
            PSDrv_DbgPrint(3, ("SubRequestCompletionRoutine completed with failure status %x\n", CompletionParams->IoStatus.Status));

            WdfRequestCompleteWithInformation(mainRequest, CompletionParams->IoStatus.Status, rwContext->Numxfer); 
        }       
        
    }

    // Since we created the subrequests, we should free it by removing the reference.
    WdfObjectDelete(Request);

    PSDrv_DbgPrint(3, ("SubRequestCompletionRoutine - ends\n"));

    return;
}
