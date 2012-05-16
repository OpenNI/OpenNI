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
#include "Keyboard.h"
#include <string.h>

// --------------------------------
// Types
// --------------------------------
typedef struct XnKeyboardAction
{
	char key;
	const char* csDescription;
	ActionFunc pCallbackFunc;
	int nCallbackArg;
} XnKeyboardAction;

typedef struct XnKeyboardGroup
{
	const char* csName;
	int nFirst;
	int nLast;
} XnKeyboardGroup;

// --------------------------------
// Global Variables
// --------------------------------
XnKeyboardAction g_KeyboardMap[500];
XnKeyboardGroup g_Groups[20];
int g_nRegisteredKeys = 0;
int g_nRegisteredGroups = 0;

// --------------------------------
// Code
// --------------------------------
void startKeyboardMap()
{
	g_nRegisteredKeys = 0;
}

void startKeyboardGroup(const char* csName)
{
	g_Groups[g_nRegisteredGroups].csName = csName;
	g_Groups[g_nRegisteredGroups].nFirst = g_nRegisteredKeys;
}

void registerKey(unsigned char key, const char* Description, ActionFunc func, int arg)
{
	XnKeyboardAction* pKey = &g_KeyboardMap[g_nRegisteredKeys];
	pKey->key = key;
	pKey->csDescription = Description;
	pKey->pCallbackFunc = func;
	pKey->nCallbackArg = arg;
	g_nRegisteredKeys++;
}

void endKeyboardGroup()
{
	g_Groups[g_nRegisteredGroups].nLast = g_nRegisteredKeys;
	g_nRegisteredGroups++;
}

void endKeyboardMap()
{

}

char getRegisteredKey(ActionFunc func, int arg)
{
	for (int i = 0; i < g_nRegisteredKeys; ++i)
	{
		if (g_KeyboardMap[i].pCallbackFunc == func && g_KeyboardMap[i].nCallbackArg == arg)
			return g_KeyboardMap[i].key;
	}

	return 0;
}

void handleKey(unsigned char k)
{
	for (int i = 0; i < g_nRegisteredKeys; ++i)
	{
		if (k == g_KeyboardMap[i].key)
		{
			g_KeyboardMap[i].pCallbackFunc(g_KeyboardMap[i].nCallbackArg);
			return;
		}
	}
}

int findGroup(const char* csGroupName)
{
	for (int i = 0; i < g_nRegisteredGroups; ++i)
	{
		if (strcmp(g_Groups[i].csName, csGroupName) == 0)
			return i;
	}

	return -1;
}

void getGroupItems(const char* csGroupName, unsigned char* pKeys, const char** pDescs, int* pCount)
{
	// find group
	int nGroup = findGroup(csGroupName);
	int nCount = 0;

	for (int nEntry = g_Groups[nGroup].nFirst; nEntry < g_Groups[nGroup].nLast; nEntry++, nCount++)
	{
		pKeys[nCount] = g_KeyboardMap[nEntry].key;
		pDescs[nCount] = g_KeyboardMap[nEntry].csDescription;
	}

	*pCount = nCount;
}
