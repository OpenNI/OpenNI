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
#include <XnLog.h>

#ifdef __INTEL_COMPILER
	#include <ia32intrin.h>
#else
	#include <intrin.h>
#endif

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
static const XnChar* GetOSName(OSVERSIONINFOEX& osVersionInfo)
{
	if (osVersionInfo.dwMajorVersion == 4)
	{
		// Windows NT
		switch (osVersionInfo.wProductType)
		{
		case VER_NT_WORKSTATION:
			return "Windows NT 4.0 Workstation";
		case VER_NT_SERVER:
		case VER_NT_DOMAIN_CONTROLLER:
			return "Windows NT 4.0 Server";
		default:
			return "Unknown Windows NT 4.0 version";
		}
	}
	else if (osVersionInfo.dwMajorVersion == 5 && osVersionInfo.dwMinorVersion == 0)
	{
		// Windows 2000
		switch (osVersionInfo.wProductType)
		{
		case VER_NT_WORKSTATION:
			return "Windows 2000 Professional";
		case VER_NT_SERVER:
		case VER_NT_DOMAIN_CONTROLLER:
			if ((osVersionInfo.wSuiteMask & VER_SUITE_DATACENTER) == VER_SUITE_DATACENTER)
				return "Windows 2000 Datacenter Server";
			else if ((osVersionInfo.wSuiteMask & VER_SUITE_ENTERPRISE) == VER_SUITE_ENTERPRISE)
				return "Windows 2000 Advanced Server";
			else
				return "Windows 2000 Server";
		default:
			return "Unknown Windows 2000 version";
		}
	}
	else if (osVersionInfo.dwMajorVersion == 5 && osVersionInfo.dwMinorVersion == 1)
	{
		// Windows XP.
		if (0 != GetSystemMetrics(SM_TABLETPC))
			return "Windows XP Tablet PC Edition";
		else if (0 != GetSystemMetrics(SM_STARTER))
			return "Windows XP Starter Edition";
		else if (0 != GetSystemMetrics(SM_MEDIACENTER))
			return "Windows XP Media Center Edition";
		else if ((osVersionInfo.wSuiteMask & VER_SUITE_PERSONAL) == VER_SUITE_PERSONAL)
			return "Windows XP Home Edition";
		else
			return "Windows XP Professional";
	}
	else if (osVersionInfo.dwMajorVersion == 5 && osVersionInfo.dwMinorVersion == 2)
	{
		// Windows Server 2003
		if((osVersionInfo.wSuiteMask & VER_SUITE_DATACENTER) == VER_SUITE_DATACENTER)
			return "Windows Server 2003 Datacenter Edition";
		else if((osVersionInfo.wSuiteMask & VER_SUITE_ENTERPRISE) == VER_SUITE_ENTERPRISE)
			return "Windows Server 2003 Enterprise Edition";
		else if((osVersionInfo.wSuiteMask & VER_SUITE_BLADE) == VER_SUITE_BLADE)
			return "Windows Server 2003 Web Edition";
		else 
			return "Windows Server 2003 Standard Edition";
	}
	else if (osVersionInfo.dwMajorVersion == 6 && osVersionInfo.dwMinorVersion == 0)
	{
		// Windows Vista / Server 2008
		switch (osVersionInfo.wProductType)
		{
		case VER_NT_WORKSTATION:
			// Vista
			return "Windows Vista";
		case VER_NT_SERVER:
		case VER_NT_DOMAIN_CONTROLLER:
			return "Windows Server 2008";
		default:
			return "Unknown Windows 6.0 version";
		}
	}
	else if (osVersionInfo.dwMajorVersion == 6 && osVersionInfo.dwMinorVersion == 1)
	{
		// Windows 7 / Server 2008 R2
		if (osVersionInfo.wProductType != VER_NT_WORKSTATION)
		{
			return ("Windows Server 2008 R2");
		}
		else
		{
			return ("Windows 7");
		}
	}
	else if (osVersionInfo.dwMajorVersion == 6 && osVersionInfo.dwMinorVersion == 2)
	{
		// Windows 8 / Server 2012
		if (osVersionInfo.wProductType != VER_NT_WORKSTATION)
		{
			return ("Windows Server 2012");
		}
		else
		{
			return ("Windows 8");
		}
	}

	return "Unknown Windows Version";
}

static void GetCPUName(XnChar* csName)
{
	int CPUInfo[4] = {-1};
    xnOSMemSet(csName, 0, XN_MAX_OS_NAME_LENGTH);

	// calling cpuid with 0x80000000 retrives the number of extended properties
	__cpuid(CPUInfo, 0x80000000);
	unsigned int nExIds = CPUInfo[0];

	if (nExIds >= 0x80000002)
	{
		__cpuid(CPUInfo, 0x80000002);
		xnOSMemCopy(csName, CPUInfo, sizeof(CPUInfo));	
	}

	if (nExIds >= 0x80000003)
	{
		__cpuid(CPUInfo, 0x80000003);
		xnOSMemCopy(csName + 16, CPUInfo, sizeof(CPUInfo));	
	}

	if (nExIds >= 0x80000004)
	{
		__cpuid(CPUInfo, 0x80000004);
		xnOSMemCopy(csName + 32, CPUInfo, sizeof(CPUInfo));	
	}

	if (nExIds > 0x80000002)
		return;

	// else, we need to build the name ourselves
	xnOSMemSet(CPUInfo, 0xFF, sizeof(CPUInfo));

	__cpuid(CPUInfo, 0);
	int nIds = CPUInfo[0];

    *((int*)csName) = CPUInfo[1];
    *((int*)(csName+4)) = CPUInfo[3];
    *((int*)(csName+8)) = CPUInfo[2];

	if (nIds >= 1)
	{
		__cpuid(CPUInfo, 1);
        int nSteppingID = CPUInfo[0] & 0xf;
        int nModel = (CPUInfo[0] >> 4) & 0xf;
        int nFamily = (CPUInfo[0] >> 8) & 0xf;
        //int nProcessorType = (CPUInfo[0] >> 12) & 0x3;
        //int nExtendedmodel = (CPUInfo[0] >> 16) & 0xf;
        //int nExtendedfamily = (CPUInfo[0] >> 20) & 0xff;
        //int nBrandIndex = CPUInfo[1] & 0xff;

		sprintf(csName+strlen(csName), " Model: %d Family: %d Stepping: %d", nModel, nFamily, nSteppingID);
	}
}

XN_C_API XnStatus xnOSGetInfo(xnOSInfo* pOSInfo)
{
	// Validate input/output arguments
	XN_VALIDATE_OUTPUT_PTR(pOSInfo);

	// Get OS Info
	OSVERSIONINFOEX osVersionInfo;
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (0 == GetVersionEx((LPOSVERSIONINFO)&osVersionInfo))
	{
		DWORD nErr = GetLastError();
		xnLogWarning(XN_MASK_OS, "Failed getting OS version information. Error code: %d", nErr);
		return XN_STATUS_ERROR;
	}

	sprintf(pOSInfo->csOSName, "%s", GetOSName(osVersionInfo));
	if (osVersionInfo.szCSDVersion[0] != '\0')
	{
		strcat(pOSInfo->csOSName, " ");
		strcat(pOSInfo->csOSName, osVersionInfo.szCSDVersion);
	}

	// Get CPU Info
	GetCPUName(pOSInfo->csCPUName);

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	pOSInfo->nProcessorsCount = systemInfo.dwNumberOfProcessors;

	// Get Memory Info
	MEMORYSTATUSEX memoryStatus;
	memoryStatus.dwLength = sizeof(memoryStatus);
	if (0 == GlobalMemoryStatusEx(&memoryStatus))
	{
		DWORD nErr = GetLastError();
		xnLogWarning(XN_MASK_OS, "Failed getting Memory information. Error code: %d", nErr);
		return XN_STATUS_ERROR;
	}

	pOSInfo->nTotalMemory = memoryStatus.ullTotalPhys;

	return (XN_STATUS_OK);
}

XnStatus XnWin32CreateKernelObjectName(XnChar* strDest, const XnUInt32 nDestLength, const XnChar* strSource, XnBool bAllowOtherUsers)
{
	XnChar* pDest = strDest;
	XnChar* pDestEnd = strDest + nDestLength;

	if (bAllowOtherUsers)
	{
		static const XnChar strGlobal[] = "Global\\";
		strcpy(strDest, strGlobal);
		pDest = strDest + sizeof(strGlobal) - 1;
	}

	const XnChar* pSource = strSource;

	while (pDest < pDestEnd && *pSource != '\0')
	{
		*pDest = *pSource == '\\' ? '_' : *pSource;
		++pDest;
		++pSource;
	}

	if (pDest == pDestEnd)
	{
		xnLogWarning(XN_MASK_OS, "Event name is too long!");
		return XN_STATUS_ERROR;
	}

	*pDest = '\0';

	return (XN_STATUS_OK);
}

XnStatus XnWin32GetSecurityAttributes(XnBool bAllowOtherUsers, SECURITY_ATTRIBUTES** ppSecurityAttributes)
{
	*ppSecurityAttributes = NULL;

	static SECURITY_DESCRIPTOR securityDesc;
	static SECURITY_ATTRIBUTES securityAttributes;
	static SECURITY_ATTRIBUTES* pSecurityAttributes = NULL;

	if (pSecurityAttributes == NULL)
	{
		// initialize it now
		if (FALSE == InitializeSecurityDescriptor(&securityDesc, SECURITY_DESCRIPTOR_REVISION))
		{
			xnLogError(XN_MASK_OS, "Failed to initialize security descriptor (%d)", GetLastError());
			return XN_STATUS_ERROR;
		}

		if (FALSE == SetSecurityDescriptorDacl(&securityDesc, TRUE, NULL, FALSE))
		{
			xnLogError(XN_MASK_OS, "Failed to set security descriptor DACL (%d)", GetLastError());
			return XN_STATUS_ERROR;
		}

		securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
		securityAttributes.lpSecurityDescriptor = &securityDesc;
		securityAttributes.bInheritHandle = FALSE;

		// initialization complete. Store a pointer
		pSecurityAttributes = &securityAttributes;
	}

	*ppSecurityAttributes = bAllowOtherUsers ? pSecurityAttributes : NULL;

	return (XN_STATUS_OK);
}
