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
#include "ClosestUserSelector.h"

#define VALIDATE(cond, errorMsg)                                    \
    if(!(cond))                                                     \
{                                                                   \
    printf("ClosestUserSelector creation failed: %s\n",errorMsg);   \
    m_bValid=FALSE;                                                 \
    return;                                                         \
}                                                                   \



#define MIN_SQR_DIST_TO_RETRY 40000
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
    ClearUserList();
    // find out who are the closest ones.
    for(UserStateHash::Iterator iter=m_hUsersState.begin(); iter!=m_hUsersState.end(); ++iter)
    {
        UserSelectionState* val=iter.Value();
        XnUserID newUser=iter.Key();
        XnPoint3D newCom;
        if(m_pUserGenerator->GetCoM(newUser,newCom)!=XN_STATUS_OK)
            continue; // irrelevant user.
        if(val->m_eState == XN_SELECTION_FAILED)
        {
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
                newCom.Z+=DIST_MOD_FOR_FAILURE;
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
        InsertNewUser(newUser,newCom.Z);
    }


    for(UserStateHash::Iterator iter=m_hUsersState.begin(); iter!=m_hUsersState.end(); ++iter)
    {
        UserSelectionState* pState=iter.Value();
        XnUInt32 curUser=iter.Key();
        if(TestIfShouldTrack(curUser))
        {
            if(pState->m_eState == XN_SELECTION_SELECTED || pState->m_eState == XN_SELECTION_TRACKING)
            {
                continue; // we are already tracking so nothing to do...
            }
            UpdateUserSelectionState(curUser, XN_SELECTION_SELECTED, 0);
            m_pTrackingInitializer->StartTracking(curUser); 
        }
        else
        {
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
    for(XnUInt32 i=0; i<m_nMaxNumUsers; i++)
    {
        m_pUsersList[i].m_userID=0;
        m_pUsersList[i].m_COMZ=-1.0f;
    }
}

void ClosestUserSelector::InsertNewUser(XnUserID userID, XnFloat COMZ)
{
    for(XnUInt32 i=0; i<m_nMaxNumUsers; i++)
    {
        if(m_pUsersList[i].m_COMZ>0 && m_pUsersList[i].m_COMZ<COMZ)
        {
            continue;
        }
        // if we are here then this is a place to put the new one.
        if(m_pUsersList[i].m_COMZ>0)
        {
            // we need to push everyone else forward...
            for(XnUInt32 j=i+1; j<m_nMaxNumUsers; j++)
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
    for(XnUserID i=0; i<m_nMaxNumUsers; i++)
    {
        if(m_pUsersList[i].m_userID==userID)
        {
            return TRUE;
        }
        if(m_pUsersList[i].m_COMZ<0)
        {
            return FALSE;
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
