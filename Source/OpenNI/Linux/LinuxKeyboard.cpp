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
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnBool xnOSWasKeyboardHit()
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	// don't echo and don't wait for ENTER
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	
	// make it non-blocking (so we can check without waiting)
	if (0 != fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK))
	{
		return FALSE;
	}

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	if (0 != fcntl(STDIN_FILENO, F_SETFL, oldf))
	{
		return FALSE;
	}

	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return TRUE;
	}

	return FALSE;
}

XN_C_API XnChar xnOSReadCharFromInput()
{
	return (XnChar)getchar();
}

