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
#ifndef XNV_SCENE_DRAWER_H_
#define XNV_SCENE_DRAWER_H_

#include "UserTracker.h"
#include "SampleManager.h"

#ifndef USE_GLES
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#else
#include "opengles.h"
#endif




/// @brief Class to draw the scene for the sample
/// 
/// The goal of this class is to contain everything needed to draw the scene (image, skeleton
/// etc.), separating the graphics implementation elements (e.g. OpenGL) from the the OpenNI elements.
/// This should enable the user to concentrate on the UserTracker class which contains the elements
/// relevant to OpenNI and ignore the graphics.
/// @note This class can be ignored by anyone who wishes to learn only the OpenNI elements.
/// 
/// @note This class is implemented based on the singleton pattern i.e. we can only have one drawer!
class SceneDrawer
{
public:
    /// @brief Start running the scene
    /// 
    /// This method starts running the scene. It will not return until the program exits!
    /// @param m_pUserTrackerObj A pointer to the user tracker object. This is where the scene drawer
    /// takes the information to draw and provides feedback from keyboard. @note The CleanUp method
    /// will be called on the user tracker when the program exits from inside the DrawScene method!.
    /// @param argc Number of command line arguments (same as main()).
    /// @param argv The command line arguments (same as main()).
    void DrawScene(UserTracker *pUserTrackerObj,int argc, char **argv,SampleManager *pSample);

    /// @brief gets the singleton instance.
    static SceneDrawer *GetInstance();
private:
    static SceneDrawer *m_pSingleton; ///< The singleton instance


    /// Private constructor just to make sure we can't create an object. Only GetInstance creates a 
    /// new object.
    SceneDrawer(); 

    /// @brief Draws the texture which shows the depth and users in the scene.
    void DrawDepthMapTexture();

    /// @brief Method to draw the labels (nUserId and state) for each user in the scene
    /// 
    /// @param nUserId The user whose labels we will draw. 
    void DrawLabels(XnUserID nUserId);

    /// @brief Method to draw the skeleton for each user in the scene
    /// 
    /// @param nUserId The user whose skeleton we will draw. 
    void DrawSkeleton(XnUserID nUserId);

    /// @brief Method to exit from the DrawScene method (and the program as a whole)
    /// 
    /// @param exitCode The exit code we will exit with.
    /// 
    /// @note this will call the CleanUp method on the m_pUserTrackerObj member!
    void ExitSample(int exitCode);

    /// @brief Does all initialization required for texture drawing.
    void InitTexture();

    /// @brief An internal callback which is called from inside DrawScene main loop to do the drawing.
    ///
    /// This method does the single frame update including both updating the user tracker and calling
    /// the various internal methods to draw the texture, labels and skeleton.
    /// @note this is a callback for glutMainLoop if using glut (rather than GLES) and called all 
    /// the time in the while loop if we are using GLES.
    static void glutDisplay (void);

#ifndef USE_GLES
    /// @brief An internal callback which is called from inside DrawScene main loop for background 
    /// frames.
    /// 
    /// This callback is called by glutMainLoop whenever the program is in the background and no drawing
    /// is required.
    static void glutIdle (void);

    /// @brief An internal callback which is called from inside DrawScene main loop whenever there is
    /// a keyboard event.
    /// 
    /// This callback is called by glutMainLoop whenever a keyboard event occurs when the program 
    /// window is in focus. 
    /// @param key The key pressed
    /// @param x The x position of the mouse when the key was pressed in windows relative coordinates
    /// @param y The y position of the mouse when the key was pressed in windows relative coordinates
    static void glutKeyboard (unsigned char key, int x, int y);

    /// @brief Internal method to perform initialization for OpenGL when using glut (rather than GLES)
    /// 
    /// @param argc Number of command line arguments (same as main()).
    /// @param argv The command line arguments (same as main()).
    void glInit (int * pargc, char ** argv);
#endif // USE_GLES



    // members

    /// @brief A pointer to the user tracker object. This is where the scene drawer
    /// takes the information to draw and provides feedback from keyboard.
    UserTracker *m_pUserTrackerObj; 


    /// @name TextureInitializationMembers
    /// members used for initializing the texture
    /// @{

    GLuint depthTexID;
    unsigned char* pDepthTexBuf;
    int texWidth, texHeight;
    GLfloat texcoords[8];
    XnPoint3D* limbsPos;
    /// @}

    /// @name PrintMemebers
    /// members used for deciding what to draw
    /// @{
  
    /// @brief True when we want to draw the background image
    XnBool g_bDrawBackground;
    /// @brief True when we want to draw any texture
    XnBool g_bDrawPixels;
    /// @brief True when we want to draw the skeleton
    XnBool g_bDrawSkeleton;
    /// @brief True when we want to draw any labels on users
    XnBool g_bPrintID;
    /// @brief True when we want to draw full labels 
    ///     
    /// If false and g_bPrintID is true we will only draw the user id.
    XnBool g_bPrintState;

    XnBool g_bPause; ///< @brief True when pausing (i.e. UserTracker not updated).

    SampleManager *m_pSample;
    /// @}
};

#endif // XNV_SCENE_DRAWER_H_
