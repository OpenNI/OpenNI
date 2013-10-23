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
#include <XnListT.h>

typedef struct TestStruct
{
	XnUInt32 nVal;
	XnDouble dVal;

	XnBool operator==(const TestStruct& other) const
	{
		return (nVal == other.nVal && dVal == other.dVal);
	}

} TestStruct;

typedef XnListT<TestStruct> TestList;

template<class TTestList, class TIterator>
void TestCurrentListStateT(TTestList& list, XnUInt32 nExpectedSize, TestStruct* pExpectedValues)
{
	ASSERT_EQ(list.IsEmpty(), (XnBool)(nExpectedSize == 0));
	ASSERT_EQ(list.Size(), nExpectedSize);

	if (nExpectedSize == 0)
	{
		EXPECT_EQ(list.Begin(), list.End());
		EXPECT_EQ(list.ReverseBegin(), list.ReverseEnd());
	}
	else
	{
		EXPECT_NE(list.Begin(), list.End());
		EXPECT_NE(list.ReverseBegin(), list.ReverseEnd());
	}

	// test CopyTo()
	XnUInt32 i = 0;
	TestStruct* aValues = new TestStruct[nExpectedSize];
	list.CopyTo(aValues);

	for (i = 0; i < nExpectedSize; ++i)
	{
		EXPECT_EQ(aValues[i].nVal, pExpectedValues[i].nVal);
		EXPECT_EQ(aValues[i].dVal, pExpectedValues[i].dVal);
	}

	delete[] aValues;

	// test forward iteration with ++x
	i = 0;
	for (TIterator it = list.Begin(); it != list.End(); ++it, ++i)
	{
		ASSERT_LT(i, nExpectedSize);

		EXPECT_EQ((*it).nVal, pExpectedValues[i].nVal);
		EXPECT_EQ((*it).dVal, pExpectedValues[i].dVal);
		EXPECT_EQ(it->nVal, pExpectedValues[i].nVal);
		EXPECT_EQ(it->dVal, pExpectedValues[i].dVal);
	}
	EXPECT_EQ(i, nExpectedSize);

	// test forward iteration with x++
	i = 0;
	for (TIterator it = list.Begin(); it != list.End(); it++, ++i)
	{
		ASSERT_LT(i, nExpectedSize);

		EXPECT_EQ((*it).nVal, pExpectedValues[i].nVal);
		EXPECT_EQ((*it).dVal, pExpectedValues[i].dVal);
		EXPECT_EQ(it->nVal, pExpectedValues[i].nVal);
		EXPECT_EQ(it->dVal, pExpectedValues[i].dVal);
	}
	EXPECT_EQ(i, nExpectedSize);

	// test backwards iteration with --x
	i = nExpectedSize - 1;
	for (TIterator it = list.ReverseBegin(); it != list.ReverseEnd(); --it, --i)
	{
		EXPECT_EQ((*it).nVal, pExpectedValues[i].nVal);
		EXPECT_EQ((*it).dVal, pExpectedValues[i].dVal);
		EXPECT_EQ(it->nVal, pExpectedValues[i].nVal);
		EXPECT_EQ(it->dVal, pExpectedValues[i].dVal);
	}
	EXPECT_EQ(i, (XnUInt32)-1);

	// test backwards iteration with x--
	i = nExpectedSize - 1;
	for (TIterator it = list.ReverseBegin(); it != list.ReverseEnd(); it--, --i)
	{
		EXPECT_EQ((*it).nVal, pExpectedValues[i].nVal);
		EXPECT_EQ((*it).dVal, pExpectedValues[i].dVal);
		EXPECT_EQ(it->nVal, pExpectedValues[i].nVal);
		EXPECT_EQ(it->dVal, pExpectedValues[i].dVal);
	}
	EXPECT_EQ(i, (XnUInt32)-1);
}

void TestCurrentListState(TestList& list, int nExpectedSize, TestStruct* pExpectedValues)
{
	TestCurrentListStateT<TestList, TestList::Iterator>(list, nExpectedSize, pExpectedValues);
	TestCurrentListStateT<TestList, TestList::ConstIterator>(list, nExpectedSize, pExpectedValues);
	TestCurrentListStateT<const TestList, TestList::ConstIterator>(list, nExpectedSize, pExpectedValues);
}

TEST(ListTests, TestAll) 
{
	static TestStruct sValues[] = 
	{
		{ 1, 1.1 },
		{ 2, 2.2 },
		{ 3, 3.3 },
		{ 4, 4.4 },
	};

	// test empty
	TestList list;
	TestCurrentListState(list, 0, NULL);

	// test Clear() when empty
	ASSERT_EQ(list.Clear(), XN_STATUS_OK);
	TestCurrentListState(list, 0, NULL);

	// make sure AddBefore and AddAfter fail when receiving End()
	ASSERT_EQ(list.AddBefore(list.End(), sValues[0]), XN_STATUS_ILLEGAL_POSITION);
	ASSERT_EQ(list.AddAfter(list.End(), sValues[0]), XN_STATUS_ILLEGAL_POSITION);

	// test AddLast when empty
	ASSERT_EQ(list.AddLast(sValues[0]), XN_STATUS_OK);
	TestCurrentListState(list, 1, sValues);

	// test Clear when only one
	ASSERT_EQ(list.Clear(), XN_STATUS_OK);
	TestCurrentListState(list, 0, NULL);

	// test AddFirst when empty
	ASSERT_EQ(list.AddFirst(sValues[2]), XN_STATUS_OK);
	TestCurrentListState(list, 1, sValues + 2);

	// test AddFirst when having one
	ASSERT_EQ(list.AddFirst(sValues[1]), XN_STATUS_OK);
	TestCurrentListState(list, 2, sValues + 1);

	// test AddBefore in the beginning
	ASSERT_EQ(list.AddBefore(list.Begin(), sValues[0]), XN_STATUS_OK);
	TestCurrentListState(list, 3, sValues);

	// test Remove by value
	ASSERT_EQ(list.Remove(sValues[1]), XN_STATUS_OK);
	TestStruct aTemp[] = 
	{
		sValues[0],
		sValues[2]
	};
	TestCurrentListState(list, 2, aTemp);

	// and Remove by iterator
	ASSERT_EQ(list.Remove(++list.Begin()), XN_STATUS_OK);
	TestCurrentListState(list, 1, sValues);

	// test AddLast when having one
	ASSERT_EQ(list.AddLast(sValues[1]), XN_STATUS_OK);
	TestCurrentListState(list, 2, sValues);

	// test AddAfter in the end
	ASSERT_EQ(list.AddAfter(list.ReverseBegin(), sValues[2]), XN_STATUS_OK);
	TestCurrentListState(list, 3, sValues);

	// test AddBefore in the middle
	ASSERT_EQ(list.AddBefore(++list.Begin(), sValues[3]), XN_STATUS_OK);
	TestStruct aTemp2[] = 
	{
		sValues[0],
		sValues[3],
		sValues[1],
		sValues[2]
	};
	TestCurrentListState(list, 4, aTemp2);

	// remove one (needed for next test)
	ASSERT_EQ(list.Remove(++list.Begin()), XN_STATUS_OK);
	TestStruct aTemp3[] = 
	{
		sValues[0],
		sValues[1],
		sValues[2]
	};
	TestCurrentListState(list, 3, aTemp3);

	// test AddAfter in the middle
	ASSERT_EQ(list.AddAfter(--list.ReverseBegin(), sValues[3]), XN_STATUS_OK);
	TestStruct aTemp4[] = 
	{
		sValues[0],
		sValues[1],
		sValues[3],
		sValues[2]
	};
	TestCurrentListState(list, 4, aTemp4);

	// test const Find()
	const TestList& constList = list;
	TestList::ConstIterator constIt = constList.Find(sValues[3]);
	ASSERT_NE(constIt, constList.End());
	ASSERT_EQ(constIt, --constList.ReverseBegin());

	// and non-const Find()
	TestList::Iterator it = list.Find(sValues[2]);
	ASSERT_NE(it, list.End());
	ASSERT_EQ(it, list.ReverseBegin());

	// try to find something that's not in the list
	TestStruct notInList = { 17, 7.4 };
	ASSERT_EQ(constList.Find(notInList), constList.End());
	ASSERT_EQ(list.Find(notInList), list.End());

	// try to remove something that's not in the list
	ASSERT_EQ(list.Remove(notInList), XN_STATUS_NO_MATCH);

	// try to remove end
	ASSERT_EQ(list.Remove(list.End()), XN_STATUS_ILLEGAL_POSITION);
}
