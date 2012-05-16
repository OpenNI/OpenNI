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
