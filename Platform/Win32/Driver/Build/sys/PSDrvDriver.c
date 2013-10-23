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
// Globals
//---------------------------------------------------------------------------
ULONG g_DebugLevel = GLOBAL_DEBUG_LEVEL;

//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#endif

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
/*++
Routine Description:
DriverEntry initializes the driver and is the first routine called by the
system after the driver is loaded.

Parameters Description:

DriverObject - represents the instance of the function driver that is loaded
into memory. DriverEntry must initialize members of DriverObject before it
returns to the caller. DriverObject is allocated by the system before the
driver is loaded, and it is released by the system after the system unloads
the function driver from memory.

RegistryPath - represents the driver specific path in the Registry.
The function driver can use the path to store driver related data between
reboots. The path does not store hardware instance specific data.

Return Value:

STATUS_SUCCESS if successful,
STATUS_UNSUCCESSFUL otherwise.
--*/
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    WDF_DRIVER_CONFIG       config;
    NTSTATUS                status;

    PSDrv_DbgPrint(3, ("Prime Sensor Driver v%s - DebEgg Edition.\n", PSDRV_BRIEF_VERSION_STRING));
    PSDrv_DbgPrint(3, ("Built %s %s\n", __DATE__, __TIME__));

    // Initialize driver config to control the attributes that
    // are global to the driver. Note that framework by default
    // provides a driver unload routine. If you create any resources
    // in the DriverEntry and want to be cleaned in driver unload,
    // you can override that by manually setting the EvtDriverUnload in the
    // config structure. In general xxx_CONFIG_INIT macros are provided to
    // initialize most commonly used members.
    WDF_DRIVER_CONFIG_INIT(&config, PSDrv_EvtDeviceAdd);

    // Create a framework driver object to represent our driver.
    status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);
    if (!NT_SUCCESS(status))
	{
        PSDrv_DbgPrint(1, ("WdfDriverCreate failed! (Status = %x)\n", status));
    }

    return status;
}
