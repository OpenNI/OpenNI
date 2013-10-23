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
	printf("Usage: %s [options] FILE [CONFIG_DIR]           (1st form)\n", procName);
	printf("  or:  %s -l                                    (2nd form)\n", procName);
	printf("\n");
	printf("In the 1st form, registers or unregisters module FILE with OpenNI. When registering, it\n");
	printf("is also possible to provide configuration dir CONFIG_DIR to be passed to that module.\n");
	printf("In the 2nd form, lists all registered modules and production nodes.\n");
	printf("\n");
	printf("Options:\n");
	printf("-r	Register (default)\n");
	printf("-u	Unregister\n");
	printf("-v	Verbose mode\n");
}

int main(XnInt argc, XnChar* argv[])
{
	const XnChar* strFilePath = NULL;
	const XnChar* strConfigDir = NULL;
	XnBool bRegister = TRUE;
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
			else if (strFilePath == NULL)
			{
				strFilePath = arg;
			}
			else if (strConfigDir == NULL)
			{
				strConfigDir = arg;
			}
			else
			{
				printUsage(argv[0]);
				return -1;
			}
		} // args for

		if (strFilePath == NULL)
		{
			printUsage(argv[0]);
			return -1;
		}
	}

	xnLogInitSystem();
	xnLogSetConsoleOutput(bVerbose || bList);
	xnLogSetMaskMinSeverity(XN_LOG_MASK_ALL, bVerbose ? XN_LOG_VERBOSE : XN_LOG_WARNING);

	XnStatus nRetVal = XN_STATUS_OK;
	if (bList)
	{
		nRetVal = xnPrintRegisteredModules();
	}
	else if (bRegister)
	{
		nRetVal = xnRegisterModule(strFilePath, strConfigDir);
	}
	else
	{
		nRetVal = xnUnregisterModule(strFilePath);
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
