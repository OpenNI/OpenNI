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
#ifndef _XN_QUEUE_T_H_
#define _XN_QUEUE_T_H_ 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnListT.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
template<class T, class TAlloc = XnLinkedNodeDefaultAllocatorT<T> >
class XnQueueT : protected XnListT<T, TAlloc>
{
public:
	typedef XnListT<T, TAlloc> Base;

	XnQueueT() : Base() {}

	XnQueueT(const XnQueueT& other) : Base() 
	{
		*this = other;
	}

	XnQueueT& operator=(const XnQueueT& other)
	{
		Base::operator=(other);
		// no other members
		return *this;
	}

	~XnQueueT() {}

	using Base::ConstIterator;
	using Base::IsEmpty;

	XnStatus Push(T const& value) 
	{ 
		return Base::AddLast(value); 
	}

	XnStatus Pop(T& value)
	{
		Iterator it = Begin();
		if (it == End())
		{
			return XN_STATUS_IS_EMPTY;
		}
		value = *it;
		return Base::Remove(it);
	}

	T const& Top() const { return *Begin(); }
	T& Top() { return *Begin(); }

	using Base::Begin;
	using Base::End;
	using Base::Size;
};



#endif // _XN_QUEUE_T_H_