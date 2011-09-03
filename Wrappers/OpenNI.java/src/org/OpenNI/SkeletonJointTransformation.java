package org.OpenNI;

public class SkeletonJointTransformation
{
	public SkeletonJointTransformation(SkeletonJointPosition position, SkeletonJointOrientation orientation)
	{
		this.position = position;
		this.orientation = orientation;
	}
	
	public SkeletonJointPosition getPosition()
	{
		return position;
	}
	public SkeletonJointOrientation getOrientation()
	{
		return orientation;
	}
	
	private SkeletonJointPosition position;
	private SkeletonJointOrientation orientation;

}
