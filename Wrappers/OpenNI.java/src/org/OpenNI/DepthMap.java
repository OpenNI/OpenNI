package org.OpenNI;

public class DepthMap extends ShortMap 
{
	DepthMap()
	{
		super();
	}
	
	DepthMap(long ptr, int xRes, int yRes)
	{
		super(ptr, xRes, yRes);
	}
}
