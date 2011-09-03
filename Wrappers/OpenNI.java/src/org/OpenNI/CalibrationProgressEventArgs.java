package org.OpenNI;

public class CalibrationProgressEventArgs extends EventArgs
{
	public CalibrationProgressEventArgs(int user, CalibrationProgressStatus state)
	{
		this.user = user;
		this.state = state;
	}
	
	public int getUser()
	{
		return this.user;
	}
	public CalibrationProgressStatus getStatus()
	{
		return this.state;
	}
	
	private final int user;
	private final CalibrationProgressStatus state;
}
