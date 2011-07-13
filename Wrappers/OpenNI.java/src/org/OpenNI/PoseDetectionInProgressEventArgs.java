package org.OpenNI;

public class PoseDetectionInProgressEventArgs extends EventArgs
{
	public PoseDetectionInProgressEventArgs(String pose, int user, PoseDetectionStatus status)
	{
		this.pose = pose;
		this.user = user;
		this.status = status;
	}
	
	public String getPose()
	{
		return this.pose;
	}
	public int getUser()
	{
		return this.user;
	}
	public PoseDetectionStatus getStatus()
	{
		return this.status;
	}
	
	private final String pose;
	private final int user;
	private final PoseDetectionStatus status;
}
