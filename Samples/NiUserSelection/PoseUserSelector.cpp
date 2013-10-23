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
#include "PoseUserSelector.h"

#define POSE_LIST_NUM_POSES 20
#define POSE_LIST_STRING_LENGTH 50

#define VALIDATE(cond, errorMsg)                                        \
        if(!(cond))                                                     \
        {                                                               \
            printf("PoseUserSelector creation failed: %s\n",errorMsg);  \
            m_bValid=FALSE;                                             \
            return;                                                     \
        }                                                               \

#define VALIDATE_WITH_ACTION(cond, errorMsg, actionToDo)                \
    if(!(cond))                                                         \
    {                                                                   \
        printf("PoseUserSelector creation failed: %s\n",errorMsg);      \
        actionToDo;                                                     \
        m_bValid=FALSE;                                                 \
        return;                                                         \
    }                                                                   \


//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
PoseUserSelector::PoseUserSelector(xn::UserGenerator* pUserGenerator,
                                   TrackingInitializer* pTrackingInitializer,
                                   const char* poseToTrack) :
                                                              UserSelector(pUserGenerator),
                                                              m_pTrackingInitializer(pTrackingInitializer),
                                                              m_strPoseToTrack(NULL),
                                                              m_hPoseDetectCallback(NULL),
                                                              m_hPoseInProgressCallback(NULL)
{
    VALIDATE(m_bValid , "UserSelector was not initialized properly");
    VALIDATE(pTrackingInitializer != NULL , "NULL tracking initializer supplied");
    VALIDATE(pTrackingInitializer->IsValid() , "tracking initializer supplied is invalid");
    VALIDATE(m_pUserGenerator != NULL,"NULL user generator supplied");
    VALIDATE(m_pUserGenerator->IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION) , "User generator supplied does not support pose capability");
    VALIDATE(poseToTrack != NULL , "NULL pose provided");
    VALIDATE(m_pUserGenerator->GetPoseDetectionCap().IsPoseSupported(poseToTrack) , "Pose detection capability does not support requested pose");
    XnStatus nRetVal = m_pUserGenerator->GetPoseDetectionCap().RegisterToPoseDetected(PoseDetectedCallback,this,m_hPoseDetectCallback);
    VALIDATE_WITH_ACTION(nRetVal == XN_STATUS_OK,"Failed to register to pose detection",m_hPoseDetectCallback = NULL);
    nRetVal = m_pUserGenerator->GetPoseDetectionCap().RegisterToPoseInProgress(PoseInProgressCallback,this,m_hPoseInProgressCallback);
    VALIDATE_WITH_ACTION(nRetVal == XN_STATUS_OK,"Failed to register to pose in progress",m_hPoseInProgressCallback = NULL);
    
    int len = (int)strlen(poseToTrack)+1;
    m_strPoseToTrack=XN_NEW_ARR(char, len); // the length is +1 to accommodate the null terminator
    xnOSStrCopy(m_strPoseToTrack,poseToTrack,len);
}

PoseUserSelector::~PoseUserSelector()
{
    m_pTrackingInitializer = NULL;
    if(m_pUserGenerator != NULL)
    {
        if(m_pUserGenerator->IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
        {
            for(UserStateHash::Iterator iter = m_hUsersState.Begin(); iter != m_hUsersState.End(); ++iter)
            {
                m_pUserGenerator->GetPoseDetectionCap().StopPoseDetection(iter->Key());
            }
            if(m_hPoseDetectCallback != NULL)
            {
                m_pUserGenerator->GetPoseDetectionCap().UnregisterFromPoseDetected(m_hPoseDetectCallback);
                m_hPoseDetectCallback = NULL;
            }
            if(m_hPoseInProgressCallback != NULL)        
            {
                m_pUserGenerator->GetPoseDetectionCap().UnregisterFromPoseInProgress(m_hPoseInProgressCallback);
                m_hPoseInProgressCallback = NULL;
            }        
        }
        m_pUserGenerator = NULL;
    }
    if(m_strPoseToTrack != NULL)
    {
        XN_DELETE_ARR(m_strPoseToTrack);
        m_strPoseToTrack = NULL;
    }
}

XnStatus PoseUserSelector::StartTracking(XnUserID nUserId)
{
    UserSelectionState* pState;
    XnStatus nRetVal=m_hUsersState.Get(nUserId, pState);
    if(nRetVal != XN_STATUS_OK)
    {
        return nRetVal; // we don't have such user ready
    }
    if(pState->m_eState != XN_SELECTION_UNSELECTED)
    {
        return XN_STATUS_ERROR; // the user is in the wrong state.
    }
    nRetVal=UpdateUserSelectionState(nUserId, XN_SELECTION_SELECTED, 0);
    if(nRetVal!=XN_STATUS_OK)
    {
        return nRetVal; // we can't continue because we can't update the state!
    }
    return m_pTrackingInitializer->StartTracking(nUserId,FALSE);
}

XnStatus PoseUserSelector::AddNewUser(XnUserID nUserId)
{
    UserSelector::AddNewUser(nUserId);
    return m_pUserGenerator->GetPoseDetectionCap().StartPoseDetection(m_strPoseToTrack,nUserId);
}

XnStatus PoseUserSelector::RemoveUser(XnUserID nUserId)
{
    UserSelector::RemoveUser(nUserId);
    return m_pUserGenerator->GetPoseDetectionCap().StopPoseDetection(nUserId);
}

XnStatus PoseUserSelector::UpdatePoseProgress(XnUserID nUserId, XnPoseDetectionStatus ePoseError)
{
    UserSelectionState* pState;
    XnStatus nRetVal=m_hUsersState.Get(nUserId, pState);
    if(nRetVal != XN_STATUS_OK)
    {
        return nRetVal;
    }
    if(pState->m_eState != XN_SELECTION_UNSELECTED)
    {
        return XN_STATUS_ERROR; // the user is in the wrong state.
    }
    return UpdateUserSelectionState(nUserId,pState->m_eState, ePoseError);
}

XnStatus PoseUserSelector::DetectPose(XnUserID nUserId)
{
    return StartTracking(nUserId);
}

void XN_CALLBACK_TYPE PoseUserSelector::PoseInProgressCallback(xn::PoseDetectionCapability& /*capability*/, const XnChar* strPose, XnUserID nUserId, XnPoseDetectionStatus ePoseError, void* pCookie)
{
    PoseUserSelector* pPoseUserSelector = (PoseUserSelector *)pCookie;
    if(xnOSStrCmp(strPose,pPoseUserSelector->m_strPoseToTrack)!=0)
    {
        return; // this is not the pose we are looking for.
    }
    pPoseUserSelector->UpdatePoseProgress(nUserId,ePoseError);
}

void XN_CALLBACK_TYPE PoseUserSelector::PoseDetectedCallback(xn::PoseDetectionCapability& /*capability*/, const XnChar* strPose, XnUserID nUserId, void* pCookie)
{
    PoseUserSelector* pPoseUserSelector = (PoseUserSelector *)pCookie;
    if(xnOSStrCmp(strPose,pPoseUserSelector->m_strPoseToTrack)!=0)
    {
        return; // this is not the pose we are looking for.
    }
    pPoseUserSelector->DetectPose(nUserId);
}

const XnChar* PoseUserSelector::GetPoseErrorString(XnPoseDetectionStatus error)
{
    switch (error)
    {
    case XN_POSE_DETECTION_STATUS_OK:
        return "OK";
    case XN_POSE_DETECTION_STATUS_NO_USER:
        return "NoUser";
    case XN_POSE_DETECTION_STATUS_TOP_FOV:
        return "Top FOV";
    case XN_POSE_DETECTION_STATUS_SIDE_FOV:
        return "Side FOV";
    case XN_POSE_DETECTION_STATUS_ERROR:
        return "General error";
    default:
        return "Unknown";
    }
}
