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
