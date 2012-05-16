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
//---------------------------------------------------------------------------
// Documentation
//---------------------------------------------------------------------------
/*
This sample demonstrates how to use an extension. It uses the 
NiSampleExtensionModule sample module.
Make sure this module is registered with OpenNI before running this sample.
Also, unregister the module after using this sample, otherwise you might
get unwanted behavior...
*/

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "../NiSampleExtensionModule/RotatingDeviceExtension.h"
#include <XnCppWrapper.h>

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define CHECK_RC(rc, what)											\
	if (rc != XN_STATUS_OK)											\
	{																\
		printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
		return rc;													\
	}

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

using namespace xn;
using namespace Sample;

int main()
{
	XnStatus nRetVal = XN_STATUS_OK;

	Context context;
	EnumerationErrors errors;

	nRetVal = context.Init();
	CHECK_RC(nRetVal, "Init context");

	RotatingDevice rotatingDevice;
	nRetVal = rotatingDevice.Create(context);
	if (nRetVal != XN_STATUS_OK)
	{
		printf("Create Rotating Device failed: %s\n\n", xnGetStatusString(nRetVal));
		printf("In order to run this sample, NiSampleExtensionModule must be registered with OpenNI.\n");
		printf("Please run:\n");
		printf("\tniReg NiSampleExtensionModule.dll\n");
		printf("before running this sample, and\n");
		printf("\tniReg -u NiSampleExtensionModule.dll\n");
		printf("afterwards.\n");

		return nRetVal;
	}

	nRetVal = rotatingDevice.SetViewAngle(2.17);
	CHECK_RC(nRetVal, "Create Rotating Device");

	nRetVal = rotatingDevice.PlayBeep();
	CHECK_RC(nRetVal, "Play Beep");

	return 0;
}