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
#include "UserSelector.h"
#include <XnLog.h>

XnStatus UserSelector::GetUserState(XnUserID nUserId, UserSelectionState& newState)
{
    UserSelectionState* pState;
    XnStatus nRetVal = m_hUsersState.Get(nUserId, pState);
    if(nRetVal != XN_STATUS_OK)
    {
        return nRetVal;
    }
    newState = *pState;
    return XN_STATUS_OK;
}


UserSelector::UserSelector(xn::UserGenerator* pUserGenerator) : m_bValid(FALSE), 
                                                                m_pUserGenerator(pUserGenerator),
                                                                m_hUserCallbacks(NULL)
{ 
    if(m_pUserGenerator == NULL)
    {
        return; // no user generator
    }
   XnStatus nRetVal = m_pUserGenerator->RegisterUserCallbacks(NewUserCallback, LostUserCallback, this, m_hUserCallbacks);
   if(nRetVal != XN_STATUS_OK)
   {
       m_hUserCallbacks = NULL;
       return; // failed to register callbacks..
   }
    m_bValid = TRUE;

}

UserSelector::~UserSelector()
{
    while(m_hUsersState.Begin() != m_hUsersState.End())
    {
        UserStateHash::Iterator iter=m_hUsersState.Begin();
        if(iter->Value() != NULL)
        {
            XN_DELETE(iter->Value());
        }
        m_hUsersState.Remove(iter);
    }
    if(m_pUserGenerator != NULL)
    {
        if(m_hUserCallbacks != NULL)
        {
            m_pUserGenerator->UnregisterUserCallbacks(m_hUserCallbacks);
            m_hUserCallbacks = NULL;
        }        
        m_pUserGenerator = NULL;
    }
}

XnStatus UserSelector::UpdateUserTrackingProgress(XnUserID nUserId, XnInt64 newSubState)
{
    // update by getting the current state, making sure it is ok and then change the relevant
    // sub state
    UserSelectionState* pState;
    XnStatus nRetVal = m_hUsersState.Get(nUserId , pState);
    if(nRetVal != XN_STATUS_OK)
    {
        return nRetVal;
    }
    if(pState == NULL || pState->m_eState != XN_SELECTION_SELECTED)
    {
        xnLogError("SAMPLE","User %d should have been in an unselected state but was instead in %d state.\n", nUserId, pState->m_eState);
        return XN_STATUS_ERROR; // this should be only called when a user is selected!
    }
    return UpdateUserSelectionState(nUserId, pState->m_eState, newSubState);
}
XnStatus UserSelector::UpdateUserTracking(XnUserID nUserId, XnBool bTracked, XnInt64 newSubState)
{
    // update by checking the current state is ok and then update it based on the success and the 
    // sub state.
    UserSelectionState* pState;
    XnStatus nRetVal = m_hUsersState.Get(nUserId,pState);
    if(nRetVal!=XN_STATUS_OK)
    {
        return nRetVal;
    }
    if(pState == NULL || pState->m_eState != XN_SELECTION_SELECTED)
    {
        xnLogError("SAMPLE","User %d should have been in an unselected state but was instead in %d state.\n", nUserId, pState->m_eState);
        return XN_STATUS_ERROR; // this should be only called when a user is selected!
    }
    XnSelectionState newState = bTracked ? XN_SELECTION_TRACKING : XN_SELECTION_FAILED;

    return UpdateUserSelectionState(nUserId, newState, newSubState);
}

XnStatus UserSelector::AddNewUser(XnUserID nUserId)
{
    UserSelectionState* pState;
    if(m_hUsersState.Get(nUserId, pState)==XN_STATUS_OK)
    {
        // the user already exists. We should remove it first as the old one is irrelevant anymore!
        RemoveUser(nUserId);
    }

    pState = CreateUserSelectionState();
    if(pState == NULL)
    {
        return XN_STATUS_ERROR;
    }
    m_hUsersState.Set(nUserId,pState); // the initial state is created in the CreateUserSelectionState
    return XN_STATUS_OK;
}

XnStatus UserSelector::RemoveUser(XnUserID nUserId)
{
    UserSelectionState* pState;
    if(m_hUsersState.Get(nUserId, pState)!=XN_STATUS_OK || pState == NULL)
    {
        return XN_STATUS_ERROR;
    }
    XN_DELETE(pState);
    return m_hUsersState.Remove(nUserId);
}


void XN_CALLBACK_TYPE UserSelector::NewUserCallback(xn::UserGenerator& /*generator*/, XnUserID nUserId, void* pCookie)
{
    UserSelector* pUserSelector=(UserSelector *)pCookie;
    pUserSelector->AddNewUser(nUserId);
}
void XN_CALLBACK_TYPE UserSelector::LostUserCallback(xn::UserGenerator& /*generator*/, XnUserID nUserId, void* pCookie)
{
    UserSelector* pUserSelector=(UserSelector *)pCookie;
    pUserSelector->RemoveUser(nUserId);
}

const XnChar* UserSelector::GetCalibrationErrorString(XnCalibrationStatus eError)
{
    switch (eError)
    {
    case XN_CALIBRATION_STATUS_OK:
        return "OK";
    case XN_CALIBRATION_STATUS_NO_USER:
        return "NoUser";
    case XN_CALIBRATION_STATUS_ARM:
        return "Arm";
    case XN_CALIBRATION_STATUS_LEG:
        return "Leg";
    case XN_CALIBRATION_STATUS_HEAD:
        return "Head";
    case XN_CALIBRATION_STATUS_TORSO:
        return "Torso";
    case XN_CALIBRATION_STATUS_TOP_FOV:
        return "Top FOV";
    case XN_CALIBRATION_STATUS_SIDE_FOV:
        return "Side FOV";
    case XN_CALIBRATION_STATUS_POSE:
        return "Pose";
    default:
        return "Unknown";
    }
}

XnStatus UserSelector::GetUserLabel(XnUserID nUserId, char* strLabel, XnUInt32 maxStrLen)
{
    UserSelectionState* pState;
    XnStatus nRetVal=m_hUsersState.Get(nUserId, pState);
    
    if(nRetVal!=XN_STATUS_OK)
    {
        return nRetVal;
    }
    return TranslateStateToLabel(pState,strLabel,maxStrLen);
}

XnStatus UserSelector::TranslateStateToLabel(const UserSelectionState* pUserState, char *strLabel, XnUInt32 maxStrLen)
{
    switch(pUserState->m_eState)
    {
    case XN_SELECTION_UNSELECTED:
        xnOSStrCopy(strLabel,"Not selected",maxStrLen);
        return XN_STATUS_OK;
    case XN_SELECTION_SELECTED:
        xnOSStrCopy(strLabel,"Selected [",maxStrLen);
        xnOSStrAppend(strLabel,GetCalibrationErrorString((XnCalibrationStatus)pUserState->m_subState),maxStrLen);
        xnOSStrAppend(strLabel,"]",maxStrLen);
        return XN_STATUS_OK;
    case XN_SELECTION_TRACKING:
        xnOSStrCopy(strLabel,"Tracking",maxStrLen);
        return XN_STATUS_OK;
    case XN_SELECTION_FAILED:
        xnOSStrCopy(strLabel,"Failed to track [",maxStrLen);
        xnOSStrAppend(strLabel,GetCalibrationErrorString((XnCalibrationStatus)pUserState->m_subState),maxStrLen);
        xnOSStrAppend(strLabel,"]",maxStrLen);
        return XN_STATUS_OK;
    default:
        xnOSStrCopy(strLabel,"User in illegal state!\n",maxStrLen);
        return XN_STATUS_ERROR;
    }
}


XnUInt32 UserSelector::GetNumberOfSelectedUsers()
{
    XnUInt32 nUsers=0;
    for(UserStateHash::Iterator iter = m_hUsersState.Begin(); iter != m_hUsersState.End(); ++iter)
    {
        UserSelectionState* val = iter->Value();
        if(val->m_eState == XN_SELECTION_SELECTED || val->m_eState == XN_SELECTION_TRACKING)
        {
            nUsers++;
        }
    }
    return nUsers;
}

UserSelectionState* UserSelector::CreateUserSelectionState()
{
    return XN_NEW(UserSelectionState);
}


XnStatus UserSelector::UpdateUserSelectionState(XnUserID nUserId, XnSelectionState eState , XnInt64 subState)
{
    UserSelectionState* tmpState;

    XnStatus nRetVal=m_hUsersState.Get(nUserId, tmpState);
    if(nRetVal != XN_STATUS_OK)
    {
        return nRetVal;
    }
    if(tmpState==NULL)
    {
        return XN_STATUS_ERROR;
    }
    tmpState->m_eState=eState;
    tmpState->m_subState=subState;
    return XN_STATUS_OK;
}
