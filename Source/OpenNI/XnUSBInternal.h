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
#ifndef _XN_USB_INTERNAL_H_
#define _XN_USB_INTERNAL_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnUSB.h>

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
extern XnBool g_bUSBWasInit;

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define XN_VALIDATE_USB_INIT()				\
	if (g_bUSBWasInit != TRUE)				\
	{										\
		return (XN_STATUS_USB_NOT_INIT);	\
	}

#define XN_VALIDATE_USB_DEV_HANDLE(x)				\
	if (x == NULL)									\
	{												\
		return (XN_STATUS_USB_DEVICE_NOT_VALID);	\
	}												\
	if (x.bValid != TRUE)							\
	{												\
		return (XN_STATUS_USB_DEVICE_NOT_VALID);	\
	}

#define XN_VALIDATE_USB_PDEV_HANDLE(x)				\
	if (x == NULL)									\
	{												\
		return (XN_STATUS_USB_DEVICE_NOT_VALID);	\
	}												\
	if (x->bValid != TRUE)							\
	{												\
		return (XN_STATUS_USB_DEVICE_NOT_VALID);	\
	}

#define XN_VALIDATE_USB_EP_HANDLE(x)				\
	if (x == NULL)									\
	{												\
		return (XN_STATUS_USB_ENDPOINT_NOT_VALID);	\
	}												\
	if (x.bValid != TRUE)							\
	{												\
		return (XN_STATUS_USB_ENDPOINT_NOT_VALID);	\
	}

#define XN_VALIDATE_USB_PEP_HANDLE(x)				\
	if (x == NULL)									\
	{												\
		return (XN_STATUS_USB_ENDPOINT_NOT_VALID);	\
	}												\
	if (x->bValid != TRUE)							\
	{												\
		return (XN_STATUS_USB_ENDPOINT_NOT_VALID);	\
	}

#endif //_XN_USB_INTERNAL_H_
