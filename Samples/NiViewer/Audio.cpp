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
// --------------------------------
// Includes
// --------------------------------
#include "Audio.h"
#include "Device.h"
#include <XnLog.h>

#if (XN_PLATFORM != XN_PLATFORM_WIN32)
void audioInit() {}
void audioPlay() {}
void audioShutdown(void) {}
#else // Win32

#pragma warning(push, 3)
#include <MMSystem.h>
#pragma warning(pop)

// --------------------------------
// Defines
// --------------------------------
#define NUMBER_OF_AUDIO_BUFFERS	20
#define AUDIO_LATENCY_THRESHOLD 80000

#define AUDIO_SYNC_DUMP_MASK	"RGBViewerAudioSync"

// --------------------------------
// Global Variables
// --------------------------------
typedef struct AudioData
{
	HWAVEOUT hWaveOut;
	WAVEHDR* pAudioBuffers;
	int nBufferSize;
	XnUInt64* pAudioTimestamps;
	int nAudioNextBuffer;
	bool bFlush;
	int nFirstToCheck;
	XnDumpFile* SyncDump;
} AudioData;

AudioData g_AudioData;

// --------------------------------
// Code
// --------------------------------
void audioPlay()
{
	if (g_AudioData.hWaveOut == NULL) // not initialized
		return;

	const AudioMetaData* pAudioMD = getAudioMetaData();
	if (pAudioMD == NULL || pAudioMD->DataSize() == 0 || !pAudioMD->IsDataNew())
		return;

	if (g_AudioData.bFlush)
	{
		printf("Audio is falling behind. Flushing all queue.\n");
		xnDumpFileWriteString(g_AudioData.SyncDump, "Flushing queue...\n");

		// mark not to check all dropped headers
		g_AudioData.nFirstToCheck = g_AudioData.nAudioNextBuffer;
		// flush all queued headers
		waveOutReset(g_AudioData.hWaveOut);

		g_AudioData.bFlush = false;
		return;
	}

	int nBufferSize = pAudioMD->DataSize();

	WAVEHDR* pHeader = &g_AudioData.pAudioBuffers[g_AudioData.nAudioNextBuffer];
	if ((pHeader->dwFlags & WHDR_DONE) == 0)
	{
		printf("No audio buffer is available!. Audio buffer will be lost!\n");
		return;
	}

	// first unprepare this header
	MMRESULT mmRes = waveOutUnprepareHeader(g_AudioData.hWaveOut, pHeader, sizeof(WAVEHDR));
	if (mmRes != MMSYSERR_NOERROR)
	{
		CHAR msg[250];
		waveOutGetErrorText(mmRes, msg, 250);
		printf("Failed unpreparing header: %s\n", msg);
	}

	int nMaxPlayedAudio = (int)(pAudioMD->SampleRate() / 1000.0 * pAudioMD->NumberOfChannels() * 2 * AUDIO_LATENCY_THRESHOLD);
	if (nBufferSize > nMaxPlayedAudio)
	{
		printf("Dropping %d bytes of audio to keep synch.\n", nBufferSize - nMaxPlayedAudio);
		nBufferSize = nMaxPlayedAudio;
	}

	const XnUInt8* pData = pAudioMD->Data();

	if (nBufferSize > g_AudioData.nBufferSize)
	{
		printf("Dropping %d bytes of audio to match buffer size.\n", nBufferSize - g_AudioData.nBufferSize);
		pData += (nBufferSize - g_AudioData.nBufferSize);
		nBufferSize = g_AudioData.nBufferSize;
	}

	pHeader->dwFlags = 0;
	xnOSMemCopy(pHeader->lpData, pData, nBufferSize);
	pHeader->dwBufferLength = nBufferSize;

	// prepare header
	mmRes = waveOutPrepareHeader(g_AudioData.hWaveOut, pHeader, sizeof(WAVEHDR));
	if (mmRes != MMSYSERR_NOERROR)
	{
		CHAR msg[250];
		waveOutGetErrorText(mmRes, msg, 250);
		printf("Unable to prepare header: %s\n", msg);
		return;
	}

	// queue header
	mmRes = waveOutWrite(g_AudioData.hWaveOut, pHeader, sizeof(WAVEHDR));
	if (mmRes != MMSYSERR_NOERROR)
	{
		CHAR msg[250];
		waveOutGetErrorText(mmRes, msg, 250);
		printf("Unable to queue header: %s\n", msg);
		return;
	}

	// place end-time as a timestamp
	g_AudioData.pAudioTimestamps[g_AudioData.nAudioNextBuffer] = (XnUInt64)(pAudioMD->Timestamp() + nBufferSize / (pAudioMD->BitsPerSample() / 8.0) / pAudioMD->NumberOfChannels() / (pAudioMD->SampleRate() / 1e6));

	xnDumpFileWriteString(g_AudioData.SyncDump, "Queued index %d with timestamp %llu (%u bytes, %f ms, end timestamp: %llu)\n", g_AudioData.nAudioNextBuffer, pAudioMD->Timestamp(), nBufferSize, nBufferSize / 2.0 / pAudioMD->NumberOfChannels() / (pAudioMD->SampleRate() / 1e3), g_AudioData.pAudioTimestamps[g_AudioData.nAudioNextBuffer]);

	g_AudioData.nAudioNextBuffer = (g_AudioData.nAudioNextBuffer + 1) % NUMBER_OF_AUDIO_BUFFERS;
}

void CALLBACK audioCallback(HWAVEOUT /*hwo*/, UINT uMsg, DWORD_PTR /*dwInstance*/, DWORD_PTR dwParam1, DWORD_PTR /*dwParam2*/)
{
	if (uMsg == WOM_DONE)
	{
		WAVEHDR* pHeader = (WAVEHDR*)dwParam1;
		int nIndex = (int)pHeader->dwUser;

		xnDumpFileWriteString(g_AudioData.SyncDump, "Done playing index %d.", nIndex);

		if (g_AudioData.nFirstToCheck == -1 || g_AudioData.nFirstToCheck == nIndex)
		{
			g_AudioData.nFirstToCheck = -1;

			// get the timestamp of the packet just done playing
			XnUInt64 nPlayedTimestamp = g_AudioData.pAudioTimestamps[nIndex];

			// check how much time is still queued
			XnUInt32 nLastQueuedIndex = (g_AudioData.nAudioNextBuffer + NUMBER_OF_AUDIO_BUFFERS - 1) % NUMBER_OF_AUDIO_BUFFERS;
			XnUInt64 nLastQueuedTimestamp = g_AudioData.pAudioTimestamps[nLastQueuedIndex];

			xnDumpFileWriteString(g_AudioData.SyncDump, " %f ms in queue.", (nLastQueuedTimestamp - nPlayedTimestamp) / 1e3);

			if (nLastQueuedTimestamp - nPlayedTimestamp > AUDIO_LATENCY_THRESHOLD)
			{
				g_AudioData.bFlush = true;
				xnDumpFileWriteString(g_AudioData.SyncDump, " Will flush queue.\n");
			}
			else
				xnDumpFileWriteString(g_AudioData.SyncDump, "\n");
		}
		else
			xnDumpFileWriteString(g_AudioData.SyncDump, "\n");
	}
}

void audioInit()
{
	g_AudioData.hWaveOut = NULL;
	g_AudioData.bFlush = false;
	g_AudioData.nFirstToCheck = -1;
	g_AudioData.SyncDump = xnDumpFileOpen(AUDIO_SYNC_DUMP_MASK, "%s.txt", AUDIO_SYNC_DUMP_MASK);;

	// check if device audio is enabled
	const AudioMetaData* pAudioMD = getAudioMetaData();
	if (pAudioMD == NULL)
		return;

	// start audio out device
	WAVEFORMATEX wf;
	wf.wFormatTag = 0x0001; // PCM
	wf.nChannels = pAudioMD->NumberOfChannels();
	wf.nSamplesPerSec = pAudioMD->SampleRate();
	wf.wBitsPerSample = pAudioMD->BitsPerSample();
	wf.nBlockAlign = wf.wBitsPerSample * wf.nChannels / 8;
	wf.nAvgBytesPerSec = wf.nBlockAlign * wf.nSamplesPerSec;
	MMRESULT mmRes = waveOutOpen(&g_AudioData.hWaveOut, WAVE_MAPPER, &wf, (DWORD_PTR)audioCallback, NULL, CALLBACK_FUNCTION);
	if (mmRes != MMSYSERR_NOERROR)
	{
		printf("Warning: Failed opening wave out device. Audio will not be played!\n");
		g_AudioData.hWaveOut = NULL;
		return;
	}

	// create some wave headers for playing
	g_AudioData.pAudioBuffers = new WAVEHDR[NUMBER_OF_AUDIO_BUFFERS];
	g_AudioData.pAudioTimestamps = new XnUInt64[NUMBER_OF_AUDIO_BUFFERS];
	xnOSMemSet(g_AudioData.pAudioBuffers, 0, sizeof(WAVEHDR)*NUMBER_OF_AUDIO_BUFFERS);

	// allocate max buffer for one second
	g_AudioData.nBufferSize = wf.nAvgBytesPerSec;

	for (int i = 0; i < NUMBER_OF_AUDIO_BUFFERS; ++i)
	{
		g_AudioData.pAudioBuffers[i].lpData = new XnChar[g_AudioData.nBufferSize];
		g_AudioData.pAudioBuffers[i].dwUser = i;
		g_AudioData.pAudioBuffers[i].dwFlags = WHDR_DONE; // mark this buffer as empty (already played)
	}

	g_AudioData.nAudioNextBuffer = 0;
}

void audioShutdown()
{
	if (g_AudioData.hWaveOut != NULL)
	{
		waveOutClose(g_AudioData.hWaveOut);

		for (int i = 0; i < NUMBER_OF_AUDIO_BUFFERS; ++i)
			delete[] g_AudioData.pAudioBuffers[i].lpData;

		delete[] g_AudioData.pAudioBuffers;
	}
}

#endif // Win32
