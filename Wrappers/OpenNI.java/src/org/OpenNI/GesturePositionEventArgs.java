package org.OpenNI;

public class GesturePositionEventArgs extends EventArgs
{
	public GesturePositionEventArgs(String gesture, Point3D position)
	{
		this.gesture = gesture;
		this.position = position;
	}
	
	public String getGesture()
	{
		return this.gesture;
	}
	public Point3D getPosition()
	{
		return this.position;
	}
	
	private final String gesture;
	private final Point3D position;

}
