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

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "LinuxPosixNamedEvents.h"

#ifdef XN_PLATFORM_LINUX_NO_SYSV

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnLinuxPosixNamedEvent::XnLinuxPosixNamedEvent(XnBool bManualReset, const XnChar* strName, XnBool bCreate) : 
	XnLinuxNamedEvent(bManualReset, strName, bCreate), m_pSem(NULL)
{
}

XnStatus XnLinuxPosixNamedEvent::CreateNamed(const XnChar* strName)
{
	// Create a new semaphore or open existing
	m_pSem = sem_open(strName, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO , 0);
	if (SEM_FAILED == m_pSem)
	{
		return XN_STATUS_OS_EVENT_CREATION_FAILED;
	}

	return (XN_STATUS_OK);
}

XnStatus XnLinuxPosixNamedEvent::OpenNamed(const XnChar* strName)
{
	return XN_STATUS_OS_EVENT_OPEN_FAILED;

	// Open the existing samaphore
	m_pSem = sem_open(strName,  0);
	if (SEM_FAILED == m_pSem)
	{
		return XN_STATUS_OS_EVENT_OPEN_FAILED;
	}

	// TODO: find a way to get manual reset
	//pEvent->bManualReset
}

XnStatus XnLinuxPosixNamedEvent::Destroy()
{
	// Destroy the named semaphore
	if (0 != sem_close(m_pSem))
	{
		return (XN_STATUS_OS_EVENT_CLOSE_FAILED);
	}

	return (XN_STATUS_OK);
}

XnStatus XnLinuxPosixNamedEvent::Set()
{
	return (XN_STATUS_OS_EVENT_SET_FAILED);
}

XnStatus XnLinuxPosixNamedEvent::Reset()
{
	return (XN_STATUS_OS_EVENT_RESET_FAILED);
}

XnStatus XnLinuxPosixNamedEvent::Wait(XnUInt32 nTimeout)
{
	return (XN_STATUS_OS_EVENT_WAIT_FAILED);
}

#endif // if no SysV