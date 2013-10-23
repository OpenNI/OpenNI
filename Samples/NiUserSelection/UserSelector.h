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
#ifndef XNV_USER_SELECTOR_H_
#define XNV_USER_SELECTOR_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnCppWrapper.h>
#include <XnHashT.h>
#include "UserSelectionStructures.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------



/// @brief Class to select which users to track
/// 
/// The goal of this class is to decide which users to track. This is an abstract class
/// used as an interface for all user selection implementations. The assumption is that the user
/// selector will receive a user generator node and will output which users are at which state as
/// well as deciding which users to track.
/// @ingroup UserSelectionClasses
class UserSelector
{
public:
    /// @brief This method is aimed to be called by the TrackingInitializer to update the sub
    /// state of a user when trying to track.
    /// 
    /// This is an update of the progress of starting to track the user, i.e. the user is in state
    /// selected (and not yet tracking) and the sub state represents the progress.
    /// @param nUserId The user whose progress is to be updated
    /// @param newSubState The new sub state achieved in the progress of starting to track.
   /// @return The success status
    virtual XnStatus UpdateUserTrackingProgress(XnUserID nUserId, XnInt64 newSubState);

   /// @brief This method is aimed to be called by the TrackingInitializer when the user 
   /// tracking initialization is complete.
   /// 
   /// This tells the user selector how a selection went (succeed or failed).
   /// @param nUserId The user whose progress is to be updated
   /// @param bTracked True if the user is now being tracked and false otherwise.
   /// @param newSubState The new sub state achieved in the tracking (mainly has meaning on failure).
   /// @return The success status
   virtual XnStatus UpdateUserTracking(XnUserID nUserId, XnBool bTracked, XnInt64 newSubState);

   /// @brief This method should be called every frame.
   /// 
   /// @note The default implementation is to do nothing as many implementations would be event driven.
   virtual void UpdateFrame() {};

   /// @brief Method to get the current state (and sub state) of a user.
   /// 
   /// @param nUserId The user whose state we want to know
   /// @param newState The state to fill with new data 
   /// @return The success status
   virtual XnStatus GetUserState(XnUserID nUserId, UserSelectionState& newState);


   /// @brief Method to Fill in the string label for a user (state + sub state).
   /// 
   /// @param nUserId The user whose label we want
   /// @param strLabel Where to put the label
   /// @param maxStrLen The maximum allowed string length.
   /// @return The success status
   XnStatus GetUserLabel(XnUserID nUserId, char* strLabel, XnUInt32 maxStrLen);

   /// @brief Returns true if the user selector is valid and false otherwise.
   XnBool IsValid() { return m_bValid; };

   /// @brief Destructor
   virtual ~UserSelector();
protected:

    /// @brief A utility function to get the number of currently selected users (both tracking and not yet tracking).
    XnUInt32 GetNumberOfSelectedUsers();

    /// @brief Translates a state to a string
    ///
    /// @param pUserState The user state to translate
    /// @param strLabel Where to put the label
    /// @param maxStrLen The maximum allowed string length.
    /// @return The success status
   XnStatus TranslateStateToLabel(const UserSelectionState* pUserState, char* strLabel, XnUInt32 maxStrLen);

    /// @brief constructor
    /// 
    /// @param pUserGenerator The user generator which holds all the users.
    UserSelector(xn::UserGenerator* pUserGenerator);

    /// @brief Adds a new user to the hash
    ///
    /// Called internally from the NewUserCallback callback.
    /// @param nUserId The user we are adding
    /// @return The success status
    virtual XnStatus AddNewUser(XnUserID nUserId);

    /// @brief Remove a user from the hash
    ///
    /// Called internally from the LostUserCallback callback.
    /// @param nUserId The user we are removing
    /// @return The success status
    virtual XnStatus RemoveUser(XnUserID nUserId);

    /// @brief translate a calibration error to a string
    /// 
    /// @param eError The calibration error
    /// @return The string version of the error.
    const XnChar* GetCalibrationErrorString(XnCalibrationStatus eError);

    /// @brief Creates a new user selection state object of the appropriate type
    /// 
    /// @return A pointer to the new object (NULL if failed).
    virtual UserSelectionState* CreateUserSelectionState();


    /// @brief Updates the user selection state object of the appropriate type
    /// 
    /// @param nUserId the user whose state we want to update
    /// @param eState The new state of the user
    /// @param subState The new sub state of the user
    /// @return The success code.
    virtual XnStatus UpdateUserSelectionState(XnUserID nUserId, XnSelectionState eState , XnInt64 subState);

    /// @brief Defines the UserHash hash which holds a state for each user
	typedef XnHashT<XnUserID, UserSelectionState*> UserStateHash;

    xn::UserGenerator *m_pUserGenerator; ///< @brief the user generator to get user information from
    /// @brief A hash for user states.
    /// 
    /// This is a hash which holds for each user in the scene a state and substate.
    /// @see @ref UserSelectionState
    UserStateHash m_hUsersState; 
    XnBool m_bValid; ///< @brief Holds true if the user selector is valid and false otherwise
    XnCallbackHandle m_hUserCallbacks; ///< @brief A handle to be able to unregister the user callbacks


private:
    /// @brief New user callback.
    /// 
    /// Called when a new user is found.
    /// @param generator The user generator which called the callback
    /// @param nUserId The newly found user
    /// @param pCookie A cookie which on registering is defined as the calling user selector object.
    static void XN_CALLBACK_TYPE NewUserCallback(xn::UserGenerator& generator, XnUserID nUserId, void* pCookie);

    /// @brief Lost user callback.
    /// 
    /// Called when a user is lost (i.e. no longer counted in the scene. Does not include exiting).
    /// @param generator The user generator which called the callback
    /// @param nUserId The lost user
    /// @param pCookie A cookie which on registering is defined as the calling user selector object.
    static void XN_CALLBACK_TYPE LostUserCallback(xn::UserGenerator& generator, XnUserID nUserId, void* pCookie);

};
#endif // XNV_USER_SELECTOR_H_
