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