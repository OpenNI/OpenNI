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
#include <XnOpenNI.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

void printUsage(const char* procName)
{
	printf("Usage: %s [options] VENDOR KEY   (1st form)\n", procName);
	printf("  or:  %s -l                     (2nd form)\n", procName);
	printf("\n");
	printf("In the 1st form, registers or unregisters a license KEY from vendor VENDOR with OpenNI.\n");
	printf("In the 2nd form, lists all registered licenses.\n");
	printf("\n");
	printf("Options:\n");
	printf("-r	Register (default)\n");
	printf("-u	Unregister\n");
	printf("-v	Verbose mode\n");
}

int main(int argc, char* argv[])
{
	const char* strVendor = NULL;
	const char* strKey = NULL;
	bool bRegister = TRUE;
	XnBool bVerbose = FALSE;
	XnBool bList = FALSE;

	if (argc > 1 && strcmp(argv[1], "-l") == 0)
	{
		bList = TRUE;
	}
	else
	{
		for (int i = 1; i < argc; ++i)
		{
			const XnChar* arg = argv[i];
			if (arg[0] == '-')
			{
				if (arg[2] != '\0')
				{
					printUsage(argv[0]);
					return -1;
				}

				switch (arg[1])
				{
				case 'r':
					bRegister = TRUE;
					break;
				case 'u':
					bRegister = FALSE;
					break;
				case 'v':
					bVerbose = TRUE;
					break;
				default:
					printf("Unknown option: -%c\n", arg[1]);
					printUsage(argv[0]);
					return -1;
				}
			}
			else if (strVendor == NULL)
			{
				strVendor = arg;
			}
			else if (strKey == NULL)
			{
				strKey = arg;
			}
			else
			{
				printUsage(argv[0]);
				return -1;
			}
		} // args for

		if (strVendor == NULL || strKey == NULL)
		{
			printUsage(argv[0]);
			return -1;
		}
	}

	xnLogInitSystem();
	xnLogSetConsoleOutput(bVerbose || bList);
	xnLogSetMaskMinSeverity(XN_LOG_MASK_ALL, bVerbose ? XN_LOG_VERBOSE : XN_LOG_WARNING);

	XnStatus nRetVal = XN_STATUS_OK;

	XnLicense license = {0};
	if (!bList)
	{
		nRetVal = xnOSStrNCopy(license.strVendor, strVendor, (XnUInt32)strlen(strVendor) + 1, (XnUInt32)sizeof(license.strVendor));
		if (nRetVal != XN_STATUS_OK)
		{
			printf("Error: Vendor name is too long (should be up to %u characters).", (XnUInt32)sizeof(license.strVendor) - 1);
			return -1;
		}

		nRetVal = xnOSStrNCopy(license.strKey, strKey, (XnUInt32)strlen(strKey) + 1, (XnUInt32)sizeof(license.strKey));
		if (nRetVal != XN_STATUS_OK)
		{
			printf("Error: Key is too long (should be up to %u characters).", (XnUInt32)sizeof(license.strKey) - 1);
			return -1;
		}
	}

	if (bList)
	{
		nRetVal = xnPrintRegisteredLicenses();
	}
	else if (bRegister)
	{
		nRetVal = xnRegisterGlobalLicense(&license);
	}
	else
	{
		nRetVal = xnUnregisterGlobalLicense(&license);
	}

	if (nRetVal != XN_STATUS_OK)
	{
		printf("Failed: %s\n", xnGetStatusString(nRetVal));
		return -1;
	}

	if (bVerbose)
	{
		printf("Done.\n");
	}

	return 0;
}
