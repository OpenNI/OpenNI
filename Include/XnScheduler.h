/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
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
