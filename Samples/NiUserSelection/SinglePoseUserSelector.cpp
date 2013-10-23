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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "SinglePoseUserSelector.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------


SinglePoseUserSelector::SinglePoseUserSelector(xn::UserGenerator* pUserGenerator,
                                               TrackingInitializer* pTrackingInitializer, 
                                               const char* poseToTrack) : 
              PoseUserSelector(pUserGenerator, pTrackingInitializer, poseToTrack)
{
}
SinglePoseUserSelector::~SinglePoseUserSelector()
{

}

XnStatus SinglePoseUserSelector::StartTracking(XnUserID nUserId)
{
    XnStatus nRetVal = PoseUserSelector::StartTracking(nUserId);
    if(nRetVal != XN_STATUS_OK)
    {
        return nRetVal;
    }

    // stop tracking for everyone other than the current one.
    for(UserStateHash::Iterator iter = m_hUsersState.Begin(); iter != m_hUsersState.End(); ++iter)
    {
        if(iter->Key() == nUserId)
        {
            continue; // this is the new one, not interesting...
        }
        if(iter->Value()->m_eState!=XN_SELECTION_FAILED)
        {
            UpdateUserSelectionState(iter->Key(),XN_SELECTION_UNSELECTED,0);
        }        
        m_pTrackingInitializer->AbortTracking(iter->Key()); // stop tracking
    }
    return XN_STATUS_OK;
}
