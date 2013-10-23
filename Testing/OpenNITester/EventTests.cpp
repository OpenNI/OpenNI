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
#include <gtest/gtest.h>
#include <XnEventT.h>

typedef XnEvent1Arg<int> XnTestEvent1;

static int g_nCalled;
static int g_nExpectedNum;
static void* g_pExpectedCookie;

static XnTestEvent1* g_pEvent;
static XnCallbackHandle g_hUnregisterFromHandler;
static XnCallbackHandle g_hRegisteredHandle;
static bool g_bRegisterFromHandler;

void XN_CALLBACK_TYPE Handler(int num, void* pCookie)
{
	XnStatus nRetVal = XN_STATUS_OK;
	g_nCalled++;
	EXPECT_EQ(num, g_nExpectedNum);
	EXPECT_EQ(pCookie, g_pExpectedCookie);

	if (g_bRegisterFromHandler)
	{
		nRetVal = g_pEvent->Register(Handler, g_pExpectedCookie, g_hRegisteredHandle);
		EXPECT_EQ(nRetVal, XN_STATUS_OK);
		g_bRegisterFromHandler = false;
	}

	if (g_hUnregisterFromHandler != NULL)
	{
		nRetVal = g_pEvent->Unregister(g_hUnregisterFromHandler);
		EXPECT_EQ(nRetVal, XN_STATUS_OK);
		g_hUnregisterFromHandler = NULL;
	}
}

TEST(EventTests, TestGeneral) 
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnTestEvent1 event;
	g_pEvent = &event;
	g_bRegisterFromHandler = false;
	g_hUnregisterFromHandler = NULL;

	/** Register to event **/

	// try to pass NULL
	XnCallbackHandle hCallback1;

	g_pExpectedCookie = (void*)13;

	nRetVal = event.Register(NULL, g_pExpectedCookie, hCallback1);
	EXPECT_EQ(nRetVal, XN_STATUS_NULL_INPUT_PTR);

	// and now register
	nRetVal = event.Register(Handler, g_pExpectedCookie, hCallback1);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);

	/* Fire event */
	g_nExpectedNum = 7;
	g_nCalled = 0;
	nRetVal = event.Raise(g_nExpectedNum);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);
	EXPECT_EQ(g_nCalled, 1);

	/* add it again, and make sure it is called twice */
	XnCallbackHandle hCallback2;
	nRetVal = event.Register(Handler, g_pExpectedCookie, hCallback2);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);

	g_nExpectedNum = 2;
	g_nCalled = 0;
	nRetVal = event.Raise(g_nExpectedNum);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);
	EXPECT_EQ(g_nCalled, 2);

	/* Remove first, and make sure it's still called one */
	nRetVal = event.Unregister(hCallback1);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);

	g_nExpectedNum = 934;
	g_nCalled = 0;
	nRetVal = event.Raise(g_nExpectedNum);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);
	EXPECT_EQ(g_nCalled, 1);

	/* Remove second and make sure none is called */
	nRetVal = event.Unregister(hCallback2);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);

	g_nExpectedNum = 1234;
	g_nCalled = 0;
	nRetVal = event.Raise(g_nExpectedNum);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);
	EXPECT_EQ(g_nCalled, 0);
}

TEST(EventTests, TestRegisterUnregisterFromCallback) 
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnTestEvent1 event;
	g_pEvent = &event;

	g_pExpectedCookie = (void*)987;

	/* Register once */
	XnCallbackHandle hMain;
	nRetVal = event.Register(Handler, g_pExpectedCookie, hMain);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);

	/* Register from callback */
	g_hRegisteredHandle = NULL;
	g_bRegisterFromHandler = true;

	// raise
	g_nExpectedNum = 1234;
	g_nCalled = 0;
	nRetVal = event.Raise(g_nExpectedNum);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);
	EXPECT_EQ(g_nCalled, 1);

	// check it was registered
	XnCallbackHandle hOther = g_hRegisteredHandle;
	EXPECT_NE(hOther, (XnCallbackHandle)NULL);

	// raise again to make sure
	g_nExpectedNum = 2345;
	g_nCalled = 0;
	nRetVal = event.Raise(g_nExpectedNum);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);
	EXPECT_EQ(g_nCalled, 2); // was supposed to be registered

	/* Unregister from callback */
	g_hRegisteredHandle = NULL;
	g_hUnregisterFromHandler = hOther;

	g_nExpectedNum = 3456;
	g_nCalled = 0;
	nRetVal = event.Raise(g_nExpectedNum);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);
	EXPECT_EQ(g_nCalled, 2);

	g_nExpectedNum = 4567;
	g_nCalled = 0;
	nRetVal = event.Raise(g_nExpectedNum);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);
	EXPECT_EQ(g_nCalled, 1); // was unregistered
}