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
#ifndef XNV_SAMPLE_MANAGER_H_
#define XNV_SAMPLE_MANAGER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "UserTracker.h"
#include "TrackingInitializer.h"
#include "UserSelector.h"

/// @brief abstract class to create and manage the sample. 
class SampleManager
{
public:
    /// @brief method to start running the sample.
    /// 
    /// @param argc The number of command line arguments (same as main).
    /// @param argv The command line arguments array (same as main).
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
    UserTracker *m_pUserTracker; ///< The user tracker used when running
    TrackingInitializer *m_pTrackingInitializer; ///< The tracking initializer to use
    UserSelector *m_pUserSelector; ///< The user selector to use.
};

class DefaultInitializerWithCalibPose : public SampleManager
{
protected:
    DefaultInitializerWithCalibPose();
    virtual XnStatus SetSelectors();
    virtual UserSelector *CreateUserSelector(xn::UserGenerator *pUserGenerator,
                                             TrackingInitializer *pTrackingInitializer)=0;
};

/// @brief class to implement the sample with a wave user selector (waving causes the user which
/// waved to be selected).
class SingleWaveSampleManager : public DefaultInitializerWithCalibPose
{
public: 
    SingleWaveSampleManager();
protected:
    virtual UserSelector *CreateUserSelector(xn::UserGenerator *pUserGenerator,
                                             TrackingInitializer *pTrackingInitializer);
};

/// @brief class to implement the sample with a wave user selector (waving causes the user which
/// waved to be selected).
class MultipleWaveSampleManager : public DefaultInitializerWithCalibPose
{
public: 
    MultipleWaveSampleManager(XnUInt32 nMaxNumUsers);
protected:
    virtual UserSelector *CreateUserSelector(xn::UserGenerator *pUserGenerator,
                                             TrackingInitializer *pTrackingInitializer);

    XnUInt32 m_nMaxNumUsers;
};

/// @brief class to implement the sample with a closest user selector (waving causes the closest 
/// users to be selected).
class ClosestSampleManager : public DefaultInitializerWithCalibPose
{
public: 
    ClosestSampleManager(XnUInt32 nMaxNumUsers);
protected:
    virtual UserSelector *CreateUserSelector(xn::UserGenerator *pUserGenerator,
                                             TrackingInitializer *pTrackingInitializer);

    XnUInt32 m_nMaxNumUsers;
};

#endif // XNV_SAMPLE_MANAGER_H_
