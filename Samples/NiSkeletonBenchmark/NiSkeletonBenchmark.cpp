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
#include <XnCppWrapper.h>
#include<sys/time.h>
#include<sys/resource.h>
#include <XnFPSCalculator.h>
//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define SAMPLE_XML_PATH "../../Config/SamplesConfig.xml"
#define SAMPLE_XML_PATH_LOCAL "SamplesConfig.xml"
//printf tweaks
#define BRIGHT 1
#define NO_BRIGHT 0
#define RED 31
#define WHITE 37
#define CYAN 36
#define MAGENTA 35
#define BLUE 34
#define BG_BLACK 40
//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
xn::Context g_Context;
xn::ScriptNode g_scriptNode;
xn::DepthGenerator g_DepthGenerator;
xn::UserGenerator g_UserGenerator;
xn::Player g_Player;
XnUInt32 TotalFrames;
XnBool g_bNeedPose = TRUE;
XnChar g_strPose[20] = "";
XnUInt8 numOfUser;

#define MAX_NUM_USERS 15
//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnBool fileExists(const char *fn)
{
	XnBool exists;
	xnOSDoesFileExist(fn, &exists);
	return exists;
}

// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    XnUserID aUsers[MAX_NUM_USERS];
    XnUInt16 nUsers;
	g_UserGenerator.GetUsers(aUsers, nUsers);
    printf("%d New User %d\n", epochTime, nId);
    // New user found
    //validate the number of users
    XnUInt16 usersTracked=0;
    for(XnUInt16 i=0; i<nUsers; i++)
    {
    	if(g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])==TRUE)
    		usersTracked++;
    }
    if(usersTracked >= numOfUser)
    {
    	printf("%d users are currently being tracked, no more users allowed\n",numOfUser);
    	return;
    }
    if (g_bNeedPose)
    {
        g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
    }
    else
    {
        g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
    }
}
// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d Lost user %d\n", epochTime, nId);	
}
// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d Pose %s detected for user %d\n", epochTime, strPose, nId);
    g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
    g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}
// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d Calibration started for user %d\n", epochTime, nId);
}

void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    if (eStatus == XN_CALIBRATION_STATUS_OK)
    {
        // Calibration succeeded
        printf("%d Calibration complete, start tracking user %d\n", epochTime, nId);		
        g_UserGenerator.GetSkeletonCap().StartTracking(nId);
    }
    else
    {
        // Calibration failed
        printf("%d Calibration failed for user %d\n", epochTime, nId);
        if(eStatus==XN_CALIBRATION_STATUS_MANUAL_ABORT)
        {
            printf("Manual abort occured, stop attempting to calibrate!");
            return;
        }
        if (g_bNeedPose)
        {
            g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
        }
        else
        {
            g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
        }
    }
}


#define CHECK_RC(nRetVal, what)					    \
    if (nRetVal != XN_STATUS_OK)				    \
{								    \
    printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));    \
    return nRetVal;						    \
}
void PrintHelpHeader(const char* cmd)
{
		printf("Usage:\n");
		printf("%s <Number of Users [1-2] (default=1)>\n",cmd);
}
int main(int argc, char **argv)
{
    XnStatus nRetVal = XN_STATUS_OK;
    xn::EnumerationErrors errors;
    TotalFrames = 0;
    static struct rusage ru;
    long double t;
    double w;
    struct timeval timStart;
    struct timeval timEnd;
    struct timeval Wall;
    bool Sample = true;
    XnFPSData xnFPS;
    XnUserID aUsers[MAX_NUM_USERS];
	XnUInt16 nUsers;
	XnSkeletonJointTransformation torsoJoint;

    if (argc > 1)
	{

    	//parse the cmd line
    	if (strcasecmp(argv[1],"--help") == 0)
    	{
    		PrintHelpHeader(argv[0]);
    		return 1;
    	}
    	numOfUser = atoi(argv[1]);
    	if(numOfUser == 0)
    	{
    		PrintHelpHeader(argv[0]);
    		return 1;
    	}
    	else if(numOfUser > 2)
    	{
    		printf("Maximal Users allowed is 2\n");
    		return 1;
    	}

	}
    else
    {
    	numOfUser = 4;
    }

    const char *fn = NULL;
    if    (fileExists(SAMPLE_XML_PATH)) fn = SAMPLE_XML_PATH;
    else if (fileExists(SAMPLE_XML_PATH_LOCAL)) fn = SAMPLE_XML_PATH_LOCAL;
    else {
        printf("Could not find '%s' nor '%s'. Aborting.\n" , SAMPLE_XML_PATH, SAMPLE_XML_PATH_LOCAL);
        return XN_STATUS_ERROR;
    }
    printf("Reading config from: '%s'\n", fn);

    nRetVal = g_Context.InitFromXmlFile(fn, g_scriptNode, &errors);
    if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
    {
        XnChar strError[1024];
        errors.ToString(strError, 1024);
        printf("%s\n", strError);
        return (nRetVal);
    }
    else if (nRetVal != XN_STATUS_OK)
    {
        printf("Open failed: %s\n", xnGetStatusString(nRetVal));
        return (nRetVal);
    }

    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
    CHECK_RC(nRetVal,"No depth");

#if (XN_PLATFORM != XN_PLATFORM_MACOSX && XN_PLATFORM != XN_PLATFORM_ANDROID_ARM)
    //we want out benchmark application will be running only on one CPU core
    cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(1,&mask);
	sched_setaffinity(0,sizeof(mask),&mask);
#endif	
	//initialize the FPS calculator
	nRetVal = xnFPSInit(&xnFPS, 90);
	CHECK_RC(nRetVal, "FPS Init");
	//ensure the User generator exists
    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
    if (nRetVal != XN_STATUS_OK)
    {
        nRetVal = g_UserGenerator.Create(g_Context);
        CHECK_RC(nRetVal, "Find user generator");
    }
    //register to generators callbacks
    XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected;
    if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
    {
        printf("Supplied user generator doesn't support skeleton\n");
        return 1;
    }
    nRetVal = g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
    CHECK_RC(nRetVal, "Register to user callbacks");
    nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, NULL, hCalibrationStart);
    CHECK_RC(nRetVal, "Register to calibration start");
    nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, NULL, hCalibrationComplete);
    CHECK_RC(nRetVal, "Register to calibration complete");
    //ensure calibration
    if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
    {
        g_bNeedPose = TRUE;
        if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
        {
            printf("Pose required, but not supported\n");
            return 1;
        }
        nRetVal = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, NULL, hPoseDetected);
        CHECK_RC(nRetVal, "Register to Pose Detected");
        g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
    }
    //set skeleton profile (all joints)
    g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
    //start to generate
    nRetVal = g_Context.StartGeneratingAll();
    CHECK_RC(nRetVal, "StartGenerating");



    printf("%c[%d;%d;%dmPrimeSense Skeleton Benchmark Application\n ", 0x1B, BRIGHT,BLUE,BG_BLACK);
    printf("%c[%d;%d;%dmSet Maximal users to %d ", 0x1B, BRIGHT,BLUE,BG_BLACK,numOfUser);
	printf("%c[%dm\n", 0x1B, 0);
    printf("Starting to run\n");
    if(g_bNeedPose)
    {
        printf("Assume calibration pose\n");
    }

    XnUInt32 epochTime = 0;
	XnUInt64 totalLatency = 0;
	XnUInt32 nFrame = 0;

    //each 30 frames (1 second) we start the CPU resources usages
    while (!xnOSWasKeyboardHit())
    {
    	if (Sample)
    	{	//get the beginning sample of CPU resources
    		getrusage(RUSAGE_SELF, &ru);
    		timStart=ru.ru_utime;
    		t=(double)timStart.tv_sec * 1000000.0 + (double)timStart.tv_usec \
    		   + (double)ru.ru_stime.tv_sec*1000000.0+(double)ru.ru_stime.tv_usec;
    		//get the wall clock time
    		gettimeofday(&Wall,NULL);

    		w=(double)Wall.tv_sec * 1000000.0 + (double)Wall.tv_usec;
    		Sample = false;
    	}

		g_DepthGenerator.WaitAndUpdateData();

		XnUInt64 before;
		xnOSGetHighResTimeStamp(&before);
		g_UserGenerator.WaitAndUpdateData();
		XnUInt64 end;
		xnOSGetHighResTimeStamp(&end);
		totalLatency += (end - before);

    	xnFPSMarkFrame(&xnFPS);
        // print the torso information for the first user already tracking every 1 second to prevent CPU of printf
        TotalFrames++;
        if(TotalFrames % 30 == 0)
        {
			nUsers=MAX_NUM_USERS;
			g_UserGenerator.GetUsers(aUsers, nUsers);
			int numTracked=0;
			int userToPrint=-1;
			for(XnUInt16 i=0; i<nUsers; i++)
			{
				if(g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])==FALSE)
					continue;

				g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i],XN_SKEL_TORSO,torsoJoint);
				printf("User %d Located At distance of %6.2f mm from the sensor\n",aUsers[i],torsoJoint.position.position.Z);
			 }
			//get the finish sample of the CPU resources
			getrusage(RUSAGE_SELF, &ru);
			timEnd=ru.ru_utime;
			t = (double)timEnd.tv_sec * 1000000.0 + (double)timEnd.tv_usec \
				+	(double)ru.ru_stime.tv_sec*1000000.0+(double)ru.ru_stime.tv_usec	- t;
			//get the wall clock
			gettimeofday(&Wall,NULL);

			w = (double)Wall.tv_sec * 1000000.0 + (double)Wall.tv_usec - w;

			XnDouble fps=xnFPSCalc(&xnFPS);
			//print stuff.
			printf("%c[%d;%d;%dmCPU Utilization=%3.2f%%\t", 0x1B, BRIGHT,RED,BG_BLACK,(double)(100*t/w));
			printf("%c[%d;%d;%dmFPS=%3.2f ", 0x1B, BRIGHT,RED,BG_BLACK,fps);
			printf("%c[%d;%d;%dmSkeleton Latency=%3.2f ms", 0x1B, BRIGHT,RED,BG_BLACK,totalLatency/3e4);
			printf("%c[%dm\n", 0x1B, 0);
			Sample= true;
			totalLatency = 0;
		}
        
    }
    g_scriptNode.Release();
    g_DepthGenerator.Release();
    g_UserGenerator.Release();
    g_Context.Release();

}
