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
#ifndef XNV_USER_SELECTION_STRUCTURE_H_
#define XNV_USER_SELECTION_STRUCTURE_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

/// @brief Enumeration of legal user selection states
/// @ingroup UserSelectionClasses
typedef enum XnSelectionState 
{
    XN_SELECTION_UNSELECTED = 0,  ///< @brief The user is not selected and is available for selection
    XN_SELECTION_SELECTED = 1,    ///< @brief The user have been selected but is not tracked yet
    XN_SELECTION_TRACKING = 2,    ///< @brief The user is being tracked
    XN_SELECTION_FAILED = 3       ///< @brief The user is not selected and not tracked and in the past when selected it failed to be tracked. 

} XnSelectionState; ///< @brief Enum for states of a user

/// @brief Holds the state and sub state of a user.
/// @ingroup UserSelectionClasses
class UserSelectionState
{
public:
    XnSelectionState m_eState;  ///< @brief The basic state of the user
    XnInt64 m_subState;         ///< @brief The progress of the basic state. The meaning of this changes based on the state.

    UserSelectionState()
    {
        m_eState=XN_SELECTION_UNSELECTED;
        m_subState=0;
    }
    ~UserSelectionState() {};
};

#endif // XNV_USER_SELECTION_STRUCTURE_H_
