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
#include <XnContext.h>
#include <XnPrdNode.h>

typedef struct 
{
	XnContext* pContext;
	const XnChar* strName;
	void* pCookie;
	XnBool bInvoked;
} ExpectedInvocation;

static ExpectedInvocation s_expectedInvocation[2];
static XnNodeHandle s_createdNode;

void OnNodeCreation(ExpectedInvocation* pExpectedInvocation, XnContext* pContext, XnNodeHandle hCreatedNode, void* pCookie)
{
	EXPECT_EQ(pExpectedInvocation->pContext, pContext);
	EXPECT_EQ(pExpectedInvocation->pCookie, pCookie);
	EXPECT_FALSE(pExpectedInvocation->bInvoked);
	pExpectedInvocation->bInvoked = TRUE;

	XnNodeHandle hNULLNode = NULL;
	ASSERT_NE(hNULLNode, hCreatedNode);
	const XnChar* strName = xnGetNodeName(hCreatedNode);
	EXPECT_STRCASEEQ(pExpectedInvocation->strName, strName);

	if (s_createdNode == NULL)
	{
		s_createdNode = hCreatedNode;
	}
}

void XN_CALLBACK_TYPE OnNodeCreation1(XnContext* pContext, XnNodeHandle hCreatedNode, void* pCookie)
{
	OnNodeCreation(&s_expectedInvocation[0], pContext, hCreatedNode, pCookie);
}

void XN_CALLBACK_TYPE OnNodeCreation2(XnContext* pContext, XnNodeHandle hCreatedNode, void* pCookie)
{
	OnNodeCreation(&s_expectedInvocation[1], pContext, hCreatedNode, pCookie);
}

void OnNodeDestruction(ExpectedInvocation* pExpectedInvocation, XnContext* pContext, const XnChar* strDestroyedNodeName, void* pCookie)
{
	EXPECT_EQ(pExpectedInvocation->pContext, pContext);
	EXPECT_EQ(pExpectedInvocation->pCookie, pCookie);
	EXPECT_FALSE(pExpectedInvocation->bInvoked);
	pExpectedInvocation->bInvoked = TRUE;

	const XnChar* strNull = NULL;
	ASSERT_NE(strNull, strDestroyedNodeName);
	EXPECT_STRCASEEQ(pExpectedInvocation->strName, strDestroyedNodeName);
}

void XN_CALLBACK_TYPE OnNodeDestruction1(XnContext* pContext, const XnChar* strDestroyedNodeName, void* pCookie)
{
	OnNodeDestruction(&s_expectedInvocation[0], pContext, strDestroyedNodeName, pCookie);
}

void XN_CALLBACK_TYPE OnNodeDestruction2(XnContext* pContext, const XnChar* strDestroyedNodeName, void* pCookie)
{
	OnNodeDestruction(&s_expectedInvocation[1], pContext, strDestroyedNodeName, pCookie);
}

void TestCreation(XnContext* pContext, void* pHandler1Cookie, void* pHandler2Cookie)
{
	const XnChar* strRequestedName = "Mock1";

	s_expectedInvocation[0].pContext = pContext;
	s_expectedInvocation[0].pCookie = pHandler1Cookie;
	s_expectedInvocation[0].strName = strRequestedName;
	s_expectedInvocation[0].bInvoked = FALSE;
	s_expectedInvocation[1].pContext = pContext;
	s_expectedInvocation[1].pCookie = pHandler2Cookie;
	s_expectedInvocation[1].strName = strRequestedName;
	s_expectedInvocation[1].bInvoked = FALSE;
	s_createdNode = NULL;

	XnNodeHandle hNode;
	XnStatus nRetVal = xnCreateMockNode(pContext, XN_NODE_TYPE_DEPTH, strRequestedName, &hNode);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	// make sure our callbacks were invoked
	EXPECT_EQ(XnBool(pHandler1Cookie != NULL), s_expectedInvocation[0].bInvoked);
	EXPECT_EQ(XnBool(pHandler2Cookie != NULL), s_expectedInvocation[1].bInvoked);

	if (s_createdNode != NULL)
	{
		EXPECT_EQ(hNode, s_createdNode);
	}

	// we no longer need this node
	xnProductionNodeRelease(hNode);
}

TEST(ContextTests_C, TestNodeCreationEvent) 
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnContext* pContext = NULL;
	nRetVal = xnInit(&pContext);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	// take random pointers as cookies
	void* pHandler1Cookie = (XnChar*)pContext + 1;
	void* pHandler2Cookie = (XnChar*)pContext + 2;

	XnCallbackHandle hHandler1 = NULL;
	XnCallbackHandle hHandler2 = NULL;

	// register first handler
	nRetVal = xnRegisterToNodeCreation(pContext, OnNodeCreation1, pHandler1Cookie, &hHandler1);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	TestCreation(pContext, pHandler1Cookie, NULL);

	// register second one
	nRetVal = xnRegisterToNodeCreation(pContext, OnNodeCreation2, pHandler2Cookie, &hHandler2);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	TestCreation(pContext, pHandler1Cookie, pHandler2Cookie);

	// unregister first
	xnUnregisterFromNodeCreation(pContext, hHandler1);
	TestCreation(pContext, NULL, pHandler2Cookie);

	// unregister second
	xnUnregisterFromNodeCreation(pContext, hHandler2);
	TestCreation(pContext, NULL, NULL);
}

void TestDestruction(XnContext* pContext, void* pHandler1Cookie, void* pHandler2Cookie)
{
	XnNodeHandle hNode;
	XnStatus nRetVal = xnCreateMockNode(pContext, XN_NODE_TYPE_DEPTH, NULL, &hNode);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	const XnChar* strName = xnOSStrDup(xnGetNodeName(hNode));

	// set expectations
	s_expectedInvocation[0].pContext = pContext;
	s_expectedInvocation[0].pCookie = pHandler1Cookie;
	s_expectedInvocation[0].strName = strName;
	s_expectedInvocation[0].bInvoked = FALSE;
	s_expectedInvocation[1].pContext = pContext;
	s_expectedInvocation[1].pCookie = pHandler2Cookie;
	s_expectedInvocation[1].strName = strName;
	s_expectedInvocation[1].bInvoked = FALSE;
	s_createdNode = NULL;
	
	// destroy node
	xnProductionNodeRelease(hNode);

	// make sure our callbacks were invoked
	EXPECT_EQ(XnBool(pHandler1Cookie != NULL), s_expectedInvocation[0].bInvoked);
	EXPECT_EQ(XnBool(pHandler2Cookie != NULL), s_expectedInvocation[1].bInvoked);

	xnOSFree(strName);
}

TEST(ContextTests_C, TestNodeDestructionEvent) 
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnContext* pContext = NULL;
	nRetVal = xnInit(&pContext);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	// take random pointers as cookies
	void* pHandler1Cookie = (XnChar*)pContext + 1;
	void* pHandler2Cookie = (XnChar*)pContext + 2;

	XnCallbackHandle hHandler1 = NULL;
	XnCallbackHandle hHandler2 = NULL;

	// register first handler
	nRetVal = xnRegisterToNodeDestruction(pContext, OnNodeDestruction1, pHandler1Cookie, &hHandler1);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	TestDestruction(pContext, pHandler1Cookie, NULL);

	// register second one
	nRetVal = xnRegisterToNodeDestruction(pContext, OnNodeDestruction2, pHandler2Cookie, &hHandler2);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	TestDestruction(pContext, pHandler1Cookie, pHandler2Cookie);

	// unregister first
	xnUnregisterFromNodeDestruction(pContext, hHandler1);
	TestDestruction(pContext, NULL, pHandler2Cookie);

	// unregister second
	xnUnregisterFromNodeDestruction(pContext, hHandler2);
	TestDestruction(pContext, NULL, NULL);
}
