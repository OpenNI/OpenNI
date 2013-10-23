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
#ifndef XNV_SAMPLE_MANAGER_H_
#define XNV_SAMPLE_MANAGER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "UserTracker.h"
#include "TrackingInitializer.h"
#include "UserSelector.h"

/// @brief abstract class to create and manage the sample. 
/// @ingroup UserSelectionSampleManagers
class SampleManager
{
public:
    /// @brief method to start running the sample.
    /// 
    /// @param argc The number of command line arguments (same as @ref main()).
    /// @param argv The command line arguments array (same as @ref main()).
    /// @return the success code.
    virtual XnStatus StartSample(int argc, char **argv);
    /// @brief Cleanup method.
    /// 
    /// This method is called by the graphics when exiting and should clean everything up.
    virtual void Cleanup();
    /// @brief destructor
    virtual ~SampleManager();
protected:
    /// @brief constructor
    SampleManager();
    /// @brief Method to choose the UserSelector and TrackingInitializer
    /// 
    /// This is the method to overwrite when inheriting. It is responsible for filling up
    /// m_pUserSelector with the user selector type and m_pTrackingInitializer with the tracking 
    /// initializer type matching the require behavior.
    virtual XnStatus SetSelectors()=0;
    UserTracker *m_pUserTracker; ///< @brief The user tracker used when running
    TrackingInitializer *m_pTrackingInitializer; ///< @brief The tracking initializer to use
    UserSelector *m_pUserSelector; ///< @brief The user selector to use.
};

/// @brief an abstract SampleManager implementation which does calibration pose user selection if the
/// skeleton requires a calibration pose and uses the default tracking initializer.
/// @ingroup UserSelectionSampleManagers
class DefaultInitializerWithCalibPose : public SampleManager
{
protected:
    /// @brief Constructor
    DefaultInitializerWithCalibPose();
    virtual XnStatus SetSelectors();
    /// @brief Internal method to construct the user selector
    /// @param pUserGenerator The user generator to use
    /// @param pTrackingInitializer The tracking initializer to use
    /// @return The user selector (null if failed).
    virtual UserSelector *CreateUserSelector(xn::UserGenerator *pUserGenerator,
                                             TrackingInitializer *pTrackingInitializer)=0;
};

/// @brief class to implement the sample with a wave user selector (waving causes the user which
/// waved to be selected).
/// @ingroup UserSelectionSampleManagers
class SingleWaveSampleManager : public DefaultInitializerWithCalibPose
{
public: 
    /// @brief Constructor
    SingleWaveSampleManager();
protected:
    virtual UserSelector *CreateUserSelector(xn::UserGenerator *pUserGenerator,
                                             TrackingInitializer *pTrackingInitializer);
};

/// @brief class to implement the sample with a wave user selector (waving causes the user which
/// waved to be selected).
/// @ingroup UserSelectionSampleManagers
class MultipleWaveSampleManager : public DefaultInitializerWithCalibPose
{
public: 
    /// @brief Constructor
    /// 
    /// @param nMaxNumUsers maximum number of tracked users
    MultipleWaveSampleManager(XnUInt32 nMaxNumUsers);
protected:
    virtual UserSelector *CreateUserSelector(xn::UserGenerator *pUserGenerator,
                                             TrackingInitializer *pTrackingInitializer);

    XnUInt32 m_nMaxNumUsers; ///< @brief The maximum allowed number of users
};

/// @brief class to implement the sample with a closest user selector (waving causes the closest 
/// users to be selected).
/// @ingroup UserSelectionSampleManagers
class ClosestSampleManager : public DefaultInitializerWithCalibPose
{
public: 
    /// @brief Constructor
    /// 
    /// @param nMaxNumUsers maximum number of tracked users
    ClosestSampleManager(XnUInt32 nMaxNumUsers);
protected:
    virtual UserSelector *CreateUserSelector(xn::UserGenerator *pUserGenerator,
                                             TrackingInitializer *pTrackingInitializer);

    XnUInt32 m_nMaxNumUsers; ///< @brief The maximum allowed number of users
};

#endif // XNV_SAMPLE_MANAGER_H_
