package org.OpenNI;

import java.util.NoSuchElementException;

public enum SkeletonProfile
{
	/** No joints at all **/
	NONE(1),

	/** All joints **/
	ALL(2),
	
	/** All the joints in the upper body (torso and upwards) **/
	UPPER_BODY(3),
	
	/** All the joints in the lower body (torso and downwards) **/
	LOWER_BODY(4),
	
	/** The head and the hands **/
	HEAD_HANDS(5);
	
	SkeletonProfile(int val)
	{
		this.val = val;
	}
	
	public int toNative() { return this.val; }
	
	public static SkeletonProfile fromNative(int value)
	{
		for (SkeletonProfile type : SkeletonProfile.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	private final int val;
}
