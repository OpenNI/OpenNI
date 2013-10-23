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
