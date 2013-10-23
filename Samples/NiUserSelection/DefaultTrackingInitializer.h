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
#ifndef XNV_DEFAULT_TRACKING_INITIALIZER_H_
#define XNV_DEFAULT_TRACKING_INITIALIZER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

#include "TrackingInitializer.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------



/// @brief Class to start tracking a user
/// 
/// The goal of this class is to start performing the actual tracking of a user. It returns the 
/// results to the UserSelector which called it.
/// @ingroup UserSelectionInitializers
class DefaultTrackingInitializer : public TrackingInitializer
{
public:
    virtual XnStatus StartTracking(XnUserID nUserId, XnBool bForce);
    virtual XnStatus AbortTracking(XnUserID nUserId);

    /// @brief Constructor
    /// 
    /// @param pUserGenerator The user generator to use
    DefaultTrackingInitializer(xn::UserGenerator *pUserGenerator);
    virtual ~DefaultTrackingInitializer();
protected:
    /// @brief Method called by the calibration start callback
    /// 
    /// @param nUserId The user the calibration started on
    virtual XnStatus CalibrationStart(XnUserID nUserId);

    /// @brief Method called by the calibration complete callback
    /// 
    /// @param nUserId The user the calibration finished on
    /// @param eStatus The status (success/fail and why)
    virtual XnStatus CalibrationComplete(XnUserID nUserId,XnCalibrationStatus eStatus);
    /// @brief Method called by the calibration in progress callback
    /// 
    /// @param nUserId The user the calibration progress is on
    /// @param eStatus The status (success/fail and why)
    virtual XnStatus CalibrationInProgress(XnUserID nUserId,XnCalibrationStatus eStatus);

    XnCallbackHandle m_hInProgressCallback; ///< @brief Callback handle to unregister the calibration in progress callback
    XnCallbackHandle m_hCalibrationStartCallback; ///< @brief Callback handle to unregister the calibration start callback
    XnCallbackHandle m_hCalibrationCompleteCallback; ///< @brief Callback handle to unregister the calibration complete callback

    /// @brief Callback method for calibration start events
    ///
    /// @param capability The skeleton capability which raised the event.
    /// @param nUserId The user the event relates to
    /// @param pCookie A user supplied cookie.
    static void XN_CALLBACK_TYPE CalibrationStartCallback(xn::SkeletonCapability& capability, XnUserID nUserId, void* pCookie);

    /// @brief Callback method for calibration complete events
    ///
    /// @param capability The skeleton capability which raised the event.
    /// @param nUserId The user the event relates to
    /// @param eStatus The status (success/fail and why)
    /// @param pCookie A user supplied cookie.
    static void XN_CALLBACK_TYPE CalibrationCompleteCallback(xn::SkeletonCapability& capability, XnUserID nUserId, XnCalibrationStatus eStatus, void* pCookie);
    /// @brief Callback method for calibration in progress events
    ///
    /// @param capability The skeleton capability which raised the event.
    /// @param nUserId The user the event relates to
    /// @param eStatus The status (success/fail and why)
    /// @param pCookie A user supplied cookie.
    static void XN_CALLBACK_TYPE CalibrationInProgressCallback(xn::SkeletonCapability& capability, XnUserID nUserId, XnCalibrationStatus eStatus, void* pCookie);

};
#endif // XNV_DEFAULT_TRACKING_INITIALIZER_H_

