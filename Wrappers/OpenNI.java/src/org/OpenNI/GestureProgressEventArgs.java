package org.OpenNI;

public class GestureProgressEventArgs extends EventArgs
{
	public GestureProgressEventArgs(String gesture, Point3D position , float progress)
	{
		this.gesture = gesture;
		this.position = position;
		this.progress = progress;
	}
	
	public String getGesture()
	{
		return this.gesture;
	}
	public Point3D getPosition()
	{
		return this.position;
	}
	public float getProgress()
	{
		return this.progress;
	}
	
	private final String gesture;
	private final Point3D position;
	private final float progress;
}
