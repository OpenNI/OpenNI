package org.OpenNI;

public class GestureRecognizedEventArgs extends EventArgs
{
	public GestureRecognizedEventArgs(String gesture, Point3D idPosition, Point3D endPosition)
	{
		this.gesture = gesture;
		this.idPosition = idPosition;
		this.endPosition = endPosition;
	}
	
	public String getGesture()
	{
		return this.gesture;
	}
	public Point3D getIdPosition()
	{
		return this.idPosition;
	}
	public Point3D getEndPosition()
	{
		return this.endPosition;
	}
	
	private final String gesture;
	private final Point3D idPosition;
	private final Point3D endPosition;

}
