#ifndef _XN_CYCLIC_STACK_T_H_
#define _XN_CYCLIC_STACK_T_H_ 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnStackT.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

template<class T, XnUInt32 TDefaultMaxDepth, class TAlloc = XnLinkedNodeDefaultAllocatorT<T> >
class XnCyclicStackT : protected XnStackT<T, TAlloc>
{
public:
	typedef XnStackT<T, TAlloc> Base;

	XnCyclicStackT(XnUInt32 nMaxDepth = TDefaultMaxDepth) : Base(), m_nMaxDepth(nMaxDepth) {}

	XnCyclicStackT(const XnCyclicStackT& other) : Base(other) 
	{
		*this = other;
	}

	XnCyclicStackT& operator=(const XnCyclicStackT& other)
	{
		Base::operator=(other);
		m_nMaxDepth = other.m_nMaxDepth;
		return *this;
	}

	~XnCyclicStackT() {}

	using typename Base::ConstIterator;
	using Base::Remove;
	using Base::IsEmpty;
	using Base::Size;

	XnStatus SetMaxSize(XnUInt32 nMaxSize)
	{
		XnStatus nRetVal = XN_STATUS_OK;

		while (Size() > nMaxSize)
		{
			nRetVal = Remove(this->ReverseBegin());
			XN_IS_STATUS_OK(nRetVal);
		}

		m_nMaxDepth = nMaxSize;

		return (XN_STATUS_OK);
	}

	XnStatus Push(T const& value) 
	{ 
		XnStatus nRetVal = XN_STATUS_OK;
		if (Size() == m_nMaxDepth)
		{
			nRetVal = Remove(this->ReverseBegin());
			XN_IS_STATUS_OK(nRetVal);
		}

		nRetVal = Base::Push(value); 
		XN_IS_STATUS_OK(nRetVal);

		return (XN_STATUS_OK);
	}

	using Base::Pop;
	using Base::Top;
	using Base::Begin;
	using Base::End;

protected:
	XnUInt32 m_nMaxDepth;
};


#endif // _XN_CYCLIC_STACK_T_H_
