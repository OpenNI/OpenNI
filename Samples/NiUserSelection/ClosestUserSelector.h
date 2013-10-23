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
#ifndef XNV_CLOSEST_USER_SELECTOR_H_
#define XNV_CLOSEST_USER_SELECTOR_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "UserSelector.h"
#include "TrackingInitializer.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------



/// @brief Closest users based selector
/// 
/// This user selector selects user and tracks that user. If a new user becomes closer, it stops 
/// tracking the old one and tracks the new, closer user.
/// @ingroup UserSelectionClosestSelectors
class ClosestUserSelector : public UserSelector
{
public:
    /// @brief Constructor
    /// 
    /// @param pUserGenerator The user generator to use
    /// @param pTrackingInitializer The tracking initializer to use
    /// @param nMaxNumUsers The maximum number of tracked users
    ClosestUserSelector(xn::UserGenerator* pUserGenerator, TrackingInitializer* pTrackingInitializer, XnUInt32 nMaxNumUsers);
    ~ClosestUserSelector();

    virtual void UpdateFrame();
protected:
    virtual UserSelectionState* CreateUserSelectionState();


    virtual XnStatus UpdateUserSelectionState(XnUserID nUserId, XnSelectionState eState , XnInt64 subState);


    TrackingInitializer* m_pTrackingInitializer; ///< @brief The tracking initializer used to start tracking
    XnUInt32 m_nMaxNumUsers; ///< @brief The maximum number of allowed users to track.

    /// @brief Internal struct to hold the user information
    /// 
    /// This class is used to hold the Z of the center of mass for each user
    /// @ingroup UserSelectionClosestSelectors
    struct UserInfo
    {
        XnUserID m_userID;  ///< @brief User ID of the user
        XnFloat m_COMZ;     ///< @brief Z value of the center of mass.
    };

    UserInfo* m_pUsersList; ///< An array holding the user info for all users (to order them).

    /// @brief Internal method to clear the user list information.
    void ClearUserList();
    /// @brief Internal method to add a user to the list of users
    /// 
    /// @param userID the user ID of the user to add
    /// @param COMZ the z value of the center of mass of the user
    void InsertNewUser(XnUserID userID, XnFloat COMZ);
    /// @brief Internal method to test if the user is close enough to be counted for tracking.
    /// 
    /// @param userID The user to test.
    /// @return True if the user should be tracked, false otherwise.
    XnBool TestIfShouldTrack(XnUserID userID);

    /// @brief An extending class for the user selection state which holds the center of mass as an
    /// additional value.
    /// @ingroup UserSelectionClosestSelectors
    class UserStatusWithCom : public UserSelectionState
    {
    public:
        XnPoint3D m_COM; ///< The user's center of mass when it failed to calibrate.
    };

};
#endif // XNV_CLOSEST_USER_SELECTOR_H_
