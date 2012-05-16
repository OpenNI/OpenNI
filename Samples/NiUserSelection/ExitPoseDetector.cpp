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
