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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnUSB.h>

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
XnBool g_bUSBWasInit = FALSE;

//---------------------------------------------------------------------------
// Function Declaration
//---------------------------------------------------------------------------
XnStatus xnUSBPlatformSpecificInit();
XnStatus xnUSBPlatformSpecificShutdown();

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XN_C_API XnStatus xnUSBInit()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (g_bUSBWasInit == FALSE)
	{
		nRetVal = xnUSBPlatformSpecificInit();
		XN_IS_STATUS_OK(nRetVal);

		g_bUSBWasInit = TRUE;
	}
	else
	{
		return (XN_STATUS_USB_ALREADY_INIT);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBShutdown()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (g_bUSBWasInit == TRUE)
	{
		nRetVal = xnUSBPlatformSpecificShutdown();
		XN_IS_STATUS_OK(nRetVal);

		g_bUSBWasInit = FALSE;
	}
	else
	{
		return (XN_STATUS_USB_NOT_INIT);
	}

	return (XN_STATUS_OK);
}
