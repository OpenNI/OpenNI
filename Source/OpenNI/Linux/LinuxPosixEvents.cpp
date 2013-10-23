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
#include "LinuxPosixEvents.h"

XnLinuxPosixEvent::XnLinuxPosixEvent(XnBool bManualReset) : XnLinuxEvent(bManualReset)
{

}

XnStatus XnLinuxPosixEvent::Init()
{
	// Create a cond object
	if (0 != pthread_cond_init(&m_cond, NULL))
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_OS_EVENT_CREATION_FAILED, XN_MASK_OS, "Failed to create event: cond_init returned %d", errno);
	}

	// create the restriction mutex object
	if (0 != pthread_mutex_init(&m_mutex, NULL))
	{
		pthread_cond_destroy(&m_cond);
		XN_LOG_WARNING_RETURN(XN_STATUS_OS_EVENT_CREATION_FAILED, XN_MASK_OS, "Failed to create event: mutex_init returned %d", errno);
	}

	return XN_STATUS_OK;
}

XnStatus XnLinuxPosixEvent::Destroy()
{
	// Destroy the cond and mutex objects
	if (0 != pthread_cond_destroy(&m_cond) || 0 != pthread_mutex_destroy(&m_mutex))
	{
		return (XN_STATUS_OS_EVENT_CLOSE_FAILED);
	}

	return (XN_STATUS_OK);
}

XnStatus XnLinuxPosixEvent::Set()
{
	// lock the mutex
	if (0 != pthread_mutex_lock(&m_mutex))
	{
		return (XN_STATUS_OS_EVENT_SET_FAILED);
	}

	// signal the condition
	m_bSignaled = TRUE;

	// wake other threads
	if (0 != pthread_cond_broadcast(&m_cond))
	{
		pthread_mutex_unlock(&m_mutex);
		return XN_STATUS_OS_EVENT_SET_FAILED;
	}

	// unlock the mutex
	if (0 != pthread_mutex_unlock(&m_mutex))
	{
		return (XN_STATUS_OS_EVENT_SET_FAILED);
	}

	return (XN_STATUS_OK);
}

XnStatus XnLinuxPosixEvent::Reset()
{
	m_bSignaled = FALSE;
	
	return (XN_STATUS_OK);
}

XnStatus XnLinuxPosixEvent::Wait(XnUInt32 nMilliseconds)
{
	XnStatus nRetVal = XN_STATUS_OK;

	struct timespec time = {0};
	if (nMilliseconds != XN_WAIT_INFINITE)
	{
		nRetVal = xnOSGetAbsTimeout(&time, nMilliseconds);
		if (nRetVal != XN_STATUS_OK)
		{
			return XN_STATUS_OS_EVENT_WAIT_FAILED;
		}
	}

	// lock the mutex
	if (0 != pthread_mutex_lock(&m_mutex))
	{
		return (XN_STATUS_OS_EVENT_WAIT_FAILED);
	}

	// wait for the condition to become signaled (or a failure)
	int rc;
	while (nRetVal == XN_STATUS_OK && !m_bSignaled)
	{
		if (nMilliseconds != XN_WAIT_INFINITE)
		{
			rc = pthread_cond_timedwait(&m_cond, &m_mutex, &time);
		}
		else // infinite
		{
			rc = pthread_cond_wait(&m_cond, &m_mutex);
		}

		if (rc == ETIMEDOUT)
		{
			nRetVal = XN_STATUS_OS_EVENT_TIMEOUT;
		}
		else if (rc != 0)
		{
			nRetVal = XN_STATUS_OS_EVENT_WAIT_FAILED;
		}
	}

	// check all went well
	if (nRetVal != XN_STATUS_OK)
	{
		pthread_mutex_unlock(&m_mutex);
		return nRetVal;	
	}

	if (!m_bManualReset)
	{
		// auto-reset the event
		m_bSignaled = FALSE;
	}

	// unlock the mutex
	if (0 != pthread_mutex_unlock(&m_mutex))
	{
		return (XN_STATUS_OS_EVENT_WAIT_FAILED);
	}

	return (XN_STATUS_OK);
}