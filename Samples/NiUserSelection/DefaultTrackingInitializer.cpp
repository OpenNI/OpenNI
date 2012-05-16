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

