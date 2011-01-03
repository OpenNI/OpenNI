/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <XnLog.h>

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
XN_C_API XnStatus xnOSCreateEvent(XN_EVENT_HANDLE* pEventHandle, XnBool bManualReset)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pEventHandle);
	
	// allocate event struct
	XN_VALIDATE_ALLOC(*pEventHandle, _XnEvent);
	
	XN_EVENT_HANDLE pEvent = *pEventHandle;
	pEvent->bSignaled = FALSE;

	// Create a cond object
	if (0 != pthread_cond_init(&pEvent->cond, NULL))
	{
		XN_FREE_AND_NULL(*pEventHandle);
		return (XN_STATUS_OS_EVENT_CREATION_FAILED);
	}
	
	// create the restriction mutex object
	if (0 != pthread_mutex_init(&pEvent->mutex, NULL))
	{
		XN_FREE_AND_NULL(*pEventHandle);
		pthread_cond_destroy(&pEvent->cond);
		return (XN_STATUS_OS_EVENT_CREATION_FAILED);
	}

	pEvent->bManualReset = bManualReset;
	pEvent->bNamed = FALSE;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateNamedEvent(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName, XnBool bManualReset)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pEventHandle);
	
	// allocate event struct
	XN_VALIDATE_ALLOC(*pEventHandle, _XnEvent);
	
	XN_EVENT_HANDLE pEvent = *pEventHandle;
	pEvent->bSignaled = FALSE;

	// translate event name to key file name
	XnUInt32 nBytesWritten;
	xnOSStrFormat(pEvent->csSemFileName,XN_FILE_MAX_PATH,&nBytesWritten,"/tmp/XnCore.Event.%s.key", cpEventName);

	// create the file (must exist for ftok)
	pEvent->hSemFile = open(pEvent->csSemFileName, O_CREAT | O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
	if (-1 == pEvent->hSemFile)
	{
		XN_FREE_AND_NULL(*pEventHandle);
		return XN_STATUS_OS_FILE_OPEN_FAILED;
	}

	// create the key
	key_t key = ftok(pEvent->csSemFileName,1);
	
	// Try to create the semaphore
	// Creating two semaphores. First semaphore will be used as reference count, and second for the signaled state
	pEvent->NamedSem = semget(key, XN_EVENT_NUM_OF_SEMS, IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (-1 == pEvent->NamedSem)
	{
		xnLogWarning(XN_MASK_OS, "Failed creating named event - semget failed with errno %d", errno);
		XN_FREE_AND_NULL(*pEventHandle);
		return XN_STATUS_OS_EVENT_CREATION_FAILED;
	}
	
	// if no one is using it so far (this is a newly created event), reset it
	if (0 == semctl(pEvent->NamedSem, XN_EVENT_SEM_REF_COUNT, GETVAL))
	{
		// set it to the non-signaled state
		semun val;
		val.val = 0;
		if (0 != semctl(pEvent->NamedSem, XN_EVENT_SEM_SIGNALED, SETVAL, val))
		{
			xnLogWarning(XN_MASK_OS, "Failed creating named event - semctl for signaled failed with errno %d", errno);
			semctl(pEvent->NamedSem, 0, IPC_RMID);
			XN_FREE_AND_NULL(*pEventHandle);
			return XN_STATUS_OS_EVENT_CREATION_FAILED;
		}
		
		// set its manual reset value
		val.val = bManualReset;
		if (0 != semctl(pEvent->NamedSem, XN_EVENT_SEM_MANUAL_RESET, SETVAL, val))
		{
			xnLogWarning(XN_MASK_OS, "Failed creating named event - semctl for manual reset failed with errno %d", errno);
			semctl(pEvent->NamedSem, 0, IPC_RMID);
			XN_FREE_AND_NULL(*pEventHandle);
			return XN_STATUS_OS_EVENT_CREATION_FAILED;
		}
	}
	
	// adding 1 to reference count
	struct sembuf op;
	op.sem_op = 1;
	op.sem_num = XN_EVENT_SEM_REF_COUNT;
	op.sem_flg = SEM_UNDO;
	semop(pEvent->NamedSem, &op, 1);

	pEvent->bManualReset = semctl(pEvent->NamedSem, XN_EVENT_SEM_MANUAL_RESET, GETVAL);
	pEvent->bNamed = TRUE;
	
	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSOpenNamedEvent(XN_EVENT_HANDLE* pEventHandle, const XnChar* cpEventName)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pEventHandle);
	
	// allocate event struct
	XN_VALIDATE_ALLOC(*pEventHandle, _XnEvent);
	
	XN_EVENT_HANDLE pEvent = *pEventHandle;
	pEvent->bSignaled = FALSE;

	// translate event name to key file name
	XnUInt32 nBytesWritten;
	xnOSStrFormat(pEvent->csSemFileName,XN_FILE_MAX_PATH,&nBytesWritten,"/tmp/XnCore.Event.%s.key", cpEventName);

	// create the key
	key_t key = ftok(pEvent->csSemFileName,1);
	if (-1 == key)
	{
		// event does not exist
		XN_FREE_AND_NULL(*pEventHandle);
		return XN_STATUS_OS_EVENT_OPEN_FAILED;
	}
	
	// Try to open the semaphore
	pEvent->NamedSem = semget(key, XN_EVENT_NUM_OF_SEMS, 0);
	if (-1 == pEvent->NamedSem)
	{
		XN_FREE_AND_NULL(*pEventHandle);
		return XN_STATUS_OS_EVENT_OPEN_FAILED;
	}
	
	// if ref count is 0, the event does not really exist
	if (0 == semctl(pEvent->NamedSem, XN_EVENT_SEM_REF_COUNT, GETVAL))
	{
		XN_FREE_AND_NULL(*pEventHandle);
		return XN_STATUS_OS_EVENT_OPEN_FAILED;
	}
	
	// adding 1 to reference count
	struct sembuf op;
	op.sem_op = 1;
	op.sem_num = XN_EVENT_SEM_REF_COUNT;
	op.sem_flg = SEM_UNDO;
	semop(pEvent->NamedSem, &op, 1);

	pEvent->bManualReset = semctl(pEvent->NamedSem, XN_EVENT_SEM_MANUAL_RESET, GETVAL);
	pEvent->bNamed = TRUE;

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSCloseEvent(XN_EVENT_HANDLE* pEventHandle)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pEventHandle);

	// Make sure the actual event handle isn't NULL
	XN_RET_IF_NULL(*pEventHandle, XN_STATUS_OS_INVALID_EVENT);
	
	XN_EVENT_HANDLE pEvent = *pEventHandle;

	if(pEvent->bNamed)
	{
		// dec ref count
		struct sembuf op;
		op.sem_num = XN_EVENT_SEM_REF_COUNT;
		op.sem_op = -1;
		op.sem_flg = IPC_NOWAIT | SEM_UNDO;
		semop(pEvent->NamedSem, &op, 1); 

		// check current ref count. If 0, destroy it
		int val = semctl(pEvent->NamedSem, 0, GETVAL);
		if (0 == val)
		{
			// destroy the semaphore
			semctl(pEvent->NamedSem, 0, IPC_RMID);
			// and remove file
			xnOSDeleteFile(pEvent->csSemFileName);
		}
	}
	else
	{
		// Destroy the cond and mutex objects
		if (0 != pthread_cond_destroy(&pEvent->cond) || 0 != pthread_mutex_destroy(&pEvent->mutex))
		{
			return (XN_STATUS_OS_EVENT_CLOSE_FAILED);
		}
	}
	
	// free the event struct
	XN_FREE_AND_NULL(*pEventHandle);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSetEvent(const XN_EVENT_HANDLE EventHandle)
{
	// Make sure the actual event handle isn't NULL
	XN_RET_IF_NULL(EventHandle, XN_STATUS_OS_INVALID_EVENT);

	if (EventHandle->bNamed)
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
		
		semop(EventHandle->NamedSem, op, 2);
		// no need to check error code. either it succeeded, or the event is already signaled
	}
	else
	{
		// lock the mutex
		if (0 != pthread_mutex_lock(&EventHandle->mutex))
		{
			return (XN_STATUS_OS_EVENT_SET_FAILED);
		}
		
		// signal the condition
		EventHandle->bSignaled = TRUE;
		
		// wake other threads
		if (0 != pthread_cond_broadcast(&EventHandle->cond))
		{
			pthread_mutex_unlock(&EventHandle->mutex);
			return XN_STATUS_OS_EVENT_SET_FAILED;
		}
			
		// unlock the mutex
		if (0 != pthread_mutex_unlock(&EventHandle->mutex))
		{
			return (XN_STATUS_OS_EVENT_SET_FAILED);
		}
	}

	// All is good...
	return (XN_STATUS_OK);
 }

XN_C_API XnStatus xnOSResetEvent(const XN_EVENT_HANDLE EventHandle)
{
	// Make sure the actual event handle isn't NULL
	XN_RET_IF_NULL(EventHandle, XN_STATUS_OS_INVALID_EVENT);

	if (EventHandle->bNamed)
	{
		// unsignal it, with NOWAIT (that way, if it's not signaled, nothing will happen)
		struct sembuf op;
		op.sem_num = XN_EVENT_SEM_SIGNALED;
		op.sem_op = -1;
		op.sem_flg = IPC_NOWAIT;
		semop(EventHandle->NamedSem, &op, 1);
	}
	else
	{	
		EventHandle->bSignaled = FALSE;
	}
	
	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnBool xnOSIsEventSet(const XN_EVENT_HANDLE EventHandle)
{
	return (xnOSWaitEvent(EventHandle, 0) == XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWaitEvent(const XN_EVENT_HANDLE EventHandle, XnUInt32 nMilliseconds)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// Make sure the actual event handle isn't NULL
	XN_RET_IF_NULL(EventHandle, XN_STATUS_OS_INVALID_EVENT);

	if(EventHandle->bNamed)
	{
		struct timespec time ={0};
		if (nMilliseconds != XN_WAIT_INFINITE)
		{
			// add timeout to this time
			time.tv_sec = (nMilliseconds / 1000);
			time.tv_nsec = ((nMilliseconds % 1000) * 1000000);
		
			// check the number of nanoseconds is still valid. If not, advance to next second
			if (time.tv_nsec > 999999999)
			{
				time.tv_nsec -= 1000000000;
				time.tv_sec++;
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
		
		XnInt32 nOpsCount = EventHandle->bManualReset ? 2 : 1;
		
		if (XN_WAIT_INFINITE != nMilliseconds)
		{
#ifndef XN_PLATFORM_HAS_NO_TIMED_OPS
			if (0 != semtimedop(EventHandle->NamedSem, op, nOpsCount, &time))
#else
			if (0 != semop(EventHandle->NamedSem, op, nOpsCount))	
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
			if (0 != semop(EventHandle->NamedSem, op, nOpsCount))
			{
				return XN_STATUS_OS_EVENT_WAIT_FAILED;
			}
		}
	}
	else // not named
	{
		struct timespec time;
		if (nMilliseconds != XN_WAIT_INFINITE)
		{
			// calculate timeout absolute time. First we take current time
			if (XN_STATUS_OK != xnOSGetMonoTime(&time))
			{
				return (XN_STATUS_OS_EVENT_WAIT_FAILED);
			}
			
			// add timeout to this time
			time.tv_sec += (nMilliseconds / 1000);
			time.tv_nsec += ((nMilliseconds % 1000) * 1000000);
			
			// check the number of nanoseconds is still valid. If not, advance to next second
			if (time.tv_nsec > 999999999)
			{
				time.tv_nsec -= 1000000000;
				time.tv_sec++;
			}
		}		
		// lock the mutex
		if (0 != pthread_mutex_lock(&EventHandle->mutex))
		{
			return (XN_STATUS_OS_EVENT_WAIT_FAILED);
		}
	
		// wait for the condition to become signaled (or a failure)
		int rc;
		while (nRetVal == XN_STATUS_OK && !EventHandle->bSignaled)
		{
			if (nMilliseconds != XN_WAIT_INFINITE)
			{
				rc = pthread_cond_timedwait(&EventHandle->cond, &EventHandle->mutex, &time);
			}
			else // infinite
			{
				rc = pthread_cond_wait(&EventHandle->cond, &EventHandle->mutex);
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
			pthread_mutex_unlock(&EventHandle->mutex);
			return nRetVal;	
		}
		
		if (!EventHandle->bManualReset)
		{
			// auto-reset the event
			EventHandle->bSignaled = FALSE;
		}
		
		// unlock the mutex
		if (0 != pthread_mutex_unlock(&EventHandle->mutex))
		{
			return (XN_STATUS_OS_EVENT_WAIT_FAILED);
		}
	}

	// All is good...
	return (XN_STATUS_OK);
}
