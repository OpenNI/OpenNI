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
#ifndef XNV_EXIT_POSE_DETECTOR_H_
#define XNV_EXIT_POSE_DETECTOR_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnCppWrapper.h>
#include <XnHashT.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------



/// @brief Class to follow exit pose state for each user
/// 
/// The goal of this class is to find exit and track exit pose state for each user.
/// @ingroup UserSelectionSampleFiles
class ExitPoseDetector
{
public:
    /// @brief Constructor
    /// 
    /// @param userGenNode The user generator node to use
    ExitPoseDetector(xn::UserGenerator userGenNode);

    /// @brief Destructor
    ~ExitPoseDetector();

    /// @brief Returns true if the user tracker is valid.
    XnBool Valid() { return m_bValid; };

    /// @brief gets the timestamp of when a user last entered an exit pose without leaving it
    /// 
    /// @param nUserId The user whose state we are checking. A user of 0 means the highest value of
    ///                all.
    /// @return The latest time when the user entered an exit pose and have not been out
    /// of pose since. If the user is currently out of pose, 0 is returned.
    XnUInt64 GetExitPoseTimeStamp(XnUserID nUserId);

private:
    /// @brief Defines the UserExitPoseTimes hash which holds the last time an exit pose was found 
    /// for each user. A value of 0 means an illegal time (e.g. because out of pose was found).
    typedef XnHashT<XnUserID, XnUInt64> UserExitPoseTimes;

    UserExitPoseTimes m_userExitPoseTimes; ///< @brief holds the hash which holds the last time and exit pose was found for each user.

    /// @brief A handle to unregister pose detection callbacks
    XnCallbackHandle m_hPoseDetectCallback;             
    /// @brief A handle to unregister out of pose detection callbacks
    XnCallbackHandle m_hPoseOutOfPoseDetectCallback;    

    XnCallbackHandle m_hUserCallbacks; ///< @brief A handle to be able to unregister the user callbacks

    xn::UserGenerator m_userGenerator; ///< @brief The user generator to use.

    XnBool m_bValid; ///< @brief holds true if we have a valid value. This only happens after initialization!

    /// @brief Callback for pose detection
    static void XN_CALLBACK_TYPE PoseDetectedCallback(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nUserId, void* pCookie);

    /// @brief Callback for out of pose detection
    static void XN_CALLBACK_TYPE OutOfPoseDetectedCallback(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nUserId, void* pCookie);
 
    /// @brief New user callback.
    /// 
    /// Called when a new user is found.
    /// @param generator The user generator which called the callback
    /// @param nUserId The newly found user
    /// @param pCookie A cookie which on registering is defined as the calling user selector object.
    static void XN_CALLBACK_TYPE NewUserCallback(xn::UserGenerator& generator, XnUserID nUserId, void* pCookie);

    /// @brief Lost user callback.
    /// 
    /// Called when a user is lost (i.e. no longer counted in the scene. Does not include exiting).
    /// @param generator The user generator which called the callback
    /// @param nUserId The lost user
    /// @param pCookie A cookie which on registering is defined as the calling user selector object.
    static void XN_CALLBACK_TYPE LostUserCallback(xn::UserGenerator& generator, XnUserID nUserId, void* pCookie);

};


#endif // XNV_EXIT_POSE_DETECTOR_H_
