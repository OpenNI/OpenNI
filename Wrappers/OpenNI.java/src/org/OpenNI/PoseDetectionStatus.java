package org.OpenNI;

import java.util.NoSuchElementException;

public enum PoseDetectionStatus
{
	OK(0),
	NO_USER(1),
	TOP_FOV(2),
	SIDE_FOV(3),
	ERROR(4);
	
	PoseDetectionStatus(int val)
	{
		this.val = val;
	}
	
	public int toNative() { return this.val; }
	
	public static PoseDetectionStatus fromNative(int value)
	{
		for (PoseDetectionStatus type : PoseDetectionStatus.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	private final int val;



}
