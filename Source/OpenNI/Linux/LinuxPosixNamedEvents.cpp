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