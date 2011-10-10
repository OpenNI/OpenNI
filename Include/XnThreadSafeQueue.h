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
#ifndef __XN_THREAD_SAFE_QUEUE_H__
#define __XN_THREAD_SAFE_QUEUE_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnQueue.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/**
* A thread safe queue.
*/
class XnThreadSafeQueue : public XnQueue
{
public:
	XnThreadSafeQueue() : m_hLock(NULL) {}

	~XnThreadSafeQueue()
	{
		xnOSCloseCriticalSection(&m_hLock);
	}

	XnStatus Init()
	{
		XnStatus nRetVal = XN_STATUS_OK;

		nRetVal = xnOSCreateCriticalSection(&m_hLock);
		XN_IS_STATUS_OK(nRetVal);

		return (XN_STATUS_OK);
	}

	XnStatus Push(XnValue const& value)
	{
		XnStatus nRetVal = XN_STATUS_OK;

		nRetVal = xnOSEnterCriticalSection(&m_hLock);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = XnQueue::Push(value);
		xnOSLeaveCriticalSection(&m_hLock);

		return nRetVal;
	}

	XnStatus Pop(XnValue& value)
	{
		XnStatus nRetVal = XN_STATUS_OK;

		nRetVal = xnOSEnterCriticalSection(&m_hLock);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = XnQueue::Pop(value);
		xnOSLeaveCriticalSection(&m_hLock);

		return nRetVal;
	}

	XnUInt32 Size() const
	{
		xnOSEnterCriticalSection(&m_hLock);
		XnUInt32 nSize = XnQueue::Size();
		xnOSLeaveCriticalSection(&m_hLock);
		return (nSize);
	}

private:
	// NOTE: we declare the lock as mutable, as it may change on const methods.
	mutable XN_CRITICAL_SECTION_HANDLE m_hLock;
};

/**
* Declares a thread safe queue of type @a Type, named @a ClassName. The class uses @a Translator for translating
* from @a Type to XnValue. It is declared using the declspec @a decl.
* It inherits from @a base. Note that @a base must be a derivative of XnQueue.
*/ 
#define XN_DECLARE_THREAD_SAFE_QUEUE_WITH_TRANSLATOR_DECL(decl, Type, ClassName, Translator)	\
	class decl ClassName : public XnThreadSafeQueue										\
	{																					\
	public:																				\
		~ClassName()																	\
		{																				\
			/* We do this using Pop() to make sure memory is freed. */					\
			Type dummy;																	\
			while (Size() != 0)															\
				Pop(dummy);																\
		}																				\
		XnStatus Push(Type const& value)												\
		{																				\
			XnValue val = Translator::CreateValueCopy(value);							\
			XnStatus nRetVal = XnThreadSafeQueue::Push(val);							\
			if (nRetVal != XN_STATUS_OK)												\
			{																			\
				Translator::FreeValue(val);												\
				return (nRetVal);														\
			}																			\
			return XN_STATUS_OK;														\
		}																				\
		XnStatus Pop(Type& value)														\
		{																				\
			XnValue val;																\
			XnStatus nRetVal = XnThreadSafeQueue::Pop(val);								\
			if (nRetVal != XN_STATUS_OK) return (nRetVal);								\
			value = Translator::GetFromValue(val);										\
			Translator::FreeValue(val);													\
			return XN_STATUS_OK;														\
		}																				\
	};

/**
* Declares a thread safe queue of type @a Type, named @a ClassName. The class uses @a Translator for translating
* from @a Type to XnValue.
* It inherits from @a base. Note that @a base must be a derivative of XnQueue.
*/ 
#define XN_DECLARE_THREAD_SAFE_QUEUE_WITH_TRANSLATOR(Type, ClassName, Translator)				\
	XN_DECLARE_THREAD_SAFE_QUEUE_WITH_TRANSLATOR_DECL(, Type, ClassName, Translator)

/**
* Declares a thread safe queue of type @a Type, named @a ClassName, that uses the default translator.
* It is declared using the declspec @a decl.
*/ 
#define XN_DECLARE_THREAD_SAFE_QUEUE_DECL(decl, Type, ClassName)											\
	XN_DECLARE_DEFAULT_VALUE_TRANSLATOR_DECL(decl, Type, XN_DEFAULT_TRANSLATOR_NAME(ClassName))				\
	XN_DECLARE_THREAD_SAFE_QUEUE_WITH_TRANSLATOR_DECL(decl, Type, ClassName, XN_DEFAULT_TRANSLATOR_NAME(ClassName))

/**
* Declares a thread safe queue of type @a Type, named @a ClassName, that uses the default translator.
*/ 
#define XN_DECLARE_THREAD_SAFE_QUEUE(Type, ClassName)		\
	XN_DECLARE_THREAD_SAFE_QUEUE_DECL(, Type, ClassName)

#endif //__XN_THREAD_SAFE_QUEUE_H__
