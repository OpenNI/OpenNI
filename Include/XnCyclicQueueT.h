#ifndef _XN_CYCLIC_QUEUE_T_H_
#define _XN_CYCLIC_QUEUE_T_H_ 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnQueueT.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

template<class T, XnUInt32 TDefaultMaxDepth, class TAlloc = XnLinkedNodeDefaultAllocatorT<T> >
class XnCyclicQueueT : protected XnQueueT<T, TAlloc>
{
public:
	typedef XnQueueT<T, TAlloc> Base;

	XnCyclicQueueT(XnUInt32 nMaxDepth = TDefaultMaxDepth) : Base(), m_nMaxDepth(nMaxDepth) {}

	XnCyclicQueueT(const XnCyclicQueueT& other) : Base(other) 
	{
		*this = other;
	}

	XnCyclicQueueT& operator=(const XnCyclicQueueT& other)
	{
		Base::operator=(other);
		m_nMaxDepth = other.m_nMaxDepth;
		return *this;
	}

	~XnCyclicQueueT() {}

	using Base::ConstIterator;
	using Base::IsEmpty;
	using Base::Size;

	XnStatus SetMaxSize(XnUInt32 nMaxSize)
	{
		XnStatus nRetVal = XN_STATUS_OK;
		
		while (Size() > nMaxSize)
		{
			nRetVal = Remove(this->Begin());
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
			nRetVal = Remove(this->Begin());
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


#endif // _XN_CYCLIC_QUEUE_T_H_