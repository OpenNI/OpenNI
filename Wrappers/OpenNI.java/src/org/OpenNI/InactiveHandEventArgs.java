package org.OpenNI;

public class InactiveHandEventArgs extends EventArgs
{
	public InactiveHandEventArgs(int id, float time)
	{
		this.id = id;
		this.time = time;
	}
	
	public int getId()
	{
		return this.id;
	}
	public float getTime()
	{
		return this.time;
	}
	
	private final int id;
	private final float time;
}
