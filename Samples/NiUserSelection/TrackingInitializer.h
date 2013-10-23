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
#ifndef XNV_TRACKING_INITIALIZER_H_
#define XNV_TRACKING_INITIALIZER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

#include <XnCppWrapper.h>
#include "UserSelector.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------



/// @brief Class to start tracking a user
/// 
/// The goal of this class is to start performing the actual tracking of a user. It returns the 
/// results to the UserSelector which called it.
/// @ingroup UserSelectionInitializers
class TrackingInitializer
{
public:
    /// @brief Method to start the tracking process
    /// 
    /// This method is called when we want to track a specific user and it starts the process.
    /// @param nUserId The user to track
    /// @param bForce If this is true then the tracking will forget all previous tracking information
    /// @return The success status 
    virtual XnStatus StartTracking(XnUserID nUserId, XnBool bForce)=0;

    /// @brief Method to stop tracking a user
    /// 
    /// @param nUserId The user to stop track
    /// @return The success status 
    virtual XnStatus AbortTracking(XnUserID nUserId)=0;
    /// @brief Returns true if the Tracking Initializer is valid and false otherwise.
    XnBool IsValid() { return m_bValid; };

    /// @brief Sets the user selector to use to send feedback on tracking progress.
    /// 
    /// @note This will fail if the user selector is not NULL before setting.
    /// @param pUserSelector The user selector
    /// @return success status
    virtual XnStatus SetUserTracker(UserSelector *pUserSelector);

    /// @brief Destructor
    virtual ~TrackingInitializer();
protected:
    /// @brief Constructor
    /// 
    /// @param pUserGenerator The user selector to update
    TrackingInitializer(xn::UserGenerator *pUserGenerator);

    UserSelector *m_pUserSelector; ///< @brief The user selector to update
    xn::UserGenerator *m_pUserGenerator; ///< @brief The user generator which holds the users
    XnBool m_bValid; ///< @brief True if the Tracking Initializer is valid and false otherwise

};
#endif // XNV_TRACKING_INITIALIZER_H_

