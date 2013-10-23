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
