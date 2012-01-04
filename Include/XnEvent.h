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
#ifndef __XN_EVENT_H__
#define __XN_EVENT_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnCallback.h"
#include "XnList.h"
#include "XnTypes.h"
#include "XnOSCpp.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnEventInterface
{
public:
	XnEventInterface() {}
	virtual ~XnEventInterface() {}

	typedef XnStatus (XN_CALLBACK_TYPE* HandlerPtr)(void* pCookie);

	virtual XnStatus Register(HandlerPtr pFunc, void* pCookie, XnCallbackHandle* pHandle = NULL) = 0;
	virtual XnStatus Unregister(XnCallbackHandle handle) = 0;
};

class XnEvent : public XnEventInterface
{
public:
	XnEvent() : m_hLock(0) 
	{
		xnOSCreateCriticalSection(&m_hLock);
	}

	virtual ~XnEvent()
	{
		Clear();
		xnOSCloseCriticalSection(&m_hLock);
	}

	XnStatus Register(HandlerPtr pFunc, void* pCookie, XnCallbackHandle* pHandle)
	{
		XnStatus nRetVal = XN_STATUS_OK;

		XN_VALIDATE_INPUT_PTR(pFunc);

		XnCallback* pCallback = NULL;
		XN_VALIDATE_NEW(pCallback, XnCallback, (XnFuncPtr)pFunc, pCookie);

		// always add to list of added (actual list will be updated in Raise method, to allow registering 
		// from a callback).
		{
			XnAutoCSLocker lock(m_hLock);
			nRetVal = m_ToBeAdded.AddLast(pCallback);
		}

		if (nRetVal != XN_STATUS_OK)
		{
			XN_DELETE(pCallback);
			return (nRetVal);
		}

		// return handle
		if (pHandle != NULL)
		{
			*pHandle = pCallback;
		}

		return XN_STATUS_OK;
	}

	XnStatus Unregister(XnCallbackHandle handle)
	{
		XnStatus nRetVal = XN_STATUS_OK;

		XnCallback* pObject = (XnCallback*)handle;

		// add it to a temp list, to allow unregistering from a callback (actual list will be updated in raise
		// function).
		{
			XnAutoCSLocker lock(m_hLock);

			// try to remove it from the ToBeAdded list.
			if (!RemoveCallback(m_ToBeAdded, pObject))
			{
				// it's not in this list, so it's probably in the main list
				nRetVal = m_ToBeRemoved.AddLast(pObject);
			}
		}
		XN_IS_STATUS_OK(nRetVal);

		return XN_STATUS_OK;
	}

	XnStatus Clear()
	{
		ApplyListChanges();

		for (XnCallbackPtrList::ConstIterator it = m_Handlers.begin(); it != m_Handlers.end(); ++it)
		{
			XnCallback* pCallback = *it;
			XN_DELETE(pCallback);
		}

		m_Handlers.Clear();
		m_ToBeRemoved.Clear();
		m_ToBeAdded.Clear();
		return (XN_STATUS_OK);
	}

protected:
	XnStatus ApplyListChanges()
	{
		// first add all
		for (XnCallbackPtrList::ConstIterator it = m_ToBeAdded.begin(); it != m_ToBeAdded.end(); ++it)
		{
			m_Handlers.AddLast(*it);
		}
		m_ToBeAdded.Clear();

		// and now remove
		for (XnCallbackPtrList::ConstIterator it = m_ToBeRemoved.begin(); it != m_ToBeRemoved.end(); ++it)
		{
			XnCallback* pCallback = *it;
			RemoveCallback(m_Handlers, pCallback);
		}
		m_ToBeRemoved.Clear();

		return (XN_STATUS_OK);
	}

#if (XN_PLATFORM == XN_PLATFORM_WIN32)
#pragma warning (push)
#pragma warning (disable: 4127)
#endif

	XN_DECLARE_LIST(XnCallback*, XnCallbackPtrList)

#if (XN_PLATFORM == XN_PLATFORM_WIN32)
#pragma warning (pop)
#endif

	XN_CRITICAL_SECTION_HANDLE m_hLock;
	XnCallbackPtrList m_Handlers;
	XnCallbackPtrList m_ToBeAdded;
	XnCallbackPtrList m_ToBeRemoved;

private:
	XnBool RemoveCallback(XnCallbackPtrList& list, XnCallback* pCallback)
	{
		XnCallbackPtrList::Iterator handlerIt = list.Find(pCallback);
		if (handlerIt != list.end())
		{
			list.Remove(handlerIt);
			XN_DELETE(pCallback);
			return TRUE;
		}

		return FALSE;
	}
};

#define _XN_RAISE_WITH_RET_CODE(args)						\
	{														\
		XnStatus nRetVal = pFunc(args pCallback->pCookie);	\
		if (nRetVal != XN_STATUS_OK)						\
		{													\
			XnEvent::ApplyListChanges();					\
			return (nRetVal);								\
		}													\
	}

#define _XN_RAISE_NO_RET_CODE(args)						\
	pFunc(args pCallback->pCookie);

/**
* Declares a class inheriting from XnEvent, that can be used as an event with specific signature.
* It is highly recommended not to use this macro directly.
*/
#define _XN_DECLARE_EVENT_CLASS(_class, _interface, _retVal, _raise, _signature, _raise_sign)				\
	XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_HIDES_PARENT_METHOD_WARNING_ID)								\
	class _interface : protected XnEvent																	\
	{																										\
	public:																									\
		typedef _retVal (XN_CALLBACK_TYPE* HandlerPtr)(_signature);											\
		virtual XnStatus Register(HandlerPtr pFunc, void* pCookie, XnCallbackHandle* pHandle = NULL) = 0;	\
		virtual XnStatus Unregister(XnCallbackHandle handle) = 0;											\
	};																										\
	class _class : public _interface																		\
	{																										\
	public:																									\
		XnStatus Register(HandlerPtr pFunc, void* pCookie, XnCallbackHandle* pHandle = NULL)				\
		{																									\
			return XnEvent::Register((XnEvent::HandlerPtr)pFunc, pCookie, pHandle);							\
		}																									\
		XnStatus Unregister(XnCallbackHandle handle)														\
		{																									\
			return XnEvent::Unregister(handle);																\
		}																									\
		XnStatus Raise(_raise_sign)																			\
		{																									\
			XnAutoCSLocker lock(m_hLock);																	\
			XnEvent::ApplyListChanges();																	\
			XnEvent::XnCallbackPtrList::Iterator it = XnEvent::m_Handlers.begin();							\
			for (; it != XnEvent::m_Handlers.end(); ++it)													\
			{																								\
				XnCallback* pCallback = *it;																\
				HandlerPtr pFunc = (HandlerPtr)pCallback->pFuncPtr;											\
				_raise																						\
			}																								\
			XnEvent::ApplyListChanges();																	\
			return (XN_STATUS_OK);																			\
		}																									\
		XnStatus Clear() { return XnEvent::Clear(); }														\
	};																										\
	XN_PRAGMA_STOP_DISABLED_WARNING_SECTION


#define _XN_SIGNATURE_0ARG()
#define _XN_FULL_SIGNATURE_0ARG()	void* pCookie
#define _XN_ARGS_0ARG()

#define _XN_SIGNATURE_1ARG(_type1, _name1)	_type1 _name1
#define _XN_FULL_SIGNATURE_1ARG(_type1, _name1)	_type1 _name1, void* pCookie
#define _XN_ARGS_1ARG(_name1) _name1,

#define _XN_SIGNATURE_2ARG(_type1, _name1, _type2, _name2)	_type1 _name1, _type2 _name2
#define _XN_FULL_SIGNATURE_2ARG(_type1, _name1, _type2, _name2)	_type1 _name1, _type2 _name2, void* pCookie
#define _XN_ARGS_2ARG(_name1, _name2) _name1, _name2,

#define _XN_SIGNATURE_3ARG(_type1, _name1, _type2, _name2, _type3, _name3)	_type1 _name1, _type2 _name2, _type3 _name3
#define _XN_FULL_SIGNATURE_3ARG(_type1, _name1, _type2, _name2, _type3, _name3)	_type1 _name1, _type2 _name2, _type3 _name3, void* pCookie
#define _XN_ARGS_3ARG(_name1, _name2, _name3) _name1, _name2, _name3,

#define _XN_SIGNATURE_4ARG(_type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4)	_type1 _name1, _type2 _name2, _type3 _name3, _type4 _name4
#define _XN_FULL_SIGNATURE_4ARG(_type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4)	_type1 _name1, _type2 _name2, _type3 _name3, _type4 _name4, void* pCookie
#define _XN_ARGS_4ARG(_name1, _name2, _name3, _name4) _name1, _name2, _name3, _name4,

#define _XN_SIGNATURE_5ARG(_type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4, _type5, _name5)	_type1 _name1, _type2 _name2, _type3 _name3, _type4 _name4, _type5 _name5
#define _XN_FULL_SIGNATURE_5ARG(_type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4, _type5, _name5)	_type1 _name1, _type2 _name2, _type3 _name3, _type4 _name4, _type5 _name5, void* pCookie
#define _XN_ARGS_5ARG(_name1, _name2, _name3, _name4, _name5) _name1, _name2, _name3, _name4, _name5,

/** Declares an event class */
#define XN_DECLARE_EVENT_0ARG(_class, _interface)											\
	_XN_DECLARE_EVENT_CLASS(_class, _interface, void, _XN_RAISE_NO_RET_CODE(_XN_ARGS_0ARG()), _XN_FULL_SIGNATURE_0ARG(), _XN_SIGNATURE_0ARG())

#define XN_DECLARE_EVENT_0ARG_RETVAL(_class, _interface)									\
	_XN_DECLARE_EVENT_CLASS(_class, _interface, XnStatus, _XN_RAISE_WITH_RET_CODE(_XN_ARGS_0ARG()), _XN_FULL_SIGNATURE_0ARG(), _XN_SIGNATURE_0ARG())

#define XN_DECLARE_EVENT_1ARG(_class, _interface, _type1, _name1)											\
	_XN_DECLARE_EVENT_CLASS(_class, _interface, void, _XN_RAISE_NO_RET_CODE(_XN_ARGS_1ARG(_name1)), _XN_FULL_SIGNATURE_1ARG(_type1, _name1), _XN_SIGNATURE_1ARG(_type1, _name1))

#define XN_DECLARE_EVENT_1ARG_RETVAL(_class, _interface, _type1, _name1)									\
	_XN_DECLARE_EVENT_CLASS(_class, _interface, XnStatus, _XN_RAISE_WITH_RET_CODE(_XN_ARGS_1ARG(_name1)), _XN_FULL_SIGNATURE_1ARG(_type1, _name1), _XN_SIGNATURE_1ARG(_type1, _name1))

#define XN_DECLARE_EVENT_2ARG(_class, _interface, _type1, _name1, _type2, _name2)							\
	_XN_DECLARE_EVENT_CLASS(_class, _interface, void, _XN_RAISE_NO_RET_CODE(_XN_ARGS_2ARG(_name1, _name2)), _XN_FULL_SIGNATURE_2ARG(_type1, _name1, _type2, _name2), _XN_SIGNATURE_2ARG(_type1, _name1, _type2, _name2))

#define XN_DECLARE_EVENT_2ARG_RETVAL(_class, _interface, _type1, _name1, _type2, _name2)					\
	_XN_DECLARE_EVENT_CLASS(_class, _interface, XnStatus, _XN_RAISE_WITH_RET_CODE(_XN_ARGS_2ARG(_name1, _name2)), _XN_FULL_SIGNATURE_2ARG(_type1, _name1, _type2, _name2), _XN_SIGNATURE_2ARG(_type1, _name1, _type2, _name2))

#define XN_DECLARE_EVENT_3ARG(_class, _interface, _type1, _name1, _type2, _name2, _type3, _name3)			\
	_XN_DECLARE_EVENT_CLASS(_class, _interface, void, _XN_RAISE_NO_RET_CODE(_XN_ARGS_3ARG(_name1, _name2, _name3)), _XN_FULL_SIGNATURE_3ARG(_type1, _name1, _type2, _name2, _type3, _name3), _XN_SIGNATURE_3ARG(_type1, _name1, _type2, _name2, _type3, _name3))

#define XN_DECLARE_EVENT_3ARG_RETVAL(_class, _interface, _type1, _name1, _type2, _name2, _type3, _name3)	\
	_XN_DECLARE_EVENT_CLASS(_class, _interface, XnStatus, _XN_RAISE_WITH_RET_CODE(_XN_ARGS_3ARG(_name1, _name2, _name3)), _XN_FULL_SIGNATURE_3ARG(_type1, _name1, _type2, _name2, _type3, _name3), _XN_SIGNATURE_3ARG(_type1, _name1, _type2, _name2, _type3, _name3))

#define XN_DECLARE_EVENT_4ARG(_class, _interface, _type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4)			\
	_XN_DECLARE_EVENT_CLASS(_class, _interface, void, _XN_RAISE_NO_RET_CODE(_XN_ARGS_4ARG(_name1, _name2, _name3, _name4)), _XN_FULL_SIGNATURE_4ARG(_type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4), _XN_SIGNATURE_4ARG(_type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4))

#define XN_DECLARE_EVENT_4ARG_RETVAL(_class, _interface, _type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4)			\
	_XN_DECLARE_EVENT_CLASS(_class, _interface, XnStatus, _XN_RAISE_WITH_RET_CODE(_XN_ARGS_4ARG(_name1, _name2, _name3, _name4)), _XN_FULL_SIGNATURE_4ARG(_type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4), _XN_SIGNATURE_4ARG(_type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4))

#define XN_DECLARE_EVENT_5ARG(_class, _interface, _type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4, _type5, _name5)			\
	_XN_DECLARE_EVENT_CLASS(_class, _interface, void, _XN_RAISE_NO_RET_CODE(_XN_ARGS_5ARG(_name1, _name2, _name3, _name4, _name5)), _XN_FULL_SIGNATURE_5ARG(_type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4, _type5, _name5), _XN_SIGNATURE_5ARG(_type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4, _type5, _name5))

#define XN_DECLARE_EVENT_5ARG_RETVAL(_class, _interface, _type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4, _type5, _name5)			\
	_XN_DECLARE_EVENT_CLASS(_class, _interface, XnStatus, _XN_RAISE_WITH_RET_CODE(_XN_ARGS_5ARG(_name1, _name2, _name3, _name4, _name5)), _XN_FULL_SIGNATURE_5ARG(_type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4, _type5, _name5), _XN_SIGNATURE_5ARG(_type1, _name1, _type2, _name2, _type3, _name3, _type4, _name4, _type5, _name5))

#endif //__XN_EVENT_H__
