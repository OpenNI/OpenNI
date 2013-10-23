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
#ifndef __XN_OS_CPP_H__
#define __XN_OS_CPP_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnAutoCSLocker
{
public:
	inline XnAutoCSLocker(const XnAutoCSLocker& other) : m_hCS(other.m_hCS), m_bLocked(FALSE)
	{
		Lock();
	}

	inline XnAutoCSLocker& operator=(const XnAutoCSLocker& other)
	{
		Unlock();
		m_hCS = other.m_hCS;
		Lock();
		return *this;
	}

	inline XnAutoCSLocker(XN_CRITICAL_SECTION_HANDLE hCS) : m_hCS(hCS), m_bLocked(FALSE)
	{
		Lock();
	}

	inline ~XnAutoCSLocker()
	{
		Unlock();
	}

	inline void Lock()
	{
		if (!m_bLocked)
		{
			xnOSEnterCriticalSection(&m_hCS);
			m_bLocked = TRUE;
		}
	}

	inline void Unlock()
	{
		if (m_bLocked)
		{
			xnOSLeaveCriticalSection(&m_hCS);
			m_bLocked = FALSE;
		}
	}

private:
	XN_CRITICAL_SECTION_HANDLE m_hCS;
	XnBool m_bLocked;
};

class XnAutoMutexLocker
{
public:
	inline XnAutoMutexLocker(XN_MUTEX_HANDLE hMutex, XnUInt32 nMilliseconds) : m_hMutex(hMutex)
	{
		m_nStatus = xnOSLockMutex(m_hMutex, nMilliseconds);
	}

	XnStatus GetStatus() const 
	{ 
		return m_nStatus; 
	}

	inline ~XnAutoMutexLocker()
	{
		if (m_nStatus == XN_STATUS_OK)
		{
			//Only unlock if we managed to lock in the first place
			xnOSUnLockMutex(m_hMutex);
		}
	}

private:
	XN_MUTEX_HANDLE m_hMutex;
	XnStatus m_nStatus;
};

class XnOSEvent
{
public:
	XnOSEvent() : m_hEvent(NULL) {}
	
	~XnOSEvent() 
	{ 
		Close(); 
	}

	operator XN_EVENT_HANDLE() const 
	{
		return m_hEvent;
	}

	XnStatus Create(XnBool bManualReset)
	{
		return xnOSCreateEvent(&m_hEvent, bManualReset);
	}

	XnStatus Create(const XnChar* strName, XnBool bManualReset, XnBool bAllowOtherUsers = FALSE)
	{
		return xnOSCreateNamedEventEx(&m_hEvent, strName, bManualReset, bAllowOtherUsers);
	}

	XnStatus Open(const XnChar* strName, XnBool bEnableOtherUsers = FALSE)
	{
		return xnOSOpenNamedEventEx(&m_hEvent, strName, bEnableOtherUsers);
	}

	XnStatus Close()
	{
		return (m_hEvent != NULL) ? xnOSCloseEvent(&m_hEvent) : XN_STATUS_OK;
	}

	XnStatus Set()
	{
		return xnOSSetEvent(m_hEvent);
	}

	XnStatus Reset()
	{
		return xnOSResetEvent(m_hEvent);
	}

	XnStatus Wait(XnUInt32 nMilliseconds)
	{
		return xnOSWaitEvent(m_hEvent, nMilliseconds);
	}

private:
	XN_EVENT_HANDLE m_hEvent;
};

#endif // __XN_OS_CPP_H__
