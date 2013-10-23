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
#include "SceneDrawer.h"

//---------------------------------------------------------------------------
// definitions
//---------------------------------------------------------------------------

#ifdef USE_GLES
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;
#endif

// window size (X,Y) of the OpenGL portion
#define GL_WIN_SIZE_X 720
#define GL_WIN_SIZE_Y 480

// Maximum number of limbs (lines) we will draw as a skeleton.
#define MAX_LIMBS 16

//---------------------------------------------------------------------------
// Initialize static members
//---------------------------------------------------------------------------

SceneDrawer *SceneDrawer::m_pSingleton=NULL;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------



void SceneDrawer::DrawScene(UserTracker *pUserTrackerObj,int argc, char **argv,SampleManager *pSample, XnBool bShowLowConfidence)
{
    m_pUserTrackerObj=pUserTrackerObj;
    m_pSample=pSample;
    m_bShowLowConfidence=bShowLowConfidence;

#ifndef USE_GLES
    glInit(&argc, argv);
#endif

    InitTexture();
#ifndef USE_GLES
    glutMainLoop();
#else
    if (!opengles_init(GL_WIN_SIZE_X, GL_WIN_SIZE_Y, &display, &surface, &context))
    {
        printf("Error initializing opengles\n");
        ExitSample(EXIT_FAILURE);
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    while (1)
    {
        glutDisplay();
        eglSwapBuffers(display, surface);
    }
    // we should never reach here! we have a while(1) above!
    ExitSample(EXIT_SUCCESS); 

#endif
}


SceneDrawer *SceneDrawer::GetInstance()
{
    if(m_pSingleton==NULL)
        m_pSingleton=new SceneDrawer();
    return m_pSingleton;
}

SceneDrawer::SceneDrawer()
{
    // drawing defaults
    g_bDrawBackground = TRUE;
    g_bDrawPixels = TRUE;
    g_bDrawSkeleton = TRUE;
    g_bPrintID = TRUE;
    g_bPrintState = TRUE;
    g_bPause=FALSE;

    // buffer initialization
    pLimbsPosArr=XN_NEW_ARR(XnPoint3D,(MAX_LIMBS*2));
    pConfidenceArr=XN_NEW_ARR(XnFloat,MAX_LIMBS);

    // following are dummy assignments which will be overriden when DrawScene is called
    // (either in DrawScene itself or in InitTexture
    m_pUserTrackerObj=NULL; 
    depthTexID=0;
    pDepthTexBuf=NULL;
    texWidth=0;
    texHeight=0;
    for(int i=0; i<8; i++)
        texcoords[i] = 0;

}



void SceneDrawer::DrawDepthMapTexture()
{
    XnUInt16 g_nXRes; 
    XnUInt16 g_nYRes; 
    m_pUserTrackerObj->GetImageRes(g_nXRes,g_nYRes);

    if (g_bDrawPixels)
    {
        m_pUserTrackerObj->FillTexture(pDepthTexBuf,texWidth,texHeight,g_bDrawBackground);
    }
    else
    {
        xnOSMemSet(pDepthTexBuf, 0, 3*2*g_nXRes*g_nYRes); // makes the texture empty.
    }

    // makes sure we draw the relevant texture
    glBindTexture(GL_TEXTURE_2D, depthTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pDepthTexBuf);

    // Display the OpenGL texture map
    glColor4f(0.75,0.75,0.75,1);

    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

    GLfloat verts[8] = { g_nXRes, g_nYRes, g_nXRes, 0, 0, 0, 0, g_nYRes };
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    //TODO: Maybe glFinish needed here instead - if there's some bad graphics crap
    glFlush();
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);
}

#define MAX_USER_LABEL_LEN 50
#define MAX_ID_LABEL_LEN 120
void SceneDrawer::DrawLabels(XnUserID nUserId)
{
#ifndef USE_GLES
    char strUserLabel[MAX_USER_LABEL_LEN] = "";
    char strOutputLabel[MAX_ID_LABEL_LEN] = "";
    XnFloat color[3];

    XnPoint3D com;
    xnOSMemSet(strOutputLabel, 0, sizeof(strOutputLabel));
    XnStatus res=m_pUserTrackerObj->GetUserColor(nUserId,color);
    XnFloat userExitPose=m_pUserTrackerObj->GetExitPoseState(nUserId)*100.0f;
    if(res!=XN_STATUS_OK)
        return; // bad user!
    xnOSMemSet(strUserLabel, 0, sizeof(strUserLabel));
    res=m_pUserTrackerObj->GetUserStringPos(nUserId,com,strUserLabel,MAX_USER_LABEL_LEN-1);
    if(res!=XN_STATUS_OK)
        return; // bad user!

    if (!g_bPrintState)
    {
        if(userExitPose>0)
        {
            sprintf(strOutputLabel, "%d - Exit wait %3.0f%% done.", nUserId,userExitPose);
        }
        else
        {
            sprintf(strOutputLabel, "%d", nUserId);
        }
        
    }
    else 
    {
        if(userExitPose>0)
        {
            sprintf(strOutputLabel, "%d - %s - Exit wait %3.0f%% done.", nUserId,strUserLabel,userExitPose);
        }
        else
        {
            sprintf(strOutputLabel, "%d - %s", nUserId,strUserLabel);
        }
        
    }

    glColor4f(1-color[0], 1-color[1], 1-color[2], 1);

    glRasterPos2i(com.X, com.Y);
    int len = (int)strlen(strOutputLabel);

    for(int c=0; c<len; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,strOutputLabel[c]);
    }
#endif
}


#define drawOneLine(x1,y1,x2,y2)  \
    glVertex2f ((x1),(y1)); glVertex2f ((x2),(y2)); ;

void SceneDrawer::DrawSkeleton(XnUserID nUserId)
{
    XnFloat color[3];

    XnUInt16 numLimbs=MAX_LIMBS;
    if(m_pUserTrackerObj->GetLimbs(nUserId,pLimbsPosArr,pConfidenceArr,numLimbs)!=XN_STATUS_OK)
        return; // no limbs to draw
    if(numLimbs==0)
        return; // no limbs to draw
    XnStatus res=m_pUserTrackerObj->GetUserColor(nUserId,color);
    if(res!=XN_STATUS_OK)
        return; // bad user!

    glColor4f(1-color[0], 1-color[1], 1-color[2], 1);
    for(XnUInt16 j=0; j<numLimbs; j++)
    {
#ifndef USE_GLES
        if(pConfidenceArr[j]<=0.5)
        {
            if(m_bShowLowConfidence==FALSE)
            {
                continue; // we simply do not show this limb...
            }
            glEnable(GL_LINE_STIPPLE);
            if(pConfidenceArr[j]==0.5)
            {
                glLineStipple(1,0x0101);
            }
            else
            {
                glLineStipple(1,0x00FF);
            }            
        }
        glBegin(GL_LINES);
        glVertex2f(pLimbsPosArr[j*2].X, pLimbsPosArr[j*2].Y);
        glVertex2f(pLimbsPosArr[(j*2)+1].X, pLimbsPosArr[(j*2)+1].Y);
        glEnd();
        if(pConfidenceArr[j]<=0.5)
        {
            glDisable(GL_LINE_STIPPLE);
        }
#else
        GLfloat verts[4] = {pLimbsPosArr[j*2].X, pLimbsPosArr[j*2].Y, pLimbsPosArr[(j*2)+1].X, pLimbsPosArr[(j*2)+1].Y};
        glVertexPointer(2, GL_FLOAT, 0, verts);
        glDrawArrays(GL_LINES, 0, 2);
        glFlush();
#endif
    }
#ifndef USE_GLES
   // glEnd();
#endif
}

void SceneDrawer::ExitSample(int exitCode)
{
#if USE_GLES
    opengles_shutdown(display, surface, context);
#endif
    m_pSample->Cleanup();
    exit(exitCode);
}



void SceneDrawer::InitTexture()
{
    XnUInt16 g_nXRes; 
    XnUInt16 g_nYRes; 
    m_pUserTrackerObj->GetImageRes(g_nXRes,g_nYRes);

    // get the width and height of the texture as the nearest power of two larger than the
    // x/y resolution respectively.
    texWidth = 2;
    while(texWidth < g_nXRes) texWidth<<=1;
    texHeight = 2;
    while(texHeight < g_nYRes) texHeight<<=1;

    // initialize the texture
    depthTexID = 0;
    glGenTextures(1,&depthTexID);
    pDepthTexBuf = new unsigned char[texWidth*texHeight*4];
    glBindTexture(GL_TEXTURE_2D,depthTexID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    memset(texcoords, 0, 8*sizeof(float));
    texcoords[0] = (float)g_nXRes/texWidth;
    texcoords[1] = (float)g_nYRes/texHeight;
    texcoords[2] = (float)g_nXRes/texWidth;
    texcoords[7] = (float)g_nYRes/texHeight;
}


void SceneDrawer::glutDisplay (void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup the OpenGL viewpoint
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    XnUInt16 g_nXRes; 
    XnUInt16 g_nYRes; 
    SceneDrawer *singleton=GetInstance();

    singleton->m_pUserTrackerObj->GetImageRes(g_nXRes,g_nYRes);

#ifndef USE_GLES
    glOrtho(0, g_nXRes, g_nYRes, 0, -1.0, 1.0);
#else
    glOrthof(0, g_nXRes, g_nYRes, 0, -1.0, 1.0);
#endif

    glDisable(GL_TEXTURE_2D);

    if(singleton->g_bPause==FALSE)
        singleton->m_pUserTrackerObj->UpdateFrame();
    if(singleton->m_pUserTrackerObj->GetExitPoseState(0)>=1.0f)
    {
        singleton->ExitSample(EXIT_SUCCESS);
    }
    // Process the data
    singleton->DrawDepthMapTexture();

    XnUserID aUsers[15];
    XnUInt16 nUsers = 15;
    xn::UserGenerator *pUserGenerator=singleton->m_pUserTrackerObj->GetUserGenerator();
    pUserGenerator->GetUsers(aUsers, nUsers);
    for (int i = 0; i < nUsers; ++i)
    {
        if (singleton->g_bPrintID)
        {
            singleton->DrawLabels(aUsers[i]);
        }
        if (singleton->g_bDrawSkeleton)
        {
            if(pUserGenerator->GetSkeletonCap().IsTracking(aUsers[i]))
            {
                singleton->DrawSkeleton(aUsers[i]);
            }
            
        }

    }

#ifndef USE_GLES
    glutSwapBuffers();
#endif
}




#ifndef USE_GLES
void SceneDrawer::glutIdle (void)
{
    // Display the frame
    glutPostRedisplay();
}



void SceneDrawer::glutKeyboard (unsigned char key, int /*x*/, int /*y*/)
{
    SceneDrawer *singleton=GetInstance();
    switch (key)
    {
    case 27:
        singleton->ExitSample(EXIT_SUCCESS); 
    case 'b':
        // Draw background?
        singleton->g_bDrawBackground = !singleton->g_bDrawBackground;
        break;
    case 'x':
        // Draw pixels at all?
        singleton->g_bDrawPixels = !singleton->g_bDrawPixels;
        break;
    case 's':
        // Draw Skeleton?
        singleton->g_bDrawSkeleton = !singleton->g_bDrawSkeleton;
        break;
    case 'i':
        // Print label?
        singleton->g_bPrintID = !singleton->g_bPrintID;
        break;
    case 'l':
        // Print ID & state as label, or only ID?
        singleton->g_bPrintState = !singleton->g_bPrintState;
        break;
    case'p':
        singleton->g_bPause=!singleton->g_bPause;
        break;
    }
}




void SceneDrawer::glInit (int * pargc, char ** argv)
{
    glutInit(pargc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
    glutCreateWindow ("User Selection Sample");
    //glutFullScreen();
    glutSetCursor(GLUT_CURSOR_NONE);

    glutKeyboardFunc(glutKeyboard);
    glutDisplayFunc(glutDisplay);
    glutIdleFunc(glutIdle);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}
#endif // USE_GLES

