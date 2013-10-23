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
#include <XnScheduler.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_SCHEDULER_WAIT_THREAD_EXIT_TIMEOUT 1000

//---------------------------------------------------------------------------
// Data Types
//---------------------------------------------------------------------------
typedef struct XnScheduledTask
{
	/* The interval in which this task should run. */
	XnUInt64 nInterval;
	/* The callback function to be called when interval is reached. */
	XnTaskCallbackFuncPtr pCallback;
	/* An argument to be passed to the callback function. */
	void* pCallbackArg;
	/* The next time this task should run. */
	XnUInt64 nNextTime;
	/* A pointer to the next task in the queue. */
	XnScheduledTask* pNextTask;
} XnScheduledTask;

struct XnScheduler
{
	/* A pointer to the first task in the linked list. */
	XnScheduledTask* pFirst;
	/* A handle to the running thread. */
	XN_THREAD_HANDLE hThread;
	/* When true, thread should stop. */
	XnBool bStopThread;
	/* An event that is raised whenever thread should be awaken. */
	XN_EVENT_HANDLE hWakeThreadEvent;
	/* All changes to the list must be performed inside critical section. */
	XN_CRITICAL_SECTION_HANDLE hCriticalSection;
};

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

/* Adds a task to the linked list. This must be called from within a critical section. */
void xnSchedulerAddTaskInternal(XnScheduler* pScheduler, XnScheduledTask* pTask)
{
	// check if should be first in queue
	if (pScheduler->pFirst == NULL || pScheduler->pFirst->nNextTime > pTask->nNextTime)
	{
		pTask->pNextTask = pScheduler->pFirst;
		pScheduler->pFirst = pTask;
	}
	else
	{
		XnScheduledTask* pAfter = pScheduler->pFirst;

		// move until place is found
		while (pAfter->pNextTask != NULL && pAfter->nNextTime < pTask->nNextTime)
			pAfter = pAfter->pNextTask;

		pTask->pNextTask = pAfter->pNextTask;
		pAfter->pNextTask = pTask;
	}
}

/* This is the actual scheduler function. It is being run in its own thread. */
XN_THREAD_PROC xnSchedulerThreadFunc(XN_THREAD_PARAM pThreadParam)
{
	XnScheduler* pScheduler = (XnScheduler*)pThreadParam;

	XnUInt64 nNow;
	while (!pScheduler->bStopThread)
	{
		// check when next task should be executed
		XnUInt64 nWait = XN_WAIT_INFINITE;

		XnScheduledTask* pTask = NULL;
		XnTaskCallbackFuncPtr pCallback = NULL;
		void* pCallbackArg = NULL;

		// check if something is in the list
		if (pScheduler->pFirst != NULL)
		{
			// enter critical section
			xnOSEnterCriticalSection(&pScheduler->hCriticalSection);

			pTask = pScheduler->pFirst;
			if (pTask != NULL)
			{
				xnOSGetTimeStamp(&nNow);
				if (pTask->nNextTime < nNow)
				{
					// task should be executed
					pCallback = pTask->pCallback;
					pCallbackArg = pTask->pCallbackArg;

					// remove it from the list
					pScheduler->pFirst = pTask->pNextTask;

					// calculate next time
					pTask->nNextTime += pTask->nInterval;

					// add it to the list again
					xnSchedulerAddTaskInternal(pScheduler, pTask);
				}
				else
				{
					nWait = pTask->nNextTime - nNow;
				}
			}

			// leave critical section
			xnOSLeaveCriticalSection(&pScheduler->hCriticalSection);

			if (pCallback != NULL)
			{
				// execute task (outside critical section)
				pCallback(pCallbackArg);
				// no need to wait (we don't know how much time did callback take)
				nWait = 0;
			}
		}

		// wait for a change of the list, or the time of the next task
		xnOSWaitEvent(pScheduler->hWakeThreadEvent, (XnUInt32)nWait);
	}

	XN_THREAD_PROC_RETURN(XN_STATUS_OK);
}

void FreeScheduler(XnScheduler* pScheduler)
{
	// stop thread
	if (pScheduler->hThread)
	{
		// mark for thread to stop
		pScheduler->bStopThread = TRUE;

		if (pScheduler->hWakeThreadEvent)
		{
			xnOSSetEvent(pScheduler->hWakeThreadEvent);
		}

		// now wait for it to exit
		xnLogVerbose(XN_MASK_SCHEDULER, "Shutting down Scheduler thread...");
		xnOSWaitAndTerminateThread(&pScheduler->hThread, XN_SCHEDULER_WAIT_THREAD_EXIT_TIMEOUT);
	}

	if (pScheduler->hWakeThreadEvent)
	{
		xnOSCloseEvent(&pScheduler->hWakeThreadEvent);
	}

	if (pScheduler->hCriticalSection)
	{
		xnOSCloseCriticalSection(&pScheduler->hCriticalSection);
	}

	while (pScheduler->pFirst != NULL)
	{
		XnScheduledTask* pTask = pScheduler->pFirst;
		pScheduler->pFirst = pTask->pNextTask;

		xnOSFree(pTask);
	}

	xnOSFree(pScheduler);
}

#define XN_CHECK_RC_AND_FREE(nRetVal, pScheduler)	\
	if (nRetVal != XN_STATUS_OK)					\
	{												\
		FreeScheduler(pScheduler);					\
		return (nRetVal);							\
	}

XN_C_API XnStatus xnSchedulerStart(XnScheduler** ppScheduler)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_OUTPUT_PTR(ppScheduler);

	*ppScheduler = NULL;

	// allocate handle
	XnScheduler* pScheduler = NULL;
	XN_VALIDATE_CALLOC(pScheduler, XnScheduler, 1);

	// create event
	nRetVal = xnOSCreateEvent(&pScheduler->hWakeThreadEvent, FALSE);
	XN_CHECK_RC_AND_FREE(nRetVal, pScheduler);

	// create critical section
	nRetVal = xnOSCreateCriticalSection(&pScheduler->hCriticalSection);
	XN_CHECK_RC_AND_FREE(nRetVal, pScheduler);

	// start thread
	nRetVal = xnOSCreateThread(xnSchedulerThreadFunc, (XN_THREAD_PARAM)pScheduler, &pScheduler->hThread);
	XN_CHECK_RC_AND_FREE(nRetVal, pScheduler);

	*ppScheduler = pScheduler;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnSchedulerShutdown(XnScheduler** ppScheduler)
{
	XN_VALIDATE_INPUT_PTR(ppScheduler);

	FreeScheduler(*ppScheduler);

	*ppScheduler = NULL;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnSchedulerAddTask(XnScheduler* pScheduler, XnUInt64 nInterval, XnTaskCallbackFuncPtr pCallback, void* pCallbackArg, XnScheduledTask** ppTask)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pScheduler);
	XN_VALIDATE_INPUT_PTR(pCallback);
	XN_VALIDATE_OUTPUT_PTR(ppTask);

	// create node
	XnScheduledTask* pTask;
	XN_VALIDATE_ALLOC(pTask, XnScheduledTask);

	pTask->nInterval = nInterval;
	pTask->pCallback = pCallback;
	pTask->pCallbackArg = pCallbackArg;

	// calculate next execution
	XnUInt64 nNow;
	xnOSGetTimeStamp(&nNow);
	pTask->nNextTime = nNow + nInterval;
	pTask->pNextTask = NULL;

	// enter critical section
	nRetVal = xnOSEnterCriticalSection(&pScheduler->hCriticalSection);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pTask);
		return (nRetVal);
	}

	xnSchedulerAddTaskInternal(pScheduler, pTask);

	// leave critical section
	nRetVal = xnOSLeaveCriticalSection(&pScheduler->hCriticalSection);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(pTask);
		return (nRetVal);
	}

	// notify that the list has changed
	nRetVal = xnOSSetEvent(pScheduler->hWakeThreadEvent);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_SCHEDULER, "Failed setting event when adding task: %s", xnGetStatusString(nRetVal));
	}

	*ppTask = pTask;

	return (XN_STATUS_OK);
}

void XnSchedulerRemoveTaskInternal(XnScheduler* pScheduler, XnScheduledTask* pTask)
{
	if (pScheduler->pFirst == NULL) // check if any tasks exist
	{
		return;
	}
	else if (pScheduler->pFirst == pTask) // check if this is the first task
	{
		pScheduler->pFirst = pTask->pNextTask;
	}
	else // not first task
	{
		XnScheduledTask* pBefore = pScheduler->pFirst;

		// move until place is found
		while (pBefore != NULL && pBefore->pNextTask != pTask)
			pBefore = pBefore->pNextTask;

		pBefore->pNextTask = pBefore->pNextTask->pNextTask;
	}
}

XN_C_API XnStatus xnSchedulerRemoveTask(XnScheduler* pScheduler, XnScheduledTask** ppTask)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pScheduler);
	XN_VALIDATE_INPUT_PTR(ppTask);
	XN_VALIDATE_INPUT_PTR(*ppTask);

	XnScheduledTask* pTask = *ppTask;

	// enter critical section
	nRetVal = xnOSEnterCriticalSection(&pScheduler->hCriticalSection);
	XN_IS_STATUS_OK(nRetVal);

	XnSchedulerRemoveTaskInternal(pScheduler, pTask);

	// leave critical section
	nRetVal = xnOSLeaveCriticalSection(&pScheduler->hCriticalSection);
	XN_IS_STATUS_OK(nRetVal);

	// notify that the list has changed
	nRetVal = xnOSSetEvent(pScheduler->hWakeThreadEvent);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_SCHEDULER, "Failed setting event when removing task: %s", xnGetStatusString(nRetVal));
	}

	xnOSFree(pTask);

	*ppTask = NULL;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnSchedulerRescheduleTask(XnScheduler* pScheduler, XnScheduledTask* pTask, XnUInt64 nInterval)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pScheduler);
	XN_VALIDATE_INPUT_PTR(pTask);

	// enter critical section
	nRetVal = xnOSEnterCriticalSection(&pScheduler->hCriticalSection);
	XN_IS_STATUS_OK(nRetVal);

	// remove it from list
	XnSchedulerRemoveTaskInternal(pScheduler, pTask);

	pTask->nInterval = nInterval;

	// update its next execution
	XnUInt64 nNow;
	xnOSGetTimeStamp(&nNow);
	pTask->nNextTime = nNow + nInterval;

	// and add it back to the queue
	xnSchedulerAddTaskInternal(pScheduler, pTask);

	// leave critical section
	nRetVal = xnOSLeaveCriticalSection(&pScheduler->hCriticalSection);
	XN_IS_STATUS_OK(nRetVal);

	// notify that the list has changed
	nRetVal = xnOSSetEvent(pScheduler->hWakeThreadEvent);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_SCHEDULER, "Failed setting event when rescheduling task: %s", xnGetStatusString(nRetVal));
	}

	return (XN_STATUS_OK);
}
