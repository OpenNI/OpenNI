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
#include <XnOS.h>
#include <errno.h>
#if (XN_PLATFORM == XN_PLATFORM_MACOSX || XN_PLATFORM == XN_PLATFORM_ANDROID_ARM)
	#include <sys/wait.h>
#else
	#include <wait.h>
#endif
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSGetCurrentProcessID(XN_PROCESS_ID* pProcID)
{
	// Validate output pointer
	XN_VALIDATE_OUTPUT_PTR(pProcID);

	// Get the current process id
	*pProcID = getpid();

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateProcess(const XnChar* strExecutable, XnUInt32 nArgs, const XnChar** pstrArgs, XN_PROCESS_ID* pProcID)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// NOTE: execv, for some weird reason, accepts the arguments as non-const strings, but in the documentation
	// it is explicitly stated that it does not change anything, so we can just use const_cast for it.
	XnChar* astrArgs[100];
	if (nArgs > 98)
	{
		xnLogWarning(XN_MASK_OS, "Too many arguments!");
		return XN_STATUS_OS_PROCESS_CREATION_FAILED;
	}

	astrArgs[0] = const_cast<char*>(strExecutable);
	for (XnUInt i = 0; i < nArgs; ++i)
	{
		astrArgs[i+1] = const_cast<char*>(pstrArgs[i]);
	}
	astrArgs[nArgs+1] = NULL;
	
	pid_t child_pid = fork();
	if (child_pid == -1) // fail
	{
		xnLogWarning(XN_MASK_OS, "Failed to start process! fork() error code is %d.", errno);
		return XN_STATUS_OS_PROCESS_CREATION_FAILED;
	}
	else if (child_pid == 0) // child process
	{
		// fork again
		// we want our process to be totally detached from creator process. To do so, we fork() twice, so that the creator process
		// is terminated, and the new process becomes the child of init.
		pid_t grandchild_pid = fork();
		if (grandchild_pid == -1)
		{
			exit(XN_STATUS_OS_PROCESS_CREATION_FAILED);
		}
		else if (grandchild_pid == 0) // grandchild process
		{
			// close standard streams (this is a background process)
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
			
			// detach it from current process (so it will keep running, even if process is terminated)
			setsid();
			
			// load the executable
			if (-1 == execv(strExecutable, astrArgs))
			{
				xnLogWarning(XN_MASK_OS, "Failed to start process! execv() error code is %d.", errno);
				return XN_STATUS_OS_PROCESS_CREATION_FAILED;
			}
		}
		else // child process
		{
			_exit(XN_STATUS_OK);
		}
	}
	else // parent process
	{
		// wait for the child process (the one creating the grandchild process), and make sure it succeeded)
		int status = 0;
		waitpid(child_pid, &status, 0);
		if (status != 0)
		{
			return XN_STATUS_OS_PROCESS_CREATION_FAILED;
		}

		// we have no way to get the grandchild pid
		*pProcID = 0;
	}
	
	return (XN_STATUS_OK);
}
