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
	xnLogSetSeverityFilter(bVerbose ? XN_LOG_VERBOSE : XN_LOG_WARNING);
	xnLogSetMaskState(XN_LOG_MASK_ALL, TRUE);

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
