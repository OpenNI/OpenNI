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

