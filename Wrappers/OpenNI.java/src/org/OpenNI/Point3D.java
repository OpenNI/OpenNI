package org.OpenNI;

public class Point3D
{
	public Point3D(float x, float y, float z)
	{
		this.X = x;
		this.Y = y;
		this.Z = z;
	}
	public Point3D()
	{
		this(0f,0f,0f);
	}
	public void setPoint(float x, float y, float z)
	{
		this.X = x;
		this.Y = y;
		this.Z = z;
	}

	public float getX()
	{
		return X;
	}
	public float getY()
	{
		return Y;
	}
	public float getZ()
	{
		return Z;
	}
	
	
	private float X;
	private float Y;
	private float Z;
}
