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
#ifndef XNV_POSE_USER_SELECTOR_H_
#define XNV_POSE_USER_SELECTOR_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "UserSelector.h"
#include "TrackingInitializer.h"
#include <XnHashT.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------



/// @brief User selector based on pose
/// 
/// This basic user selector select users which perform a certain pose.
/// The pose is define in initialization.
/// The base class selects all users which perform the pose but inheriting classes can override this
/// behavior by using the ForgetTracking method.
/// @ingroup UserSelectionPoseSelectors
class PoseUserSelector : public UserSelector
{
public:
    /// @brief constructor.
    /// 
    /// @param pUserGenerator A pointer to the user generator node which holds the users.
    /// @param pTrackingInitializer A pointer to the tracking initializer to use to start tracking.
    /// @param poseToTrack The pose to use for selection.
    PoseUserSelector(xn::UserGenerator *pUserGenerator,TrackingInitializer *pTrackingInitializer, const char *poseToTrack);

    /// @brief destructor.
    ~PoseUserSelector();
protected:
    /// @brief Internal method to do the actual tracking.
    /// 
    /// This method is called internally when we decide to track a user. It does all tracking elements.
    /// @param nUserId The user we want to start tracking.
    /// @return The success status
    virtual XnStatus StartTracking(XnUserID nUserId);

    // override the base AddNewUser (calls the base too)
    virtual XnStatus AddNewUser(XnUserID nUserId);

    // override the base AddNewUser (calls the base too)
    virtual XnStatus RemoveUser(XnUserID nUserId);

    /// @brief Internal method which is called whenever a PoseInProgressCallback callback is called.
    /// 
    /// @param nUserId the user whose pose progress is being updated
    /// @param ePoseError the pose error (representing the pose progress) for the user
    /// @return The success code.
    virtual XnStatus UpdatePoseProgress(XnUserID nUserId,XnPoseDetectionStatus ePoseError);

    /// @brief Internal method which is called from the callback whenever a Pose is detected.
    /// 
    /// @param nUserId the user whose pose progress was detected
    /// @return The success code.
    virtual XnStatus DetectPose(XnUserID nUserId);

    /// @brief translate a calibration error to a string
    /// 
    /// @param eError The calibration error
    /// @return The string version of the error.
    const XnChar* GetPoseErrorString(XnPoseDetectionStatus eError);


    TrackingInitializer *m_pTrackingInitializer; ///< @brief The tracking initializer used to start tracking
    XnCallbackHandle m_hPoseInProgressCallback;  ///< @brief A handle to unregister the pose in progress callbacks.
    XnCallbackHandle m_hPoseDetectCallback;      ///< @brief A handle to unregister pose detection callbacks

    char *m_strPoseToTrack; ///< @brief The pose used for selection
private:
    /// @brief Callback for in progress updates of the pose
    static void XN_CALLBACK_TYPE PoseInProgressCallback(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nUserId, XnPoseDetectionStatus ePoseError, void* pCookie);
    /// @brief Callback for pose detection
    static void XN_CALLBACK_TYPE PoseDetectedCallback(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nUserId, void* pCookie);
};
#endif // XNV_POSE_USER_SELECTOR_H_
