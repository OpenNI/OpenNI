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
#define XN_OS_IMPL
#include <XnOS.h>
#include <XnOSCpp.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

#define XN_MEM_PROF_MAX_FRAME_LEN 80
#define XN_MEM_PROF_MAX_FRAMES 20
#define XN_MASK_MEM_PROFILING	"MemoryProfiling"
//#define XN_MEMORY_PROFILING_DUMP

typedef XnChar XnFrame[XN_MEM_PROF_MAX_FRAME_LEN];

typedef struct
{
	void* pMemBlock;
	XnAllocationType nAllocType;
	XnUInt32 nBytes;
	const XnChar* csFunction;
	const XnChar* csFile;
	XnUInt32 nLine;
	const XnChar* csAdditional;
	XnUInt32 nFrames;
	XnFrame aFrames[XN_MEM_PROF_MAX_FRAMES];
} XnMemBlockData;

typedef struct XnMemBlockDataNode
{
	XnMemBlockData Data;
	XnMemBlockDataNode* pNext;
} XnMemBlockDataNode;

typedef struct
{
	XnMemBlockDataNode* pFirst;
	XnMemBlockDataNode* pLast;
} XnMemBlockDataLinkedList;

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
static XnMemBlockDataLinkedList g_allocatedMemory = {NULL, NULL};
static XN_CRITICAL_SECTION_HANDLE g_hCS;
static XnDumpFile* g_dump = NULL;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
const XnChar* XnGetAllocTypeString(XnAllocationType nType)
{
	switch (nType)
	{
	case XN_ALLOCATION_MALLOC:
		return "xnOSMalloc";
	case XN_ALLOCATION_MALLOC_ALIGNED:
		return "xnOSMallocAligned";
	case XN_ALLOCATION_CALLOC:
		return "xnOSCalloc";
	case XN_ALLOCATION_CALLOC_ALIGNED:
		return "xnOSCallocAligned";
	case XN_ALLOCATION_NEW:
		return "XN_NEW";
	case XN_ALLOCATION_NEW_ARRAY:
		return "XN_NEW_ARR";
	default:
		return "Unknown";
	}
}

XN_C_API void* xnOSLogMemAlloc(void* pMemBlock, XnAllocationType nAllocType, XnUInt32 nBytes, const XnChar* csFunction, const XnChar* csFile, XnUInt32 nLine, const XnChar* csAdditional)
{
	static XnBool bFirstTime = TRUE;
	static XnBool bReentrent = FALSE;

	if (bFirstTime)
	{
		bFirstTime = FALSE;
		printf("************************************************************\n");
		printf("**  WARNING: Memory Profiling is on!                      **\n");
		printf("************************************************************\n");

		bReentrent = TRUE;
		xnOSCreateCriticalSection(&g_hCS);

#ifdef XN_MEMORY_PROFILING_DUMP
		xnDumpSetMaskState("MemProf", TRUE);
#endif
		g_dump = xnDumpFileOpen("MemProf", "MemProfiling.log");
		xnDumpFileWriteString(g_dump, "Entry,Address,AllocType,Bytes,Function,File,Line,AdditionalInfo\n");
		bReentrent = FALSE;
	}

	// ignore stuff that is being allocated during "first time"
	if (bReentrent)
	{
		return pMemBlock;
	}

	XnMemBlockDataNode* pNode;
	pNode = (XnMemBlockDataNode*)xnOSMalloc(sizeof(XnMemBlockDataNode));
	pNode->Data.pMemBlock = pMemBlock;
	pNode->Data.nAllocType = nAllocType;
	pNode->Data.nBytes = nBytes;
	pNode->Data.csFunction = csFunction;
	pNode->Data.csFile = csFile;
	pNode->Data.nLine = nLine;
	pNode->Data.csAdditional = csAdditional;
	pNode->Data.nFrames = XN_MEM_PROF_MAX_FRAMES;
	xnDumpFileWriteString(g_dump, "Alloc,0x%x,%s,%u,%s,%s,%u,%s\n", pMemBlock, XnGetAllocTypeString(nAllocType), nBytes, csFunction, csFile, nLine, csAdditional);

	// try to get call stack (skip 2 frames - this one and the alloc func)
	XnChar* pstrFrames[XN_MEM_PROF_MAX_FRAMES];
	for (XnUInt32 i = 0; i < XN_MEM_PROF_MAX_FRAMES; ++i)
	{
		pstrFrames[i] = pNode->Data.aFrames[i];
	}
	if (XN_STATUS_OK != xnOSGetCurrentCallStack(2, pstrFrames, XN_MEM_PROF_MAX_FRAME_LEN, &pNode->Data.nFrames))
	{
		pNode->Data.nFrames = 0;
	}

	pNode->pNext = NULL;

	XnAutoCSLocker lock(g_hCS);
	if (g_allocatedMemory.pLast == NULL)
	{
		g_allocatedMemory.pFirst = g_allocatedMemory.pLast = pNode;
	}
	else
	{
		g_allocatedMemory.pLast->pNext = pNode;
		g_allocatedMemory.pLast = pNode;
	}

	return pMemBlock;
}

XN_C_API void xnOSLogMemFree(const void* pMemBlock)
{
	if (pMemBlock == NULL)
		return;

	XnMemBlockDataNode* pPrev = NULL;

	XnAutoCSLocker lock(g_hCS);
	XnMemBlockDataNode* pNode = g_allocatedMemory.pFirst;
	while (pNode != NULL)
	{
		if (pNode->Data.pMemBlock == pMemBlock)
		{
			// found. Remove it from the list
			if (pPrev == NULL) // no previous
				g_allocatedMemory.pFirst = pNode->pNext;
			else
				pPrev->pNext = pNode->pNext;

			// if it was last, update last
			if (g_allocatedMemory.pLast == pNode)
				g_allocatedMemory.pLast = pPrev;

			xnDumpFileWriteString(g_dump, "Free,0x%x\n", pMemBlock);

			// deallocate memory
			xnOSFree(pNode);

			return;
		}

		// move to next
		pPrev = pNode;
		pNode = pNode->pNext;
	}

	// if we got here then we're trying to free a non-allocated memory
	XN_ASSERT(FALSE);
}

XN_C_API void xnOSWriteMemoryReport(const XnChar* csFileName)
{
	XN_FILE_HANDLE FileHandle;
	XnStatus nRetVal = xnOSOpenFile(csFileName, XN_OS_FILE_WRITE | XN_OS_FILE_TRUNCATE, &FileHandle);
	if (nRetVal != XN_STATUS_OK) return;

	const XnUInt32 nReportLineMaxSize = 2048;
	XnChar csReportLine[nReportLineMaxSize];
	XnUInt32 nReportLength = 0;

	XnUInt32 nChars;

	XnUInt32 nSum = 0;
	xnOSStrFormat(csReportLine + nReportLength, nReportLineMaxSize - nReportLength, &nChars, "Allocated memory blocks:\n");
	nReportLength += nChars;
	xnOSStrFormat(csReportLine + nReportLength, nReportLineMaxSize - nReportLength, &nChars, "============================================\n");
	nReportLength += nChars;

	xnOSWriteFile(FileHandle, csReportLine, nReportLength);
	nReportLength = 0;

	{
		XnAutoCSLocker lock(g_hCS);
		for (XnMemBlockDataNode* pNode = g_allocatedMemory.pFirst; pNode != NULL; pNode = pNode->pNext)
		{
			xnOSStrFormat(csReportLine + nReportLength, nReportLineMaxSize - nReportLength, &nChars, "%d bytes allocated at 0x%08x using %s", pNode->Data.nBytes, pNode->Data.pMemBlock, XnGetAllocTypeString(pNode->Data.nAllocType));
			nReportLength += nChars;

			if (pNode->Data.csAdditional != NULL)
			{
				xnOSStrFormat(csReportLine + nReportLength, nReportLineMaxSize - nReportLength, &nChars, " (%s)", pNode->Data.csAdditional);
				nReportLength += nChars;
			}

			xnOSStrFormat(csReportLine + nReportLength, nReportLineMaxSize - nReportLength, &nChars, " at %s [%s, %d]\n", pNode->Data.csFunction, pNode->Data.csFile, pNode->Data.nLine);
			nReportLength += nChars;

			if (pNode->Data.nFrames > 0)
			{
				xnOSStrFormat(csReportLine + nReportLength, nReportLineMaxSize - nReportLength, &nChars, "Callstack:\n");
				nReportLength += nChars;

				for (XnUInt i = 0; i < pNode->Data.nFrames; ++i)
				{
					xnOSStrFormat(csReportLine + nReportLength, nReportLineMaxSize - nReportLength, &nChars, "\t%s\n", pNode->Data.aFrames[i]);
					nReportLength += nChars;
				}
			}

			xnOSStrFormat(csReportLine + nReportLength, nReportLineMaxSize - nReportLength, &nChars, "\n");
			nReportLength += nChars;

			xnOSWriteFile(FileHandle, csReportLine, nReportLength);
			nReportLength = 0;

			nSum += pNode->Data.nBytes;
		}
	}

	xnOSStrFormat(csReportLine + nReportLength, nReportLineMaxSize - nReportLength, &nChars, "*******************************************************************\n\n");
	nReportLength += nChars;
	xnOSStrFormat(csReportLine + nReportLength, nReportLineMaxSize - nReportLength, &nChars, "Total allocated memory: %d bytes\n", nSum);
	nReportLength += nChars;

	xnOSWriteFile(FileHandle, csReportLine, nReportLength);
	nReportLength = 0;

	xnOSCloseFile(&FileHandle);
}
