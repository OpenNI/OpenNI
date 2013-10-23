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
// Functions Declaration
//---------------------------------------------------------------------------
#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PSDrv_EvtDeviceAdd)
#pragma alloc_text(PAGE, PSDrv_EvtDevicePrepareHardware)
#pragma alloc_text(PAGE, ReadAndSelectDescriptors)
#pragma alloc_text(PAGE, ConfigureDevice)
#pragma alloc_text(PAGE, SelectInterfaces)
#pragma alloc_text(PAGE, SetPowerPolicy)
#pragma alloc_text(PAGE, AbortPipes)
#pragma alloc_text(PAGE, ReadFdoRegistryKeyValue)
#endif

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
/*++
Routine Description:

EvtDeviceAdd is called by the framework in response to AddDevice
call from the PnP manager. We create and initialize a device object to
represent a new instance of the device. All the software resources
should be allocated in this callback.

Arguments:

Driver - Handle to a framework driver object created in DriverEntry

DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

NTSTATUS
--*/
NTSTATUS PSDrv_EvtDeviceAdd(IN WDFDRIVER Driver, IN PWDFDEVICE_INIT DeviceInit)
{
    WDF_FILEOBJECT_CONFIG				fileConfig;
    WDF_PNPPOWER_EVENT_CALLBACKS        pnpPowerCallbacks;
    WDF_OBJECT_ATTRIBUTES               fdoAttributes;
    WDF_OBJECT_ATTRIBUTES               fileObjectAttributes;
    WDF_OBJECT_ATTRIBUTES               requestAttributes;
    NTSTATUS                            status;
    WDFDEVICE                           device;
    WDF_DEVICE_PNP_CAPABILITIES         pnpCaps;
    WDF_IO_QUEUE_CONFIG                 ioQueueConfig;
    PDEVICE_CONTEXT                     pDevContext;
    WDFQUEUE                            queue;
    ULONG                               maximumTransferSize;

    UNREFERENCED_PARAMETER(Driver);

    PSDrv_DbgPrint (3, ("PSDrv_EvtDeviceAdd - begins\n"));

    PAGED_CODE();

    // Initialize the pnpPowerCallbacks structure.  Callback events for PNP
    // and Power are specified here.  If you don't supply any callbacks,
    // the Framework will take appropriate default actions based on whether
    // DeviceInit is initialized to be an FDO, a PDO or a filter device
    // object.
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);

    pnpPowerCallbacks.EvtDevicePrepareHardware = PSDrv_EvtDevicePrepareHardware;

    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

    // Initialize the request attributes to specify the context size and type
    // for every request created by framework for this device.
    WDF_OBJECT_ATTRIBUTES_INIT(&requestAttributes);
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&requestAttributes, REQUEST_CONTEXT);

    WdfDeviceInitSetRequestAttributes(DeviceInit, &requestAttributes);

    // Initialize WDF_FILEOBJECT_CONFIG_INIT struct to tell the
    // framework whether you are interested in handle Create, Close and
    // Cleanup requests that gets generate when an application or another
    // kernel component opens an handle to the device. If you don't register
    // the framework default behaviour would be complete these requests
    // with STATUS_SUCCESS. A driver might be interested in registering these
    // events if it wants to do security validation and also wants to maintain
    // per handle (fileobject) context.
    WDF_FILEOBJECT_CONFIG_INIT(&fileConfig, PSDrv_EvtDeviceFileCreate, WDF_NO_EVENT_CALLBACK, WDF_NO_EVENT_CALLBACK);

    // Specify a context for FileObject. If you register FILE_EVENT callbacks,
    // the framework by default creates a framework FILEOBJECT corresponding
    // to the WDM fileobject. If you want to track any per handle context,
    // use the context for FileObject. Driver that typically use FsContext
    // field should instead use Framework FileObject context.
    WDF_OBJECT_ATTRIBUTES_INIT(&fileObjectAttributes);
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&fileObjectAttributes, FILE_CONTEXT);

    WdfDeviceInitSetFileObjectConfig(DeviceInit, &fileConfig, &fileObjectAttributes);

    // We can do iso transfer only if the io type is directio.
    WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoDirect);

    // Now specify the size of device extension where we track per device
    // context.DeviceInit is completely initialized. So call the framework
    // to create the device and attach it to the lower stack.
    WDF_OBJECT_ATTRIBUTES_INIT(&fdoAttributes);
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&fdoAttributes, DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit, &fdoAttributes, &device);
    if (!NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(1, ("WdfDeviceCreate failed! (Status = %x)\n", status));
        return status;
    }

    // Get the DeviceObject context by using accessory function specified in
    // the WDF_DECLARE_CONTEXT_TYPE_WITH_NAME macro for DEVICE_CONTEXT.
    pDevContext = GetDeviceContext(device);

    //Get MaximumTransferSize from registry
    maximumTransferSize=0;

    status = ReadFdoRegistryKeyValue(Driver, L"MaximumTransferSize", &maximumTransferSize);
    if (!NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(1, ("ReadFdoRegistryKeyValue failed with Status code %!STATUS!\n", status));
        return status;
    }

    if (maximumTransferSize)
	{
        pDevContext->MaximumTransferSize = maximumTransferSize;
    }
    else
	{
        pDevContext->MaximumTransferSize = DEFAULT_REGISTRY_TRANSFER_SIZE;
    }

    // Tell the framework to set the SurpriseRemovalOK in the DeviceCaps so
    // that you don't get the popup in usermode (on Win2K) when you surprise
    // remove the device.
    WDF_DEVICE_PNP_CAPABILITIES_INIT(&pnpCaps);

	// Do not show the little green remove icon
    pnpCaps.SurpriseRemovalOK = WdfTrue;

/*
	// Show the little green remove icon...
	pnpCaps.SurpriseRemovalOK = WdfFalse;
	pnpCaps.Removable = WdfTrue;
*/

    WdfDeviceSetPnpCapabilities(device, &pnpCaps);

    // Register I/O callbacks to tell the framework that you are interested
    // in handling WdfRequestTypeRead, WdfRequestTypeWrite, and IRP_MJ_DEVICE_CONTROL requests.
    // WdfIoQueueDispatchParallel means that we are capable of handling
    // all the I/O request simultaneously and we are responsible for protecting
    // data that could be accessed by these callbacks simultaneously.
    // This queue will be,  by default,  auto managed by the framework with
    // respect to PNP and Power events. That is, framework will take care
    // of queuing, failing, dispatching incoming requests based on the current
    // pnp/power state of the device.
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig, WdfIoQueueDispatchParallel);

    ioQueueConfig.EvtIoRead = PSDrv_EvtIoRead;
    ioQueueConfig.EvtIoWrite = PSDrv_EvtIoWrite;
    ioQueueConfig.EvtIoDeviceControl = PSDrv_EvtIoDeviceControl;
    ioQueueConfig.EvtIoStop = PSDrvEvtIoStop;
    ioQueueConfig.EvtIoResume = PSDrvEvtIoResume;

    status = WdfIoQueueCreate(device, &ioQueueConfig, WDF_NO_OBJECT_ATTRIBUTES, &queue);
    if (!NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(1, ("WdfIoQueueCreate failed! (Status = %x)\n", status));
        return status;
    }

    // Register a device interface so that app can find our device and talk to it.
    status = WdfDeviceCreateDeviceInterface(device, (LPGUID)&GUID_CLASS_PSDRV_USB, NULL);
    if (!NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(1, ("WdfDeviceCreateDeviceInterface failed! (Status = %x)\n", status));
        return status;
    }

    PSDrv_DbgPrint(3, ("PSDrv_EvtDeviceAdd - ends\n"));

    return status;
}

/*++
Routine Description:

In this callback, the driver does whatever is necessary to make the
hardware ready to use.  In the case of a USB device, this involves
reading and selecting descriptors.
--*/
NTSTATUS PSDrv_EvtDevicePrepareHardware(IN WDFDEVICE Device, IN WDFCMRESLIST ResourceList, IN WDFCMRESLIST ResourceListTranslated)
{
    NTSTATUS                    status;
    PDEVICE_CONTEXT             pDeviceContext;
    WDF_USB_DEVICE_INFORMATION  info;

    UNREFERENCED_PARAMETER(ResourceList);
    UNREFERENCED_PARAMETER(ResourceListTranslated);

    PSDrv_DbgPrint(3, ("PSDrv_EvtDevicePrepareHardware - begins\n"));

    PAGED_CODE();

    pDeviceContext = GetDeviceContext(Device);

    // Read the device descriptor, configuration descriptor and select the interface descriptors
    status = ReadAndSelectDescriptors(Device);

    if (!NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(1, ("ReadAndSelectDescriptors failed! (Status = %x)\n", status));
        return status;
    }

    WDF_USB_DEVICE_INFORMATION_INIT(&info);

    // Retrieve USBD version information, port driver capabilities and device capabilities such as speed, power, etc.
    status = WdfUsbTargetDeviceRetrieveInformation(pDeviceContext->WdfUsbTargetDevice, &info);
    if (NT_SUCCESS(status))
	{
        pDeviceContext->IsDeviceHighSpeed = (info.Traits & WDF_USB_DEVICE_TRAIT_AT_HIGH_SPEED) ? TRUE : FALSE;

        PSDrv_DbgPrint(3, ("DeviceIsHighSpeed: %s\n", pDeviceContext->IsDeviceHighSpeed ? "TRUE" : "FALSE"));
    }
	else
	{
        pDeviceContext->IsDeviceHighSpeed = FALSE;
    }

    PSDrv_DbgPrint(3, ("IsDeviceSelfPowered: %s\n", (info.Traits & WDF_USB_DEVICE_TRAIT_SELF_POWERED) ? "TRUE" : "FALSE"));

    pDeviceContext->WaitWakeEnable = info.Traits & WDF_USB_DEVICE_TRAIT_REMOTE_WAKE_CAPABLE;

    PSDrv_DbgPrint(3, ("IsDeviceRemoteWakeable: %s\n", (info.Traits & WDF_USB_DEVICE_TRAIT_REMOTE_WAKE_CAPABLE) ? "TRUE" : "FALSE"));

    // Enable wait-wake and idle timeout if the device supports it
    if(pDeviceContext->WaitWakeEnable)
	{
        status = SetPowerPolicy(Device);
        if (!NT_SUCCESS (status))
		{
            PSDrv_DbgPrint(3, ("SetPowerPolicy failed! (Status = %x)\n", status));
            return status;
        }
    }

    PSDrv_DbgPrint(3, ("PSDrv_EvtDevicePrepareHardware - ends\n"));

    return status;
}

NTSTATUS SetPowerPolicy(__in WDFDEVICE Device)
{
    WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS idleSettings;
    WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS wakeSettings;
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    // Init the idle policy structure.
    WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT(&idleSettings, IDLE_CAPS_TYPE);
    idleSettings.IdleTimeout = 10000; // 10-sec

    status = WdfDeviceAssignS0IdleSettings(Device, &idleSettings);
    if (!NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(3, ("WdfDeviceAssignS0IdleSettings failed! (Status = %x)\n", status));
        return status;
    }

    // Init wait-wake policy structure.
    WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_INIT(&wakeSettings);

    status = WdfDeviceAssignSxWakeSettings(Device, &wakeSettings);
    if (!NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(3, ("WdfDeviceAssignSxWakeSettings failed! (Status = %x)\n", status));
        return status;
    }

    return status;
}


/*++
Routine Description:

This routine configures the USB device.
In this routines we get the device descriptor,
the configuration descriptor and select the
configuration.

Arguments:

Device - Handle to a framework device

Return Value:

NTSTATUS - NT status value.
--*/
NTSTATUS ReadAndSelectDescriptors(IN WDFDEVICE Device)
{
    NTSTATUS               status;
    PDEVICE_CONTEXT        pDeviceContext;

    PAGED_CODE();

    // initialize variables
    pDeviceContext = GetDeviceContext(Device);

    // Create a USB device handle so that we can communicate with the
    // underlying USB stack. The WDFUSBDEVICE handle is used to query,
    // configure, and manage all aspects of the USB device.
    // These aspects include device properties, bus properties,
    // and I/O creation and synchronization. We only create device the first
    // the PrepareHardware is called. If the device is restarted by pnp manager
    // for resource re-balance, we will use the same device handle but then select
    // the interfaces again because the USB stack could reconfigure the device on
    // restart.
    if (pDeviceContext->WdfUsbTargetDevice == NULL)
	{
        status = WdfUsbTargetDeviceCreate(Device, WDF_NO_OBJECT_ATTRIBUTES, &pDeviceContext->WdfUsbTargetDevice);
        if (!NT_SUCCESS(status))
		{
			PSDrv_DbgPrint(3, ("WdfUsbTargetDeviceCreate failed! (Status = %x)\n", status));
            return status;
        }
    }
    
    WdfUsbTargetDeviceGetDeviceDescriptor(pDeviceContext->WdfUsbTargetDevice, &pDeviceContext->UsbDeviceDescriptor);

    ASSERT(pDeviceContext->UsbDeviceDescriptor.bNumConfigurations);

    status = ConfigureDevice(Device);

    return status;
}

/*++
Routine Description:

This helper routine reads the configuration descriptor
for the device in couple of steps.

Arguments:

Device - Handle to a framework device

Return Value:

NTSTATUS - NT status value
--*/
NTSTATUS ConfigureDevice(IN WDFDEVICE Device)
{
    USHORT                        size = 0;
    NTSTATUS                      status;
    PDEVICE_CONTEXT               pDeviceContext;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor;
    WDF_OBJECT_ATTRIBUTES		  attributes;
    WDFMEMORY                     memory;

    PAGED_CODE();

    //
    // initialize the variables
    //
    configurationDescriptor = NULL;
    pDeviceContext = GetDeviceContext(Device);

    // Read the first configuration descriptor
    // This requires two steps:
    // 1. Ask the WDFUSBDEVICE how big it is
    // 2. Allocate it and get it from the WDFUSBDEVICE
    status = WdfUsbTargetDeviceRetrieveConfigDescriptor(pDeviceContext->WdfUsbTargetDevice, NULL, &size);

    if (status != STATUS_BUFFER_TOO_SMALL || size == 0)
	{
		PSDrv_DbgPrint(3, ("WdfUsbTargetDeviceRetrieveConfigDescriptor failed! (Status = %x)\n", status));
        return status;
    }

    // Create a memory object and specify usbdevice as the parent so that it will be freed automatically.
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);

    attributes.ParentObject = pDeviceContext->WdfUsbTargetDevice;

    status = WdfMemoryCreate(&attributes, NonPagedPool, POOL_TAG, size, &memory, &configurationDescriptor);
    if (!NT_SUCCESS(status))
	{
		PSDrv_DbgPrint(3, ("WdfMemoryCreate for configurationDescriptor failed! (Status = %x)\n", status));
        return status;
    }

    status = WdfUsbTargetDeviceRetrieveConfigDescriptor(pDeviceContext->WdfUsbTargetDevice, configurationDescriptor, &size);
    if (!NT_SUCCESS(status))
	{
		PSDrv_DbgPrint(3, ("WdfUsbTargetDeviceRetrieveConfigDescriptor failed! (Status = %x)\n", status));
        return status;
    }

    pDeviceContext->UsbConfigurationDescriptor = configurationDescriptor;

    status = SelectInterfaces(Device);

    return status;
}

/*++
Routine Description:

This helper routine selects the configuration, interface and
creates a context for every pipe (end point) in that interface.

Arguments:

Device - Handle to a framework device

Return Value:

NT status value
--*/
NTSTATUS SelectInterfaces(IN WDFDEVICE Device)
{
    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS configParams;
    NTSTATUS                            status;
    PDEVICE_CONTEXT                     pDeviceContext;
	UCHAR								numInterfaces;

    PAGED_CODE();

    pDeviceContext = GetDeviceContext(Device);

	numInterfaces = WdfUsbTargetDeviceGetNumInterfaces(pDeviceContext->WdfUsbTargetDevice);

	if (numInterfaces == 1)
	{
		// The device has only 1 interface.
		PSDrv_DbgPrint(3, ("Device has a single interface!\n"));

		WDF_USB_DEVICE_SELECT_CONFIG_PARAMS_INIT_SINGLE_INTERFACE(&configParams);

		status = WdfUsbTargetDeviceSelectConfig(pDeviceContext->WdfUsbTargetDevice, WDF_NO_OBJECT_ATTRIBUTES, &configParams);
		if (NT_SUCCESS(status) && numInterfaces > 0)
		{
			pDeviceContext->UsbInterface = configParams.Types.SingleInterface.ConfiguredUsbInterface;
			pDeviceContext->NumberConfiguredPipes = configParams.Types.SingleInterface.NumberConfiguredPipes;
		}
	}
	else
	{
		// The device has more then one interface...
		PSDrv_DbgPrint(3, ("Device has multiple interfaces (%d) - this is not supported yet!\n", numInterfaces));
		status = STATUS_NOT_SUPPORTED;
	}

	pDeviceContext->nCurrIf = 0;
	pDeviceContext->nCurrAltIf = 0;

    return status;
}

/*++
Routine Description

sends an abort pipe request on all open pipes.

Arguments:

Device - Handle to a framework device

Return Value:

NT status value
--*/
NTSTATUS AbortPipes(IN WDFDEVICE Device)
{
    UCHAR              i;
    ULONG              count;
    NTSTATUS           status;
    PDEVICE_CONTEXT    pDevContext;

    PAGED_CODE();

    // initialize variables
    pDevContext = GetDeviceContext(Device);

    PSDrv_DbgPrint(3, ("AbortPipes - begins\n"));

    count = pDevContext->NumberConfiguredPipes;
    for (i = 0; i < count; i++)
	{
        WDFUSBPIPE pipe;
        pipe = WdfUsbInterfaceGetConfiguredPipe(pDevContext->UsbInterface, i, NULL);

		PSDrv_DbgPrint(3, ("Aborting open pipe: %d\n", i));

        status = WdfUsbTargetPipeAbortSynchronously(pipe, WDF_NO_HANDLE, NULL);

        if (!NT_SUCCESS(status))
		{
            PSDrv_DbgPrint(1, ("WdfUsbTargetPipeAbortSynchronously failed! (Status = %x)\n", status));
            break;
        }
    }

    PSDrv_DbgPrint(3, ("AbortPipes - ends\n"));

    return STATUS_SUCCESS;
}

/*++
Routine Description:

Can be used to read any REG_DWORD registry value stored
under Device Parameter.

Arguments:

Driver - pointer to the device object
Name - Name of the registry value
Value - Value...
--*/
NTSTATUS ReadFdoRegistryKeyValue(__in  WDFDRIVER Driver, __in LPWSTR Name, __out PULONG Value)
{
    WDFKEY      hKey = NULL;
    NTSTATUS    status;
    UNICODE_STRING  valueName;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    *Value = 0;

    status = WdfDriverOpenParametersRegistryKey(WdfGetDriver(), KEY_READ, WDF_NO_OBJECT_ATTRIBUTES, &hKey);
    if (NT_SUCCESS(status))
	{
        RtlInitUnicodeString(&valueName,Name);

        status = WdfRegistryQueryULong (hKey, &valueName, Value);

        WdfRegistryClose(hKey);
    }

    return status;
}


/*++
Routine Description:

This callback is invoked for every request pending in the driver - in-flight
request - to notify that the hardware is ready for continuing the processing
of the request.

Arguments:

Queue - Queue the request currently belongs to
Request - Request that is currently out of queue and being processed by the driver

Return Value:

None.
--*/
VOID PSDrvEvtIoResume( IN WDFQUEUE   Queue, IN WDFREQUEST Request)
{
	UNREFERENCED_PARAMETER(Queue);
	UNREFERENCED_PARAMETER(Request);

	// Leave this function here for now in case we want to add anything in the future
}

/*++
Routine Description:

This callback is invoked on every inflight request when the device
is suspended or removed. Since our inflight read and write requests
are actually pending in the target device, we will just acknowledge
its presence. Until we acknowledge, complete, or requeue the requests
framework will wait before allowing the device suspend or remove to
proceed. When the underlying USB stack gets the request to suspend or
remove, it will fail all the pending requests.

Arguments:

Return Value:
None
--*/
VOID PSDrvEvtIoStop(__in WDFQUEUE Queue, __in WDFREQUEST Request, __in ULONG ActionFlags)
{
	PREQUEST_CONTEXT reqContext;

	UNREFERENCED_PARAMETER(Queue);

	reqContext=GetRequestContext(Request);

	if (ActionFlags & WdfRequestStopActionSuspend ) {
		WdfRequestStopAcknowledge(Request, FALSE); // Don't requeue
	} else if(ActionFlags & WdfRequestStopActionPurge) {
		WdfRequestCancelSentRequest(Request);
	}
	return;
}
