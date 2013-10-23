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

#if XN_PLATFORM == XN_PLATFORM_WIN32
#pragma warning(push,3)
#include <MMSystem.h>
#pragma warning(pop)
#endif

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define SAMPLE_XML_PATH "../../../../Data/SamplesConfig.xml"

#define NUMBER_OF_AUDIO_BUFFERS 100

#define CHECK_RC(rc, what)										\
	if (rc != XN_STATUS_OK)										\
	{															\
		printf("%s failed: %s\n", what, xnGetStatusString(rc));	\
		return rc;												\
	}

const XnUInt32 nSupportedNodes = 10;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
using namespace xn;

enum Mode
{
	MODE_PLAY,
	MODE_RECORD,
};

void printUsage(const XnChar* arg0)
{
	printf("usage: %s [play | record]\n", arg0);
}

XnStatus play(Context& /*context*/, AudioGenerator* aGens, XnUInt32 nNodes)
{
	XnStatus nRetVal = XN_STATUS_OK;

#if XN_PLATFORM == XN_PLATFORM_WIN32
	HWAVEOUT hWaveOut;
	WAVEHDR* pAudioBuffers;
	int nAudioNextBuffer;

	if (nNodes > 1)
	{
		printf("Warning: only supports playing of a single node. Will play node '%s'.\n", aGens[0].GetName());
	}

	XnWaveOutputMode waveMode;
	nRetVal = aGens[0].GetWaveOutputMode(waveMode);
	CHECK_RC(nRetVal, "Failed getting wave output mode");

	// init wave out
	WAVEFORMATEX wf;
	wf.wFormatTag = 0x0001; // PCM
	wf.nChannels = waveMode.nChannels;
	wf.nSamplesPerSec = waveMode.nSampleRate;
	wf.wBitsPerSample = waveMode.nBitsPerSample;
	wf.nBlockAlign = wf.wBitsPerSample * wf.nChannels / 8;
	wf.nAvgBytesPerSec = wf.nBlockAlign * wf.nSamplesPerSec;
	MMRESULT mmRes = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wf, NULL, NULL, CALLBACK_NULL);
	if (mmRes != MMSYSERR_NOERROR)
	{
		printf("Warning: Failed opening wave out device. Audio could not be played!\n");
		return XN_STATUS_ERROR;
	}

	// create some wave headers for playing
	pAudioBuffers = new WAVEHDR[NUMBER_OF_AUDIO_BUFFERS];
	xnOSMemSet(pAudioBuffers, 0, sizeof(WAVEHDR)*NUMBER_OF_AUDIO_BUFFERS);

	const XnUInt32 nMaxBufferSize = 2 * 1024 * 1024;
	for (int i = 0; i < NUMBER_OF_AUDIO_BUFFERS; ++i)
	{
		pAudioBuffers[i].lpData = new XnChar[nMaxBufferSize];
		pAudioBuffers[i].dwUser = i;
		pAudioBuffers[i].dwFlags = WHDR_DONE; // mark this buffer as empty (already played)
	}

	nAudioNextBuffer = 0;

	printf ("Press any key to exit...\n");

	// drop data till now
	aGens[0].WaitAndUpdateData();

	while (!xnOSWasKeyboardHit())
	{
		// Read audio chunk
		nRetVal = aGens[0].WaitAndUpdateData();
		if (nRetVal != XN_STATUS_OK)
		{
			printf("Waiting for new data failed: %s\n", xnGetStatusString(nRetVal));
		}

		XnUInt32 nBufferSize = aGens[0].GetDataSize();

		WAVEHDR* pHeader = &pAudioBuffers[nAudioNextBuffer];
		if ((pHeader->dwFlags & WHDR_DONE) == 0)
		{
			printf("No audio buffer is available!. Audio buffer will be lost!\n");
			continue;
		}

		// first unprepare this header
		MMRESULT mmRes = waveOutUnprepareHeader(hWaveOut, pHeader, sizeof(WAVEHDR));
		if (mmRes != MMSYSERR_NOERROR)
		{
			CHAR msg[250];
			waveOutGetErrorText(mmRes, msg, 250);
			printf("Failed unpreparing header: %s", msg);
		}

		pHeader->dwFlags = 0;
		xnOSMemCopy(pHeader->lpData, aGens[0].GetAudioBuffer(), nBufferSize);
		pHeader->dwBufferLength = nBufferSize;

		// prepare header
		mmRes = waveOutPrepareHeader(hWaveOut, pHeader, sizeof(WAVEHDR));
		if (mmRes != MMSYSERR_NOERROR)
		{
			CHAR msg[250];
			waveOutGetErrorText(mmRes, msg, 250);
			printf("Unable to prepare header: %s", msg);
			continue;
		}

		// queue header
		mmRes = waveOutWrite(hWaveOut, pHeader, sizeof(WAVEHDR));
		if (mmRes != MMSYSERR_NOERROR)
		{
			CHAR msg[250];
			waveOutGetErrorText(mmRes, msg, 250);
			printf("Unable to queue header: %s", msg);
			continue;
		}

		nAudioNextBuffer = (nAudioNextBuffer + 1) % NUMBER_OF_AUDIO_BUFFERS;
	}

	// stop playback
	if (MMSYSERR_NOERROR != waveOutReset(hWaveOut))
	{
		printf("Warning: Failed stopping wave out device.!\n");
		return XN_STATUS_ERROR;
	}

	// close device
	waveOutClose(hWaveOut);

	// release memory
	for (int i = 0; i < NUMBER_OF_AUDIO_BUFFERS; ++i)
	{
		delete[] pAudioBuffers[i].lpData;
	}
	delete pAudioBuffers;

#endif

	return XN_STATUS_OK;
}

XnStatus record(Context& context, AudioGenerator* aGens, XnUInt32 nNodes)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// open files
	XN_FILE_HANDLE aFiles[nSupportedNodes];

	for (XnUInt32 i = 0; i < nNodes; ++i)
	{
		XnChar strFileName[XN_FILE_MAX_PATH];
		sprintf(strFileName, "audio.%s.pcm", aGens[i].GetName());
		nRetVal = xnOSOpenFile(strFileName, XN_OS_FILE_WRITE, &aFiles[i]);
		CHECK_RC(nRetVal, "Open file");
	}
	
	printf ("Press any key to exit...\n");

	// drop data till now
	context.WaitAnyUpdateAll();

	while (!xnOSWasKeyboardHit())
	{
		// Read audio chunk
		nRetVal = context.WaitAnyUpdateAll();
		if (nRetVal != XN_STATUS_OK)
		{
			printf("Waiting for new data failed: %s\n", xnGetStatusString(nRetVal));
		}

		// check which node has new data
		for (XnUInt32 i = 0; i < nNodes; ++i)
		{
			if (aGens[i].IsDataNew())
			{
				nRetVal = xnOSWriteFile(aFiles[i], aGens[i].GetAudioBuffer(), aGens[i].GetDataSize());
				CHECK_RC(nRetVal, "Write to file");
			}
		}
	}

	// close files
	for (XnUInt32 i = 0; i < nNodes; ++i)
	{
		xnOSCloseFile(&aFiles[i]);
	}

	return (XN_STATUS_OK);
}

int main(int argc, char* argv[])
{
	XnStatus nRetVal = XN_STATUS_OK;

	Context context;
	EnumerationErrors errors;
	Mode mode;

	// default mode
#if XN_PLATFORM == XN_PLATFORM_WIN32
	mode = MODE_PLAY;
#else
	mode = MODE_RECORD;
#endif

	// check if mode was provided by user
	if (argc > 1)
	{
		if (strcmp(argv[1], "play") == 0)
		{
			mode = MODE_PLAY;
		}
		else if (strcmp(argv[1], "record") == 0)
		{
			mode = MODE_RECORD;
		}
		else
		{
			printUsage(argv[0]);
			return -1;
		}
	}

	// make sure mode is valid
#if XN_PLATFORM != XN_PLATFORM_WIN32
	if (mode == MODE_PLAY)
	{
		printf("Playing is not supported on this platform!\n");
		return -1;
	}
#endif

	ScriptNode scriptNode;

	nRetVal = context.InitFromXmlFile(SAMPLE_XML_PATH, scriptNode);
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

	// find audio nodes
	AudioGenerator gens[nSupportedNodes];
	XnUInt32 nNodes = 0;

	NodeInfoList list;
	nRetVal = context.EnumerateExistingNodes(list, XN_NODE_TYPE_AUDIO);
	CHECK_RC(nRetVal, "Enumerate audio nodes");

	for (NodeInfoList::Iterator it = list.Begin(); it != list.End(); ++it)
	{
		NodeInfo info = *it;
		nRetVal = info.GetInstance(gens[nNodes]);
		CHECK_RC(nRetVal, "Get audio node");
		nNodes++;
	}

	if (nNodes == 0)
	{
		printf("No audio node was found!\n");
		return -1;
	}

	if (mode == MODE_PLAY)
	{
		nRetVal = play(context, gens, nNodes);
	}
	else if (mode == MODE_RECORD)
	{
		nRetVal = record(context, gens, nNodes);
	}

	scriptNode.Release();
	for (int i = 0; i < nSupportedNodes; ++i)
		gens[i].Release();
	context.Release();

	return nRetVal;
}
