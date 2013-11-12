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
package org.openni;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * This class stores a complete set of audio configuration parameters. <BR><BR>
 * 
 * This is what the Audio Generators use to store/recall their settings.
 *
 */
public class AudioMetaData extends OutputMetaData
{
	/**
	 * Getter function for the current sample rate
	 * @return current sample rate in samples per second
	 */
	public int getSampleRate()
	{
		return this.sampleRate;
	}
	/**
	 * Setter function for the sample rate
	 * @param sampleRate Desired new sample rate in samples per second
	 */
	public void setSampleRate(int sampleRate)
	{
		this.sampleRate = sampleRate;
	}
	/**
	 * Getter function for the current sample resolution
	 * @return Resolution of each sample, in bits per sample
	 */
	public short getBitsPerSample()
	{
		return this.bitsPerSample;
	}
	/**
	 * Setter function for the current sample resolution
	 * @param bitsPerSample Desired new resolution, in bits per sample
	 */
	public void setBitsPerSample(short bitsPerSample)
	{
		this.bitsPerSample = bitsPerSample;
	}
	/**
	 * Getter function for the number of audio channels
	 * @return Current number of audio channels
	 */
	public byte getNumberOfChannels()
	{
		return this.numberOfChannels;
	}
	/** Setter function for the number of audio channels
	 * @param numberOfChannels Desired new value for number of audio channels
	 */
	public void setNumberOfChannels(byte numberOfChannels)
	{
		this.numberOfChannels = numberOfChannels;
	}
	
	/**
	 * Allocates an audio buffer, with a size determined by the values of
	 * number of channels, bits per sample and samples per second.  This is 
	 * more useful than direct allocation, since it takes into account the varying 
	 * size of the audio data as settings change.
	 * @return
	 */
	public ByteBuffer createByteBuffer()
	{
		int size = getDataSize();
		ByteBuffer buffer = ByteBuffer.allocateDirect(size);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		NativeMethods.copyToBuffer(buffer, getDataPtr(), size);
		return buffer;
	}

	private int sampleRate;
	private short bitsPerSample;
	private byte numberOfChannels;
}
