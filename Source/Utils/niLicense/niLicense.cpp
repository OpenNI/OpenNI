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
