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
#include "SampleManager.h"
#include "SceneDrawer.h"
#include "DefaultTrackingInitializer.h"
#include "SinglePoseUserSelector.h"
#include "PoseToggleUserSelector.h"
#include "ClosestUserSelector.h"

// Utility macro. This calls cleanup, prints a message and returns.
#define RETURN_WITH_CLEANUP(returnCode,errorMsg)                                \
                            Cleanup();                                          \
                            printf("Failed to start sample: %s\n", errorMsg);   \
                            return returnCode; 


SampleManager::SampleManager() : m_pUserTracker(NULL), 
                                   m_pTrackingInitializer(NULL), 
                                   m_pUserSelector(NULL)
{

}


XnStatus SampleManager::StartSample(int argc, char **argv)
{
    // first we create a UserTracker object. This initializes all the openNI portions...
    m_pUserTracker = XN_NEW(UserTracker,argc,argv,3000000); 

    // make sure the initialization was a success
    if(m_pUserTracker->Valid()==FALSE) 
    {
        RETURN_WITH_CLEANUP(XN_STATUS_ERROR, "User tracker invalid"); 
    }

    // set the user selector and tracking initializers. This is the main method overriden by 
    // inheriting classes.
    if(SetSelectors()!=XN_STATUS_OK)
    {
        RETURN_WITH_CLEANUP(XN_STATUS_ERROR, "Failed to initialize user selector and tracking initializers"); 
    }

    // start the actual running (handled by the scene drawer).
    // note pUserTracker is deleted inside DrawScene when quitting.
    SceneDrawer *singleton=SceneDrawer::GetInstance();
    // This starts the actual program. NOTE: the DrawScene method never ends except for
    // using the exit call to end the program. 
    singleton->DrawScene(m_pUserTracker,argc,argv,this);
    return XN_STATUS_OK; 
}

void SampleManager::Cleanup()
{
    if(m_pUserSelector!=NULL)
    {
        XN_DELETE(m_pUserSelector);
        m_pUserSelector=NULL;
    }
    if(m_pTrackingInitializer!=NULL)
    {
        XN_DELETE(m_pTrackingInitializer);
        m_pTrackingInitializer=NULL;
    }
    if(m_pUserTracker!=NULL)
    {
        XN_DELETE(m_pUserTracker);
        m_pUserTracker=NULL;
    }
}
SampleManager::~SampleManager()
{
    Cleanup();
}

DefaultInitializerWithCalibPose::DefaultInitializerWithCalibPose()
{

}

XnStatus DefaultInitializerWithCalibPose::SetSelectors()
{
    // get the user generator (required as an input).
    xn::UserGenerator *pUserGenerator=m_pUserTracker->GetUserGenerator();

    // make sure the user generator is good.
    if(pUserGenerator->IsCapabilitySupported(XN_CAPABILITY_SKELETON)==FALSE)
    {
        RETURN_WITH_CLEANUP(XN_STATUS_ERROR, "User generator does not support skeleton!"); 
    }

    // tracking initializer to use is the default one.
    m_pTrackingInitializer=XN_NEW(DefaultTrackingInitializer,pUserGenerator); 
    if(m_pTrackingInitializer->IsValid()==FALSE)
    {
        RETURN_WITH_CLEANUP(XN_STATUS_ERROR, "Failed to create tracking initializer"); 
    }

    // pick the user selector.

    // make sure we have a pose detection capability (which is what this selector is all about).
    if(pUserGenerator->IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)==FALSE)
    {
        RETURN_WITH_CLEANUP(XN_STATUS_ERROR, "User tracker invalid"); 
    }


    // figure out which type of user selection to use.
    // Option 1: if we need calibration pose - that is the type to use (and exit if we can't support it)
    // Option 2: if do not need calibration pose we choose wave (assuming it is supported, otherwise, exit).
    if(pUserGenerator->GetSkeletonCap().NeedPoseForCalibration()==TRUE)
    {
        // first we get the pose from the skeleton capability
        char poseString[50];
        pUserGenerator->GetSkeletonCap().GetCalibrationPose(poseString);
        // we now create the appropriate pose selector chooser
        m_pUserSelector=XN_NEW(PoseUserSelector,pUserGenerator,m_pTrackingInitializer,poseString);
    }
    else
    {
        // get a single user wave pose selector
        m_pUserSelector=CreateUserSelector(pUserGenerator,m_pTrackingInitializer);
    }

    // make sure the user selector is valid
    if(m_pUserSelector->IsValid()==FALSE)
    {
        RETURN_WITH_CLEANUP(XN_STATUS_ERROR,"Failed to create user selector");
    }

    // initialize the tracking initializer with the relevant user selector.
    m_pTrackingInitializer->SetUserTracker(m_pUserSelector);

    // set the user selector and tracking initializer to the 
    m_pUserTracker->InitUserSelection(m_pUserSelector,m_pTrackingInitializer);

    return XN_STATUS_OK; 
}





SingleWaveSampleManager::SingleWaveSampleManager()
{

}
UserSelector * SingleWaveSampleManager::CreateUserSelector(xn::UserGenerator *pUserGenerator,
                                                           TrackingInitializer *pTrackingInitializer)
{
    return XN_NEW(SinglePoseUserSelector,pUserGenerator,pTrackingInitializer,"Wave");
}



MultipleWaveSampleManager::MultipleWaveSampleManager(XnUInt32 nMaxNumUsers) : m_nMaxNumUsers(nMaxNumUsers)
{

}

UserSelector * MultipleWaveSampleManager::CreateUserSelector(xn::UserGenerator *pUserGenerator,
                                                             TrackingInitializer *pTrackingInitializer)
{
    return XN_NEW(PoseToggleUserSelector,pUserGenerator,pTrackingInitializer,"Wave",m_nMaxNumUsers,2000000);
}

ClosestSampleManager::ClosestSampleManager(XnUInt32 nMaxNumUsers) : m_nMaxNumUsers(nMaxNumUsers)
{

}


UserSelector * ClosestSampleManager::CreateUserSelector(xn::UserGenerator *pUserGenerator,
                                                        TrackingInitializer *pTrackingInitializer)
{
    return XN_NEW(ClosestUserSelector,pUserGenerator,pTrackingInitializer,m_nMaxNumUsers);
}


