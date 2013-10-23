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
#include <dbghelp.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef BOOL (__stdcall *StackWalk64Prot)(
	DWORD MachineType,
	HANDLE hProcess,
	HANDLE hThread,
	LPSTACKFRAME64 StackFrame,
	PVOID ContextRecord,
	PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
	PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
	PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
	PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress
	);

typedef BOOL (__stdcall *SymInitializeProt)(HANDLE hProcess, PSTR UserSearchPath, BOOL fInvadeProcess);
typedef DWORD (__stdcall *SymSetOptionsProt)(DWORD SymOptions);
typedef BOOL (__stdcall *SymFromAddrProt)(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFO Symbol);
typedef PVOID (__stdcall *SymFunctionTableAccess64Prot)(HANDLE hProcess, DWORD64 AddrBase);
typedef DWORD64 (__stdcall *SymGetModuleBase64Prot)(HANDLE hProcess, DWORD64 qwAddr);

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
static XnBool g_bInitialized = FALSE;
StackWalk64Prot g_pStackWalk64 = NULL;
SymFromAddrProt g_pSymFromAddr = NULL;
SymFunctionTableAccess64Prot g_pSymFunctionTableAccess64 = NULL;
SymGetModuleBase64Prot g_pSymGetModuleBase64 = NULL;
static XnBool g_bAvailable = FALSE;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

static void Init()
{
	// NOTE: we load dbghelp dynamically, so that it won't crash if a system doesn't have it
	HMODULE hDbg = LoadLibrary("dbghelp.dll");
	if (hDbg == NULL)
	{
		return;
	}

	SymInitializeProt pSymInitialize = (SymInitializeProt)GetProcAddress(hDbg, "SymInitialize");
	SymSetOptionsProt pSymSetOptions = (SymSetOptionsProt)GetProcAddress(hDbg, "SymSetOptions");

	if (FALSE == pSymInitialize(GetCurrentProcess(), NULL, TRUE))
	{
		return;
	}

	pSymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);

	g_pStackWalk64 = (StackWalk64Prot)GetProcAddress(hDbg, "StackWalk64");
	g_pSymFromAddr = (SymFromAddrProt)GetProcAddress(hDbg, "SymFromAddr");
	g_pSymFunctionTableAccess64 = (SymFunctionTableAccess64Prot)GetProcAddress(hDbg, "SymFunctionTableAccess64");
	g_pSymGetModuleBase64 = (SymGetModuleBase64Prot)GetProcAddress(hDbg, "SymGetModuleBase64");

	g_bAvailable = 
		(g_pStackWalk64 != NULL) && 
		(g_pSymFromAddr != NULL) &&
		(g_pSymFunctionTableAccess64 != NULL) &&
		(g_pSymGetModuleBase64 != NULL);
}

XN_C_API XnStatus xnOSGetCurrentCallStack(XnUInt32 nFramesToSkip, XnChar** astrFrames, XnUInt32 nMaxNameLength, XnUInt32* pnFrames)
{
	if (*pnFrames == 0 || nMaxNameLength == 0)
	{
		return XN_STATUS_OK;
	}

	// Init
	if (!g_bInitialized)
	{
		Init();
		g_bInitialized = TRUE;
	}

	if (!g_bAvailable)
	{
		xnOSStrNCopy(astrFrames[0], "dbghelp.dll unavailable!", nMaxNameLength-1, nMaxNameLength);
		return XN_STATUS_ERROR;
	}

	// Get context
	CONTEXT context;
	RtlCaptureContext(&context);

	// init STACKFRAME for first call - Fill data according to processor (see WalkTrace64 and STACKFRAME64 documentation)
	STACKFRAME64 stackFrame;
	memset(&stackFrame, 0, sizeof(stackFrame));

	DWORD MachineType;
#ifdef _M_IX86
	MachineType = IMAGE_FILE_MACHINE_I386;
	stackFrame.AddrPC.Offset = context.Eip;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = context.Ebp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = context.Esp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
	MachineType = IMAGE_FILE_MACHINE_AMD64;
	stackFrame.AddrPC.Offset = context.Rip;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = context.Rsp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = context.Rsp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
	MachineType = IMAGE_FILE_MACHINE_IA64;
	stackFrame.AddrPC.Offset = context.StIIP;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = context.IntSp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrBStore.Offset = context.RsBSP;
	stackFrame.AddrBStore.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = context.IntSp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

	XnUInt32 nFrames = 0;
	XnUInt32 iFrame = 0;
	const XnUInt32 BUFFER_SIZE = 1024;
	XnChar symbolBuffer[BUFFER_SIZE];
	SYMBOL_INFO* pSymbolInfo = (SYMBOL_INFO*)symbolBuffer;
	pSymbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbolInfo->MaxNameLen = BUFFER_SIZE - sizeof(SYMBOL_INFO) - 1;

	while (iFrame < *pnFrames)
	{
		// walk the stack
		if (!g_pStackWalk64(MachineType, GetCurrentProcess(), GetCurrentThread(), &stackFrame, &context, NULL, g_pSymFunctionTableAccess64, g_pSymGetModuleBase64, NULL))
		{
			// probably reached end
			break;
		}

		if (nFrames >= nFramesToSkip)
		{
			// resolve function name
			XnUInt32 nWritten;
			if (g_pSymFromAddr(GetCurrentProcess(), stackFrame.AddrPC.Offset, NULL, pSymbolInfo))
			{
				xnOSStrFormat(astrFrames[iFrame], nMaxNameLength, &nWritten, "%s()", pSymbolInfo->Name);
			}
			else
			{
				xnOSStrFormat(astrFrames[iFrame], nMaxNameLength, &nWritten, "0x%x", stackFrame.AddrPC.Offset);
			}

			++iFrame;
		}

		++nFrames;
	}

	*pnFrames = iFrame;

	return XN_STATUS_OK;
}
