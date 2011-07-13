package org.OpenNI;

public class ActiveHandDirectionEventArgs extends EventArgs
{
	public ActiveHandDirectionEventArgs(int id, Point3D position, float time, Direction direction)
	{
		this.id = id;
		this.position = position;
		this.time = time;
		this.direction = direction;
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
	public Direction getDirection()
	{
		return this.direction;
	}
	
	
	private final int id;
	private final Point3D position;
	private final float time;
	private final Direction direction;

}
