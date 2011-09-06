/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
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
			XnStatus nRetVal = xnOSEnterCriticalSection(&m_hCS);
			XN_ASSERT(nRetVal == XN_STATUS_OK);
			m_bLocked = TRUE;
		}
	}

	inline void Unlock()
	{
		if (m_bLocked)
		{
			XnStatus nRetVal = xnOSLeaveCriticalSection(&m_hCS);
			XN_ASSERT(nRetVal == XN_STATUS_OK);
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

	XnStatus Create(const XnChar* strName, XnBool bManualReset)
	{
		return xnOSCreateNamedEvent(&m_hEvent, strName, bManualReset);
	}

	XnStatus Open(const XnChar* strName)
	{
		return xnOSOpenNamedEvent(&m_hEvent, strName);
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
