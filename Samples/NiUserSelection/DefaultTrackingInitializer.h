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
class DefaultTrackingInitializer : public TrackingInitializer
{
public:
    virtual XnStatus StartTracking(XnUserID nUserId);
    virtual XnStatus AbortTracking(XnUserID nUserId);

    DefaultTrackingInitializer(xn::UserGenerator *pUserGeneratorr);
    virtual ~DefaultTrackingInitializer();
protected:
    virtual XnStatus CalibrationStart(XnUserID nUserId);
    virtual XnStatus CalibrationComplete(XnUserID nUserId,XnCalibrationStatus eStatus);
    virtual XnStatus CalibrationInProgress(XnUserID nUserId,XnCalibrationStatus eStatus);

    XnCallbackHandle m_hInProgressCallback;
    XnCallbackHandle m_hCalibrationStartCallback;
    XnCallbackHandle m_hCalibrationCompleteCallback;

    static void XN_CALLBACK_TYPE CalibrationStartCallback(xn::SkeletonCapability& capability, XnUserID nUserId, void* pCookie);
    static void XN_CALLBACK_TYPE CalibrationCompleteCallback(xn::SkeletonCapability& capability, XnUserID nUserId, XnCalibrationStatus eStatus, void* pCookie);
    static void XN_CALLBACK_TYPE CalibrationInProgressCallback(xn::SkeletonCapability& capability, XnUserID nUserId, XnCalibrationStatus eStatus, void* pCookie);

};
#endif // XNV_DEFAULT_TRACKING_INITIALIZER_H_

