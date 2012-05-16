#ifndef _XN_STACK_T_H_
#define _XN_STACK_T_H_ 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnListT.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
template<class T, class TAlloc = XnLinkedNodeDefaultAllocatorT<T> >
class XnStackT : protected XnListT<T, TAlloc>
{
public:
	typedef XnListT<T, TAlloc> Base;

	typedef typename Base::ConstIterator ConstIterator;

	XnStackT() : Base() {}

	XnStackT(const XnStackT& other) : Base() 
	{
		*this = other;
	}

	XnStackT& operator=(const XnStackT& other)
	{
		Base::operator=(other);
		// no other members
		return *this;
	}

	~XnStackT() {}

	XnBool IsEmpty() const { return Base::IsEmpty(); }

	XnStatus Push(T const& value) { return Base::AddFirst(value); }

	XnStatus Pop(T& value)
	{
		ConstIterator it = Begin();
		if (it == End())
		{
			return XN_STATUS_IS_EMPTY;
		}
		value = *it;
		return Base::Remove(it);
	}

	T const& Top() const { return *Begin(); }
	T& Top() { return *Begin(); }

	ConstIterator Begin() const { return Base::Begin(); }
	ConstIterator End() const { return Base::End(); }
};

#endif // _XN_STACK_T_H_