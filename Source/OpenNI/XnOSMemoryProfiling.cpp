/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
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
static XnDump m_dump;

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

		m_dump = XN_DUMP_CLOSED;

		bReentrent = TRUE;
		xnOSCreateCriticalSection(&g_hCS);

#ifdef XN_MEMORY_PROFILING_DUMP
		xnDumpForceInit(&m_dump, "Entry,Address,AllocType,Bytes,Function,File,Line,AdditionalInfo\n", "MemProfiling.log");
#endif
		bReentrent = FALSE;
	}

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
	xnDumpWriteString(m_dump, "Alloc,0x%x,%s,%u,%s,%s,%u,%s\n", pMemBlock, XnGetAllocTypeString(nAllocType), nBytes, csFunction, csFile, nLine, csAdditional);

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

			xnDumpWriteString(m_dump, "Free,0x%x\n", pMemBlock);

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
