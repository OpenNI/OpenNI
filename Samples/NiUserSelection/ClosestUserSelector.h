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
/// This user selector selects user and tracks that user. If a new user becomes closer, it stops tracking the old one and tracks the new, closer user.
class ClosestUserSelector : public UserSelector
{
public:
    ClosestUserSelector(xn::UserGenerator* pUserGenerator, TrackingInitializer* pTrackingInitializer, XnUInt32 nMaxNumUsers);
    ~ClosestUserSelector();

    virtual void UpdateFrame();
protected:
    virtual UserSelectionState* CreateUserSelectionState();


    virtual XnStatus UpdateUserSelectionState(XnUserID nUserId, XnSelectionState eState , XnInt64 subState);


    TrackingInitializer* m_pTrackingInitializer; ///< The tracking initializer used to start tracking
    XnUInt32 m_nMaxNumUsers;
    struct UserInfo
    {
        XnUserID m_userID;
        XnFloat m_COMZ;
    };

    UserInfo* m_pUsersList;

    void ClearUserList();
    void InsertNewUser(XnUserID userID, XnFloat COMZ);
    XnBool TestIfShouldTrack(XnUserID userID);

    class UserStatusWithCom : public UserSelectionState
    {
    public:
        XnPoint3D m_COM;
    };

};
#endif // XNV_CLOSEST_USER_SELECTOR_H_
