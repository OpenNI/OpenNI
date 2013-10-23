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
#include <XnPlatform.h>

#if (XN_PLATFORM == XN_PLATFORM_LINUX_ARM)

#include <XnUSBDevice.h>
#include <linux/usb/gadgetfs.h>
#include <asm/byteorder.h>
#include <poll.h>
#include <errno.h>
#include <XnLog.h>
#include <XnOSCpp.h>
#include <XnDump.h>
#include <aio.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define GADGET_DEVICE_DIR "/dev/gadget/"
#define GADGET_DEVICE_FILE_NAME "musb_hdrc"
#define GADGET_DEVICE_FILE_PATH GADGET_DEVICE_DIR GADGET_DEVICE_FILE_NAME
#define USB_LANGUAGE_ENGLISH_US 0x0409
#define XN_USB_DEVICE_ENDPOINT_MAX_COUNT 16
#define XN_USB_CONTROL_TIMEOUT 5000
#define XN_USB_WRITE_DATA_TIMEOUT 2000
#define XN_USB_MAX_WRITE_BUFFER_MULTIPLIER 100
#define XN_USB_MAX_WRITE_TX 20

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define WRITE_TO_BUF(buf, data, size)	\
	memcpy(buf, data, size);			\
	buf += size;

#define WRITE_OBJ_TO_BUF(buf, obj)		\
	WRITE_TO_BUF(buf, &obj, sizeof(obj));

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef enum
{
	/** Control is idle **/
	DEVICE_CONTROL_CLEAR,
	/** Control request was received **/
	DEVICE_CONTROL_REQUEST_RECEIVED,
	/** Control request was read by device, no reply yet **/
	DEVICE_CONTROL_REQUEST_READ,
	/** Control reply received, waiting for host in-request **/
	DEVICE_CONTROL_REPLY_READY,
} DeviceControlState;

typedef enum
{
	/** Control is idle **/
	HOST_CONTROL_CLEAR,
	/** Control request was received **/
	HOST_CONTROL_REQUEST_RECEIVED,
	/** Control in-request was received, waiting for device to have reply **/
	HOST_CONTROL_WAITING_FOR_REPLY,
} HostControlState;

typedef struct XnUSBDeviceEndpointWriteTx
{
	struct aiocb cb;
	XnUChar* pBuffer;
} XnUSBDeviceEndpointWriteTx;

typedef struct XnUSBDeviceEndpoint
{
	int fd;
	XnUSBDeviceEndpointWriteTx txs[XN_USB_MAX_WRITE_TX];
	XnUInt32 nFirst;
	XnUInt32 nQueued;
	XnUInt32 nBufferSize;
} XnUSBDeviceEndpoint;

struct XnUSBDevice
{
	const XnUSBDeviceDescriptorHolder* pDescriptors;
	int deviceFD;
	XnBool bShutdown;
	XN_THREAD_HANDLE hThread;
	XN_CRITICAL_SECTION_HANDLE hLock;
	XN_EVENT_HANDLE hReplyEvent;
	XnUSBDeviceConnectionState connectionState;
	enum usb_device_speed speed;
	XnUInt32 nControlMessageMaxSize;
	XnUInt8 nConfigID;
	XnUInt8 nInterfaceID;
	XnUInt8 nAltInterfaceID;
	XnUSBDeviceEndpoint endpoints[XN_USB_DEVICE_ENDPOINT_MAX_COUNT];

	// all control members should be accessed with a lock. They can be modified either from the user thread
	// or the ep0 thread.
	DeviceControlState eDeviceControlState;
	HostControlState eHostControlState;
	XnUChar* pControlBuffer;
	XnUInt32 nControlSize;
	XnUSBDeviceNewControlRequestCallback pNewControlRequestCallback;
	void* pNewControlRequestCallbackCookie;
	XnUSBDeviceConnectivityChangedCallback pConnectivityChangedCallback;
	void* pConnectivityChangedCallbackCookie;
	XnDumpFile* pDump;
};

//---------------------------------------------------------------------------
// Utilities
//---------------------------------------------------------------------------
const XnChar* GetDeviceStateName(DeviceControlState state)
{
	static const XnChar* aNames[] = { "Clear", "RequestReceived", "RequestRead", "ReplyReady" };
	return aNames[state];
}

const XnChar* GetHostStateName(HostControlState state)
{
	static const XnChar* aNames[] = { "Clear", "RequestReceived", "WaitingForReply" };
	return aNames[state];
}

void SetConnectivityState(XnUSBDevice* pDevice, XnUSBDeviceConnectionState state)
{
	XnAutoCSLocker locker(pDevice->hLock);
	pDevice->connectionState = state;
	if (pDevice->pConnectivityChangedCallback != NULL)
	{
		pDevice->pConnectivityChangedCallback(pDevice, state, pDevice->pConnectivityChangedCallbackCookie);
	}
}

//---------------------------------------------------------------------------
// Configuring GadgetFS Device
//---------------------------------------------------------------------------
static XnStatus buildGadgetFSInterfaceDescriptor(const XnUSBInterfaceDescriptorHolder* pInterface, XnChar*& buf)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// write interface descriptor
	WRITE_OBJ_TO_BUF(buf, pInterface->descriptor);
	
	// now write each endpoint
	for (__u8 iEP = 0; iEP < pInterface->descriptor.bNumEndpoints; ++iEP)
	{
		WRITE_TO_BUF(buf, pInterface->aEndpoints[iEP], USB_DT_ENDPOINT_SIZE);
	}
	
	return XN_STATUS_OK;	
}

static XnStatus buildGadgetFSConfigDescriptor(const XnUSBConfigDescriptorHolder* pConfig, XnChar*& buf)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	struct usb_config_descriptor* pTarget = (struct usb_config_descriptor*)buf;

	// write configuration descriptor
	WRITE_OBJ_TO_BUF(buf, pConfig->descriptor);
	
	// for now, gadget FS supports a single interface
	if (pConfig->descriptor.bNumInterfaces > 1)
	{
		xnLogError(XN_MASK_OS, "GadgetFS does not support more than 1 interface!");
		return XN_STATUS_INVALID_OPERATION;
	}

	// now write the interface
	buildGadgetFSInterfaceDescriptor(pConfig->aInterfaces[0], buf);
	
	pTarget->wTotalLength = buf - (XnChar*)pTarget;
	
	return XN_STATUS_OK;	
}

static XnStatus buildGadgetFSDescriptors(const XnUSBDeviceDescriptorHolder* pDescriptors, XnChar*& buf)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// write format ID
	XnUInt32 nFormatID = 0;
	WRITE_OBJ_TO_BUF(buf, nFormatID);
	
	// for now, gadget FS supports a single configuration
	if (pDescriptors->descriptor.bNumConfigurations > 1)
	{
		xnLogError(XN_MASK_OS, "GadgetFS does not support more than 1 configuration!");
		return XN_STATUS_INVALID_OPERATION;
	}
	
	XN_VALIDATE_INPUT_PTR(pDescriptors->aConfigurations);
	XN_VALIDATE_INPUT_PTR(pDescriptors->aConfigurations[0]);
	
	// we first write the full-speed one (backwards compatibility) and then the high-speed one
	buildGadgetFSConfigDescriptor(pDescriptors->aConfigurations[0], buf);
	buildGadgetFSConfigDescriptor(pDescriptors->aConfigurations[0], buf);
	
	// write device
	WRITE_OBJ_TO_BUF(buf, pDescriptors->descriptor);
    
    return nRetVal;
}

//---------------------------------------------------------------------------
// Configuring GadgetFS Endpoints
//---------------------------------------------------------------------------
static int openEndpointFile(struct usb_endpoint_descriptor* pDesc)
{
	// build file name
	XnChar fileName[255];
	bool bIn = (pDesc->bEndpointAddress & 0x80) != 0;
	sprintf(fileName, "%s%s%d%s", GADGET_DEVICE_DIR, "ep", pDesc->bEndpointAddress & 0xF, bIn ? "in" : "out");
	
	// open it
	int fd = open(fileName, O_RDWR);
	
	// config it
	XnUChar bufConfig[1024];
	XnUChar* buf = bufConfig;
	
	XnUInt32 nFormatID = 1;
	WRITE_OBJ_TO_BUF(buf, nFormatID);
	
	// now we should write the full-speed descriptor. Take high-speed one and reduce speed
	struct usb_endpoint_descriptor* pFSDesc = (struct usb_endpoint_descriptor*)buf;

	WRITE_TO_BUF(buf, pDesc, USB_DT_ENDPOINT_SIZE);
	
	// now write the real one (high-speed)
	WRITE_TO_BUF(buf, pDesc, USB_DT_ENDPOINT_SIZE);
	
	int status = write(fd, bufConfig, buf - bufConfig);
	if (status < 0)
	{
		xnLogError(XN_MASK_OS, "Failed to write endpoint descriptors (%d - %s)\n", errno, strerror(errno));
		close (fd);
		return -1;
	}
	
	return fd;
}

static XnBool configureEndpoints(XnUSBDevice* pDevice, int nConfigID)
{
	// first of all, we need to close all previous open endpoints
	for (int i = 0; i < XN_USB_DEVICE_ENDPOINT_MAX_COUNT; ++i)
	{
		if (pDevice->endpoints[i].fd != -1)
		{
			close(pDevice->endpoints[i].fd);
			pDevice->endpoints[i].fd = -1;

			for (int j = 0; j < XN_USB_MAX_WRITE_TX; ++j)
			{
				xnOSFreeAligned(pDevice->endpoints[i].txs[j].pBuffer);
			}
		}
	}
	
	if (nConfigID == 0)
	{
		// device is unconfigured
		return TRUE;
	}

	XnUSBInterfaceDescriptorHolder* pInterface = pDevice->pDescriptors->aConfigurations[0]->aInterfaces[0];
	for (int i = 0; i < pInterface->descriptor.bNumEndpoints; ++i)
	{
		// open endpoint file
		__u8 nAddress = pInterface->aEndpoints[i]->bEndpointAddress & 0xF;
		pDevice->endpoints[nAddress].fd = openEndpointFile(pInterface->aEndpoints[i]);
		if (pDevice->endpoints[nAddress].fd == -1)
		{
			return FALSE;
		}

		xnOSMemSet(pDevice->endpoints[nAddress].txs, 0, sizeof(pDevice->endpoints[nAddress].txs));

		pDevice->endpoints[nAddress].nBufferSize = pInterface->aEndpoints[i]->wMaxPacketSize * XN_USB_MAX_WRITE_BUFFER_MULTIPLIER;

		// create write buffers
		for (int j = 0; j < XN_USB_MAX_WRITE_TX; ++j)
		{
			pDevice->endpoints[nAddress].txs[j].pBuffer = (XnUChar*)xnOSMallocAligned(pDevice->endpoints[nAddress].nBufferSize, XN_DEFAULT_MEM_ALIGN);
			if (pDevice->endpoints[nAddress].txs[j].pBuffer == NULL)
			{
				return FALSE;
			}
		}
	}
	
	return TRUE;
}

//---------------------------------------------------------------------------
// EP0 Handling
//---------------------------------------------------------------------------
static XnBool handleVendorControl(XnUSBDevice* pDevice, struct usb_ctrlrequest *setup);

static inline void put_unaligned_le16(__u16 val, __u16 *cp)
{
	__u8	*p = (__u8 *)cp;

	*p++ = (__u8) val;
	*p++ = (__u8) (val >> 8);
}

static int utf8_to_utf16le(const char *s, __u16 *cp, unsigned len)
{
	int	count = 0;
	__u8	c;
	__u16	uchar;

	/* this insists on correct encodings, though not minimal ones.
	 * BUT it currently rejects legit 4-byte UTF-8 code points,
	 * which need surrogate pairs.  (Unicode 3.1 can use them.)
	 */
	while (len != 0 && (c = (__u8) *s++) != 0) {
		if (c & 0x80) {
			// 2-byte sequence:
			// 00000yyyyyxxxxxx = 110yyyyy 10xxxxxx
			if ((c & 0xe0) == 0xc0) {
				uchar = (c & 0x1f) << 6;

				c = (__u8) *s++;
				if ((c & 0xc0) != 0xc0)
					goto fail;
				c &= 0x3f;
				uchar |= c;

			// 3-byte sequence (most CJKV characters):
			// zzzzyyyyyyxxxxxx = 1110zzzz 10yyyyyy 10xxxxxx
			} else if ((c & 0xf0) == 0xe0) {
				uchar = (c & 0x0f) << 12;

				c = (__u8) *s++;
				if ((c & 0xc0) != 0xc0)
					goto fail;
				c &= 0x3f;
				uchar |= c << 6;

				c = (__u8) *s++;
				if ((c & 0xc0) != 0xc0)
					goto fail;
				c &= 0x3f;
				uchar |= c;

				/* no bogus surrogates */
				if (0xd800 <= uchar && uchar <= 0xdfff)
					goto fail;

			// 4-byte sequence (surrogate pairs, currently rare):
			// 11101110wwwwzzzzyy + 110111yyyyxxxxxx
			//     = 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
			// (uuuuu = wwww + 1)
			// FIXME accept the surrogate code points (only)

			} else
				goto fail;
		} else
			uchar = c;
		put_unaligned_le16 (uchar, cp++);
		count++;
		len--;
	}
	return count;
fail:
	return -1;
}

static XnBool handleGetStringDescriptor(XnUSBDevice* pDevice, __u16 nMaxLength, __u16 lang, __u8 nIndex)
{
	if (nIndex != 0 && lang != USB_LANGUAGE_ENGLISH_US)
	{
		return FALSE;
	}
	
	XnUChar buf[256];
	
	// descriptor 0 has the language id
	if (nIndex == 0)
	{
		buf [0] = 4;
		buf [1] = USB_DT_STRING;
		*(__u16*)&buf[2] = __cpu_to_le16(USB_LANGUAGE_ENGLISH_US);
	}
	else
	{
		// look for index
		const XnChar* strString = NULL;
		
		for (__u8 i = 0; i < pDevice->pDescriptors->nStrings; ++i)
		{
			if (pDevice->pDescriptors->aStrings[i].nID == nIndex)
			{
				strString = pDevice->pDescriptors->aStrings[i].strString;
				break;
			}
		}
		
		if (strString == NULL)
		{
			return FALSE;
		}
		
		int len = strlen(strString);
		len = XN_MIN(len, 126);
		
		// zero all bytes
		memset(buf+2, 0, 2*len);
		len = utf8_to_utf16le(strString, (__u16 *)&buf[2], len);
		if (len < 0)
		{
			return FALSE;
		}
		buf [0] = (len + 1) * 2;
		buf [1] = USB_DT_STRING;
	}
	
	XnUInt32 nReplySize = XN_MIN(buf[0], nMaxLength);
	int status = write(pDevice->deviceFD, buf, nReplySize);
	if (status < 0)
	{
		if (errno == EIDRM)
			fprintf (stderr, "string timeout\n");
		else
			perror ("write string data");
	}
	else if (status != nReplySize)
	{
		fprintf (stderr, "short string write, %d\n", status);
	}

	return TRUE;
}

static XnBool handleChapter9Requests(XnUSBDevice* pDevice, struct usb_ctrlrequest *setup)
{
	__u16 value = __le16_to_cpu(setup->wValue);
	__u16 index = __le16_to_cpu(setup->wIndex);
	__u16 length = __le16_to_cpu(setup->wLength);
	
	switch (setup->bRequest)
	{
	case USB_REQ_GET_DESCRIPTOR:
		{
			if (setup->bRequestType != USB_DIR_IN)
			{
				return FALSE;
			}

			if ((value >> 8) != USB_DT_STRING)
			{
				return FALSE;
			}
			
			// index is language index and string index is last byte
			return handleGetStringDescriptor(pDevice, length, index, value & 0xFF);
		}
		break;
	case USB_REQ_SET_CONFIGURATION:
		{
			if (setup->bRequestType != USB_DIR_OUT)
			{
				return FALSE;
			}
			
			__u8 nConfigID = value;

			// make sure this is the only supported configuration
			if (nConfigID != 0 && nConfigID != pDevice->pDescriptors->aConfigurations[0]->descriptor.bConfigurationValue)
			{
				return FALSE;
			}

			if (!configureEndpoints(pDevice, value))
			{
				return FALSE;	
			}
			
			pDevice->nConfigID = value;
			
			// send ACK
			int status = read (pDevice->deviceFD, &status, 0);
			if (status)
				perror ("ack SET_CONFIGURATION");
		}
		break;
	case USB_REQ_GET_INTERFACE:
		{
			if (setup->bRequestType != (USB_DIR_IN | USB_RECIP_INTERFACE) ||
				index != pDevice->nInterfaceID ||
				length > 1)
			{
				return FALSE;
			}
			
			__u8 nAlt = pDevice->nAltInterfaceID;
			int status = write(pDevice->deviceFD, &nAlt, 1);
			if (status < 0)
			{
				if (errno == EIDRM)
					fprintf (stderr, "GET_INTERFACE timeout\n");
				else
					perror ("write GET_INTERFACE data");
			}
			else if (status != length)
			{
				fprintf (stderr, "short GET_INTERFACE write, %d\n",
					status);
			}
		}
		break;
	case USB_REQ_SET_INTERFACE:
		{
			__u16 nInt = index;
			__u16 nAlt = value;
			
			if (setup->bRequestType != USB_RECIP_INTERFACE)
			{
				return FALSE;
			}

			// make sure this is the only interface
			if (nInt != 0 || nAlt != 0)
			{
				return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}
	
	return TRUE;
}

static XnBool handleControl(XnUSBDevice* pDevice, struct usb_ctrlrequest *setup)
{
	if ((setup->bRequestType & USB_TYPE_MASK) == USB_TYPE_VENDOR)
	{
		return handleVendorControl(pDevice, setup);
	}
	else
	{
		// usb 2.0 spec ch9 requests
		return handleChapter9Requests(pDevice, setup);
	}
}

XN_THREAD_PROC xnUSBDeviceEndPoint0Handler(XN_THREAD_PARAM pThreadParam)
{
	XnUSBDevice* pDevice = (XnUSBDevice*)pThreadParam;

	int status;

	struct pollfd ep0_poll;
	ep0_poll.fd = pDevice->deviceFD;
	ep0_poll.events = POLLIN | POLLOUT | POLLHUP;
	
	while (!pDevice->bShutdown)
	{
		status = poll(&ep0_poll, 1, -1);
		if (status < 0)
		{
			xnLogError(XN_MASK_OS, "Failed polling device file! (%d)", errno);
			sleep(1);
			continue;
		}
		
		struct usb_gadgetfs_event event;
		status = read(pDevice->deviceFD, &event, sizeof(event));
		if (status < 0)
		{
			if (errno == EL2HLT)
			{
				// after previous disconnect
				xnLogWarning(XN_MASK_OS, "Device was in level-2-halt. Continuing anyway...");
				continue;
			}
			else if (errno != EAGAIN)
			{
				xnLogError(XN_MASK_OS, "Failed reading from device file! (%d)", errno);
			}
			
			sleep (1);
			continue;
		}
		
		switch (event.type)
		{
		case GADGETFS_NOP:
			break;
		case GADGETFS_CONNECT:
			pDevice->speed = event.u.speed;
			SetConnectivityState(pDevice, XN_USB_DEVICE_CONNECTED);
			break;
		case GADGETFS_DISCONNECT:
			pDevice->speed = USB_SPEED_UNKNOWN;
			SetConnectivityState(pDevice, XN_USB_DEVICE_DISCONNECTED);
			break;
		case GADGETFS_SUSPEND:
			SetConnectivityState(pDevice, XN_USB_DEVICE_SUSPENDED);
			break;
		case GADGETFS_SETUP:
			// setup stage
			if (!handleControl(pDevice, &event.u.setup))
			{
				// stall is done by reading/writing in the wrong direction
				if ((event.u.setup.bRequestType & USB_DIR_IN) != 0)
					status = read (pDevice->deviceFD, &status, 0);
				else
					status = write (pDevice->deviceFD, &status, 0);
					
				if (status != -1)
				{
					//fprintf (stderr, "can't stall ep0 for %02x.%02x\n",
					//	setup->bRequestType, setup->bRequest);
				}
				else if (errno != EL2HLT)
				{
					//perror ("ep0 stall");
				}
			}
			break;
		default:
			xnLogWarning(XN_MASK_OS, "Got unknown gadgetfs event: %d", event.type);
		}
	}
	
	XN_THREAD_PROC_RETURN(XN_STATUS_OK);
}

//---------------------------------------------------------------------------
// Init & Shutdown
//---------------------------------------------------------------------------
XN_C_API XnStatus XN_C_DECL xnUSBDeviceInit(const XnUSBDeviceDescriptorHolder* pDescriptors, XnUInt32 nControlMessageMaxSize, XnUSBDevice** ppDevice)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pDescriptors);
	XN_VALIDATE_OUTPUT_PTR(ppDevice);
	
	// open device file
	int device_fd = open (GADGET_DEVICE_FILE_PATH, O_RDWR);
	if (device_fd < 0)
	{
		xnLogWarning(XN_MASK_OS, "Failed opening %s (%d)", GADGET_DEVICE_FILE_PATH, errno);
		return XN_STATUS_ERROR;
	}
	
	// build descriptors buffer
	XnChar bufDescriptors[4096];
	XnUInt32 nBufSize = 4096;
	XnChar* buf = bufDescriptors;
	nRetVal = buildGadgetFSDescriptors(pDescriptors, buf);
	XN_IS_STATUS_OK(nRetVal);
	
	int status = write(device_fd, bufDescriptors, buf - bufDescriptors);
	if (status < 0)
	{
		xnLogWarning(XN_MASK_OS, "Failed writing descriptor to device file (%d)", errno);
		close (device_fd);
		return XN_STATUS_ERROR;
	}
	else if (status != (buf - bufDescriptors))
	{
		xnLogWarning(XN_MASK_OS, "Failed writing descriptors to device file. Write %d, expected %d.", status, buf - bufDescriptors);
		close (device_fd);
		return XN_STATUS_ERROR;
	}
	
	XnUSBDevice* pDevice = (XnUSBDevice*)xnOSCalloc(1, sizeof(XnUSBDevice));
	if (pDevice == NULL)
	{
		xnLogError(XN_MASK_OS, "Failed to allocate USB Device");
		close(device_fd);
		return XN_STATUS_ALLOC_FAILED;
	}

	pDevice->deviceFD = device_fd;
	for (int i = 0; i < XN_USB_DEVICE_ENDPOINT_MAX_COUNT; ++i)
	{
		pDevice->endpoints[i].fd = -1;
	}
	pDevice->nControlMessageMaxSize = nControlMessageMaxSize;
	pDevice->pDescriptors = pDescriptors;
	pDevice->pControlBuffer = (XnUChar*)xnOSMallocAligned(nControlMessageMaxSize, XN_DEFAULT_MEM_ALIGN);

	if (pDevice->pControlBuffer == NULL)
	{
		xnLogError(XN_MASK_OS, "Failed to allocate control buffer");
		xnUSBDeviceShutdown(pDevice);
		return XN_STATUS_ALLOC_FAILED;
	}

	nRetVal = xnOSCreateCriticalSection(&pDevice->hLock);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_OS, "Failed to create device critical section: %s", xnGetStatusString(nRetVal));
		xnUSBDeviceShutdown(pDevice);
		return (nRetVal);
	}
	
	nRetVal = xnOSCreateEvent(&pDevice->hReplyEvent, FALSE);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_OS, "Failed to create device event: %s", xnGetStatusString(nRetVal));
		xnUSBDeviceShutdown(pDevice);
		return (nRetVal);
	}
	
	nRetVal = xnOSCreateThread(xnUSBDeviceEndPoint0Handler, pDevice, &pDevice->hThread);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_OS, "Failed to create endpoint handler thread: %s", xnGetStatusString(nRetVal));
		xnUSBDeviceShutdown(pDevice);
		return (nRetVal);
	}

	pDevice->pDump = xnDumpFileOpen("Gadget", "gadget.csv");
	xnDumpFileWriteString(pDevice->pDump, "Time,HostState,DeviceState,Event,NewHostState,NewDeviceState\n","");
	
	*ppDevice = pDevice;
	return XN_STATUS_OK;
}

XN_C_API void XN_C_DECL xnUSBDeviceShutdown(XnUSBDevice* pDevice)
{
	XN_ASSERT(pDevice != NULL);
	
	pDevice->bShutdown = TRUE;
	
	if (pDevice->hThread != NULL)
	{
		xnOSWaitAndTerminateThread(&pDevice->hThread, 10000);
		pDevice->hThread = NULL;
	}

	if (pDevice->hLock != NULL)
	{
		xnOSCloseCriticalSection(&pDevice->hLock);
		pDevice->hLock = NULL;
	}

	if (pDevice->hReplyEvent != NULL)
	{
		xnOSCloseEvent(&pDevice->hReplyEvent);
		pDevice->hReplyEvent = NULL;
	}

	if (pDevice->pControlBuffer != NULL)
	{
		xnOSFreeAligned(pDevice->pControlBuffer);
		pDevice->pControlBuffer = NULL;
	}
	
	if (pDevice->deviceFD != -1)
	{
		close(pDevice->deviceFD);
		pDevice->deviceFD = -1;
	}

	xnOSFree(pDevice);
}

//---------------------------------------------------------------------------
// Handling Vendor Controls
//---------------------------------------------------------------------------
XN_C_API XnBool XN_C_DECL xnUSBDeviceIsControlRequestPending(XnUSBDevice* pDevice)
{
	XN_ASSERT(pDevice != NULL);
	if (pDevice == NULL)
		return FALSE;

	XnAutoCSLocker locker(pDevice->hLock);
	return (pDevice->eDeviceControlState == DEVICE_CONTROL_REQUEST_RECEIVED);
}

XN_C_API XnStatus XN_C_DECL xnUSBDeviceReceiveControlRequest(XnUSBDevice* pDevice, XnUChar* pBuffer, XnUInt32* pnRequestSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pDevice);
	XN_VALIDATE_INPUT_PTR(pBuffer);
	XN_VALIDATE_OUTPUT_PTR(pnRequestSize);
	
	XnAutoCSLocker locker(pDevice->hLock);

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);
	xnDumpFileWriteString(pDevice->pDump, "%llu,%s,%s,DeviceReadRequest,%s,%s\n", nNow, GetHostStateName(pDevice->eHostControlState), GetDeviceStateName(pDevice->eDeviceControlState), GetHostStateName(pDevice->eHostControlState), GetDeviceStateName(DEVICE_CONTROL_REQUEST_READ));

	if (pDevice->eDeviceControlState != DEVICE_CONTROL_REQUEST_RECEIVED)
	{
		return XN_STATUS_USB_NO_REQUEST_PENDING;
	}

	if (pDevice->nControlSize > *pnRequestSize)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	xnOSMemCopy(pBuffer, pDevice->pControlBuffer, pDevice->nControlSize);
	*pnRequestSize = pDevice->nControlSize;
	pDevice->eDeviceControlState = DEVICE_CONTROL_REQUEST_READ;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus XN_C_DECL xnUSBDeviceSendControlReply(XnUSBDevice* pDevice, const XnUChar* pBuffer, XnUInt32 nReplySize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pDevice);
	XN_VALIDATE_INPUT_PTR(pBuffer);
	
	XnAutoCSLocker locker(pDevice->hLock);

	HostControlState prevHost = pDevice->eHostControlState;
	DeviceControlState prevDevice = pDevice->eDeviceControlState;

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);
	
	if (pDevice->nControlSize != 0 || pDevice->eDeviceControlState != DEVICE_CONTROL_REPLY_READY)
	{
		if (pDevice->eDeviceControlState != DEVICE_CONTROL_REQUEST_READ)
		{
			xnDumpFileWriteString(pDevice->pDump, "%llu,%s,%s,DeviceSentReply,,,Device requested to send reply, but no request was received!\n", nNow, GetHostStateName(prevHost), GetDeviceStateName(prevDevice));
			xnLogError(XN_MASK_OS, "Device requested to send reply, but no request was received!");
			return XN_STATUS_USB_NO_REQUEST_PENDING;
		}
	}
	
	switch (pDevice->eHostControlState)
	{
	case HOST_CONTROL_WAITING_FOR_REPLY:
		{
			// already got in control. we can write reply directly to USB
			int status = write(pDevice->deviceFD, pBuffer, nReplySize);
			if (status < 0)
			{
				xnLogError(XN_MASK_OS, "failed to send control reply (%d)!", errno);
				return XN_STATUS_USB_CONTROL_SEND_FAILED;
			}

			if (nReplySize > 0)
			{
				pDevice->eDeviceControlState = DEVICE_CONTROL_CLEAR;
				pDevice->eHostControlState = HOST_CONTROL_CLEAR;
				xnDumpFileWriteString(pDevice->pDump, "%llu,%s,%s,DeviceSentReply,%s,%s\n", nNow, GetHostStateName(prevHost), GetDeviceStateName(prevDevice), GetHostStateName(pDevice->eHostControlState), GetDeviceStateName(pDevice->eDeviceControlState));
			}
			else
			{
				// fake response
				pDevice->eDeviceControlState = DEVICE_CONTROL_REQUEST_READ;
				pDevice->eHostControlState = HOST_CONTROL_REQUEST_RECEIVED;
				xnDumpFileWriteString(pDevice->pDump, "%llu,%s,%s,DeviceSentEmptyReply,%s,%s\n", nNow, GetHostStateName(prevHost), GetDeviceStateName(prevDevice), GetHostStateName(pDevice->eHostControlState), GetDeviceStateName(pDevice->eDeviceControlState));
			}

			nRetVal = xnOSSetEvent(pDevice->hReplyEvent);
			if (nRetVal != XN_STATUS_OK)
			{
				xnLogWarning(XN_MASK_OS, "Failed to set control event: %s", xnGetStatusString(nRetVal));
				// reply was sent, so no need to return an error
			}

			break;
		}
	case HOST_CONTROL_REQUEST_RECEIVED:
		{
			// no in control yet. keep the reply for later
			xnOSMemCopy(pDevice->pControlBuffer, pBuffer, nReplySize);
			pDevice->nControlSize = nReplySize;
			pDevice->eDeviceControlState = DEVICE_CONTROL_REPLY_READY;
			xnDumpFileWriteString(pDevice->pDump, "%llu,%s,%s,DeviceSentReply,%s,%s\n", nNow, GetHostStateName(prevHost), GetDeviceStateName(prevDevice), GetHostStateName(pDevice->eHostControlState), GetDeviceStateName(pDevice->eDeviceControlState));
			break;
		}
	default:
		xnDumpFileWriteString(pDevice->pDump, "%llu,%s,%s,DeviceSentReply,BAD-STATE\n", nNow, GetHostStateName(prevHost), GetDeviceStateName(prevDevice));
		xnLogError(XN_MASK_OS, "Bad host state: %d", pDevice->eHostControlState);
		return XN_STATUS_USB_NO_REQUEST_PENDING;
	}

	return (XN_STATUS_OK);
}

static XnBool handleVendorControl(XnUSBDevice* pDevice, struct usb_ctrlrequest *setup)
{
	__u16 length = __le16_to_cpu(setup->wLength);

	XnBool bHostRequest = (setup->bRequestType & USB_DIR_IN) ? 0 : 1;

	if (bHostRequest)
	{
		// read data stage
		if (length == 0)
		{
			xnLogError(XN_MASK_OS, "Got a control message with size 0!");
			return FALSE;
		}

		if (length > pDevice->nControlMessageMaxSize)
		{
			xnLogError(XN_MASK_OS, "Got a control message bigger than max allowed size!");
			return FALSE;
		}

		XnAutoCSLocker locker(pDevice->hLock);
		if (pDevice->eDeviceControlState != DEVICE_CONTROL_CLEAR)
		{
			xnLogError(XN_MASK_OS, "Got a control request before previous one was replied!");
			return FALSE;
		}

		// read the data
		int status = read(pDevice->deviceFD, pDevice->pControlBuffer, length);
		if (status < 0)
		{
			if (errno == EIDRM)
			{
				xnLogWarning(XN_MASK_OS, "control request was aborted. Ignoring it...");
				return TRUE;
			}
			else if (errno == ECANCELED)
			{
				xnLogWarning(XN_MASK_OS, "control request was canceled. Ignoring it...");
				return TRUE;
			}
			else
			{
				xnLogError(XN_MASK_OS, "Got a control request but failed to read its data (length was %u, errorcode is %d)!", length, errno);
				return FALSE;
			}
		}
		
		pDevice->nControlSize = length;
		pDevice->eDeviceControlState = DEVICE_CONTROL_REQUEST_RECEIVED;
		pDevice->eHostControlState = HOST_CONTROL_REQUEST_RECEIVED;

		XnUInt64 nNow;
		xnOSGetHighResTimeStamp(&nNow);
		xnDumpFileWriteString(pDevice->pDump, "%llu,%s,%s,HostOut,%s,%s\n", nNow, GetHostStateName(HOST_CONTROL_CLEAR), GetDeviceStateName(DEVICE_CONTROL_CLEAR), GetHostStateName(pDevice->eHostControlState), GetDeviceStateName(pDevice->eDeviceControlState));

		// raise callback
		if (pDevice->pNewControlRequestCallback != NULL)
		{
			pDevice->pNewControlRequestCallback(pDevice, pDevice->pNewControlRequestCallbackCookie);
		}
	}
	else
	{
		// host requests to read.
		XnAutoCSLocker locker(pDevice->hLock);

		HostControlState prevHost = pDevice->eHostControlState;
		DeviceControlState prevDevice = pDevice->eDeviceControlState;

		XnUInt64 nNow;
		xnOSGetHighResTimeStamp(&nNow);
		
		if (pDevice->eHostControlState != HOST_CONTROL_REQUEST_RECEIVED)
		{
			xnDumpFileWriteString(pDevice->pDump, "%llu,%s,%s,HostIn,,,Host asks for reply, but no request was received!\n", nNow, GetHostStateName(prevHost), GetDeviceStateName(prevDevice));
			xnLogError(XN_MASK_OS, "Host asks for reply, but no request was received!");
			return FALSE;
		}
		
		switch (pDevice->eDeviceControlState)
		{
		case DEVICE_CONTROL_REPLY_READY:
			{
				// reply is waiting to be send. send it now
				int status = write(pDevice->deviceFD, pDevice->pControlBuffer, pDevice->nControlSize);
				if (status < 0)
				{
					return FALSE;
				}

				if (pDevice->nControlSize > 0)
				{
					pDevice->eDeviceControlState = DEVICE_CONTROL_CLEAR;
					pDevice->eHostControlState = HOST_CONTROL_CLEAR;
					xnDumpFileWriteString(pDevice->pDump, "%llu,%s,%s,HostIn,%s,%s\n", nNow, GetHostStateName(prevHost), GetDeviceStateName(prevDevice), GetHostStateName(pDevice->eHostControlState), GetDeviceStateName(pDevice->eDeviceControlState));
				}
				else
				{
					// fake response
					pDevice->eDeviceControlState = DEVICE_CONTROL_REQUEST_READ;
					pDevice->eHostControlState = HOST_CONTROL_REQUEST_RECEIVED;
					xnDumpFileWriteString(pDevice->pDump, "%llu,%s,%s,HostIn-EmptyReply,%s,%s\n", nNow, GetHostStateName(prevHost), GetDeviceStateName(prevDevice), GetHostStateName(pDevice->eHostControlState), GetDeviceStateName(pDevice->eDeviceControlState));
				}

				break;
			}
		case DEVICE_CONTROL_REQUEST_RECEIVED:
		case DEVICE_CONTROL_REQUEST_READ:
			{
				// no reply yet. mark that we got the in control
				pDevice->eHostControlState = HOST_CONTROL_WAITING_FOR_REPLY;

				xnDumpFileWriteString(pDevice->pDump, "%llu,%s,%s,HostIn-Waiting,%s,%s\n", nNow, GetHostStateName(prevHost), GetDeviceStateName(prevDevice), GetHostStateName(pDevice->eHostControlState), GetDeviceStateName(pDevice->eDeviceControlState));

				// we must not read any more events until a reply is made. Wait.
				locker.Unlock();
				if (xnOSWaitEvent(pDevice->hReplyEvent, XN_USB_CONTROL_TIMEOUT) == XN_STATUS_OS_EVENT_TIMEOUT)
				{
					xnLogError(XN_MASK_OS, "control command did not finish in %u milliseconds!", XN_USB_CONTROL_TIMEOUT);
					pDevice->eHostControlState = HOST_CONTROL_REQUEST_RECEIVED;
					return FALSE;
				}

				break;
			}
		default:
			// bad state
			xnLogError(XN_MASK_OS, "Bad device state: %d", pDevice->eDeviceControlState);
			return FALSE;
		}
	}

	return TRUE;
}

XN_C_API XnStatus XN_C_DECL xnUSBDeviceSetNewControlRequestCallback(XnUSBDevice* pDevice, XnUSBDeviceNewControlRequestCallback pFunc, void* pCookie)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pDevice);
	
	XnAutoCSLocker locker(pDevice->hLock);
	
	pDevice->pNewControlRequestCallback = pFunc;
	pDevice->pNewControlRequestCallbackCookie = pCookie;
	
	return XN_STATUS_OK;
}

XN_C_API XnStatus XN_C_DECL xnUSBDeviceSetConnectivityChangedCallback(XnUSBDevice* pDevice, XnUSBDeviceConnectivityChangedCallback pFunc, void* pCookie)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pDevice);
	
	XnAutoCSLocker locker(pDevice->hLock);
	
	pDevice->pConnectivityChangedCallback = pFunc;
	pDevice->pConnectivityChangedCallbackCookie = pCookie;
	
	return XN_STATUS_OK;
}

//---------------------------------------------------------------------------
// Writing Data
//---------------------------------------------------------------------------

XN_C_API XnStatus XN_C_DECL xnUSBDeviceWriteEndpoint(XnUSBDevice* pDevice, XnUInt8 nEndpointID, const XnUChar* pData, XnUInt32 nDataSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pDevice);
	XN_VALIDATE_INPUT_PTR(pData);
	
	if ((nEndpointID & 0x7F) >= XN_USB_DEVICE_ENDPOINT_MAX_COUNT)
	{
		xnLogError(XN_MASK_OS, "Got bad endpoint ID: 0x%X", nEndpointID);
		XN_ASSERT(FALSE);
		return XN_STATUS_BAD_PARAM;
	}

	__u8 nIndex = nEndpointID & 0x0F;

	// clear ended txs
	while (pDevice->endpoints[nIndex].nQueued > 0)
	{
		XnUInt32 iTx = pDevice->endpoints[nIndex].nFirst;
		int status = aio_error(&pDevice->endpoints[nIndex].txs[iTx].cb);
		if (status == EINPROGRESS)
		{
			break;
		}

		aio_return(&pDevice->endpoints[nIndex].txs[iTx].cb);
		pDevice->endpoints[nIndex].nFirst = (pDevice->endpoints[nIndex].nFirst + 1) % XN_USB_MAX_WRITE_TX;
		pDevice->endpoints[nIndex].nQueued -= 1;
	}

	// check there's enough room in queue
	if (pDevice->endpoints[nIndex].nQueued >= XN_USB_MAX_WRITE_TX)
	{
		xnLogWarning(XN_MASK_OS, "Gadget: Output queue has overflowed!");
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	// check write size is in correct size
	if (nDataSize > pDevice->endpoints[nIndex].nBufferSize)
	{
		xnLogWarning(XN_MASK_OS, "Gadget: Too much data!");
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	XnUInt32 iTx = (pDevice->endpoints[nIndex].nFirst + pDevice->endpoints[nIndex].nQueued) % XN_USB_MAX_WRITE_TX;
	xnOSMemCopy(pDevice->endpoints[nIndex].txs[iTx].pBuffer, pData, nDataSize);

	struct aiocb& cb = pDevice->endpoints[nIndex].txs[iTx].cb;
	xnOSMemSet(&cb, 0, sizeof(cb));
	cb.aio_fildes = pDevice->endpoints[nIndex].fd;
	cb.aio_buf = pDevice->endpoints[nIndex].txs[iTx].pBuffer;
	cb.aio_nbytes = nDataSize;
	
	int status = aio_write(&cb);
	if (status < 0)
	{
		xnLogWarning(XN_MASK_OS, "Failed to start asynch write! (%d)", errno);
		return XN_STATUS_USB_ENDPOINT_WRITE_FAILED;
	}

	++pDevice->endpoints[nIndex].nQueued;
	
	return XN_STATUS_OK;
}

XN_C_API XnStatus XN_C_DECL xnUSBDeviceResetEndpoint(XnUSBDevice* pDevice, XnUInt8 nEndpointID)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XN_VALIDATE_INPUT_PTR(pDevice);

	if ((nEndpointID & 0x7F) >= XN_USB_DEVICE_ENDPOINT_MAX_COUNT)
	{
		xnLogError(XN_MASK_OS, "Got bad endpoint ID: 0x%X", nEndpointID);
		XN_ASSERT(FALSE);
		return XN_STATUS_BAD_PARAM;
	}

	__u8 nIndex = nEndpointID & 0x0F;

	aio_cancel(pDevice->endpoints[nIndex].fd, NULL);
	pDevice->endpoints[nIndex].nQueued = 0;
	pDevice->endpoints[nIndex].nFirst = 0;
}

#endif // XN_PLATFORM == XN_PLATFORM_LINUX_ARM
