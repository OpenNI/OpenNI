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
