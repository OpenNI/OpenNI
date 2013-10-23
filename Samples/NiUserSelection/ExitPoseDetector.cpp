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
#include "ExitPoseDetector.h"

#define CROSS_HANDS_POSE "CrossHandsPose"

ExitPoseDetector::ExitPoseDetector(xn::UserGenerator userGenNode) : m_hPoseDetectCallback(NULL),
                                                                    m_hPoseOutOfPoseDetectCallback(NULL),
                                                                    m_hUserCallbacks(NULL),
                                                                    m_userGenerator(userGenNode),
                                                                    m_bValid(FALSE)
{
    if(!m_userGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
    {
        return;
    }
    xn::PoseDetectionCapability cap=m_userGenerator.GetPoseDetectionCap();
    if(!cap.IsPoseSupported(CROSS_HANDS_POSE))
    {
        return; // we need this pose to continue...
    }
    if(cap.RegisterToPoseDetected(PoseDetectedCallback,this,m_hPoseDetectCallback)!=XN_STATUS_OK)
    {
        m_hPoseDetectCallback=NULL;
        return;
    }
    if(cap.RegisterToOutOfPose(OutOfPoseDetectedCallback,this,m_hPoseOutOfPoseDetectCallback)!=XN_STATUS_OK)
    {
        m_hPoseOutOfPoseDetectCallback=NULL;
        return;
    }
    if(m_userGenerator.RegisterUserCallbacks(NewUserCallback,LostUserCallback,this,m_hUserCallbacks)!=XN_STATUS_OK)
    {
        m_hUserCallbacks=NULL;
        return;
    }
    m_bValid=TRUE;
}


ExitPoseDetector::~ExitPoseDetector()
{
    m_bValid=FALSE;

    if(m_hUserCallbacks!=NULL)
    {
        m_userGenerator.UnregisterUserCallbacks(m_hUserCallbacks);
        m_hUserCallbacks=NULL;
    }

    if(!m_userGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
    {
        return;
    }
    xn::PoseDetectionCapability cap=m_userGenerator.GetPoseDetectionCap();
    if(m_hPoseDetectCallback!=NULL)
    {
        cap.UnregisterFromPoseDetected(m_hPoseDetectCallback);
        m_hPoseDetectCallback=NULL;
    }
    if(m_hPoseOutOfPoseDetectCallback!=NULL)
    {
        cap.UnregisterFromOutOfPose(m_hPoseOutOfPoseDetectCallback);
        m_hPoseOutOfPoseDetectCallback=NULL;
    }
}

XnUInt64 ExitPoseDetector::GetExitPoseTimeStamp(XnUserID nUserId)
{
    if(nUserId==0)
    {
        XnUInt64 maxTime=0;
        for(UserExitPoseTimes::Iterator iter=m_userExitPoseTimes.Begin(); iter!=m_userExitPoseTimes.End(); ++iter)
        {
            XnUInt64 userTime=iter->Value();
            if(userTime>maxTime)
            {
                maxTime=userTime;
            }
        }
        return maxTime;
    }
    // we have an actual user
    XnUInt64 userTime;
    if(m_userExitPoseTimes.Get(nUserId,userTime)!=XN_STATUS_OK)
    {
        return 0; // no user data
    }
    return userTime;
}


void XN_CALLBACK_TYPE ExitPoseDetector::PoseDetectedCallback(xn::PoseDetectionCapability& /*capability*/, const XnChar* strPose, XnUserID nUserId, void* pCookie)
{
    if(xnOSStrCmp(strPose,CROSS_HANDS_POSE)!=0)
    {
        return; // irrelevant pose
    }
    ExitPoseDetector *detector=(ExitPoseDetector *)pCookie;
    XnUInt64 tmpTime;
    if(detector->m_userExitPoseTimes.Get(nUserId,tmpTime)!=XN_STATUS_OK || tmpTime==0)
    {
        tmpTime=detector->m_userGenerator.GetTimestamp();
        detector->m_userExitPoseTimes.Set(nUserId,tmpTime);
    }
}

void XN_CALLBACK_TYPE ExitPoseDetector::OutOfPoseDetectedCallback(xn::PoseDetectionCapability& /*capability*/, const XnChar* strPose, XnUserID nUserId, void* pCookie)
{
    if(xnOSStrCmp(strPose,CROSS_HANDS_POSE)!=0)
    {
        return; // irrelevant pose
    }
    ExitPoseDetector *detector=(ExitPoseDetector *)pCookie;
    XnUInt64 tmpTime=0;
    detector->m_userExitPoseTimes.Set(nUserId,tmpTime); // we are not in pose any more.
}

void XN_CALLBACK_TYPE ExitPoseDetector::NewUserCallback(xn::UserGenerator& /*generator*/, XnUserID nUserId, void* pCookie)
{
    ExitPoseDetector *detector=(ExitPoseDetector *)pCookie;
    detector->m_userGenerator.GetPoseDetectionCap().StartPoseDetection(CROSS_HANDS_POSE,nUserId);
}

void XN_CALLBACK_TYPE ExitPoseDetector::LostUserCallback(xn::UserGenerator& /*generator*/, XnUserID nUserId, void* pCookie)
{
    ExitPoseDetector *detector=(ExitPoseDetector *)pCookie;
    detector->m_userExitPoseTimes.Remove(nUserId);
}
