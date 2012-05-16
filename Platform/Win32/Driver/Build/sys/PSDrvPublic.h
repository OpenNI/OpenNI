/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
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
