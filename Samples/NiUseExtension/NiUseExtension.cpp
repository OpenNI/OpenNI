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