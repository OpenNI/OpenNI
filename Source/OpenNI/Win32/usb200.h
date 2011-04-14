/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
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
#ifndef   __USB200_H__
#define   __USB200_H__

#include "usb100.h"


#include <PSHPACK1.H>


#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning (disable:4201)
#pragma warning(disable:4214) // named type definition in parentheses

typedef enum _USB_DEVICE_SPEED {
    UsbLowSpeed = 0,
    UsbFullSpeed,
    UsbHighSpeed
} USB_DEVICE_SPEED;

typedef enum _USB_DEVICE_TYPE {
    Usb11Device = 0,
    Usb20Device
} USB_DEVICE_TYPE;


// standard definitions for the port status
// word of the HUB port register

#define USB_PORT_STATUS_CONNECT         0x0001
#define USB_PORT_STATUS_ENABLE          0x0002
#define USB_PORT_STATUS_SUSPEND         0x0004
#define USB_PORT_STATUS_OVER_CURRENT    0x0008
#define USB_PORT_STATUS_RESET           0x0010
#define USB_PORT_STATUS_POWER           0x0100
#define USB_PORT_STATUS_LOW_SPEED       0x0200
#define USB_PORT_STATUS_HIGH_SPEED      0x0400

typedef union _BM_REQUEST_TYPE {
    struct _BM {
        UCHAR   Recipient:2;
        UCHAR   Reserved:3;
        UCHAR   Type:2;
        UCHAR   Dir:1;
    };
    UCHAR B;
} BM_REQUEST_TYPE, *PBM_REQUEST_TYPE;

typedef struct _USB_DEFAULT_PIPE_SETUP_PACKET {
    BM_REQUEST_TYPE bmRequestType;
    UCHAR bRequest;

    union _wValue {
        struct {
            UCHAR LowByte;
            UCHAR HiByte;
        };
        USHORT W;
    } wValue;

    union _wIndex {
        struct {
            UCHAR LowByte;
            UCHAR HiByte;
        };
        USHORT W;
    } wIndex;
    USHORT wLength;
} USB_DEFAULT_PIPE_SETUP_PACKET, *PUSB_DEFAULT_PIPE_SETUP_PACKET;

// setup packet is eight bytes -- defined by spec
C_ASSERT(sizeof(USB_DEFAULT_PIPE_SETUP_PACKET) == 8);


#define USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE            0x06
#define USB_OTHER_SPEED_CONFIGURATION_DESCRIPTOR_TYPE   0x07

typedef struct _USB_DEVICE_QUALIFIER_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT bcdUSB;
    UCHAR bDeviceClass;
    UCHAR bDeviceSubClass;
    UCHAR bDeviceProtocol;
    UCHAR bMaxPacketSize0;
    UCHAR bNumConfigurations;
    UCHAR bReserved;
} USB_DEVICE_QUALIFIER_DESCRIPTOR, *PUSB_DEVICE_QUALIFIER_DESCRIPTOR;


typedef union _USB_HIGH_SPEED_MAXPACKET {
    struct _MP {
        USHORT   MaxPacket:11;  /* 0..10 */
        USHORT   HSmux:2;        /* 11..12 */
        USHORT   Reserved:3;    /* 13..15 */
    };
    USHORT us;
  } USB_HIGH_SPEED_MAXPACKET, *PUSB_HIGH_SPEED_MAXPACKET;

#define USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE 0x0B

typedef struct _USB_INTERFACE_ASSOCIATION_DESCRIPTOR {

    UCHAR   bLength;
    UCHAR   bDescriptorType;
    UCHAR   bFirstInterface;
    UCHAR   bInterfaceCount;
    UCHAR   bFunctionClass;
    UCHAR   bFunctionSubClass;
    UCHAR   bFunctionProtocol;
    UCHAR   iFunction;

} USB_INTERFACE_ASSOCIATION_DESCRIPTOR, *PUSB_INTERFACE_ASSOCIATION_DESCRIPTOR;


#if _MSC_VER >= 1200
#pragma warning(pop)
#endif



#include <POPPACK.H>

#endif // __USB200_H__

