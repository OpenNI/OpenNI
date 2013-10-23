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
#include <XnProfiling.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------
#define MAX_PROFILED_SECTIONS	100
#define MAX_SECTION_NAME		256
#define MAX_CALL_STACK_SIZE		10

#define XN_MASK_PROFILING		"Profiler"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct
{
	XnChar csName[MAX_SECTION_NAME];
	XnBool bMultiThreaded;
	XN_CRITICAL_SECTION_HANDLE hLock;
	XnUInt64 nCurrStartTime;
	XnUInt64 nTotalTime;
	XnUInt32 nTimesExecuted;
	XnUInt32 nIndentation;
} XnProfiledSection;

typedef struct
{
	XnBool bInitialized;
	XnProfiledSection* aSections;
	XnUInt32 nSectionCount;
	XN_THREAD_HANDLE hThread;
	XN_CRITICAL_SECTION_HANDLE hCriticalSection;
	XnSizeT nMaxSectionName;
	XnUInt32 nProfilingInterval;
	XnBool bKillThread;
} XnProfilingData;

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
static XnProfilingData g_ProfilingData = {0};
static XN_THREAD_STATIC XnUInt32 gt_nStackDepth = 0;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_THREAD_PROC xnProfilingThread(XN_THREAD_PARAM /*pThreadParam*/)
{
	XnChar csReport[4096];
	int nReportChars;

	XnUInt64 nLastTime;
	xnOSGetHighResTimeStamp(&nLastTime);

	while (!g_ProfilingData.bKillThread)
	{
		xnOSSleep(g_ProfilingData.nProfilingInterval);

		XnUInt64 nNow;
		xnOSGetHighResTimeStamp(&nNow);

		// print profiled sections
		nReportChars = 0;
		nReportChars += sprintf(csReport + nReportChars, "Profiling Report:\n");
		nReportChars += sprintf(csReport + nReportChars, "%-*s %-5s %-6s %-9s %-7s\n", (int)g_ProfilingData.nMaxSectionName, "TaskName", "Times", "% Time", "TotalTime", "AvgTime");
		nReportChars += sprintf(csReport + nReportChars, "%-*s %-5s %-6s %-9s %-7s\n", (int)g_ProfilingData.nMaxSectionName, "========", "=====", "======", "=========", "=======");

		XnUInt64 nTotalTime = 0;

		for (XnUInt32 i = 0; i < g_ProfilingData.nSectionCount; ++i)
		{
			XnProfiledSection* pSection = &g_ProfilingData.aSections[i];

			XnUInt64 nAvgTime = 0;
			XnDouble dCPUPercentage = ((XnDouble)pSection->nTotalTime) / (nNow - nLastTime) * 100.0;

			if (pSection->nTimesExecuted != 0)
			{
				nAvgTime = pSection->nTotalTime / pSection->nTimesExecuted;
			}

			nReportChars += sprintf(csReport + nReportChars, "%-*s %5u %6.2f %9llu %7llu\n", (int)g_ProfilingData.nMaxSectionName, 
				pSection->csName, pSection->nTimesExecuted, dCPUPercentage, pSection->nTotalTime, nAvgTime);

			if (pSection->nIndentation == 0)
				nTotalTime += pSection->nTotalTime;

			// clear accumulated data
			pSection->nTotalTime = 0;
			pSection->nTimesExecuted = 0;
		}

		// print total
		XnDouble dCPUPercentage = ((XnDouble)nTotalTime) / (nNow - nLastTime) * 100.0;
		nReportChars += sprintf(csReport + nReportChars, "%-*s %5s %6.2f %9llu %7s\n", 
			(int)g_ProfilingData.nMaxSectionName, "*** Total ***", "-", dCPUPercentage, nTotalTime, "-");

		xnLogVerbose(XN_MASK_PROFILING, "%s", csReport);

		nLastTime = nNow;
	}

	XN_THREAD_PROC_RETURN(XN_STATUS_OK);
}

XN_C_API XnStatus xnProfilingInit(XnUInt32 nProfilingInterval)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (nProfilingInterval == 0)
	{
		xnProfilingShutdown();
	}
	else if (!g_ProfilingData.bInitialized)
	{
		g_ProfilingData.nMaxSectionName = 0;
		g_ProfilingData.nSectionCount = 0;
		g_ProfilingData.nProfilingInterval = nProfilingInterval;
		g_ProfilingData.bKillThread = FALSE;

		XN_VALIDATE_CALLOC(g_ProfilingData.aSections, XnProfiledSection, MAX_PROFILED_SECTIONS);
		g_ProfilingData.nSectionCount = 0;

		nRetVal = xnOSCreateThread(xnProfilingThread, (XN_THREAD_PARAM)NULL, &g_ProfilingData.hThread);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnOSCreateCriticalSection(&g_ProfilingData.hCriticalSection);
		XN_IS_STATUS_OK(nRetVal);

		g_ProfilingData.bInitialized = TRUE;
	}

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnProfilingInitFromINI(const XnChar* cpINIFileName, const XnChar* cpSectionName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUInt32 nProfilingInterval = 0;
	xnOSReadIntFromINI(cpINIFileName, cpSectionName, "ProfilingInterval", &nProfilingInterval);

	nRetVal = xnProfilingInit(nProfilingInterval);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnProfilingShutdown()
{
	if (g_ProfilingData.hThread != NULL)
	{
		g_ProfilingData.bKillThread = TRUE;
		xnLogVerbose(XN_MASK_PROFILING, "Shutting down Profiling thread...");
		xnOSWaitAndTerminateThread(&g_ProfilingData.hThread, g_ProfilingData.nProfilingInterval * 2);
		g_ProfilingData.hThread = NULL;
	}

	if (g_ProfilingData.hCriticalSection != NULL)
	{
		xnOSCloseCriticalSection(&g_ProfilingData.hCriticalSection);
		g_ProfilingData.hCriticalSection = NULL;
	}

	XN_FREE_AND_NULL(g_ProfilingData.aSections);

	g_ProfilingData.bInitialized = FALSE;

	return XN_STATUS_OK;
}

XN_C_API XnBool xnProfilingIsActive()
{
	return (g_ProfilingData.bInitialized && g_ProfilingData.nProfilingInterval > 0);
}

XN_C_API XnStatus xnProfilingSectionStart(const char* csSectionName, XnBool bMT, XnProfilingHandle* pHandle)
{
	if (!g_ProfilingData.bInitialized)
		return XN_STATUS_OK;

	if (*pHandle == INVALID_PROFILING_HANDLE)
	{
		xnOSEnterCriticalSection(&g_ProfilingData.hCriticalSection);
		if (*pHandle == INVALID_PROFILING_HANDLE)
		{
			XnUInt32 nIndex = g_ProfilingData.nSectionCount;
			g_ProfilingData.nSectionCount++;
			XnProfiledSection* pSection = &g_ProfilingData.aSections[nIndex];
			pSection->nIndentation = gt_nStackDepth;

			XnUInt32 nChar = 0;
			for (nChar = 0; nChar < gt_nStackDepth*2; ++nChar)
				pSection->csName[nChar] = ' ';

			strncpy(pSection->csName + nChar, csSectionName, MAX_SECTION_NAME);

			if (strlen(pSection->csName) > g_ProfilingData.nMaxSectionName)
				g_ProfilingData.nMaxSectionName = strlen(pSection->csName);

			if (bMT)
			{
				pSection->bMultiThreaded = TRUE;
				xnOSCreateCriticalSection(&pSection->hLock);
			}

			*pHandle = nIndex;
		}
		xnOSLeaveCriticalSection(&g_ProfilingData.hCriticalSection);
	}

	gt_nStackDepth++;

	XnProfiledSection* pSection = &g_ProfilingData.aSections[*pHandle];
	xnOSGetHighResTimeStamp(&pSection->nCurrStartTime);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnProfilingSectionEnd(XnProfilingHandle* pHandle)
{
	if (!g_ProfilingData.bInitialized)
		return XN_STATUS_OK;

	XnUInt64 nNow;
	xnOSGetHighResTimeStamp(&nNow);

	XnProfiledSection* pSection = &g_ProfilingData.aSections[*pHandle];
	if (pSection->bMultiThreaded)
	{
		xnOSEnterCriticalSection(&pSection->hLock);
	}
	pSection->nTimesExecuted++;
	pSection->nTotalTime += nNow - pSection->nCurrStartTime;
	if (pSection->bMultiThreaded)
	{
		xnOSLeaveCriticalSection(&pSection->hLock);
	}

	gt_nStackDepth--;

	return XN_STATUS_OK;
}
