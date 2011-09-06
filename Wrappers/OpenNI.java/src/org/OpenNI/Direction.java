package org.OpenNI;

import java.util.NoSuchElementException;

public enum Direction 
{
	ILLEGAL(0),
	LEFT(1),
	RIGHT(2),
	UP(3),
	DOWN(4),
	FORWARD(5),
	BACKWARD(6);
	
	Direction(int val)
	{
		this.val = val;
	}
	
	public int toNative() { return this.val; }
	
	public static Direction fromNative(int value)
	{
		for (Direction type : Direction.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	private final int val;

}
