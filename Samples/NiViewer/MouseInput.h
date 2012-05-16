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
// Types
// --------------------------------
typedef struct
{
	int X;
	int Y;
} IntPair;

typedef struct
{
	double X;
	double Y;
} DoublePair;

typedef struct 
{
	double fBottom;
	double fLeft;
	double fTop;
	double fRight;
} DoubleRect;

typedef struct  
{
	int uBottom;
	int uLeft;
	int uTop;
	int uRight;
} IntRect;

typedef enum SelectionState
{
	SELECTION_NONE,
	SELECTION_ACTIVE,
	SELECTION_DONE
} SelectionState;

typedef void (*SelectionRectangleChangedPtr)(SelectionState state, IntRect selection);
typedef void (*CursorMovedPtr)(IntPair location);

// --------------------------------
// Functions
// --------------------------------
void mouseInputMotion(int x, int y);
void mouseInputButton(int button, int state, int x, int y);
void mouseInputRegisterForSelectionRectangle(SelectionRectangleChangedPtr pFunc);
void mouseInputRegisterForCursorMovement(CursorMovedPtr pFunc);


