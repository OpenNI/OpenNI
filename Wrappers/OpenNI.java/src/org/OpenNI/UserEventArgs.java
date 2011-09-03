package org.OpenNI;

public class UserEventArgs extends EventArgs
{
	public UserEventArgs(int id)
	{
		this.id = id;
	}
	
	public int getId()
	{
		return this.id;
	}
	
	private final int id;
}
