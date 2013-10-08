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
#include <XnOpenNI.h>
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <XnPropNames.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define SAMPLE_XML_PATH "../../../../Data/SamplesConfig.xml"

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define CHECK_RC(rc, what)											\
	if (rc != XN_STATUS_OK)											\
	{																\
		printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
		return rc;													\
	}

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

using namespace xn;

void transformDepthMD(DepthMetaData& depthMD)
{
	DepthMap& depthMap = depthMD.WritableDepthMap();
	for (XnUInt32 y = 0; y < depthMap.YRes(); y++)
	{
		for (XnUInt32 x = 0; x < depthMap.XRes(); x++)
		{
			//Punch vertical cut lines in the depth image
			if ((x % 2) == 0)
			{
				depthMap(x, y) = 0;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = xnLogInitFromXmlFile(SAMPLE_XML_PATH);
	if (nRetVal != XN_STATUS_OK)
	{
		printf("Log couldn't be opened: %s. Running without log", xnGetStatusString(nRetVal));
	}

	if (argc < 3)
	{
		printf("usage: %s <inputFile> <outputFile>\n", argv[0]);
		return -1;
	}

	const char* strInputFile = argv[1];
	const char* strOutputFile = argv[2];

	Context context;
	nRetVal = context.Init();
	CHECK_RC(nRetVal, "Init");

	// open input file
	Player player;
	nRetVal = context.OpenFileRecording(strInputFile, player);
	CHECK_RC(nRetVal, "Open input file");

	// Get depth node from recording
	DepthGenerator depth;
	nRetVal = context.FindExistingNode(XN_NODE_TYPE_DEPTH, depth);
	CHECK_RC(nRetVal, "Find depth generator");

	// Create mock node based on depth node from recording
	MockDepthGenerator mockDepth;
	nRetVal = mockDepth.CreateBasedOn(depth);
	CHECK_RC(nRetVal, "Create mock depth node");

	// create recorder
	Recorder recorder;

	nRetVal = recorder.Create(context);
	CHECK_RC(nRetVal, "Create recorder");

	nRetVal = recorder.SetDestination(XN_RECORD_MEDIUM_FILE, strOutputFile);
	CHECK_RC(nRetVal, "Set recorder destination file");

	// add depth node to recorder
	nRetVal = recorder.AddNodeToRecording(mockDepth);
	CHECK_RC(nRetVal, "Add node to recording");

	nRetVal = player.SetRepeat(FALSE);
	XN_IS_STATUS_OK(nRetVal);

	XnUInt32 nNumFrames = 0;

	nRetVal = player.GetNumFrames(depth.GetName(), nNumFrames);
	CHECK_RC(nRetVal, "Get player number of frames");

	DepthMetaData depthMD;

	while ((nRetVal = depth.WaitAndUpdateData()) != XN_STATUS_EOF)
	{
		CHECK_RC(nRetVal, "Read next frame");
		
		// Get depth meta data
		depth.GetMetaData(depthMD);

		//-----------------------------------------------//
		// Transform depth! This is the interesting part //
		//-----------------------------------------------//
		
		/* Enable the depth data to be modified. This is done implicitly by depthMD.WritableDepthMap(),
		   but we're calling it just to be clear. */
		nRetVal = depthMD.MakeDataWritable();
		CHECK_RC(nRetVal, "Make depth data writable");

		transformDepthMD(depthMD);

		// Pass the transformed data to the mock depth generator
		nRetVal = mockDepth.SetData(depthMD);
		CHECK_RC(nRetVal, "Set mock node new data");

		/* We need to call recorder.Record explicitly because we're not using WaitAndUpdateAll(). */
		nRetVal = recorder.Record();
		CHECK_RC(nRetVal, "Record");

		printf("Recorded: frame %u out of %u\r", depthMD.FrameID(), nNumFrames);
	}

	printf("\n");

	return 0;
}
