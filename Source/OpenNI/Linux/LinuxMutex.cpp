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
#include <XnOS.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <XnLog.h>

#ifndef XN_PLATFORM_LINUX_NO_SYSV
#include <sys/sem.h>
#endif

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
struct XnMutex
{
	XnBool bIsNamed;
	pthread_mutex_t ThreadMutex;
	int NamedSem;
	XnChar csSemFileName[XN_FILE_MAX_PATH];
	int hSemFile;
	XnBool bIsLocked; // used for named semaphore, to make sure a porcess doesn't unlock more than once (counting semaphore will no longer be a mutex).
};

typedef struct XnMutex XnMutex;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus xnOSUnNamedMutexCreate(XnMutex* pMutex)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// make the mutex recursive (re-entrent)
	pthread_mutexattr_t tAttributes;

	if (0 != pthread_mutexattr_init(&tAttributes))
		return (XN_STATUS_OS_MUTEX_CREATION_FAILED);
	
	if (0 != pthread_mutexattr_settype(&tAttributes, PTHREAD_MUTEX_RECURSIVE))
		return (XN_STATUS_OS_MUTEX_CREATION_FAILED);
	
	// Init the mutex
	int rc = pthread_mutex_init(&pMutex->ThreadMutex, &tAttributes);
	
	// destroy the attributes object
	pthread_mutexattr_destroy(&tAttributes);
	
	// check if init succeeded
	if (0 != rc)
	{
		return (XN_STATUS_OS_MUTEX_CREATION_FAILED);
	}

	return (XN_STATUS_OK);
}

XnStatus xnOSNamedMutexCreate(XnMutex* pMutex, const XnChar* csMutexName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	int rc;

#ifdef XN_PLATFORM_LINUX_NO_SYSV
	xnLogError(XN_MASK_OS, "Named mutex is not implemented for this platform!");
	return XN_STATUS_OS_MUTEX_CREATION_FAILED;
#else

	// remove bad chars from name
	XnChar strMutexOSName[XN_FILE_MAX_PATH];
	int i = 0;
	for (; (i < XN_FILE_MAX_PATH) && (csMutexName[i] != '\0'); ++i)
		strMutexOSName[i] = csMutexName[i] == '/' ? '_' : csMutexName[i];

	if (i == XN_FILE_MAX_PATH)
	{
		xnLogWarning(XN_MASK_OS, "Mutex name is too long!");
		return XN_STATUS_OS_MUTEX_CREATION_FAILED;
	}

	strMutexOSName[i] = '\0';

	// tanslate mutex name to key file name
	XnUInt32 nBytesWritten;
	xnOSStrFormat(pMutex->csSemFileName, XN_FILE_MAX_PATH, &nBytesWritten, "/tmp/XnCore.Mutex.%s.key", strMutexOSName);
	
	// open this file (we hold it open until mutex is closed. That way it cannot be deleted as long
	// as any process is holding the mutex, and the mutex can be destroyed if the file can be deleted).
	pMutex->hSemFile = open(pMutex->csSemFileName, O_CREAT | O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
	if (pMutex->hSemFile == -1)
	{
		return XN_STATUS_OS_FILE_OPEN_FAILED;
	}
	
	// create the key
	key_t key = ftok(pMutex->csSemFileName, 1);

	// first we try to create it. If we fail, we'll know it already existed
	XnBool bCreated = TRUE;
	
	// we created a set of 2 sems - first is the lock, second counts processes
	pMutex->NamedSem = semget(key, 2, IPC_CREAT | IPC_EXCL | 0666);
	if (pMutex->NamedSem == -1 && errno == EEXIST)
	{
		// get existing one
		pMutex->NamedSem = semget(key, 2, IPC_CREAT | 0666);
		if (pMutex->NamedSem == -1)
		{
			close(pMutex->hSemFile);
			return (XN_STATUS_OS_MUTEX_CREATION_FAILED);
		}
		
		bCreated = FALSE;
	}

	if (bCreated)
	{
		union semun 
		{
				   int              val;    /* Value for SETVAL */
				   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
				   unsigned short  *array;  /* Array for GETALL, SETALL */
				   struct seminfo  *__buf;  /* Buffer for IPC_INFO
											   (Linux-specific) */
		} init;
		
		// init lock semaphore to 1 (not locked)
		init.val = 1;
		rc = semctl(pMutex->NamedSem, 0, SETVAL, init);
		if (rc != 0)
		{
			semctl(pMutex->NamedSem, 0, IPC_RMID);
			return (XN_STATUS_OS_MUTEX_CREATION_FAILED);
		}

		// init process counter to 0
		init.val = 0;
		rc = semctl(pMutex->NamedSem, 1, SETVAL, init);
		if (rc != 0)
		{
			semctl(pMutex->NamedSem, 0, IPC_RMID);
			return (XN_STATUS_OS_MUTEX_CREATION_FAILED);
		}
	}

	// increment second sem
	struct sembuf op;
	op.sem_num = 1;
	op.sem_op = 1;
	op.sem_flg = SEM_UNDO; // mark it to be decreased when process terminates
	
	rc = semop(pMutex->NamedSem, &op, 1);
	if (rc != 0)
	{
		xnOSCloseMutex(&pMutex);
		return (XN_STATUS_OS_MUTEX_CREATION_FAILED);
	}
#endif

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateMutex(XN_MUTEX_HANDLE* pMutexHandle)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;
	int rc = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pMutexHandle);
	
	XnMutex* pMutex;
	XN_VALIDATE_CALLOC(pMutex, XnMutex, 1);
	pMutex->bIsNamed = FALSE;
	
	nRetVal = xnOSUnNamedMutexCreate(pMutex);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pMutex);
		return (nRetVal);
	}
	
	*pMutexHandle = pMutex;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateNamedMutex(XN_MUTEX_HANDLE* pMutexHandle, const XnChar* cpMutexName)
{
	return xnOSCreateNamedMutexEx(pMutexHandle, cpMutexName, FALSE);
}

XN_C_API XnStatus XN_C_DECL xnOSCreateNamedMutexEx(XN_MUTEX_HANDLE* pMutexHandle, const XnChar* cpMutexName, XnBool bAllowOtherUsers)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;
	int rc = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pMutexHandle);
	
	XnMutex* pMutex;
	XN_VALIDATE_CALLOC(pMutex, XnMutex, 1);
	pMutex->bIsNamed = TRUE;
	
	nRetVal = xnOSNamedMutexCreate(pMutex, cpMutexName);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pMutex);
		return (nRetVal);
	}
	
	*pMutexHandle = pMutex;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseMutex(XN_MUTEX_HANDLE* pMutexHandle)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pMutexHandle);

	// Make sure the actual mutex handle isn't NULL
	XN_RET_IF_NULL(*pMutexHandle, XN_STATUS_OS_INVALID_MUTEX);
	
	XnMutex* pMutex = *pMutexHandle;

	// check the kind of mutex
	if (pMutex->bIsNamed)
	{
#ifndef XN_PLATFORM_LINUX_NO_SYSV

		// decrement second sem
		struct sembuf op;
		op.sem_num = 1;
		op.sem_op = -1;
		op.sem_flg = SEM_UNDO;
		
		if (0 != semop(pMutex->NamedSem, &op, 1))
		{
			return (XN_STATUS_OS_MUTEX_CLOSE_FAILED);
		}
		
		// check if sem reached 0 (if so, it can be deleted)
		int val = semctl(pMutex->NamedSem, 1, GETVAL);
		if (val == 0)
		{
			// destroy the semaphore
			semctl(pMutex->NamedSem, 0, IPC_RMID);
			// and remove file
			xnOSDeleteFile(pMutex->csSemFileName);
		}
		
		// in any case, close the file
		close(pMutex->hSemFile);
#endif
	}
	else
	{
		// destroy the mutex via the OS
		if (0 != pthread_mutex_destroy(&pMutex->ThreadMutex))
		{
			return (XN_STATUS_OS_MUTEX_CLOSE_FAILED);
		}
	}
	
	// free the handle
	XN_FREE_AND_NULL(*pMutexHandle);
	
	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSLockMutex(const XN_MUTEX_HANDLE MutexHandle, XnUInt32 nMilliseconds)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;
	int rc = 0;

	// Make sure the actual mutex handle isn't NULL
	XN_RET_IF_NULL(MutexHandle, XN_STATUS_OS_INVALID_MUTEX);

#ifndef XN_PLATFORM_LINUX_NO_SYSV
	struct sembuf op;
	// try to decrease it by 1 (if it's 0, we'll wait)
	op.sem_num = 0;
	op.sem_op = -1;
	op.sem_flg = SEM_UNDO;
#endif

	if (nMilliseconds == XN_WAIT_INFINITE)
	{
		// lock via the OS
		if (MutexHandle->bIsNamed)
		{
#ifndef XN_PLATFORM_LINUX_NO_SYSV
			if (0 != semop(MutexHandle->NamedSem, &op, 1))
			{
				rc = errno;
			}
#endif
		}
		else
		{
			rc = pthread_mutex_lock(&MutexHandle->ThreadMutex);
		}
	}
	else
	{
		struct timespec time;
		
		// lock via the OS
		if (MutexHandle->bIsNamed)
		{
#ifndef XN_PLATFORM_LINUX_NO_SYSV
			nRetVal = xnOSGetTimeout(&time, nMilliseconds);
			if (nRetVal != XN_STATUS_OK)
			{
				return XN_STATUS_OS_MUTEX_LOCK_FAILED;
			}

#ifndef XN_PLATFORM_HAS_NO_TIMED_OPS
			if (0 != semtimedop(MutexHandle->NamedSem, &op, 1, &time))
#else
			if (0 != semop(MutexHandle->NamedSem, &op, 1))
#endif
			{
				rc = errno;
			}
#endif
		}
		else
		{
			// calculate timeout absolute time. First we take current time
			nRetVal = xnOSGetAbsTimeout(&time, nMilliseconds);
			if (nRetVal != XN_STATUS_OK)
			{
				return XN_STATUS_OS_MUTEX_LOCK_FAILED;
			}
			
#ifndef XN_PLATFORM_HAS_NO_TIMED_OPS
			rc = pthread_mutex_timedlock(&MutexHandle->ThreadMutex, &time);
#else
			rc = pthread_mutex_lock(&MutexHandle->ThreadMutex);
#endif
		}
	}
	
	// check for failures
	if (rc == ETIMEDOUT)
	{
		return (XN_STATUS_OS_MUTEX_TIMEOUT);
	}
	else if (rc != 0)
	{
		return (XN_STATUS_OS_MUTEX_LOCK_FAILED);
	}
	
	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSUnLockMutex(const XN_MUTEX_HANDLE MutexHandle)
{
	// Local function variables
	XnStatus nRetVal = XN_STATUS_OK;
	int rc = 0;

	// Make sure the actual mutex handle isn't NULL
	XN_RET_IF_NULL(MutexHandle, XN_STATUS_OS_INVALID_MUTEX);
	
	// unlock via the OS
	if (MutexHandle->bIsNamed)
	{
#ifndef XN_PLATFORM_LINUX_NO_SYSV
		struct sembuf op;
		op.sem_num = 0;
		op.sem_op = 1; // increase by 1
		op.sem_flg = SEM_UNDO;
		
		if (0 != semop(MutexHandle->NamedSem, &op, 1))
		{
			rc = errno;
		}
#endif
	}
	else
	{
		rc = pthread_mutex_unlock(&MutexHandle->ThreadMutex);
	}
	
	if (0 != rc)
	{
		return (XN_STATUS_OS_MUTEX_UNLOCK_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}
