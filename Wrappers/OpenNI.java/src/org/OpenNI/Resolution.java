package org.OpenNI;

import java.util.NoSuchElementException;

public enum Resolution 
{
	CUSTOM (0),
	QQVGA (1),
	CGA (2),
	QVGA (3),
	VGA (4),
	SVGA (5),
	XGA (6),
	P720 (7),
	SXGA (8),
	UXGA (9),
	P1080 (10);
	
	Resolution(int val)
	{
		this.val = val;
		this.xRes = NativeMethods.xnResolutionGetXRes(val);
		this.yRes = NativeMethods.xnResolutionGetYRes(val);
		this.name = NativeMethods.xnResolutionGetName(val);
	}
	
	public int getxRes()
	{
		return this.xRes;
	}

	public int getyRes()
	{
		return this.yRes;
	}

	public String getName()
	{
		return this.name;
	}
	
	public int toNative() { return this.val; }
	
	public static Resolution fromNative(int value)
	{
		for (Resolution type : Resolution.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	public static Resolution fromName(String name)
	{
		return fromNative(NativeMethods.xnResolutionGetFromName(name));
	}
	
	public static Resolution fromXYRes(int xRes, int yRes)
	{
		return fromNative(NativeMethods.xnResolutionGetFromXYRes(xRes, yRes));
	}
	
	private final int val;
	private final int xRes;
	private final int yRes;
	private final String name;
}
