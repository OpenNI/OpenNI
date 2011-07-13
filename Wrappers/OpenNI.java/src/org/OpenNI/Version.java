package org.OpenNI;

public class Version 
{
	public Version(byte major, byte minor, short maintenance, int build)
	{
		this.major = major;
		this.minor = minor;
		this.maintenance = maintenance;
		this.build = build;
	}

	public byte getMajor()
	{
		return this.major;
	}
	public byte getMinor()
	{
		return this.minor;
	}
	public short getMaintenance()
	{
		return this.maintenance;
	}
	public int getBuild()
	{
		return this.build;
	}
	
	private final byte major;
	private final byte minor;
	private final short maintenance;
	private final int build;
}
