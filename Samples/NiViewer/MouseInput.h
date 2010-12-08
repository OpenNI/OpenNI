/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




// --------------------------------
// Types
// --------------------------------
typedef struct
{
	unsigned int X;
	unsigned int Y;
} UIntPair;

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
	unsigned int uBottom;
	unsigned int uLeft;
	unsigned int uTop;
	unsigned int uRight;
} UIntRect;

typedef enum SelectionState
{
	SELECTION_NONE,
	SELECTION_ACTIVE,
	SELECTION_DONE
} SelectionState;

typedef void (*SelectionRectangleChangedPtr)(SelectionState state, UIntRect selection);
typedef void (*CursorMovedPtr)(UIntPair location);

// --------------------------------
// Functions
// --------------------------------
void mouseInputMotion(int x, int y);
void mouseInputButton(int button, int state, int x, int y);
void mouseInputRegisterForSelectionRectangle(SelectionRectangleChangedPtr pFunc);
void mouseInputRegisterForCursorMovement(CursorMovedPtr pFunc);


