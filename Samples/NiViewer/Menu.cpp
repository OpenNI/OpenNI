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
