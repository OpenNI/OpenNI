package org.OpenNI;

import java.nio.ShortBuffer;

public class ShortMap extends Map 
{
	ShortMap()
	{
		super();
		this.setBytesPerPixel(BYTES_PER_PIXEL);
	}
	
	ShortMap(long ptr, int xRes, int yRes)
	{
		super(ptr, xRes, yRes, BYTES_PER_PIXEL);
	}
	
	public short readPixel(int x, int y)
	{
		return NativeMethods.readShort(getPixelPtr(x, y));
	}
	
	public ShortBuffer createShortBuffer()
	{
		return createByteBuffer().asShortBuffer();
	}
	
	private static final int BYTES_PER_PIXEL = Short.SIZE / 8;
}
