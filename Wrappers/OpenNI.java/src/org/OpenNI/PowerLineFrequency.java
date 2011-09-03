package org.OpenNI;

import java.util.NoSuchElementException;

public enum PowerLineFrequency 
{
	OFF (0),
	HZ50 (50),
	HZ60 (60);
	
	PowerLineFrequency(int val)
	{
		this.val = val;
	}
	
	public int toNative() { return this.val; }
	
	public static PowerLineFrequency fromNative(int value)
	{
		for (PowerLineFrequency type : PowerLineFrequency.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	private final int val;
}
