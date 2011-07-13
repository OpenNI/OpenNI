/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <XnCodecIDs.h>
#include <XnLog.h>

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

int main(int argc, char* argv[])
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (argc < 3)
	{
		printf("usage: %s <inputFile> <outputFile> [nodeType] [startFrame] [endFrame]\n", argv[0]);
		return -1;
	}

	const char* strInputFile = argv[1];
	const char* strOutputFile = argv[2];
	const char* strNodeType = NULL;
	XnUInt32 nStartFrame = 1;
	XnUInt32 nEndFrame = XN_MAX_UINT32;
	XnProductionNodeType seekNodeType = XN_NODE_TYPE_INVALID;

	if (argc >= 4)
	{
		strNodeType = argv[3];
		nRetVal = xnProductionNodeTypeFromString(strNodeType, &seekNodeType);
		if (nRetVal != XN_STATUS_OK)
		{
			printf("Bad node type specified: %s\n", strNodeType);
			return nRetVal;
		}

		if (argc >= 5)
		{
			nStartFrame = atoi(argv[4]);
			if (argc >= 6)
			{
				nEndFrame = atoi(argv[5]);
			}
		}
	}

	Context context;
	nRetVal = context.Init();
	CHECK_RC(nRetVal, "Init");

	// open input file
	Player player;
	nRetVal = context.OpenFileRecording(strInputFile, player);
	CHECK_RC(nRetVal, "Open input file");

	nRetVal = player.SetPlaybackSpeed(XN_PLAYBACK_SPEED_FASTEST);
	CHECK_RC(nRetVal, "Setting playback speed");

	// get the list of all created nodes
	NodeInfoList nodes;
	nRetVal = player.EnumerateNodes(nodes);
	CHECK_RC(nRetVal, "Enumerate nodes");
	
	// create recorder
	Recorder recorder;
	nRetVal = recorder.Create(context);
	CHECK_RC(nRetVal, "Create recorder");

	nRetVal = recorder.SetDestination(XN_RECORD_MEDIUM_FILE, strOutputFile);
	CHECK_RC(nRetVal, "Set recorder destination file");

	ProductionNode seekNode;

	// add all nodes to recorder
	for (NodeInfoList::Iterator it = nodes.Begin(); it != nodes.End(); ++it)
	{
		NodeInfo nodeInfo = *it;

		// NOTE: for now, ignore audio
		if (nodeInfo.GetDescription().Type == XN_NODE_TYPE_AUDIO)
		{
			continue;
		}

		ProductionNode node;
		nRetVal = nodeInfo.GetInstance(node);
		CHECK_RC(nRetVal, "Get instance");

		if (seekNodeType == XN_NODE_TYPE_INVALID)
		{
			//No node type specified - record all nodes.
			nRetVal = recorder.AddNodeToRecording(node);
			CHECK_RC(nRetVal, "Add node to recording");
		}
		else if (seekNodeType == nodeInfo.GetDescription().Type)
		{
			//If node type is specified, we only record nodes of that type.
			nRetVal = player.SeekToFrame(node.GetName(), nStartFrame, XN_PLAYER_SEEK_SET);
			CHECK_RC(nRetVal, "Seek player to frame");
			nRetVal = recorder.AddNodeToRecording(node);
			CHECK_RC(nRetVal, "Add node to recording");
		}
	}

	nRetVal = player.SetRepeat(FALSE);
	XN_IS_STATUS_OK(nRetVal);

	int nFrame = 0;

	while ((nRetVal = context.WaitAnyUpdateAll()) != XN_STATUS_EOF)
	{
		CHECK_RC(nRetVal, "Read next frame");
		printf("Recording: %u\r", nFrame++);
		if ((seekNodeType != XN_NODE_TYPE_INVALID) && (nFrame == nEndFrame))
		{
			break;			
		}
	}

	player.Release();
	context.Release();

	return 0;
}
