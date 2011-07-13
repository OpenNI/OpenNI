package org.OpenNI;

import java.util.NoSuchElementException;

public enum PixelFormat 
{
	RGB24 (1),
	YUV422 (2),
	GRAYSCALE_8BIT (3),
	GRAYSCALE_16BIT (4);
	
	PixelFormat(int val)
	{
		this.val = val;
	}
	
	public int toNative() { return this.val; }
	
	public int getBytesPerPixel()
	{
		return NativeMethods.xnGetBytesPerPixelForPixelFormat(this.val);
	}
	
	public static PixelFormat fromNative(int value)
	{
		for (PixelFormat type : PixelFormat.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	private int val;
}
