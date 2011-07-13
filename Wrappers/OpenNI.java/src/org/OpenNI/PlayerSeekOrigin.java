package org.OpenNI;

import java.util.NoSuchElementException;

public enum PlayerSeekOrigin
{
	SET (0),
	CURRENT (1),
	END (2);
	
	PlayerSeekOrigin(int val)
	{
		this.val = val;
	}
	
	public int toNative() { return this.val; }
	
	public static PlayerSeekOrigin fromNative(int value)
	{
		for (PlayerSeekOrigin type : PlayerSeekOrigin.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	private final int val;

}
