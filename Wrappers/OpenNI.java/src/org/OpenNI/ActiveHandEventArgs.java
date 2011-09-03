package org.OpenNI;

public class ActiveHandEventArgs extends EventArgs
{
	public ActiveHandEventArgs(int id, Point3D position, float time)
	{
		this.id = id;
		this.position = position;
		this.time = time;
	}
	
	public int getId()
	{
		return this.id;
	}
	public Point3D getPosition()
	{
		return this.position;
	}
	public float getTime()
	{
		return this.time;
	}
	
	
	private final int id;
	private final Point3D position;
	private final float time;
}
