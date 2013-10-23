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
