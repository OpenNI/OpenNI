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
#ifndef _H_PSDRV_PUBLIC_H_
#define _H_PSDRV_PUBLIC_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <initguid.h>

//---------------------------------------------------------------------------
// GUIDs
//---------------------------------------------------------------------------

// c3b5f022-5a42-1980-1909-ea7209560
DEFINE_GUID(GUID_CLASS_PSDRV_USB, 0xc3b5f022, 0x5a42, 0x1980, 0x19, 0x09, 0xea, 0x72, 0x09, 0x56, 0x01, 0xb1);

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define PSUSBDRV_PIPE_PROPERTY_TIMEOUT 1

#define PSUSBDRV_DEFAULT_TIMEOUT 1000
#define PSUSBDRV_DEFAULT_CONTROL_TIMEOUT 5000

//---------------------------------------------------------------------------
// Structs & Enums
//---------------------------------------------------------------------------
#pragma pack (push, 1)

typedef enum PSUSBDRV_CONTROL_REQUEST_DIRECTION
{
	RequestHostToDevice = 0,
	RequestDeviceToHost,
} PSUSBDRV_CONTROL_REQUEST_DIRECTION;

typedef enum PSUSBDRV_CONTROL_REQUEST_TYPE
{
	RequestStandard = 0,
	RequestClass,
	RequestVendor,
} PSUSBDRV_CONTROL_REQUEST_TYPE;

typedef enum PSUSBDRV_DEVICE_SPEED
{
	PSUSBDRV_DEVICE_LOW_SPEED = 0,
	PSUSBDRV_DEVICE_FULL_SPEED,
	PSUSBDRV_DEVICE_HIGH_SPEED,
} PSUSBDRV_DEVICE_SPEED;

typedef struct PSUSBDRV_CONTROL_TRANSFER
{
	unsigned char  cDirection;
	unsigned char  cRequestType;
	unsigned char  cRequest;
	unsigned short nValue;
	unsigned short nIndex;
	unsigned int   nTimeout;
} PSUSBDRV_CONTROL_TRANSFER;

typedef struct PSUSBDRV_PIPE_PROPERTY
{
	unsigned int nIndex;
	unsigned int nValue;
} PSUSBDRV_PIPE_PROPERTY;

typedef struct PSUSBDRV_DRIVER_VERSION
{
	unsigned int nMajor;
	unsigned int nMinor;
	unsigned int nMaintenance;
	unsigned int nBuild;
} PSUSBDRV_DRIVER_VERSION;

typedef struct PSUSBDRV_INTERFACE_PROPERTY
{
	unsigned char nIF;
	unsigned char nAltIF;
} PSUSBDRV_INTERFACE_PROPERTY;

#pragma pack (pop)

//---------------------------------------------------------------------------
// IOCTLs
//---------------------------------------------------------------------------
#define IOCTL_INDEX             0x0000

#define IOCTL_PSDRV_GET_CONFIG_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,     \
												   IOCTL_INDEX + 0,			\
                                                   METHOD_BUFFERED,         \
                                                   FILE_ANY_ACCESS)
                                                   
#define IOCTL_PSDRV_RESET_DEVICE          CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                   IOCTL_INDEX + 1,			\
                                                   METHOD_BUFFERED,         \
                                                   FILE_ANY_ACCESS)

#define IOCTL_PSDRV_RESET_PIPE            CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                   IOCTL_INDEX + 2,			\
                                                   METHOD_BUFFERED,         \
                                                   FILE_ANY_ACCESS)

#define IOCTL_PSDRV_ABORT_PIPE            CTL_CODE(FILE_DEVICE_UNKNOWN,     \
												   IOCTL_INDEX + 3,			\
												   METHOD_BUFFERED,         \
												   FILE_ANY_ACCESS)

#define IOCTL_PSDRV_CONTROL_TRANSFER      CTL_CODE(FILE_DEVICE_UNKNOWN,     \
												   IOCTL_INDEX + 4,			\
	                                               METHOD_OUT_DIRECT,       \
	                                               FILE_ANY_ACCESS)

#define IOCTL_PSDRV_SET_PIPE_PROPERTY     CTL_CODE(FILE_DEVICE_UNKNOWN,     \
												   IOCTL_INDEX + 5,			\
												   METHOD_BUFFERED,         \
												   FILE_ANY_ACCESS)

#define IOCTL_PSDRV_GET_DRIVER_VERSION    CTL_CODE(FILE_DEVICE_UNKNOWN,     \
												   IOCTL_INDEX + 6,			\
												   METHOD_BUFFERED,         \
												   FILE_ANY_ACCESS)

#define IOCTL_PSDRV_GET_DEVICE_SPEED    CTL_CODE(FILE_DEVICE_UNKNOWN,		\
												   IOCTL_INDEX + 7,			\
												   METHOD_BUFFERED,         \
												   FILE_ANY_ACCESS)

#define IOCTL_PSDRV_SET_INTERFACE		CTL_CODE(FILE_DEVICE_UNKNOWN,		\
												   IOCTL_INDEX + 8,			\
												   METHOD_BUFFERED,         \
												   FILE_ANY_ACCESS)

#define IOCTL_PSDRV_GET_INTERFACE		CTL_CODE(FILE_DEVICE_UNKNOWN,		\
												   IOCTL_INDEX + 9,			\
												   METHOD_BUFFERED,         \
												   FILE_ANY_ACCESS)

#endif //_H_PSDRV_PUBLIC_H_
