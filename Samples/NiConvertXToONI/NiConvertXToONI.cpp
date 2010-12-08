/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




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
		printf("usage: %s <inputFile> <outputFile>\n", argv[0]);
		return -1;
	}

	const char* strInputFile = argv[1];
	const char* strOutputFile = argv[2];

	Context context;
	nRetVal = context.Init();
	CHECK_RC(nRetVal, "Init");

	// open input file
	nRetVal = context.OpenFileRecording(strInputFile);
	CHECK_RC(nRetVal, "Open input file");

	Player player;
	nRetVal = context.FindExistingNode(XN_NODE_TYPE_PLAYER, player);
	CHECK_RC(nRetVal, "Get player node");

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

		nRetVal = recorder.AddNodeToRecording(node);
		CHECK_RC(nRetVal, "Add node to recording");
	}

	nRetVal = player.SetRepeat(FALSE);
	XN_IS_STATUS_OK(nRetVal);

	int n = 0;

	while ((nRetVal = context.WaitAnyUpdateAll()) != XN_STATUS_EOF)
	{
		CHECK_RC(nRetVal, "Read next frame");
		printf("Recording: %u\r", n++);
	}

	context.Shutdown();

	return 0;
}
