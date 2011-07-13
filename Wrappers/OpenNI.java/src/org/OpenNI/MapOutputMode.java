package org.OpenNI;

public class MapOutputMode 
{
	public MapOutputMode(int xRes, int yRes, int FPS)
	{
		this.xRes = xRes;
		this.yRes = yRes;
		this.FPS = FPS;
	}
	
	public void setXRes(int xRes)
	{
		this.xRes = xRes;
	}

	public void setYRes(int yRes)
	{
		this.yRes = yRes;
	}

	public void setFPS(int fPS)
	{
		this.FPS = fPS;
	}

	public int getXRes() { return this.xRes; }
	public int getYRes() { return this.yRes; }
	public int getFPS() { return this.FPS; }
	
	private int xRes;
	private int yRes;
	private int FPS;
}
