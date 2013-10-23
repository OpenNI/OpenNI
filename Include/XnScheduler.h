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
#ifndef _XN_SCHEDULER_H_
#define _XN_SCHEDULER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MASK_SCHEDULER "Scheduler"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
struct XnScheduler; // forward declaration
typedef struct XnScheduler XnScheduler;

struct XnScheduledTask; // forward declaration
typedef struct XnScheduledTask XnScheduledTask;

typedef void (XN_CALLBACK_TYPE* XnTaskCallbackFuncPtr)(void* pCallbackArg);

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------

/**
* Starts a scheduler thread. Multiple timers can be added to the created scheduler.
*
* @param	ppScheduler		[out]	Upon successful return, holds a handle to created scheduler.
*/
XN_C_API XnStatus XN_C_DECL xnSchedulerStart(XnScheduler** ppScheduler);

/**
* Shuts down a scheduler thread. All timers on that scheduler will be stopped.
*
* @param	ppScheduler		[in/out]	A pointer to the scheduler to be shut down.
*/
XN_C_API XnStatus XN_C_DECL xnSchedulerShutdown(XnScheduler** ppScheduler);

/**
* Adds a task to the scheduler.
*
* @param	pScheduler		[in]	The scheduler to handle this task.
* @param	nInterval		[in]	The interval, in milliseconds, in which callback should be called.
* @param	pCallback		[in]	The function to be called when time arrived.
* @param	pCallbackArg	[in]	The argument that will be passed to the callback method.
* @param	ppTask			[out]	Upon successful return, holds a handle to the started task.
*/
XN_C_API XnStatus XN_C_DECL xnSchedulerAddTask(XnScheduler* pScheduler, XnUInt64 nInterval, XnTaskCallbackFuncPtr pCallback, void* pCallbackArg, XnScheduledTask** ppTask);

/**
* Removes a task from the scheduler.
*
* @param	pScheduler	[in]		The scheduler this task is registered to.
* @param	ppTask		[in/out]	The task to be removed from the scheduler.
*/
XN_C_API XnStatus XN_C_DECL xnSchedulerRemoveTask(XnScheduler* pScheduler, XnScheduledTask** ppTask);

/**
* Changes scheduling of a specific task.
*
* @param	pScheduler	[in]	The scheduler this task is registered to.
* @param	pTask		[in]	The task to be removed from the scheduler.
* @param	nInterval	[in]	The new interval to be used.
*/
XN_C_API XnStatus XN_C_DECL xnSchedulerRescheduleTask(XnScheduler* pScheduler, XnScheduledTask* pTask, XnUInt64 nInterval);

#endif //_XN_SCHEDULER_H_
