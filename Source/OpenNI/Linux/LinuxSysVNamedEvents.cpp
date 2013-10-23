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
#include "LinuxSysVNamedEvents.h"

#ifndef XN_PLATFORM_LINUX_NO_SYSV

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
enum _XN_EVENT_SEM_TYPE
{
	XN_EVENT_SEM_REF_COUNT = 0,
	XN_EVENT_SEM_SIGNALED,
	XN_EVENT_SEM_MANUAL_RESET,
	XN_EVENT_NUM_OF_SEMS
};

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
#ifndef XN_PLATFORM_HAS_BUILTIN_SEMUN
union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };
#endif

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnLinuxSysVNamedEvent::XnLinuxSysVNamedEvent(XnBool bManualReset, const XnChar* strName, XnBool bCreate) : XnLinuxNamedEvent(bManualReset, strName, bCreate)
{
}

XnStatus XnLinuxSysVNamedEvent::CreateNamed(const XnChar* strName)
{
	// translate event name to key file name
	XnUInt32 nBytesWritten;
	xnOSStrFormat(m_csSemFileName, XN_FILE_MAX_PATH, &nBytesWritten, "/tmp/XnCore.Event.%s.key", strName);

	// create the file (must exist for ftok)
	m_hSemFile = open(m_csSemFileName, O_CREAT | O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
	if (-1 == m_hSemFile)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_OS_EVENT_CREATION_FAILED, XN_MASK_OS, "Create named event: failed to create key file (%d)", errno);
	}

	// create the key
	key_t key = ftok(m_csSemFileName, 1);

	// Try to create the semaphore
	// Creating two semaphores. First semaphore will be used as reference count, and second for the signaled state
	m_hSem = semget(key, XN_EVENT_NUM_OF_SEMS, IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (-1 == m_hSem)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_OS_EVENT_CREATION_FAILED, XN_MASK_OS, "Create named event: failed to create semaphore (%d)", errno);
	}

	// if no one is using it so far (this is a newly created event), reset it
	if (0 == semctl(m_hSem, XN_EVENT_SEM_REF_COUNT, GETVAL))
	{
		// set it to the non-signaled state
		semun val;
		val.val = 0;
		if (0 != semctl(m_hSem, XN_EVENT_SEM_SIGNALED, SETVAL, val))
		{
			xnLogWarning(XN_MASK_OS, "Create named event: semctl for signaled failed (%d)", errno);
			semctl(m_hSem, 0, IPC_RMID);
			return XN_STATUS_OS_EVENT_CREATION_FAILED;
		}

		// set its manual reset value
		val.val = m_bManualReset;
		if (0 != semctl(m_hSem, XN_EVENT_SEM_MANUAL_RESET, SETVAL, val))
		{
			xnLogWarning(XN_MASK_OS, "Create named event: semctl for manual reset failed (%d)", errno);
			semctl(m_hSem, 0, IPC_RMID);
			return XN_STATUS_OS_EVENT_CREATION_FAILED;
		}
	}

	// adding 1 to reference count
	struct sembuf op;
	op.sem_op = 1;
	op.sem_num = XN_EVENT_SEM_REF_COUNT;
	op.sem_flg = SEM_UNDO;
	semop(m_hSem, &op, 1);

	m_bManualReset = semctl(m_hSem, XN_EVENT_SEM_MANUAL_RESET, GETVAL);

	return (XN_STATUS_OK);
}

XnStatus XnLinuxSysVNamedEvent::OpenNamed(const XnChar* strName)
{
	// translate event name to key file name
	XnUInt32 nBytesWritten;
	xnOSStrFormat(m_csSemFileName, XN_FILE_MAX_PATH, &nBytesWritten, "/tmp/XnCore.Event.%s.key", strName);

	// create the key
	key_t key = ftok(m_csSemFileName, 1);
	if (-1 == key)
	{
		// event does not exist
		XN_LOG_WARNING_RETURN(XN_STATUS_OS_EVENT_OPEN_FAILED, XN_MASK_OS, "Open named event: failed to open key file (%d) - event might not exist...", errno);
	}

	// Try to open the semaphore
	m_hSem = semget(key, XN_EVENT_NUM_OF_SEMS, 0);
	if (-1 == m_hSem)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_OS_EVENT_OPEN_FAILED, XN_MASK_OS, "Open named event: failed to get semaphore (%d)", errno);
	}

	// if ref count is 0, the event does not really exist
	if (0 == semctl(m_hSem, XN_EVENT_SEM_REF_COUNT, GETVAL))
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_OS_EVENT_OPEN_FAILED, XN_MASK_OS, "Open named event: ref count is zero - event does not exist!", errno);
	}

	// adding 1 to reference count
	struct sembuf op;
	op.sem_op = 1;
	op.sem_num = XN_EVENT_SEM_REF_COUNT;
	op.sem_flg = SEM_UNDO;
	semop(m_hSem, &op, 1);

	m_bManualReset = semctl(m_hSem, XN_EVENT_SEM_MANUAL_RESET, GETVAL);
	
	return XN_STATUS_OK;
}

XnStatus XnLinuxSysVNamedEvent::Destroy()
{
	// dec ref count
	struct sembuf op;
	op.sem_num = XN_EVENT_SEM_REF_COUNT;
	op.sem_op = -1;
	op.sem_flg = IPC_NOWAIT | SEM_UNDO;
	semop(m_hSem, &op, 1); 

	// check current ref count. If 0, destroy it
	int val = semctl(m_hSem, 0, GETVAL);
	if (0 == val)
	{
		// destroy the semaphore
		semctl(m_hSem, 0, IPC_RMID);
		// and remove file
		xnOSDeleteFile(m_csSemFileName);
	}

	return (XN_STATUS_OK);
}

XnStatus XnLinuxSysVNamedEvent::Set()
{
	struct sembuf op[2];

	// check if it's not signaled (by waiting for 0, with IPC_NOWAIT)
	op[0].sem_num = XN_EVENT_SEM_SIGNALED;
	op[0].sem_op = 0;
	op[0].sem_flg = IPC_NOWAIT;

	// signal (if zero)
	op[1].sem_num = XN_EVENT_SEM_SIGNALED;
	op[1].sem_op = 1;
	op[1].sem_flg = SEM_UNDO;

	semop(m_hSem, op, 2);
	// no need to check error code. either it succeeded, or the event is already signaled

	return (XN_STATUS_OK);
}

XnStatus XnLinuxSysVNamedEvent::Reset()
{
	// unsignal it, with NOWAIT (that way, if it's not signaled, nothing will happen)
	struct sembuf op;
	op.sem_num = XN_EVENT_SEM_SIGNALED;
	op.sem_op = -1;
	op.sem_flg = IPC_NOWAIT;
	semop(m_hSem, &op, 1);

	return XN_STATUS_OK;
}

XnStatus XnLinuxSysVNamedEvent::Wait(XnUInt32 nMilliseconds)
{
	XnStatus nRetVal = XN_STATUS_OK;

	struct timespec time = {0};
	if (nMilliseconds != XN_WAIT_INFINITE)
	{
		nRetVal = xnOSGetTimeout(&time, nMilliseconds);
		if (nRetVal != XN_STATUS_OK)
		{
			return XN_STATUS_OS_EVENT_WAIT_FAILED;
		}
	}

	struct sembuf op[2];

	// wait for it to be 1
	op[0].sem_num = XN_EVENT_SEM_SIGNALED;
	op[0].sem_op = -1;
	op[0].sem_flg = 0;

	// re set it (if manual reset)
	op[1].sem_num = XN_EVENT_SEM_SIGNALED;
	op[1].sem_op = 1;
	op[1].sem_flg = 0;

	XnInt32 nOpsCount = m_bManualReset ? 2 : 1;

	if (XN_WAIT_INFINITE != nMilliseconds)
	{
#if _GNU_SOURCE
		if (0 != semtimedop(m_hSem, op, nOpsCount, &time))
#else
		if (0 != semop(m_hSem, op, nOpsCount))	
#endif
		{
			if(EAGAIN == errno)
			{
				return XN_STATUS_OS_EVENT_TIMEOUT;
			}
			return XN_STATUS_OS_EVENT_WAIT_FAILED;
		}
	}
	else
	{
		if (0 != semop(m_hSem, op, nOpsCount))
		{
			return XN_STATUS_OS_EVENT_WAIT_FAILED;
		}
	}

	return (XN_STATUS_OK);
}

#endif // if we have SysV