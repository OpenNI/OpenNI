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
#include "ClosestUserSelector.h"

/// @brief Utility macro which validates a condition and if it fails it prints an error message.
/// 
/// This macro is aimed to be called from within the constructor as if it fails it invalidates the
/// object (m_bValid=false).
#define VALIDATE(cond, errorMsg)                                    \
    if(!(cond))                                                     \
{                                                                   \
    printf("ClosestUserSelector creation failed: %s\n",errorMsg);   \
    m_bValid=FALSE;                                                 \
    return;                                                         \
}                                                                   \


/// @brief The square distance for changes of COM (set for 20cm squared) to be considered moved 
/// in order to avoid the modifier @ref DIST_MOD_FOR_FAILURE.
#define MIN_SQR_DIST_TO_RETRY 40000
/// @brief A modifier for failing to calibrated (set at 10m).
#define DIST_MOD_FOR_FAILURE 10000
//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
ClosestUserSelector::ClosestUserSelector(xn::UserGenerator *pUserGenerator,
                                         TrackingInitializer *pTrackingInitializer,
                                         XnUInt32 nMaxNumUsers) :
                                                            UserSelector(pUserGenerator),
                                                            m_pTrackingInitializer(pTrackingInitializer),
                                                            m_nMaxNumUsers(nMaxNumUsers),
                                                            m_pUsersList(NULL)
{
    VALIDATE(m_bValid,"UserSelector was not initialized properly");
    VALIDATE(pTrackingInitializer!=NULL,"NULL tracking initializer supplied");
    VALIDATE(pTrackingInitializer->IsValid(),"tracking initializer supplied is invalid");
    VALIDATE(m_pUserGenerator!=NULL,"NULL user generator supplied");
    VALIDATE(m_nMaxNumUsers>0, "0 users tracking");
    m_pUsersList=XN_NEW_ARR(UserInfo,m_nMaxNumUsers);
}

ClosestUserSelector::~ClosestUserSelector()
{
    m_pTrackingInitializer=NULL;
    if(m_pUsersList!=NULL)
    {
        XN_DELETE_ARR(m_pUsersList);
        m_pUsersList=NULL;
    }
}

void ClosestUserSelector::UpdateFrame()
{
    // clear the user list so we can fill it out.
    ClearUserList();

    // find out who are the closest ones.
    for(UserStateHash::Iterator iter = m_hUsersState.Begin(); iter!=m_hUsersState.End(); ++iter)
    {
        UserSelectionState* val = iter->Value();
        XnUserID newUser = iter->Key();
        XnPoint3D newCom;
        if(m_pUserGenerator->GetCoM(newUser,newCom)!=XN_STATUS_OK)
            continue; // irrelevant user.
        if(newCom.Z<=0)
        {
            continue; // this is not someone we can use.
        }
        if(val->m_eState == XN_SELECTION_FAILED)
        {
            // we failed last time so we might want to give a penalty
            UserStatusWithCom* valWithCom=(UserStatusWithCom*)val;
            XnFloat distSqr=0.0f;
            XnFloat diff;
            diff = newCom.X - valWithCom->m_COM.X;
            distSqr += diff * diff;
            diff = newCom.Y - valWithCom->m_COM.Y;
            distSqr += diff * diff;
            diff = newCom.Z - valWithCom->m_COM.Z;
            distSqr += diff * diff;
            if(distSqr<MIN_SQR_DIST_TO_RETRY)
            {
                newCom.Z+=DIST_MOD_FOR_FAILURE; // we haven't moved too much since we failed, lets give a penalty
            }

        }
        if(val->m_eState == XN_SELECTION_SELECTED || val->m_eState == XN_SELECTION_TRACKING)
        {
            newCom.Z-=100.0f; // hysteresis to make the current user closer to avoid back and forth.
            if(newCom.Z<=0)
            {
                newCom.Z=1.0f; // minimum dist!
            }
        }
        InsertNewUser(newUser,newCom.Z); // add it to the list in an ordered manner
    }


    // go over the list and start or stop tracking.
    for(UserStateHash::Iterator iter = m_hUsersState.Begin(); iter!=m_hUsersState.End(); ++iter)
    {
        UserSelectionState* pState = iter->Value();
        XnUInt32 curUser = iter->Key();
        if(TestIfShouldTrack(curUser))
        {
            // this means the user is in the list of those which need to track
            if(pState->m_eState == XN_SELECTION_SELECTED || pState->m_eState == XN_SELECTION_TRACKING)
            {
                continue; // we are already tracking so nothing to do...
            }
            // make the user selected
            UpdateUserSelectionState(curUser, XN_SELECTION_SELECTED, 0);
            XnBool bForce=FALSE;
            if(pState->m_eState==XN_SELECTION_FAILED)
            {
                // if we failed in the past we can't count on the calibration data
                bForce=TRUE;
            }
            // start tracking
            m_pTrackingInitializer->StartTracking(curUser,bForce); 
        }
        else
        {
            // if we are here we shouldn't track
            if(pState->m_eState == XN_SELECTION_SELECTED || pState->m_eState == XN_SELECTION_TRACKING)
            {
                // we need to unselect it...
                m_pTrackingInitializer->AbortTracking(curUser);
                UpdateUserSelectionState(curUser,XN_SELECTION_UNSELECTED,0);
            }
        }
    }
}

void ClosestUserSelector::ClearUserList()
{
    // go over the internal list and make all values invalid (user 0 which is background and
    // negative z for the COM which is impossible as it is behind the sensor
    for(XnUInt32 i=0; i<m_nMaxNumUsers; i++)
    {
        m_pUsersList[i].m_userID=0;
        m_pUsersList[i].m_COMZ=-1.0f;
    }
}

void ClosestUserSelector::InsertNewUser(XnUserID userID, XnFloat COMZ)
{
    // go over all users and figure out where to put the new one
    for(XnUInt32 i=0; i<m_nMaxNumUsers; i++)
    {
        if(m_pUsersList[i].m_COMZ>0 && m_pUsersList[i].m_COMZ<COMZ)
        {
            continue; // the current user is closer, move along
        }
        // if we are here then this is a place to put the new one.
        if(m_pUsersList[i].m_COMZ>0)
        {
            // we need to push everyone else forward...
            for(XnUInt32 j=m_nMaxNumUsers-1; j>i; j--)
            {
                m_pUsersList[j]=m_pUsersList[j-1];
            }
        }
        m_pUsersList[i].m_userID=userID;
        m_pUsersList[i].m_COMZ=COMZ;
        return; // successfully entered.
    }
}

XnBool ClosestUserSelector::TestIfShouldTrack(XnUserID userID)
{
    // go over all users and see if it is legal
    for(XnUserID i=0; i<m_nMaxNumUsers; i++)
    {
        if(m_pUsersList[i].m_userID==userID)
        {
            return TRUE; // we found the user so it should be tracked
        }
        if(m_pUsersList[i].m_COMZ<0)
        {
            return FALSE; // we got to a negative Z so since everything is ordered this means we didn't find the user
        }
    }
    return FALSE;
}


UserSelectionState* ClosestUserSelector::CreateUserSelectionState()
{
    return XN_NEW(UserStatusWithCom);
}


XnStatus ClosestUserSelector::UpdateUserSelectionState(XnUserID nUserId, XnSelectionState eState , XnInt64 subState)
{
    XnStatus nRetVal = UserSelector::UpdateUserSelectionState(nUserId,eState,subState);
    if(nRetVal != XN_STATUS_OK)
    {
        return nRetVal;
    }

    if(eState != XN_SELECTION_FAILED)
    {
        return XN_STATUS_OK;
    }

    UserSelectionState* tmpState;

    nRetVal=m_hUsersState.Get(nUserId, tmpState);
    if(nRetVal != XN_STATUS_OK)
    {
        return nRetVal;
    }
    if(tmpState==NULL)
    {
        return XN_STATUS_ERROR;
    }

    UserStatusWithCom* pState=(UserStatusWithCom*)tmpState;
    return m_pUserGenerator->GetCoM(nUserId,pState->m_COM);
}
