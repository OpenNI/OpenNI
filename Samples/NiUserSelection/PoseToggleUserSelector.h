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
