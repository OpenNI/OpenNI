package org.OpenNI;

public class BoundingBox3D
{
	public BoundingBox3D(Point3D leftBottomNear, Point3D rightTopFar)
	{
		this.leftBottomNear = leftBottomNear;
		this.rightTopFar = rightTopFar;
	}

	public Point3D getLeftBottomNear()
	{
		return this.leftBottomNear;
	}
	public Point3D getRightTopFar()
	{
		return this.rightTopFar;
	}
	
	public Point3D getMins()
	{
		return getLeftBottomNear();
	}
	public Point3D getMaxs()
	{
		return getRightTopFar();
	}
	
	private final Point3D leftBottomNear;
	private final Point3D rightTopFar;
}
