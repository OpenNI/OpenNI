package org.OpenNI;

public class CalibrationStartEventArgs extends EventArgs
{
	CalibrationStartEventArgs(int user)
	{
		this.user = user;
	}
	
	public int getUser()
	{
		return this.user;
	}
	
	private int user;
	

}
