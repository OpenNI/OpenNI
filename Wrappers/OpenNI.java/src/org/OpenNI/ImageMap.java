package org.OpenNI;

import java.nio.ByteBuffer;

public class ImageMap extends Map
{
	public ImageMap()
	{
	}

	public ImageMap(long ptr, int xRes, int yRes, int bytesPerPixel)
	{
		super(ptr, xRes, yRes, bytesPerPixel);
	}
	
	public ByteBuffer createByteBuffer()
	{
		return super.createByteBuffer();
	}
}
