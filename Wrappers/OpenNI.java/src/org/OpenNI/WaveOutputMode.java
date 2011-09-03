package org.OpenNI;

public class WaveOutputMode
{
	public WaveOutputMode(int sampleRate, short bitsPerSample, byte numberOfChannels)
	{
		super();
		this.sampleRate = sampleRate;
		this.bitsPerSample = bitsPerSample;
		this.numberOfChannels = numberOfChannels;
	}
	
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
