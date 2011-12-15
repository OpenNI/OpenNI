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
#ifndef XNV_USER_TRACKER_H_
#define XNV_USER_TRACKER_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnPropNames.h>
#include "UserSelector.h"
#include "TrackingInitializer.h"
#include "ExitPoseDetector.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------



/// @brief Class to provide the OpenNI portions of the user selection sample
/// 
/// The goal of this class is to contain all elements of the user selection sample which are
/// OpenNI related (as opposed from the "main" and any drawing (OpenGL or otherwise).
/// since the drawing would require various inputs to know what to draw, this class exposes 
/// interfaces for this purpose.
class UserTracker
{
public:
    /// @brief Constructor
    /// 
    /// @param argc The number of command line arguments (same as main)
    /// @param argv The command line arguments (same as main)
    /// @param timeSpanForExitPose the number of microseconds exit pose must be 
    UserTracker(int argc, char **argv, XnUInt64 timeSpanForExitPose);


    /// @brief Destructor
    virtual ~UserTracker();

    /// @brief Method to initialize the user selector and tracking initializer to use.
    /// 
    /// @brief pUserSelector A pointer to the user selector to use
    /// @brief pTrackingInitializer A pointer to the tracking initializer to use.
    XnStatus InitUserSelection(UserSelector *pUserSelector,TrackingInitializer *pTrackingInitializer);

    /// @brief Returns true if the user tracker is valid.
    XnBool Valid() { return m_bValid; };

    /// @brief accessor to the user generator.
    xn::UserGenerator *GetUserGenerator() { return &m_UserGenerator; };

    /// @name MainLoopInterfaceMethods
    /// Methods used as an interface for the graphics part of the applications handled by the main
    /// loop and inputs.(i.e. enable the external graphic elements to control the OpenNI portions
    /// @{

    /// @brief Updates a single frame (mainly updating the OpenNI portions)
    void UpdateFrame();

    /// @brief cleans up elements before exiting
    void CleanUp();

    /// @}

    /// @name GraphicsInterfaceMethods
    /// Methods used as an interface for the graphics part of the applications to get the data
    /// to display.
    /// @{

    /// @brief gets the resolution of the depth node.
    /// 
    /// @param xRes The x resolution of the depth (the size of the map to draw on the screen)
    /// @param yRes The x resolution of the depth (the size of the map to draw on the screen)
    void GetImageRes(XnUInt16 &xRes, XnUInt16 &yRes);

    /// @brief Fills a texture with data to display.
    /// 
    /// This method fills the input textures with a color (RGB) for each pixel which represents the
    /// depth in intensity (multiplier) and the user (label) in color.
    /// @param pTexBuf the texture to fill. It is assumed the texture is RGB arranged in lines based
    /// on the height and width supplied. @note only the resolution defined by GetImageRes will be filled,
    /// Everything outside of it will not be changed.
    /// @param nTexWidth the width of the texture
    /// @param nTexHeight the height of the texture
    /// @param bDrawBackground If this is true then the background will have its own color, otherwise
    ///                        the background will not be drawn.
    void FillTexture(unsigned char* pTexBuf, XnUInt16 nTexWidth, XnUInt16 nTexHeight, XnBool bDrawBackground);

    /// @brief Gets the center of mass (com) and a label to show (state/error) for a specific user. 
    /// 
    /// @param nUserId The user to get info on
    /// @param com The center of mass to fill.
    /// @param strLabel A string to display (showing the state/error for the user).
    /// @param maxStrLen The maximum allowed size of the string.
    /// @return The success status (data irrelevant if not XN_STATUS_OK)
    XnStatus GetUserStringPos(XnUserID nUserId, XnPoint3D &com, char *strLabel, XnUInt32 maxStrLen);

    /// @brief Fills the limbs array. 
    /// 
    /// This method fills an array of limbs to draw. These limbs represent the skeleton.
    /// @param nUserID the user whose limbs we need.
    /// @param limbs An array of points. The array is of size 2*numlimbs as each limb is defined by 
    /// two points (i.e. Limb 'i' will be defined by limbs[i*2] and limbs[i*2+1]).
    /// @param numlimbs The maximum allowed number of limbs in the input and is changed to the
    ///                 number of actually found limbs.
    /// @return The success status (data irrelevant if not XN_STATUS_OK)
    XnStatus GetLimbs(XnUserID nUserID, XnPoint3D *limbs,XnUInt16 &numLimbs);

    /// @brief gets the color (an array of 3 floats) for a specific user.
    /// 
    /// @param nUserID the user whose color we need.
    /// @param userColor An array (assumed to be of size 3) to be filled with the color in RGB format
    /// @return The success status (data irrelevant if not XN_STATUS_OK)
    XnStatus GetUserColor(XnUserID nUserId, XnFloat* userColor);
    

    /// @brief gets a value between 0 and 1 of the exit pose state
    /// 
    /// When the value is 0, no exit pose has been detected. The value then slowly rises to
    /// 1 to show the progress of being in the exit pose for a timeout.
    /// @param nUserId The user whose state we are checking. A user of 0 means the highest value of
    ///                all.
    XnFloat GetExitPoseState(XnUserID nUserId);
    /// @}

protected:
    // calculate the cumulative histogram. Add an explanation here.
    void CalcHistogram(const XnDepthPixel* pDepth, XnUInt16 xRes,XnUInt16 yRes);
    // gets the depth data. Not sure if shouldn't be entered internally to FillTexture
    const XnDepthPixel*GetDepthData();
    // gets the scene data. Not sure if shouldn't be entered internally to FillTexture
    const XnLabel*GetUsersPixelsData();


    UserSelector *m_pUserSelector; ///< @brief a pointer to the user selector to use
    TrackingInitializer *m_pTrackingInitializer; ///< @brief a pointer to the tracking initializer to use
    xn::Context m_Context; ///< @brief The context of all OpenNI behavior
    xn::ScriptNode m_ScriptNode; ///< @brief the script node used when opening using XML. Used for orderly release
    xn::DepthGenerator m_DepthGenerator; ///< A depth generator (the base for depth info)
    xn::UserGenerator m_UserGenerator; ///< A user generator (will hold user and skeleton data).
    xn::Player m_Player; ///< The node used for the player (to play recordings).

    ExitPoseDetector *m_pExitPoseDetector;
    XnUInt64 m_timeSpanForExitPose;
private:
    static float* s_pDepthHist; // the cumulative histogram (explain it is nulled and changed in CalcHistogram each frame.
    static XnFloat s_Colors[][3]; // the list of colors (make sure it is initialized correctly)
    static XnUInt32 s_nColors; // the number of colors


    XnBool m_bRecord; // if we are recording or not.
    XnBool m_bValid; // holds true if we have a valid value. This only happens after initialization!
};


#endif // XNV_USER_TRACKER_H_
