package org.OpenNI;

public class Cropping 
{
	public Cropping(int xOffset, int yOffset, int xSize, int ySize, boolean enabled) 
	{
		super();
		this.xOffset = xOffset;
		this.yOffset = yOffset;
		this.xSize = xSize;
		this.ySize = ySize;
		this.enabled = enabled;
	}

	public int getXOffset() 
	{
		return xOffset;
	}
	public void setXOffset(int xOffset) 
	{
		this.xOffset = xOffset;
	}
	public int getYOffset() 
	{
		return yOffset;
	}
	public void setYOffset(int yOffset) 
	{
		this.yOffset = yOffset;
	}
	public int getXSize() 
	{
		return xSize;
	}
	public void setXSize(int xSize) 
	{
		this.xSize = xSize;
	}
	public int getYSize() 
	{
		return ySize;
	}
	public void setYSize(int ySize) 
	{
		this.ySize = ySize;
	}
	public boolean isEnabled() 
	{
		return enabled;
	}
	public void setEnabled(boolean enabled) 
	{
		this.enabled = enabled;
	}
	
	private int xOffset;
	private int yOffset;
	private int xSize;
	private int ySize;
	private boolean enabled;
}
