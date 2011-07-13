package org.OpenNI;

public class Plane3D
{
	public Plane3D(Point3D normal, Point3D point)
	{
		this.normal = normal;
		this.point = point;
	}
	
	public Point3D getNormal()
	{
		return this.normal;
	}
	public Point3D getPoint()
	{
		return this.point;
	}
	
	
	private final Point3D normal;
	private final Point3D point;

}
