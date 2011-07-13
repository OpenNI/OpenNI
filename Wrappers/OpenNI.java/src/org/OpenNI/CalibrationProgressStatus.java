package org.OpenNI;

import java.util.NoSuchElementException;

public enum CalibrationProgressStatus
{
	OK(0),
	NO_USER(1),
	ARM(2),
	LEG(3),
	HEAD(4),
	TORSO(5),
	TOP_FOV(6),
	SIDE_FOV(7),
	POSE(8);
	
	CalibrationProgressStatus(int val)
	{
		this.val = val;
	}
	
	public int toNative() { return this.val; }
	
	public static CalibrationProgressStatus fromNative(int value)
	{
		for (CalibrationProgressStatus type : CalibrationProgressStatus.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	private final int val;



}
