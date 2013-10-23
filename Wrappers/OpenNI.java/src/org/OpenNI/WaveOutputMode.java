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

/**
 * Stores wave output settings used by the Audio Generator. <BR><BR>
 *
 */
public class WaveOutputMode
{
	/**
	 * Constructor to create a new WaveOutputMode with specified sampleRate, bitsPerSample, and numberOfChannels
	 * @param sampleRate Desired sample rate, in samples per second
	 * @param bitsPerSample Desired sample resolution, in bits per sample
	 * @param numberOfChannels Desired number of channels
	 */
	public WaveOutputMode(int sampleRate, short bitsPerSample, byte numberOfChannels)
	{
		super();
		this.sampleRate = sampleRate;
		this.bitsPerSample = bitsPerSample;
		this.numberOfChannels = numberOfChannels;
	}
	
	/**
	 * Getter function for the sample rate
	 * @return Current sample rate, in samples per second
	 */
	public int getSampleRate()
	{
		return this.sampleRate;
	}
	/**
	 * Setter function for the sample rate
	 * @param sampleRate Desired new sample rate, in samples per second
	 */
	public void setSampleRate(int sampleRate)
	{
		this.sampleRate = sampleRate;
	}
	/**
	 * Getter function for the sample resolution
	 * @return Current sample resolution, in bits per sample
	 */
	public short getBitsPerSample()
	{
		return this.bitsPerSample;
	}
	/** 
	 * Setter function for the sample resolution
	 * @param bitsPerSample Desired new sample resolution, in bits per sample
	 */
	public void setBitsPerSample(short bitsPerSample)
	{
		this.bitsPerSample = bitsPerSample;
	}
	/**
	 * Getter function for the number of channels
	 * @return Current number of channels
	 */
	public byte getNumberOfChannels()
	{
		return this.numberOfChannels;
	}
	/**
	 * Setter function for the number of channels
	 * @param numberOfChannels Desired new value for number of channels
	 */
	public void setNumberOfChannels(byte numberOfChannels)
	{
		this.numberOfChannels = numberOfChannels;
	}
	
	private int sampleRate;
	private short bitsPerSample;
	private byte numberOfChannels;
}
