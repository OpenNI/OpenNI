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
// --------------------------------
// Includes
// --------------------------------
#include <XnPlatform.h>
#include "Menu.h"
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif
#include <stdio.h>
#include <string.h>

// --------------------------------
// Defines
// --------------------------------
#define XN_MAX_MENU_ENTRIES 300
#define XN_MAX_MENU_ENTRY_NAME_LEN 70
#define XN_MAX_MENU_DEPTH 20

// --------------------------------
// Types
// --------------------------------
struct XnMenuBuilder
{
	int aMenuPath[XN_MAX_MENU_DEPTH];
	int nMenuPathIndex;
	int nCurrMenuItem;
} g_MenuBuilder;

typedef struct XnMenuEntry
{
	char csName[XN_MAX_MENU_ENTRY_NAME_LEN];
	ActionFunc pCallbackFunc;
	int nCallbackArg;
} XnMenuEntry;

// --------------------------------
// Global Variables
// --------------------------------
/* Holds a list of all menu entries and their callbacks. */
XnMenuEntry	g_MenuEntries[XN_MAX_MENU_ENTRIES];

// --------------------------------
// Code
// --------------------------------
void mainMenuCallback(int nMenuID)
{
	g_MenuEntries[nMenuID].pCallbackFunc(g_MenuEntries[nMenuID].nCallbackArg);
}

void startMenu()
{
	g_MenuBuilder.aMenuPath[0] = glutCreateMenu(mainMenuCallback);
	g_MenuBuilder.nMenuPathIndex = 0;
	g_MenuBuilder.nCurrMenuItem = 0;
}

void startSubMenu(const char* csName)
{
	int nNewMenuID = glutCreateMenu(mainMenuCallback);
	glutSetMenu(g_MenuBuilder.aMenuPath[g_MenuBuilder.nMenuPathIndex]);
	glutAddSubMenu(csName, nNewMenuID);

	(g_MenuBuilder.nMenuPathIndex)++;
	g_MenuBuilder.aMenuPath[g_MenuBuilder.nMenuPathIndex] = nNewMenuID;
	glutSetMenu(g_MenuBuilder.aMenuPath[g_MenuBuilder.nMenuPathIndex]);
}

void createMenuEntry(const char* csName, ActionFunc func, int arg)
{
	strcpy(g_MenuEntries[g_MenuBuilder.nCurrMenuItem].csName, csName);

	char key = getRegisteredKey(func, arg);
	if (key != 0)
	{
		if (key == 27)
			strcat(g_MenuEntries[g_MenuBuilder.nCurrMenuItem].csName + strlen(csName), " [Esc]");
		else
			sprintf(g_MenuEntries[g_MenuBuilder.nCurrMenuItem].csName + strlen(csName), " [%c]", key);
	}

	g_MenuEntries[g_MenuBuilder.nCurrMenuItem].pCallbackFunc = func;
	g_MenuEntries[g_MenuBuilder.nCurrMenuItem].nCallbackArg = arg;

	glutAddMenuEntry(g_MenuEntries[g_MenuBuilder.nCurrMenuItem].csName, g_MenuBuilder.nCurrMenuItem);

	(g_MenuBuilder.nCurrMenuItem)++;
}

void endSubMenu()
{
	(g_MenuBuilder.nMenuPathIndex)--;
	int nParentMenuID = g_MenuBuilder.aMenuPath[g_MenuBuilder.nMenuPathIndex];

	glutSetMenu(nParentMenuID);
}

void endMenu()
{
	glutSetMenu(g_MenuBuilder.aMenuPath[0]);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
