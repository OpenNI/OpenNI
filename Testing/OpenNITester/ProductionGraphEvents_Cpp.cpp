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
#include <XnCppWrapper.h>

using namespace xn;

typedef struct 
{
	Context context;
	const XnChar* strName;
	void* pCookie;
	XnBool bInvoked;
} ExpectedInvocation;

static ExpectedInvocation s_expectedInvocation[2];
static ProductionNode s_createdNode;

#define EXPECT_XN_TRUE(x)	EXPECT_TRUE((x) == TRUE)
#define ASSERT_XN_TRUE(x)	ASSERT_TRUE((x) == TRUE)

void OnNodeCreation(ExpectedInvocation* pExpectedInvocation, Context& context, ProductionNode& createdNode, void* pCookie)
{
	EXPECT_XN_TRUE(pExpectedInvocation->context == context);
	EXPECT_EQ(pExpectedInvocation->pCookie, pCookie);
	EXPECT_FALSE(pExpectedInvocation->bInvoked);
	pExpectedInvocation->bInvoked = TRUE;

	ASSERT_XN_TRUE(createdNode.IsValid());
	const XnChar* strName = createdNode.GetName();
	EXPECT_STRCASEEQ(pExpectedInvocation->strName, strName);

	if (!s_createdNode.IsValid())
	{
		s_createdNode = createdNode;
	}
}

void XN_CALLBACK_TYPE OnNodeCreation1(Context& context, ProductionNode& createdNode, void* pCookie)
{
	OnNodeCreation(&s_expectedInvocation[0], context, createdNode, pCookie);
}

void XN_CALLBACK_TYPE OnNodeCreation2(Context& context, ProductionNode& createdNode, void* pCookie)
{
	OnNodeCreation(&s_expectedInvocation[1], context, createdNode, pCookie);
}

void OnNodeDestruction(ExpectedInvocation* pExpectedInvocation, Context& context, const XnChar* strDestroyedNodeName, void* pCookie)
{
	EXPECT_XN_TRUE(pExpectedInvocation->context == context);
	EXPECT_EQ(pExpectedInvocation->pCookie, pCookie);
	EXPECT_FALSE(pExpectedInvocation->bInvoked);
	pExpectedInvocation->bInvoked = TRUE;

	const XnChar* strNull = NULL;
	ASSERT_NE(strNull, strDestroyedNodeName);
	EXPECT_STRCASEEQ(pExpectedInvocation->strName, strDestroyedNodeName);
}

void XN_CALLBACK_TYPE OnNodeDestruction1(Context& context, const XnChar* strDestroyedNodeName, void* pCookie)
{
	OnNodeDestruction(&s_expectedInvocation[0], context, strDestroyedNodeName, pCookie);
}

void XN_CALLBACK_TYPE OnNodeDestruction2(Context& context, const XnChar* strDestroyedNodeName, void* pCookie)
{
	OnNodeDestruction(&s_expectedInvocation[1], context, strDestroyedNodeName, pCookie);
}

void TestCreation(Context& context, void* pHandler1Cookie, void* pHandler2Cookie)
{
	const XnChar* strRequestedName = "Mock1";

	s_expectedInvocation[0].context = context;
	s_expectedInvocation[0].pCookie = pHandler1Cookie;
	s_expectedInvocation[0].strName = strRequestedName;
	s_expectedInvocation[0].bInvoked = FALSE;
	s_expectedInvocation[1].context = context;
	s_expectedInvocation[1].pCookie = pHandler2Cookie;
	s_expectedInvocation[1].strName = strRequestedName;
	s_expectedInvocation[1].bInvoked = FALSE;
	s_createdNode.Release();

	ProductionNode node;
	XnStatus nRetVal = context.CreateMockNode(XN_NODE_TYPE_DEPTH, strRequestedName, node);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	// make sure our callbacks were invoked
	EXPECT_EQ(XnBool(pHandler1Cookie != NULL), s_expectedInvocation[0].bInvoked);
	EXPECT_EQ(XnBool(pHandler2Cookie != NULL), s_expectedInvocation[1].bInvoked);

	if (s_createdNode.IsValid())
	{
		EXPECT_EQ(node.GetHandle(), s_createdNode.GetHandle());
	}

	// we no longer need this node
	node.Release();
}

TEST(ContextTests_Cpp, TestNodeCreationEvent) 
{
	XnStatus nRetVal = XN_STATUS_OK;

	Context context;
	nRetVal = context.Init();
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	// take random pointers as cookies
	void* pHandler1Cookie = &context + 1;
	void* pHandler2Cookie = &context + 2;

	XnCallbackHandle hHandler1 = NULL;
	XnCallbackHandle hHandler2 = NULL;

	// register first handler
	nRetVal = context.RegisterToNodeCreation(OnNodeCreation1, pHandler1Cookie, hHandler1);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	TestCreation(context, pHandler1Cookie, NULL);

	// register second one
	nRetVal = context.RegisterToNodeCreation(OnNodeCreation2, pHandler2Cookie, hHandler2);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	TestCreation(context, pHandler1Cookie, pHandler2Cookie);

	// unregister first
	context.UnregisterFromNodeCreation(hHandler1);
	TestCreation(context, NULL, pHandler2Cookie);

	// unregister second
	context.UnregisterFromNodeCreation(hHandler2);
	TestCreation(context, NULL, NULL);
}

void TestDestruction(Context& context, void* pHandler1Cookie, void* pHandler2Cookie)
{
	ProductionNode node;
	XnStatus nRetVal = context.CreateMockNode(XN_NODE_TYPE_DEPTH, NULL, node);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	const XnChar* strName = xnOSStrDup(node.GetName());

	// set expectations
	s_expectedInvocation[0].context = context;
	s_expectedInvocation[0].pCookie = pHandler1Cookie;
	s_expectedInvocation[0].strName = strName;
	s_expectedInvocation[0].bInvoked = FALSE;
	s_expectedInvocation[1].context = context;
	s_expectedInvocation[1].pCookie = pHandler2Cookie;
	s_expectedInvocation[1].strName = strName;
	s_expectedInvocation[1].bInvoked = FALSE;
	s_createdNode.Release();

	// destroy node
	node.Release();

	// make sure our callbacks were invoked
	EXPECT_EQ(XnBool(pHandler1Cookie != NULL), s_expectedInvocation[0].bInvoked);
	EXPECT_EQ(XnBool(pHandler2Cookie != NULL), s_expectedInvocation[1].bInvoked);

	xnOSFree(strName);
}

TEST(ContextTests_Cpp, TestNodeDestructionEvent) 
{
	XnStatus nRetVal = XN_STATUS_OK;

	Context context;
	nRetVal = context.Init();
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	// take random pointers as cookies
	void* pHandler1Cookie = &context + 1;
	void* pHandler2Cookie = &context + 2;

	XnCallbackHandle hHandler1 = NULL;
	XnCallbackHandle hHandler2 = NULL;

	// register first handler
	nRetVal = context.RegisterToNodeDestruction(OnNodeDestruction1, pHandler1Cookie, hHandler1);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	TestDestruction(context, pHandler1Cookie, NULL);

	// register second one
	nRetVal = context.RegisterToNodeDestruction(OnNodeDestruction2, pHandler2Cookie, hHandler2);
	ASSERT_EQ(XN_STATUS_OK, nRetVal);

	TestDestruction(context, pHandler1Cookie, pHandler2Cookie);

	// unregister first
	context.UnregisterFromNodeDestruction(hHandler1);
	TestDestruction(context, NULL, pHandler2Cookie);

	// unregister second
	context.UnregisterFromNodeDestruction(hHandler2);
	TestDestruction(context, NULL, NULL);
}
