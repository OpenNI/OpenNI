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
#ifndef XNV_SINGLE_POSE_USER_SELECTOR_H_
#define XNV_SINGLE_POSE_USER_SELECTOR_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "PoseUserSelector.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

/// @brief User selector of one user based on pose
/// 
/// This basic user selector select users which perform a certain pose. The difference between this
/// and PoseUserSelector is that when a user is selected in this selector, all others stop tracking.
/// @ingroup UserSelectionPoseSelectors
class  SinglePoseUserSelector : public PoseUserSelector
{
public:
    /// @brief Constructor
    /// 
    /// @param pUserGenerator The user generator to use
    /// @param pTrackingInitializer The tracking initializer to use
    /// @param poseToTrack The pose name used to select
    SinglePoseUserSelector(xn::UserGenerator *pUserGenerator,TrackingInitializer *pTrackingInitializer, const char *poseToTrack);
    ~SinglePoseUserSelector();
protected:
    virtual XnStatus StartTracking(XnUserID nUserId);
};
#endif // XNV_POSE_USER_SELECTOR_H_
