package org.OpenNI;

import java.util.NoSuchElementException;

public enum SkeletonPoseProcessingMode
{
	NONE(0),
	ALL(1);
	
	SkeletonPoseProcessingMode(int val)
	{
		this.val = val;
	}
	
	public int toNative() { return this.val; }
	
	public static SkeletonPoseProcessingMode fromNative(int value)
	{
		for (SkeletonPoseProcessingMode type : SkeletonPoseProcessingMode.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	private final int val;
}
