package org.OpenNI;

public class PoseDetectionEventArgs extends EventArgs
{
	public PoseDetectionEventArgs(String pose, int user)
	{
		this.pose = pose;
		this.user = user;
	}
	
	public String getPose()
	{
		return this.pose;
	}
	public int getUser()
	{
		return this.user;
	}
	
	private final String pose;
	private final int user;

}
