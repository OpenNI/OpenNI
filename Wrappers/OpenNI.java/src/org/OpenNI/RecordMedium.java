package org.OpenNI;

import java.util.NoSuchElementException;

public enum RecordMedium
{
	FILE (0);
	
	RecordMedium(int val)
	{
		this.val = val;
	}
	
	public int toNative() { return this.val; }
	
	public static RecordMedium fromNative(int value)
	{
		for (RecordMedium type : RecordMedium.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	private final int val;
}
