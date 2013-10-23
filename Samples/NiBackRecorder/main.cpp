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
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define CHECK_RC(rc, what)											\
	if (rc != XN_STATUS_OK)											\
	{																\
		printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
		return rc;													\
	}

#define CHECK_RC_ERR(rc, what, errors)			\
{												\
	if (rc == XN_STATUS_NO_NODE_PRESENT)		\
	{											\
		XnChar strError[1024];					\
		errors.ToString(strError, 1024);		\
		printf("%s\n", strError);				\
	}											\
	CHECK_RC(rc, what)							\
}

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

void printUsage(XnChar* strName)
{
	printf("%s "
		"time <seconds> [depth [qvga|vga]] [image [qvga|vga]] [verbose] [mirror <on|off>] [registration] [framesync] [outdir <directory>]\n", strName);
}

XnMapOutputMode QVGAMode = { 320, 240, 30 };
XnMapOutputMode VGAMode = { 640, 480, 30 };

// Configuration
struct RecConfiguration
{
	RecConfiguration()
	{
		pDepthMode = &QVGAMode;
		pImageMode = &QVGAMode;
		bRecordDepth = FALSE;
		bRecordImage = FALSE;
		
		bMirrorIndicated = FALSE;
		bMirror = TRUE;

		bRegister = FALSE;
		bFrameSync = FALSE;
		bVerbose = FALSE;

		nDumpTime = 0;
		sprintf(strDirName, ".");
	}
	XnMapOutputMode* pDepthMode;
	XnMapOutputMode* pImageMode;
	XnBool bRecordDepth;
	XnBool bRecordImage;

	XnBool bMirrorIndicated;
	XnBool bMirror;
	XnBool bRegister;
	XnBool bFrameSync;
	XnBool bVerbose;

	XnUInt32 nDumpTime;
	XnChar strDirName[XN_FILE_MAX_PATH];
};

// Parse the command line arguments
XnBool ParseArgs(int argc, char** argv, RecConfiguration& config)
{
	XnBool bError = FALSE;

	for (int i = 1; i < argc; ++i)
	{
		if (xnOSStrCaseCmp(argv[i], "time") == 0)
		{
			// Set the time of each recording
			if (argc > i+1)
			{
				config.nDumpTime = atoi(argv[i+1]);
				if (config.nDumpTime == 0)
					bError = TRUE;
				i++;
			}
			else
			{
				bError = TRUE;
			}
		}
		else if (xnOSStrCaseCmp(argv[i], "depth") == 0)
		{
			// Set depth resolution (default is QVGA)
			if (argc > i+1)
			{
				if (xnOSStrCaseCmp(argv[i+1], "vga") == 0)
				{
					config.pDepthMode = &VGAMode;
					++i;
				}
				else if (xnOSStrCaseCmp(argv[i+1], "qvga") ==0)
				{
					config.pDepthMode = &QVGAMode;
					++i;
				}
			}
			config.bRecordDepth = TRUE;
		}
		else if (xnOSStrCaseCmp(argv[i], "image") == 0)
		{
			// Set image resolution (default is QVGA)
			if (argc > i+1)
			{
				if (xnOSStrCaseCmp(argv[i+1], "vga") == 0)
				{
					config.pImageMode = &VGAMode;
					++i;
				}
				else if (xnOSStrCaseCmp(argv[i+1], "qvga") ==0)
				{
					config.pImageMode = &QVGAMode;
					++i;
				}
			}
			config.bRecordImage = TRUE;
		}
		else if (xnOSStrCaseCmp(argv[i], "verbose") == 0)
		{
			// Control the log
			config.bVerbose = TRUE;
		}
		else if (xnOSStrCaseCmp(argv[i], "mirror") == 0)
		{
			// Set mirror mode (does nothing if missing)
			config.bMirrorIndicated = TRUE;
			if (argc > i+1)
			{
				if (xnOSStrCaseCmp(argv[i+1], "on") == 0)
				{
					config.bMirror = TRUE;
					++i;
				}
				else if (xnOSStrCaseCmp(argv[i+1], "off") ==0)
				{
					config.bMirror = FALSE;
					++i;
				}
			}
		}
		else if (xnOSStrCaseCmp(argv[i], "registration") == 0)
		{
			// Set registration of depth to image
			config.bRegister = TRUE;
		}
		else if (xnOSStrCaseCmp(argv[i], "framesync") == 0)
		{
			// Sync the image and the depth
			config.bFrameSync = TRUE;
		}
		else if (xnOSStrCaseCmp(argv[i], "outdir") == 0)
		{
			// Set the directory in which the files will be created
			if (argc > i+1)
			{
				xnOSStrCopy(config.strDirName, argv[i+1], XN_FILE_MAX_PATH);
				++i;
			}
		}
		else
		{
			printf("Unknown option %s\n", argv[i]);
		}
	}

	// Must record something!
	if (!config.bRecordDepth && !config.bRecordImage)
	{
		printf("Recording nothing!\n");
		printUsage(argv[0]);
		return FALSE;
	}
	// No default time
	if (config.nDumpTime == 0)
	{
		printf("Missing time\n");
		return FALSE;
	}

	return !bError;
}

XnStatus ConfigureGenerators(const RecConfiguration& config, xn::Context& context, xn::DepthGenerator& depthGenerator, xn::ImageGenerator& imageGenerator)
{
	XnStatus nRetVal = XN_STATUS_OK;
	xn::EnumerationErrors errors;

	// Configure the depth, if needed
	if (config.bRecordDepth)
	{
		nRetVal = context.CreateAnyProductionTree(XN_NODE_TYPE_DEPTH, NULL, depthGenerator, &errors);
		CHECK_RC_ERR(nRetVal, "Create Depth", errors);
		nRetVal = depthGenerator.SetMapOutputMode(*config.pDepthMode);
		CHECK_RC(nRetVal, "Set Mode");
		if (config.bMirrorIndicated && depthGenerator.IsCapabilitySupported(XN_CAPABILITY_MIRROR))
		{
			depthGenerator.GetMirrorCap().SetMirror(config.bMirror);
		}

		// Set Hole Filter
		depthGenerator.SetIntProperty("HoleFilter", TRUE);
	}
	// Configure the image, if needed
	if (config.bRecordImage)
	{
		nRetVal = context.CreateAnyProductionTree(XN_NODE_TYPE_IMAGE, NULL, imageGenerator, &errors);
		CHECK_RC_ERR(nRetVal, "Create Image", errors);
		nRetVal = imageGenerator.SetMapOutputMode(*config.pImageMode);
		CHECK_RC(nRetVal, "Set Mode");

		if (config.bMirrorIndicated && imageGenerator.IsCapabilitySupported(XN_CAPABILITY_MIRROR))
		{
			imageGenerator.GetMirrorCap().SetMirror(config.bMirror);
		}
	}

	// Configuration for when there are both streams
	if (config.bRecordDepth && config.bRecordImage)
	{
		// Registration
		if (config.bRegister && depthGenerator.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT))
		{
			nRetVal = depthGenerator.GetAlternativeViewPointCap().SetViewPoint(imageGenerator);
			CHECK_RC(nRetVal, "Registration");
		}
		// Frame Sync
		if (config.bFrameSync && depthGenerator.IsCapabilitySupported(XN_CAPABILITY_FRAME_SYNC))
		{
			if (depthGenerator.GetFrameSyncCap().CanFrameSyncWith(imageGenerator))
			{
				nRetVal = depthGenerator.GetFrameSyncCap().FrameSyncWith(imageGenerator);
				CHECK_RC(nRetVal, "Frame sync");
			}
		}
	}

	return XN_STATUS_OK;
}

// The cyclic buffer, to which frames will be added and from where they will be dumped to files
class CyclicBuffer
{
public:
	// Creation - set the OpenNI objects
	CyclicBuffer(xn::Context& context, xn::DepthGenerator& depthGenerator, xn::ImageGenerator& imageGenerator, const RecConfiguration& config) :
		m_context(context),
		m_depthGenerator(depthGenerator),
		m_imageGenerator(imageGenerator),
		m_pFrames(NULL)
	{
		m_bDepth = config.bRecordDepth;
		m_bImage = config.bRecordImage;
		m_nNextWrite = 0;
		m_nBufferSize = 0;
		m_nBufferCount = 0;
	}
	// Initialization - set outdir and time of each recording
	void Initialize(XnChar* strDirName, XnUInt32 nSeconds)
	{
		xnOSStrCopy(m_strDirName, strDirName, XN_FILE_MAX_PATH);
		m_nBufferSize = nSeconds*30;
		m_pFrames = XN_NEW_ARR(SingleFrame, m_nBufferSize);
	}
	// Save new data from OpenNI
	void Update(const xn::DepthGenerator& depthGenerator, const xn::ImageGenerator& imageGenerator)
	{
		if (m_bDepth)
		{
			// Save latest depth frame
			xn::DepthMetaData dmd;
			depthGenerator.GetMetaData(dmd);
			m_pFrames[m_nNextWrite].depthFrame.CopyFrom(dmd);
		}
		if (m_bImage)
		{
			// Save latest image frame
			xn::ImageMetaData imd;
			imageGenerator.GetMetaData(imd);
			m_pFrames[m_nNextWrite].imageFrame.CopyFrom(imd);
		}

		// See if buffer is already full
		if (m_nBufferCount < m_nBufferSize)
		{
			m_nBufferCount++;
		}
		// Make sure cylic buffer pointers are good
		m_nNextWrite++;
		if (m_nNextWrite == m_nBufferSize)
		{
			m_nNextWrite = 0;
		}
	}

	// Save the current state of the buffer to a file
	XnStatus Dump()
	{
		xn::MockDepthGenerator mockDepth;
		xn::MockImageGenerator mockImage;

		xn::EnumerationErrors errors;
		XnStatus rc;

		// Create recorder
		rc = m_context.CreateAnyProductionTree(XN_NODE_TYPE_RECORDER, NULL, m_recorder, &errors);
		CHECK_RC_ERR(rc, "Create recorder", errors);

		// Create name of new file
		time_t rawtime;
		struct tm *timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		XnChar strFileName[XN_FILE_MAX_PATH];
		sprintf(strFileName, "%s/%04d%02d%02d-%02d%02d%02d.oni", m_strDirName,
			timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

		m_recorder.SetDestination(XN_RECORD_MEDIUM_FILE, strFileName);
		printf("Creating file %s\n", strFileName);

		// Create mock nodes based on the depth generator, to save depth
		if (m_bDepth)
		{
			// Create mock nodes based on the depth generator, to save depth
			rc = m_context.CreateMockNodeBasedOn(m_depthGenerator, NULL, mockDepth);
			CHECK_RC(rc, "Create depth node");
			rc = m_recorder.AddNodeToRecording(mockDepth);
			CHECK_RC(rc, "Add depth node");
		}
		if (m_bImage)
		{
			// Create mock nodes based on the image generator, to save image
			rc = m_context.CreateMockNodeBasedOn(m_imageGenerator, NULL, mockImage);
			CHECK_RC(rc, "Create image node");
			rc = m_recorder.AddNodeToRecording(mockImage);
			CHECK_RC(rc, "Add image node");
		}

		// Write frames from next index (which will be next to be written, and so the first available)
		// this is only if a full loop was done, and this frame has meaningful data
		if (m_nNextWrite < m_nBufferCount)
		{
			// Not first loop, right till end
			for (XnUInt32 i = m_nNextWrite; i < m_nBufferSize; ++i)
			{
				if (m_bDepth)
				{
					mockDepth.SetData(m_pFrames[i].depthFrame);
				}
				if (m_bImage)
				{
					mockImage.SetData(m_pFrames[i].imageFrame);
				}

				m_recorder.Record();
			}
		}
		// Write frames from the beginning of the buffer to the last on written
		for (XnUInt32 i = 0; i < m_nNextWrite; ++i)
		{
			if (m_bDepth)
			{
				mockDepth.SetData(m_pFrames[i].depthFrame);
			}
			if (m_bImage)
			{
				mockImage.SetData(m_pFrames[i].imageFrame);
			}

			m_recorder.Record();
		}

		// Close recorder
		m_recorder.Release();

		return XN_STATUS_OK;
	}
protected:
	struct SingleFrame
	{
		xn::DepthMetaData depthFrame;
		xn::ImageMetaData imageFrame;
	};

	XnBool m_bDepth, m_bImage;
	SingleFrame* m_pFrames;
	XnUInt32 m_nNextWrite;
	XnUInt32 m_nBufferSize;
	XnUInt32 m_nBufferCount;
	XnChar m_strDirName[XN_FILE_MAX_PATH];

	xn::Context& m_context;
	xn::DepthGenerator& m_depthGenerator;
	xn::ImageGenerator& m_imageGenerator;
	xn::Recorder m_recorder;

private:
	XN_DISABLE_COPY_AND_ASSIGN(CyclicBuffer);
};

// The recorder
int main(int argc, char** argv)
{
	// OpenNi objects
	xn::Context context;
	xn::DepthGenerator depthGenerator;
	xn::ImageGenerator imageGenerator;

	// To count missed frames
	XnUInt64 nLastDepthTime = 0;
	XnUInt64 nLastImageTime = 0;
	XnUInt32 nMissedDepthFrames = 0;
	XnUInt32 nMissedImageFrames = 0;
	XnUInt32 nDepthFrames = 0;
	XnUInt32 nImageFrames = 0;

	RecConfiguration config;

	XnStatus nRetVal = XN_STATUS_OK;

	// Parse the command line arguments
	if (!ParseArgs(argc, argv, config))
	{
		printf("Parse error\n");
		return 1;
	}

	if (config.bVerbose)
	{
		// Turn on log
		xnLogInitSystem();
		xnLogSetConsoleOutput(TRUE);
		xnLogSetMaskMinSeverity(XN_LOG_MASK_ALL, XN_LOG_VERBOSE);
	}

	// Initialize OpenNI
	nRetVal = context.Init();
	CHECK_RC(nRetVal, "Init");

	nRetVal = ConfigureGenerators(config, context, depthGenerator, imageGenerator);
	CHECK_RC(nRetVal, "Config generators");

	nRetVal = context.StartGeneratingAll();
	CHECK_RC(nRetVal, "Generate all");

	// Create and initialize the cyclic buffer
	CyclicBuffer cyclicBuffer(context, depthGenerator, imageGenerator, config);
	cyclicBuffer.Initialize(config.strDirName, config.nDumpTime);

	// Mainloop
	for (;;)
	{
		if (xnOSWasKeyboardHit())
		{
			char c = xnOSReadCharFromInput();
			XnBool bStop = FALSE;
			switch (c)
			{
			case 27:
				bStop = TRUE;
				break;
			case 'd':
				cyclicBuffer.Dump();
				break;
			}
			if (bStop)
			{
				break;
			}
		}
		// Get next data
		context.WaitAndUpdateAll();
		// Save data
		cyclicBuffer.Update(depthGenerator, imageGenerator);

		// Check for missed frames
		if (config.bRecordDepth)
		{
			++nDepthFrames;
			XnUInt64 nTimestamp = depthGenerator.GetTimestamp();
			if (nLastDepthTime != 0 && nTimestamp - nLastDepthTime > 35000)
			{
				int missed = (int)(nTimestamp-nLastDepthTime)/32000 - 1;
				printf("Missed depth: %llu -> %llu = %d > 35000 - %d frames\n",
					nLastDepthTime, nTimestamp, XnUInt32(nTimestamp-nLastDepthTime), missed);
				nMissedDepthFrames += missed;
			}
			nLastDepthTime = nTimestamp;
		}
		if (config.bRecordImage)
		{
			++nImageFrames;
			XnUInt64 nTimestamp = imageGenerator.GetTimestamp();
			if (nLastImageTime != 0 && nTimestamp - nLastImageTime > 35000)
			{
				int missed = (int)(nTimestamp-nLastImageTime)/32000 - 1;
				printf("Missed image: %llu -> %llu = %d > 35000 - %d frames\n",
					nLastImageTime, nTimestamp, XnUInt32(nTimestamp-nLastImageTime), missed);
				nMissedImageFrames += missed;
			}
			nLastImageTime = nTimestamp;

		}

	}

	if (config.bRecordDepth)
	{
		printf("Missed %d of %d depth frames (%5.2f%%)\n", nMissedDepthFrames, (nMissedDepthFrames+nDepthFrames), (nMissedDepthFrames*100.0)/(nMissedDepthFrames+nDepthFrames));
	}
	if (config.bRecordImage)
	{
		printf("Missed %d of %d image frames (%5.2f%%)\n", nMissedImageFrames, (nMissedImageFrames+nImageFrames), (nMissedImageFrames*100.0)/(nMissedImageFrames+nImageFrames));
	}

	imageGenerator.Release();
	depthGenerator.Release();
	context.Release();

	return 0;
}
