package org.OpenNI;

public class AudioMetaData extends OutputMetaData
{
	public int getSampleRate()
	{
		return this.sampleRate;
	}
	public void setSampleRate(int sampleRate)
	{
		this.sampleRate = sampleRate;
	}
	public short getBitsPerSample()
	{
		return this.bitsPerSample;
	}
	public void setBitsPerSample(short bitsPerSample)
	{
		this.bitsPerSample = bitsPerSample;
	}
	public byte getNumberOfChannels()
	{
		return this.numberOfChannels;
	}
	public void setNumberOfChannels(byte numberOfChannels)
	{
		this.numberOfChannels = numberOfChannels;
	}

	private int sampleRate;
	private short bitsPerSample;
	private byte numberOfChannels;
}
