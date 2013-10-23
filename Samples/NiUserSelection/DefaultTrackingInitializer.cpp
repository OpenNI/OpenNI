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
#include "DefaultTrackingInitializer.h"

DefaultTrackingInitializer::DefaultTrackingInitializer(xn::UserGenerator *pUserGenerator) :
                                                          TrackingInitializer(pUserGenerator),
                                                          m_hCalibrationStartCallback(NULL),
                                                          m_hCalibrationCompleteCallback(NULL),
                                                          m_hInProgressCallback(NULL)
{
    if(m_pUserGenerator->IsCapabilitySupported(XN_CAPABILITY_SKELETON)==FALSE)
    {
        m_bValid=FALSE;
        return; // we need a skeleton, otherwise, we don't have calibration
    }

    XnStatus res=m_pUserGenerator->GetSkeletonCap().RegisterToCalibrationStart(CalibrationStartCallback,this,m_hCalibrationStartCallback);
    if(res!=XN_STATUS_OK)
    {
        m_hCalibrationStartCallback=NULL;
        m_bValid=FALSE;
        return; // if we can't register to the calibration, we shouldn't do anything
    }
    res=m_pUserGenerator->GetSkeletonCap().RegisterToCalibrationComplete(CalibrationCompleteCallback,this,m_hCalibrationStartCallback);
    if(res!=XN_STATUS_OK)
    {
        m_hCalibrationStartCallback=NULL;
        m_bValid=FALSE;
        return; // if we can't register to the calibration, we shouldn't do anything
    }
    res=m_pUserGenerator->GetSkeletonCap().RegisterToCalibrationInProgress(CalibrationInProgressCallback,this,m_hInProgressCallback);
    if(res!=XN_STATUS_OK)
    {
        m_hInProgressCallback=NULL;
        m_bValid=FALSE;
        return; // if we can't register to the calibration, we shouldn't do anything
    }
}

DefaultTrackingInitializer::~DefaultTrackingInitializer()
{
    // clean up
    if(m_pUserGenerator==NULL)
        return;
    if(m_hCalibrationStartCallback!=NULL)
    {
        m_pUserGenerator->GetSkeletonCap().UnregisterFromCalibrationStart(m_hCalibrationStartCallback);
        m_hCalibrationStartCallback=NULL;
    }

    if(m_hCalibrationCompleteCallback!=NULL)
    {
        m_pUserGenerator->GetSkeletonCap().UnregisterFromCalibrationComplete(m_hCalibrationCompleteCallback);
        m_hCalibrationCompleteCallback=NULL;
    }
    if(m_hInProgressCallback!=NULL)
    {
        m_pUserGenerator->GetSkeletonCap().UnregisterFromCalibrationInProgress(m_hInProgressCallback);
        m_hInProgressCallback=NULL;
    }
}



XnStatus DefaultTrackingInitializer::StartTracking(XnUserID nUserId, XnBool bForce)
{
    if(m_pUserGenerator->GetSkeletonCap().IsTracking(nUserId)==TRUE)
    {
        // we don't need to do anything (other than notifying the user selector) as we are 
        // already tracking
        if(m_pUserSelector!=NULL)
        {
            return m_pUserSelector->UpdateUserTracking(nUserId,TRUE,0); // already tracking
        }
        else
            return XN_STATUS_OK;
        
    }
    // request calibration.
    return m_pUserGenerator->GetSkeletonCap().RequestCalibration(nUserId,bForce);
}


XnStatus DefaultTrackingInitializer::AbortTracking(XnUserID nUserId)
{
    // note: the assumption is that this will not be called if not either calibrating or
    // tracking (although it will just fail if neither).
    if(m_pUserGenerator->GetSkeletonCap().IsTracking(nUserId)==TRUE)
    {
        // if we are tracking we just need to stop tracking
        XnStatus res=m_pUserGenerator->GetSkeletonCap().StopTracking(nUserId);
        return res;
    }
    // if we are not tracking we need to abort the calibration.
    return m_pUserGenerator->GetSkeletonCap().AbortCalibration(nUserId);
}

XnStatus DefaultTrackingInitializer::CalibrationStart(XnUserID /*nUserId*/)
{
    return XN_STATUS_OK;
}
XnStatus DefaultTrackingInitializer::CalibrationComplete(XnUserID nUserId,XnCalibrationStatus eStatus)
{
    XnBool retVal=FALSE;
    if (eStatus == XN_CALIBRATION_STATUS_OK)
    {
        // we start the actual tracking!
        if(m_pUserGenerator->GetSkeletonCap().StartTracking(nUserId)==XN_STATUS_OK) 
        {
            retVal=TRUE;
        }
        
    }
    if(m_pUserSelector!=NULL)
    {
        return m_pUserSelector->UpdateUserTracking(nUserId,retVal,eStatus);
    }
    return XN_STATUS_OK;
}

XnStatus DefaultTrackingInitializer::CalibrationInProgress(XnUserID nUserId,XnCalibrationStatus eStatus)
{
    if(m_pUserSelector!=NULL)
        return m_pUserSelector->UpdateUserTrackingProgress(nUserId,eStatus);
    return XN_STATUS_OK;
}




// Callback: Started calibration
void XN_CALLBACK_TYPE DefaultTrackingInitializer::CalibrationStartCallback(xn::SkeletonCapability& /*capability*/, XnUserID nUserId, void* pCookie)
{
    DefaultTrackingInitializer *pDefaultTrackingInitializer=(DefaultTrackingInitializer *)pCookie;
    pDefaultTrackingInitializer->CalibrationStart(nUserId);
}

void XN_CALLBACK_TYPE DefaultTrackingInitializer::CalibrationCompleteCallback(xn::SkeletonCapability& /*capability*/, XnUserID nUserId, XnCalibrationStatus eStatus, void* pCookie)
{
    DefaultTrackingInitializer *pDefaultTrackingInitializer=(DefaultTrackingInitializer *)pCookie;
    pDefaultTrackingInitializer->CalibrationComplete(nUserId,eStatus);
}

void XN_CALLBACK_TYPE DefaultTrackingInitializer::CalibrationInProgressCallback(xn::SkeletonCapability& /*capability*/, XnUserID nUserId, XnCalibrationStatus eStatus, void* pCookie)
{
    DefaultTrackingInitializer *pDefaultTrackingInitializer=(DefaultTrackingInitializer *)pCookie;
    pDefaultTrackingInitializer->CalibrationInProgress(nUserId,eStatus);
}

