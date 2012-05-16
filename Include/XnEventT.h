/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
#ifndef _XN_EVENT_T_H_
#define _XN_EVENT_T_H_ 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnOSCpp.h"
#include "XnListT.h"
#include "XnTypes.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/** 
* A struct for holding a callback function with its cookie
*
* @tparam	FuncPtr		The callback signature
*/
template<typename FuncPtr>
struct XnCallbackT
{
	XnCallbackT(FuncPtr func, void* cookie) : pFunc(func), pCookie(cookie) {}

	FuncPtr pFunc;
	void* pCookie;
};

/** 
* A class that contains the interface of an event (i.e. Register and Unregister)
*
* @tparam	FuncPtr		The callback signature
*/
template<typename FuncPtr>
class XnEventInterfaceT
{
public:
	typedef FuncPtr HandlerPtr;
	typedef XnCallbackT<FuncPtr> TCallback;
	typedef XnEventInterfaceT TInterface;

	~XnEventInterfaceT()
	{
		Clear();
		xnOSCloseCriticalSection(&m_hLock);
	}

	XnStatus Register(FuncPtr pFunc, void* pCookie, XnCallbackHandle& hCallback)
	{
		XnStatus nRetVal = XN_STATUS_OK;

		XN_VALIDATE_INPUT_PTR(pFunc);

		TCallback* pCallback = NULL;
		XN_VALIDATE_NEW(pCallback, TCallback, pFunc, pCookie);

		// always add to list of added (actual list will be updated in Raise method, to allow registering 
		// from a callback).
		{
			XnAutoCSLocker locker(m_hLock);
			nRetVal = m_toAdd.AddLast(pCallback);
		}

		if (nRetVal != XN_STATUS_OK)
		{
			XN_DELETE(pCallback);
			return nRetVal;
		}

		// return handle
		hCallback = (XnCallbackHandle)pCallback;

		return XN_STATUS_OK;
	}

	XnStatus Unregister(XnCallbackHandle hCallback)
	{
		XnStatus nRetVal = XN_STATUS_OK;

		TCallback* pCallback = (TCallback*)hCallback;

		// add it to a temp list, to allow unregistering from a callback (actual list will be updated in raise
		// function).
		{
			XnAutoCSLocker locker(m_hLock);

			// try to remove it from the ToBeAdded list.
			if (!RemoveCallback(m_toAdd, pCallback))
			{
				// it's not in this list, so it's probably in the main list
				nRetVal = m_toRemove.AddLast(pCallback);
			}
		}
		XN_IS_STATUS_OK(nRetVal);

		return XN_STATUS_OK;
	}

protected:
	typedef XnListT<TCallback*> CallbackPtrList;

	// Constructors are protected, so that this class cannot be instantiated directly.
	XnEventInterfaceT()
	{
		Init();
	}

	XnEventInterfaceT(const XnEventInterfaceT& other)
	{
		Init();
		*this = other;
	}

	XnEventInterfaceT& operator=(const XnEventInterfaceT& other)
	{
		Clear();

		// lock other one (so it won't change while being copied)
		XnAutoCSLocker otherLocker(other.m_hLock);
		// lock this one (we're making changes)
		XnAutoCSLocker locker(m_hLock);

		m_callbacks = other.m_callbacks;
		m_toAdd = other.m_toAdd;
		m_toRemove = other.m_toRemove;

		ApplyListChanges();

		return *this;
	}

	XnStatus Clear()
	{
		XnAutoCSLocker locker(m_hLock);
		ApplyListChanges();

		for (typename CallbackPtrList::ConstIterator it = m_callbacks.Begin(); it != m_callbacks.End(); ++it)
		{
			TCallback* pCallback = *it;
			XN_DELETE(pCallback);
		}

		m_callbacks.Clear();
		m_toRemove.Clear();
		m_toAdd.Clear();
		return (XN_STATUS_OK);
	}

	XnStatus ApplyListChanges()
	{
		XnAutoCSLocker locker(m_hLock);

		// first add all
		for (typename CallbackPtrList::ConstIterator it = m_toAdd.Begin(); it != m_toAdd.End(); ++it)
		{
			m_callbacks.AddLast(*it);
		}
		m_toAdd.Clear();

		// and now remove
		for (typename CallbackPtrList::ConstIterator it = m_toRemove.Begin(); it != m_toRemove.End(); ++it)
		{
			TCallback* pCallback = *it;
			RemoveCallback(m_callbacks, pCallback);
		}
		m_toRemove.Clear();

		return (XN_STATUS_OK);
	}

	XnBool RemoveCallback(CallbackPtrList& list, TCallback* pCallback)
	{
		typename CallbackPtrList::Iterator it = list.Find(pCallback);
		if (it != list.End())
		{
			list.Remove(it);
			XN_DELETE(pCallback);
			return TRUE;
		}

		return FALSE;
	}

	XN_CRITICAL_SECTION_HANDLE m_hLock;
	CallbackPtrList m_callbacks;
	CallbackPtrList m_toAdd;
	CallbackPtrList m_toRemove;

private:
	void Init()
	{
		m_hLock = NULL;
		XnStatus nRetVal = xnOSCreateCriticalSection(&m_hLock);
		if (nRetVal != XN_STATUS_OK)
		{
			XN_ASSERT(FALSE);
		}
	}
};

// Handlers
struct XnHandlerFuncNoArgs
{
	typedef void (XN_CALLBACK_TYPE* FuncPtr)(void* pCookie);
};

template<class TArg1>
struct XnHandlerFunc1Arg
{
	typedef void (XN_CALLBACK_TYPE* FuncPtr)(TArg1 arg1, void* pCookie);
};

template<class TArg1, class TArg2>
struct XnHandlerFunc2Args
{
	typedef void (XN_CALLBACK_TYPE* FuncPtr)(TArg1 arg1, TArg2 arg2, void* pCookie);
};

template<class TArg1, class TArg2, class TArg3>
struct XnHandlerFunc3Args
{
	typedef void (XN_CALLBACK_TYPE* FuncPtr)(TArg1 arg1, TArg2 arg2, TArg3 arg3, void* pCookie);
};

template<class TArg1, class TArg2, class TArg3, class TArg4>
struct XnHandlerFunc4Args
{
	typedef void (XN_CALLBACK_TYPE* FuncPtr)(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, void* pCookie);
};

template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
struct XnHandlerFunc5Args
{
	typedef void (XN_CALLBACK_TYPE* FuncPtr)(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, void* pCookie);
};

// Event classes (there's a class per number of arguments)
class XnEventNoArgs : public XnEventInterfaceT<XnHandlerFuncNoArgs::FuncPtr>
{
public:
	XnStatus Raise()
	{
		XnAutoCSLocker locker(this->m_hLock);
		ApplyListChanges();

		for (CallbackPtrList::ConstIterator it = m_callbacks.Begin(); it != m_callbacks.End(); ++it)
		{
			TCallback* pCallback = *it;
			pCallback->pFunc(pCallback->pCookie);
		}

		ApplyListChanges();
		return (XN_STATUS_OK);
	}
};


template<class TArg1>
class XnEvent1Arg : public XnEventInterfaceT<typename XnHandlerFunc1Arg<TArg1>::FuncPtr>
{
	typedef XnEventInterfaceT<typename XnHandlerFunc1Arg<TArg1>::FuncPtr> Base;

public:
	XnStatus Raise(TArg1 arg)
	{
		XnAutoCSLocker locker(this->m_hLock);
		this->ApplyListChanges();

		for (typename Base::CallbackPtrList::ConstIterator it = this->m_callbacks.Begin(); it != this->m_callbacks.End(); ++it)
		{
			typename Base::TCallback* pCallback = *it;
			pCallback->pFunc(arg, pCallback->pCookie);
		}

		this->ApplyListChanges();
		return (XN_STATUS_OK);
	}
};

template<class TEventArgs>
class XnEventT : public XnEvent1Arg<const TEventArgs&>
{};

template<class TArg1, class TArg2>
class XnEvent2Args : public XnEventInterfaceT<typename XnHandlerFunc2Args<TArg1, TArg2>::FuncPtr>
{
	typedef XnEventInterfaceT<typename XnHandlerFunc2Args<TArg1, TArg2>::FuncPtr> Base;

public:
	XnStatus Raise(TArg1 arg1, TArg2 arg2)
	{
		XnAutoCSLocker locker(this->m_hLock);
		this->ApplyListChanges();

		for (typename Base::CallbackPtrList::ConstIterator it = this->m_callbacks.Begin(); it != this->m_callbacks.End(); ++it)
		{
			typename Base::TCallback* pCallback = *it;
			pCallback->pFunc(arg1, arg2, pCallback->pCookie);
		}

		this->ApplyListChanges();
		return (XN_STATUS_OK);
	}
};

template<class TArg1, class TArg2, class TArg3>
class XnEvent3Args : public XnEventInterfaceT<typename XnHandlerFunc3Args<TArg1, TArg2, TArg3>::FuncPtr>
{
	typedef XnEventInterfaceT<typename XnHandlerFunc3Args<TArg1, TArg2, TArg3>::FuncPtr> Base;
	
public:
	XnStatus Raise(TArg1 arg1, TArg2 arg2, TArg3 arg3)
	{
		XnAutoCSLocker locker(this->m_hLock);
		this->ApplyListChanges();

		for (typename Base::CallbackPtrList::ConstIterator it = this->m_callbacks.Begin(); it != this->m_callbacks.End(); ++it)
		{
			typename Base::TCallback* pCallback = *it;
			pCallback->pFunc(arg1, arg2, arg3, pCallback->pCookie);
		}

		this->ApplyListChanges();
		return (XN_STATUS_OK);
	}
};

template<class TArg1, class TArg2, class TArg3, class TArg4>
class XnEvent4Args : public XnEventInterfaceT<typename XnHandlerFunc4Args<TArg1, TArg2, TArg3, TArg4>::FuncPtr>
{
	typedef XnEventInterfaceT<typename XnHandlerFunc4Args<TArg1, TArg2, TArg3, TArg4>::FuncPtr> Base;
	
public:
	XnStatus Raise(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4)
	{
		XnAutoCSLocker locker(this->m_hLock);
		this->ApplyListChanges();

		for (typename Base::CallbackPtrList::ConstIterator it = this->m_callbacks.Begin(); it != this->m_callbacks.End(); ++it)
		{
			typename Base::TCallback* pCallback = *it;
			pCallback->pFunc(arg1, arg2, arg3, arg4, pCallback->pCookie);
		}

		this->ApplyListChanges();
		return (XN_STATUS_OK);
	}
};

template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
class XnEvent5Args : public XnEventInterfaceT<typename XnHandlerFunc5Args<TArg1, TArg2, TArg3, TArg4, TArg5>::FuncPtr>
{
	typedef XnEventInterfaceT<typename XnHandlerFunc5Args<TArg1, TArg2, TArg3, TArg4, TArg5>::FuncPtr> Base;
	
public:
	XnStatus Raise(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5)
	{
		XnAutoCSLocker locker(this->m_hLock);
		this->ApplyListChanges();

		for (typename Base::CallbackPtrList::ConstIterator it = this->m_callbacks.Begin(); it != this->m_callbacks.End(); ++it)
		{
			typename Base::TCallback* pCallback = *it;
			pCallback->pFunc(arg1, arg2, arg3, arg4, arg5, pCallback->pCookie);
		}

		this->ApplyListChanges();
		return (XN_STATUS_OK);
	}
};

#endif // _XN_EVENT_T_H_