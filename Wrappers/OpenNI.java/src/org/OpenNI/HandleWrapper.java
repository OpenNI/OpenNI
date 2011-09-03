package org.OpenNI;

public class HandleWrapper 
{
	public HandleWrapper(int handle)
	{
		this.handle = handle;
	}
	
	public int toNative() { return this.handle; }
	
	private int handle;
}
