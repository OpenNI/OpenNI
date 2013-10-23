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

#include "UserTracker.h"

// the filename for saving/loading calibration.
#define XN_CALIBRATION_FILE_NAME "UserCalibration.bin"

#define SAMPLE_XML_PATH "../../../../Data/SamplesConfig.xml"


float *UserTracker::s_pDepthHist=NULL;

// initialization of different colors for different users
XnFloat UserTracker::s_Colors[][3] =
{
    {0,1,1},
    {0,0,1},
    {0,1,0},
    {1,1,0},
    {1,0,0},
    {1,.5,0},
    {.5,1,0},
    {0,.5,1},
    {.5,0,1},
    {1,1,.5},
    {1,1,1}
};
XnUInt32 UserTracker::s_nColors = 10;


#define CHECK_RC(nRetVal, what)										\
    if (nRetVal != XN_STATUS_OK)									\
{																\
    printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));\
    return;												\
}


UserTracker::UserTracker(int argc, char **argv, XnUInt64 timeSpanForExitPose) : m_bValid(FALSE), 
                                                                                m_timeSpanForExitPose(timeSpanForExitPose),
                                                                                m_pExitPoseDetector(NULL)
{
    m_bRecord=FALSE;
    XnStatus nRetVal = XN_STATUS_OK;

    XnBool OpenedRecording=FALSE;

    if(argc>1)
    {
        for(int i=1; i<argc-1; i++)
        {
            if(xnOSStrCmp(argv[i],"-RecordingFilename")==0)
            {
                // initialize the context from scratch
                nRetVal = m_Context.Init();
                CHECK_RC(nRetVal, "Context Initialization");
                // try to open the recording as the source for sensor data
                nRetVal = m_Context.OpenFileRecording(argv[i+1], m_Player);
                CHECK_RC(nRetVal, "Couldn't open recording with command line argument");
                OpenedRecording=TRUE;
                break; // we only support one recording at a time..
            }
        }
    }
    if(OpenedRecording==FALSE)
    {
        xn::EnumerationErrors errors;
        nRetVal = m_Context.InitFromXmlFile(SAMPLE_XML_PATH, m_ScriptNode, &errors);
        if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
        {
            XnChar strError[1024];
            errors.ToString(strError, 1024);
            printf("%s\n", strError);
            return;
        }
        CHECK_RC(nRetVal, "Open XML failed");
    }

    nRetVal = m_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, m_DepthGenerator);
    if (nRetVal != XN_STATUS_OK)
    {
        printf("No depth generator found. Using a default one...");
        xn::MockDepthGenerator mockDepth;
        nRetVal = mockDepth.Create(m_Context);
        CHECK_RC(nRetVal, "Create mock depth");

        // set some defaults
        XnMapOutputMode defaultMode;
        defaultMode.nXRes = 320;
        defaultMode.nYRes = 240;
        defaultMode.nFPS = 30;
        nRetVal = mockDepth.SetMapOutputMode(defaultMode);
        CHECK_RC(nRetVal, "set default mode");

        // set FOV
        XnFieldOfView fov;
        fov.fHFOV = 1.0225999419141749;
        fov.fVFOV = 0.79661567681716894;
        nRetVal = mockDepth.SetGeneralProperty(XN_PROP_FIELD_OF_VIEW, sizeof(fov), &fov);
        CHECK_RC(nRetVal, "set FOV");

        XnUInt32 nDataSize = defaultMode.nXRes * defaultMode.nYRes * sizeof(XnDepthPixel);
        XnDepthPixel* pData = (XnDepthPixel*)xnOSCallocAligned(nDataSize, 1, XN_DEFAULT_MEM_ALIGN);

        nRetVal = mockDepth.SetData(1, 0, nDataSize, pData);
        CHECK_RC(nRetVal, "set empty depth map");

        m_DepthGenerator = mockDepth;
    }

    nRetVal = m_Context.FindExistingNode(XN_NODE_TYPE_USER, m_UserGenerator);
    if (nRetVal != XN_STATUS_OK)
    {
        nRetVal = m_UserGenerator.Create(m_Context);
        CHECK_RC(nRetVal, "Find user generator");
    }

    if (!m_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
    {
        printf("Supplied user generator doesn't support skeleton\n");
        return;
    }
    m_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
    nRetVal = m_Context.StartGeneratingAll();
    CHECK_RC(nRetVal, "StartGenerating");

    m_pExitPoseDetector=XN_NEW(ExitPoseDetector,m_UserGenerator);
    if(!(m_pExitPoseDetector->Valid()))
    {
        printf("Exit pose is not supported by user node. Therefore will continue to run without support for exit pose\n");
        XN_DELETE(m_pExitPoseDetector);
        m_pExitPoseDetector=NULL;
    }
    m_bValid=TRUE;
}


XnStatus UserTracker::InitUserSelection(UserSelector *pUserSelector,TrackingInitializer *pTrackingInitializer)
{
    m_pUserSelector=pUserSelector;
    m_pTrackingInitializer=pTrackingInitializer;
    return XN_STATUS_OK;
}


void UserTracker::CalcHistogram(const XnDepthPixel* pDepth, XnUInt16 xRes,XnUInt16 yRes)
{
	XnDepthPixel zRes = m_DepthGenerator.GetDeviceMaxDepth() + 1;
    if(s_pDepthHist==NULL)
        s_pDepthHist=XN_NEW_ARR(float,zRes); // initialize the histogram's buffer


    XnUInt32 nNumberOfPoints = 0; // will hold the number of points in the histogram
    XnDepthPixel nValue; // will hold temporary pixel values. 

    xnOSMemSet(s_pDepthHist, 0, zRes*sizeof(float)); // clear everything, we start from scratch

    // calculate the basic histogram (i.e. the buffer in position X will hold the number of pixels
    // in the depth map which have a value of X.
    // NOTE: a value of 0 is not counted (i.e. m_pDepthHist[0] will always be 0) and we assume
    // MAX_DEPTH is large enough so that we never have a value larger than it.
    for (XnUInt16 nY=0; nY<yRes; nY++)
    {
        for (XnUInt16 nX=0; nX<xRes; nX++)
        {
            nValue = *pDepth;

            if (nValue != 0)
            {
                s_pDepthHist[nValue]++;
                nNumberOfPoints++;
            }
            pDepth++;
        }
    }

    if(nNumberOfPoints==0)
        return; // nothing to do, there are no values.
    // turn the histogram to a cumulative histogram
    for (XnUInt16 nIndex=1; nIndex<zRes; nIndex++)
    {
        s_pDepthHist[nIndex] += s_pDepthHist[nIndex-1];
    }
    // normalize the values to a value between 0 and 256. This is the color we want to see elements
    // of this value as (multiplier).

    for (XnUInt16 nIndex=1; nIndex<zRes; nIndex++)
    {
        s_pDepthHist[nIndex] = (256 * (1.0f - (s_pDepthHist[nIndex] / nNumberOfPoints)));
    }
}

void UserTracker::FillTexture(unsigned char* pTexBuf, XnUInt16 nTexWidth, XnUInt16 /*nTexHeight*/, XnBool bDrawBackground)
{
    // get the size of the depth map and its pixels.
    XnUInt16 xRes,yRes;
    GetImageRes(xRes, yRes);
    const XnDepthPixel* pDepth = GetDepthData(); // holds the depth map, i.e. the depth for each pixel

    CalcHistogram(pDepth, xRes, yRes);

    XnDepthPixel nValue; // will hold temporary pixel values. 

    const XnLabel* pLabels = GetUsersPixelsData(); // holds a label map, i.e. the label (user ID) for each pixel

    // Prepare the texture map, i.e. go over all relevant elements and set their value based
    // on the depth and user.
    // NOTE: we go over the original map and the assumption is that the texture size is equal or larger
    // to the depth map resolution and that anything larger (e.g. because we increase to the closest,
    // larger power of two) is not changed (therefore will remain whatever color was set before which
    // in the initialization is probably 0). 
    for (XnUInt16 nY=0; nY<yRes; nY++)
    {
        for (XnUInt16 nX=0; nX < xRes; nX++)
        {
            // init to 0 (just in case we don't have better data.
            pTexBuf[0] = 0;
            pTexBuf[1] = 0;
            pTexBuf[2] = 0;
            // if pLabels is 0 it is background. Therefore we only set the color of the background
            // if bDrawBackground is true.
            if (bDrawBackground || *pLabels != 0)
            {
                nValue = *pDepth; // the depth of the current pixel
                XnLabel label = *pLabels; // the label of the current pixel
                XnUInt32 nColorID = label % s_nColors; // the color for the current label
                if (label == 0)
                {
                    nColorID = s_nColors; // special background color
                }

                if (nValue != 0)
                {
                    XnFloat newValue = s_pDepthHist[nValue]; // translate to the multiplier from the histogram

                    pTexBuf[0] = (unsigned char)(newValue * s_Colors[nColorID][0]); 
                    pTexBuf[1] = (unsigned char)(newValue * s_Colors[nColorID][1]);
                    pTexBuf[2] = (unsigned char)(newValue * s_Colors[nColorID][2]);
                }
            }

            pDepth++;
            pLabels++;
            pTexBuf+=3; // each element is RGB so we move 3 at a time.
        }

        pTexBuf += (nTexWidth - xRes) *3; // move to the next line
    }
}

XnStatus UserTracker::GetUserColor(XnUserID nUserId, XnFloat* userColor)
{
    XnUInt32 nColorID = nUserId % s_nColors; // the color for the user
    userColor[0]=s_Colors[nColorID][0];
    userColor[1]=s_Colors[nColorID][1];
    userColor[2]=s_Colors[nColorID][2];
    return XN_STATUS_OK;
}

XnFloat UserTracker::GetExitPoseState(XnUserID nUserId)
{
    if(m_pExitPoseDetector==NULL)
    {
        return 0.0f; // no exit pose detector.
    }
    XnUInt64 tmpTime=m_pExitPoseDetector->GetExitPoseTimeStamp(nUserId);
    if(tmpTime==0)
    {
        return 0.0f;
    }
    tmpTime=m_UserGenerator.GetTimestamp()-tmpTime;
    if(tmpTime>=m_timeSpanForExitPose)
    {
        return 1.0f;
    }
    return ((XnFloat)tmpTime)/((XnFloat)m_timeSpanForExitPose);
}


XnStatus UserTracker::GetUserStringPos(XnUserID nUserId, XnPoint3D &com, char *strLabel, XnUInt32 maxStrLen)
{
    XnStatus res;
    res=m_UserGenerator.GetCoM(nUserId, com);
    if(res!=XN_STATUS_OK)
        return res;
    res=m_DepthGenerator.ConvertRealWorldToProjective(1, &com, &com);
    if(res!=XN_STATUS_OK)
        return res;

    m_pUserSelector->GetUserLabel(nUserId,strLabel,maxStrLen);
    return XN_STATUS_OK;
}

XnStatus UserTracker::GetLimbs(XnUserID nUserID, XnPoint3D *pLimbs,XnFloat *pConfidence, XnUInt16 &numLimbs)
{
    // following is a static array of the limbs to draw. Each pair represents a line which
    // should be drawn
    static XnSkeletonJoint jointsToPrint[][2] = 
    {
        { XN_SKEL_HEAD, XN_SKEL_NECK },
        { XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER },
        { XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW },
        { XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND },
        { XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER },
        { XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW },
        { XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND },
        { XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO },
        { XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO },
        { XN_SKEL_TORSO, XN_SKEL_LEFT_HIP },
        { XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE },
        { XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT },
        { XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP },
        { XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE },
        { XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT },
        { XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP },
    };
    static XnUInt16 MaxNumLimbs=16;

	if (!m_UserGenerator.GetSkeletonCap().IsTracking(nUserID))
	{
		return XN_STATUS_NO_SUCH_USER;
	}

    XnUInt16 limbCount=0;
    XnSkeletonJointPosition joint1, joint2;
    for(XnUInt16 i=0; i<MaxNumLimbs; i++)
    {
        if(limbCount>=numLimbs)
            break; // we can't put any new ones

        if(m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(nUserID, jointsToPrint[i][0], joint1)!=XN_STATUS_OK)
        {
            continue; // bad joint
        }
        if(m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(nUserID, jointsToPrint[i][1], joint2)!=XN_STATUS_OK)
        {
            continue; // bad joint
        }

        pConfidence[limbCount]=joint1.fConfidence;
        if(pConfidence[limbCount]>joint2.fConfidence)
        {
            pConfidence[limbCount]=joint2.fConfidence;
        }
        pLimbs[limbCount*2] = joint1.position;
        pLimbs[(limbCount*2)+1] = joint2.position;
        limbCount++;
    }
    if(limbCount>0)
        m_DepthGenerator.ConvertRealWorldToProjective(limbCount*2, pLimbs, pLimbs);
    numLimbs=limbCount;
    return XN_STATUS_OK;
}



UserTracker::~UserTracker()
{
    if(m_bValid==TRUE)
        CleanUp();
}

void UserTracker::CleanUp()
{
    if(m_pExitPoseDetector!=NULL)
    {
        XN_DELETE(m_pExitPoseDetector);
        m_pExitPoseDetector=NULL;
    }
    m_ScriptNode.Release();
    m_DepthGenerator.Release();
    m_UserGenerator.Release();
    m_Player.Release();
    m_Context.Release();
    m_bValid=FALSE;
}


void UserTracker::UpdateFrame()
{
    // Read next available data
    m_Context.WaitOneUpdateAll(m_UserGenerator);
    m_pUserSelector->UpdateFrame();
    // now we need to update the users for tracking the exit pose.
}


void UserTracker::GetImageRes(XnUInt16 &xRes, XnUInt16 &yRes)
{
    xn::DepthMetaData depthMD;
    m_DepthGenerator.GetMetaData(depthMD);
    xRes = (XnUInt16)depthMD.XRes();
    yRes = (XnUInt16)depthMD.YRes();
}

const XnDepthPixel* UserTracker::GetDepthData()
{
    xn::DepthMetaData depthMD;
    m_DepthGenerator.GetMetaData(depthMD);
    return depthMD.Data();
}
const XnLabel* UserTracker::GetUsersPixelsData()
{
    xn::SceneMetaData sceneMD;
    m_UserGenerator.GetUserPixels(0, sceneMD);
    return sceneMD.Data();
}


