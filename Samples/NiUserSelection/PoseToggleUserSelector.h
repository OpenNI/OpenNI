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
#ifndef XNV_POSE_TOGGLE_USER_SELECTOR_H_
#define XNV_POSE_TOGGLE_USER_SELECTOR_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "PoseUserSelector.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------



/// @brief User selector which toggles selection based on pose
/// 
/// This user selector allows users to be selected when the perform a pose and unselects them when
/// they perform it again.
/// @ingroup UserSelectionPoseSelectors
class PoseToggleUserSelector : public PoseUserSelector
{
public:
    /// @brief constructor.
    /// 
    /// @param pUserGenerator A pointer to the user generator node which holds the users.
    /// @param pTrackingInitializer A pointer to the tracking initializer to use to start tracking.
    /// @param poseToTrack The pose to use for selection.
    /// @param maxUsersToTrack The maximum allowed users to track
    /// @param timeBeforeToggle The pose can toggle only if this number of microseconds have passed
    ///                         from the first out of pose for the user after starting tracking.
    PoseToggleUserSelector(xn::UserGenerator *pUserGenerator,
                           TrackingInitializer *pTrackingInitializer, 
                           const char *poseToTrack,
                           XnUInt32 maxUsersToTrack,
                           XnUInt64 timeBeforeToggle);

    /// @brief destructor.
    ~PoseToggleUserSelector();
protected:
    /// @brief Internal method which is called from the callback whenever a Pose is detected.
    /// 
    /// @param nUserId the user who'se pose was detected
    /// @return The success code.
    virtual XnStatus DetectPose(XnUserID nUserId);

    // override the base AddNewUser (calls the base too)
    virtual XnStatus AddNewUser(XnUserID nUserId);

    // override the base AddNewUser (calls the base too)
    virtual XnStatus RemoveUser(XnUserID nUserId);

    /// @brief Internal method which is called from the callback whenever a out of Pose is detected.
    /// 
    /// @param nUserId the user who'se out of pose was detected
    /// @return The success code.
    virtual XnStatus DetectOutOfPose(XnUserID nUserId);

    /// @brief Internal hash for when a user got out of pose.
    typedef XnHashT<XnUserID, XnUInt64> UserOutOfPoseHash;

    UserOutOfPoseHash m_usersOutOfPoseTime; ///< @brief holds for each user, the first time they were out of pose AFTER being tracked. 0 means no data.
    /// @param maxUsersToTrack 
    /// @param timeBeforeToggle 

    XnUInt32 m_nMaxUsersToTrack; ///< @brief The maximum allowed users to track
    XnUInt64 m_timeBeforeToggle; ///< @brief The pose can toggle only if this number of microseconds have passed

    /// @brief A handle to unregister out of pose detection callbacks
    XnCallbackHandle m_hOutOfPoseDetectCallback;      

private:
    /// @brief Callback for detecting out of pose.
    static void XN_CALLBACK_TYPE OutOfPoseCallback(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nUserId,void* pCookie);
};
#endif // XNV_POSE_TOGGLE_USER_SELECTOR_H_
