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
#include <XnQueueT.h>

typedef struct XnQueueTestStruct
{
	XnUInt64 nValue;
	XnDouble dValue;
} XnQueueTestStruct;

/* Compilation tests: */
/* declare a queue of a struct. */
typedef XnQueueT<XnQueueTestStruct> XnQueueTestStruct_Queue;

/* declare a queue of pointers. */
typedef XnQueueT<XnQueueTestStruct*> XnQueueTestStructPtr_Queue;

TEST(QueueTests, TestAll)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnQueueTestStruct_Queue queue;

	XnQueueTestStruct temp1;
	temp1.nValue = 2;
	temp1.dValue = 4.2;
	nRetVal = queue.Push(temp1);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);
	EXPECT_EQ(queue.Size(), 1);

	XnQueueTestStruct& rTop = queue.Top();
	// make sure we got same values
	EXPECT_EQ(rTop.nValue, temp1.nValue);
	EXPECT_EQ(rTop.dValue, temp1.dValue);

	// make sure element is still in the queue
	EXPECT_EQ(queue.Size(), 1);

	// make sure the reference is to the actual stored element
	rTop.nValue = 5;

	XnQueueTestStruct& rTop2 = queue.Top();
	EXPECT_EQ(rTop2.nValue, 5);

	// add another one
	XnQueueTestStruct temp2;
	temp2.nValue = 13;
	temp2.dValue = 0.6;
	nRetVal = queue.Push(temp2);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);
	EXPECT_EQ(queue.Size(), 2);

	// make sure top is still temp1
	XnQueueTestStruct& rTop3 = queue.Top();
	EXPECT_EQ(rTop3.nValue, 5);
	EXPECT_EQ(rTop3.dValue, temp1.dValue);

	// pop an element
	XnQueueTestStruct top;
	nRetVal = queue.Pop(top);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);
	// make sure this is the top
	EXPECT_EQ(top.nValue, 5);
	EXPECT_EQ(top.dValue, temp1.dValue);
	// and that size was reduced
	EXPECT_EQ(queue.Size(), 1);

	// make sure top is temp2
	XnQueueTestStruct& rTop4 = queue.Top();
	EXPECT_EQ(rTop4.nValue, temp2.nValue);
	EXPECT_EQ(rTop4.dValue, temp2.dValue);

	// pop it
	nRetVal = queue.Pop(top);
	EXPECT_EQ(nRetVal, XN_STATUS_OK);
	EXPECT_EQ(queue.Size(), 0);

	// try to pop again
	nRetVal = queue.Pop(top);
	EXPECT_EQ(nRetVal, XN_STATUS_IS_EMPTY);
}