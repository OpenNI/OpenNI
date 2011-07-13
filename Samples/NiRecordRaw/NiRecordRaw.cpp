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
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <xnPropNames.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define SAMPLE_XML_PATH "../../../../Data/SamplesConfig.xml"
#define RECORDING_FILE_NAME "recordingWithRaw.oni"
#define RAW_NODE_NAME "Raw1"

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	#include <conio.h>
	#define XN_KB_HIT _kbhit()
#else
	#define XN_KB_HIT FALSE
#endif

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
	nRetVal = xnLogInitFromXmlFile(SAMPLE_XML_PATH);
	if (nRetVal != XN_STATUS_OK)
	{
		printf("Log couldn't be opened: %s. Running without log", xnGetStatusString(nRetVal));
	}

	Context context;
	nRetVal = context.Init();
	CHECK_RC(nRetVal, "Init");

	// create recorder
	Recorder recorder;

	nRetVal = recorder.Create(context);
	CHECK_RC(nRetVal, "Create recorder");

	nRetVal = recorder.SetDestination(XN_RECORD_MEDIUM_FILE, RECORDING_FILE_NAME);
	CHECK_RC(nRetVal, "Set recorder destination file");

	// Create mock raw node
	MockRawGenerator rawGenerator;
	nRetVal = rawGenerator.Create(context, "MockRaw");
	CHECK_RC(nRetVal, "Create mock raw generator");

	nRetVal = rawGenerator.SetStringProperty("Type", "ReverseT");
	CHECK_RC(nRetVal, "Set property");

	nRetVal = rawGenerator.SetIntProperty("X", 5);
	CHECK_RC(nRetVal, "Set property");

	nRetVal = recorder.AddNodeToRecording(rawGenerator);
	CHECK_RC(nRetVal, "Add node to recording");

	//Now we set one property after adding to recording
	nRetVal = rawGenerator.SetIntProperty("Y", 8);
	CHECK_RC(nRetVal, "Set property");

	XnChar buff[20];
	
	for (XnUInt32 i = 1; i <= 10; i++)
	{
		for (XnUInt32 j = 0; j < 20; j++)
		{
			buff[j] = i;
		}

		printf("Recording frame %d/10...\r", i);
		nRetVal = rawGenerator.SetData(1000 * i, i, sizeof(buff), buff);
		CHECK_RC(nRetVal, "Set raw node new data");

		nRetVal = recorder.Record();
		CHECK_RC(nRetVal, "Record");
	}

	nRetVal = recorder.RemoveNodeFromRecording(rawGenerator);
	CHECK_RC(nRetVal, "Remove node from recording");

	recorder.Release();
	printf("\nDone recording.\n");

	rawGenerator.Release();

	nRetVal = context.GetProductionNodeByName("MockRaw", rawGenerator);
	XN_ASSERT(nRetVal == XN_STATUS_BAD_NODE_NAME);

	Player player;
	nRetVal = context.OpenFileRecording(RECORDING_FILE_NAME, player);
	CHECK_RC(nRetVal, "Open file recording");
	
	nRetVal = player.SetRepeat(FALSE);
	CHECK_RC(nRetVal, "Turn repeat off");

	nRetVal = context.GetProductionNodeByName("MockRaw", rawGenerator);
	CHECK_RC(nRetVal, "Get raw node by name");

	const XnChar* pData = NULL;
	XnUInt32 nSize = 0;

	printf("Reading Data:\n");
	while ((nRetVal = context.WaitAnyUpdateAll()) != XN_STATUS_EOF)
	{
		CHECK_RC(nRetVal, "Read data from file");
		pData = (const XnChar*)rawGenerator.GetData();
		nSize = rawGenerator.GetDataSize();
		for (XnUInt32 i = 0; i < nSize; i++)
		{
			printf("%d ", pData[i]);
		}
		printf("\n");
	}

	player.Release();
	rawGenerator.Release();
	recorder.Release();
	context.Release();

	return 0;
}
