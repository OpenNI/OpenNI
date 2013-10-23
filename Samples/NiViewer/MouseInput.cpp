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
#include "MouseInput.h"
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif

// --------------------------------
// Types
// --------------------------------
typedef struct MouseInput
{
	SelectionState nSelectionState;
	IntPair StartSelection;
	IntPair LastLocation;
	SelectionRectangleChangedPtr pSelectionCallback;
	CursorMovedPtr pCursorCallback;
} MouseInput;

MouseInput g_MouseInput = {SELECTION_NONE, {0,0}, {0,0}, NULL, NULL};

void mouseInputCallSelection()
{
	if (g_MouseInput.pSelectionCallback != NULL)
	{
		IntRect selection;
		selection.uBottom = XN_MIN(g_MouseInput.StartSelection.Y, g_MouseInput.LastLocation.Y);
		selection.uTop = XN_MAX(g_MouseInput.StartSelection.Y, g_MouseInput.LastLocation.Y);
		selection.uLeft = XN_MIN(g_MouseInput.StartSelection.X, g_MouseInput.LastLocation.X);
		selection.uRight = XN_MAX(g_MouseInput.StartSelection.X, g_MouseInput.LastLocation.X);

		g_MouseInput.pSelectionCallback(g_MouseInput.nSelectionState, selection);
	}
}

void mouseInputMotion(int x, int y)
{
	g_MouseInput.LastLocation.X = x;
	g_MouseInput.LastLocation.Y	= y;

	if (g_MouseInput.pCursorCallback != NULL)
		g_MouseInput.pCursorCallback(g_MouseInput.LastLocation);

	if (g_MouseInput.nSelectionState == SELECTION_ACTIVE)
		mouseInputCallSelection();
}

void mouseInputButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			g_MouseInput.nSelectionState = SELECTION_ACTIVE;
			g_MouseInput.StartSelection.X = x;
			g_MouseInput.StartSelection.Y = y;
		}
		else if (state == GLUT_UP && g_MouseInput.nSelectionState == SELECTION_ACTIVE)
		{
			// this is only a selection if mouse has moved
			if (x != g_MouseInput.StartSelection.X && y != g_MouseInput.StartSelection.Y)
			{
				g_MouseInput.nSelectionState = SELECTION_DONE;
				mouseInputCallSelection();
			}

			g_MouseInput.nSelectionState = SELECTION_NONE;
		}
	}
}

void mouseInputRegisterForSelectionRectangle(SelectionRectangleChangedPtr pFunc)
{
	g_MouseInput.pSelectionCallback = pFunc;
}

void mouseInputRegisterForCursorMovement(CursorMovedPtr pFunc)
{
	g_MouseInput.pCursorCallback = pFunc;
}

