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
#include "PoseToggleUserSelector.h"



#define VALIDATE(cond, errorMsg)                                        \
    if(!cond)                                                       \
        {                                                               \
        printf("PoseUserSelector creation failed: %s\n",errorMsg);  \
        m_bValid=FALSE;                                             \
        return;                                                     \
        }                                                               \

#define VALIDATE_WITH_ACTION(cond, errorMsg, actionToDo)                \
    if(!cond)                                                           \
    {                                                                   \
    printf("PoseUserSelector creation failed: %s\n",errorMsg);      \
    actionToDo;                                                     \
    m_bValid=FALSE;                                                 \
    return;                                                         \
    }                                                                   \

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------



PoseToggleUserSelector::PoseToggleUserSelector(xn::UserGenerator* pUserGenerator,
                                               TrackingInitializer* pTrackingInitializer, 
                                               const char* poseToTrack,
                                               XnUInt32 maxUsersToTrack,
                                               XnUInt64 timeBeforeToggle) : 
                    PoseUserSelector(pUserGenerator,pTrackingInitializer,poseToTrack),
                    m_nMaxUsersToTrack(maxUsersToTrack),
                    m_timeBeforeToggle(timeBeforeToggle)
{
    VALIDATE(m_bValid,"PoseUserSelector was not initialized properly"); // note we assume this tests the validity of the pose capability
    XnStatus nRetVal = m_pUserGenerator->GetPoseDetectionCap().RegisterToOutOfPose(OutOfPoseCallback,this,m_hOutOfPoseDetectCallback);
    VALIDATE_WITH_ACTION(nRetVal == XN_STATUS_OK,"Failed to register to pose detection",m_hOutOfPoseDetectCallback = NULL);
}
                                     

    /// @brief destructor.
PoseToggleUserSelector::~PoseToggleUserSelector()
{
    if(m_hOutOfPoseDetectCallback != NULL)
    {
        m_pUserGenerator->GetPoseDetectionCap().UnregisterFromOutOfPose(m_hOutOfPoseDetectCallback);
        m_hOutOfPoseDetectCallback = NULL;
    }
}

XnStatus PoseToggleUserSelector::DetectPose(XnUserID nUserId)
{
    UserSelectionState* pState;
    XnStatus nRetVal = m_hUsersState.Get(nUserId, pState);
    if(nRetVal!=XN_STATUS_OK)
    {
        return nRetVal;
    }
    XnUInt64 lastOutOfPoseTime;

    nRetVal=m_usersOutOfPoseTime.Get(nUserId, lastOutOfPoseTime);
    if(nRetVal!=XN_STATUS_OK)
    {
        return nRetVal; // we don't have a user time at all. This should not happen!!!
    }
    XnUInt64 curOutOfPoseTime=m_pUserGenerator->GetTimestamp();


    if(pState->m_eState == XN_SELECTION_UNSELECTED || pState->m_eState == XN_SELECTION_FAILED)
    {

        // we want to select the user
        if(lastOutOfPoseTime > 0 && (curOutOfPoseTime-lastOutOfPoseTime < m_timeBeforeToggle))
        {
            return XN_STATUS_OK; // not enough time has passed since out of pose
        }

        if(GetNumberOfSelectedUsers() >= m_nMaxUsersToTrack)
        {
            return XN_STATUS_OK; // we don't select because we already have enough selected users
        }
        XnUInt64 tmpTime = 0;
        m_usersOutOfPoseTime.Set(nUserId,tmpTime); // we start so we don't care about previous out of pose any more
        return PoseUserSelector::DetectPose(nUserId); // we select as the parent does.
    }
    else
    {
        // we might want to unselect the user.

        if(lastOutOfPoseTime == 0 || (curOutOfPoseTime - lastOutOfPoseTime < m_timeBeforeToggle))
        {
            return XN_STATUS_OK; // not enough time has passed
        }
        // we need to unselect!
        UpdateUserSelectionState(nUserId,XN_SELECTION_UNSELECTED,0);
        m_pTrackingInitializer->AbortTracking(nUserId); // stop tracking
        m_usersOutOfPoseTime.Set(nUserId,curOutOfPoseTime); // we are not tracking so no time is relevant

    }
    return XN_STATUS_ERROR;
}

XnStatus PoseToggleUserSelector::AddNewUser(XnUserID nUserId)
{
    XnStatus res=PoseUserSelector::AddNewUser(nUserId);
    if(res!=XN_STATUS_OK)
    {
        return res;
    }
    return m_usersOutOfPoseTime.Set(nUserId,0);
}

XnStatus PoseToggleUserSelector::RemoveUser(XnUserID nUserId)
{
    XnStatus res=PoseUserSelector::RemoveUser(nUserId);
    if(res!=XN_STATUS_OK)
    {
        return res;
    }
    return m_usersOutOfPoseTime.Remove(nUserId);
}

XnStatus PoseToggleUserSelector::DetectOutOfPose(XnUserID nUserId)
{
    XnUInt64 lastOutOfPoseTime;

    UserSelectionState* pState;
    XnStatus nRetVal=m_hUsersState.Get(nUserId, pState);
    if(nRetVal!=XN_STATUS_OK)
    {
        return nRetVal;
    }
    nRetVal=m_usersOutOfPoseTime.Get(nUserId,lastOutOfPoseTime);
    if(nRetVal!=XN_STATUS_OK)
    {
        return nRetVal;
    }
    if(lastOutOfPoseTime!=0)
    {
        return XN_STATUS_OK; // we already have a number...
    }
    lastOutOfPoseTime=m_pUserGenerator->GetTimestamp();
    m_usersOutOfPoseTime.Set(nUserId,lastOutOfPoseTime);
    return XN_STATUS_OK;
}

void XN_CALLBACK_TYPE PoseToggleUserSelector::OutOfPoseCallback(xn::PoseDetectionCapability& /*capability*/, const XnChar* strPose, XnUserID nUserId,void* pCookie)
{
    PoseToggleUserSelector *pPoseUserSelector=(PoseToggleUserSelector *)pCookie;
    if(xnOSStrCmp(strPose,pPoseUserSelector->m_strPoseToTrack)!=0)
        return; // this is not the pose we are looking for.
    pPoseUserSelector->DetectOutOfPose(nUserId);

}
