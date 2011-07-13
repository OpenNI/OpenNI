package org.OpenNI;

public class SkeletonJointPosition
{
	public SkeletonJointPosition(Point3D position, float confidence)
	{
		this.position = position;
		this.confidence = confidence;
	}
	
	public Point3D getPosition()
	{
		return this.position;
	}
	public float getConfidence()
	{
		return this.confidence;
	}
	
	private Point3D position;
	private float confidence;
}
