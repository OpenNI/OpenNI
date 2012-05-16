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
#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

// --------------------------------
// Defines
// --------------------------------
#define KEYBOARD_GROUP_PRESETS "Presets"
#define KEYBOARD_GROUP_DISPLAY "Display"
#define KEYBOARD_GROUP_GENERAL "General"
#define KEYBOARD_GROUP_DEVICE "Device"
#define KEYBOARD_GROUP_CAPTURE "Capture"
#define KEYBOARD_GROUP_PLAYER "Player"

// --------------------------------
// Types
// --------------------------------
typedef void (*ActionFunc)(int);

// --------------------------------
// Function Declarations
// --------------------------------
void startKeyboardMap();
void startKeyboardGroup(const char* csName);
void registerKey(unsigned char key, const char* Description, ActionFunc func, int arg);
void endKeyboardGroup();
void endKeyboardMap();
char getRegisteredKey(ActionFunc func, int arg);
void handleKey(unsigned char key);

void getGroupItems(const char* csGroupName, unsigned char* pKeys, const char** pDescs, int* pCount);

#endif //__KEYBOARD_H__