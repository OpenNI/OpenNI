package org.OpenNI;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Map 
{
	Map()
	{
	}
	
	Map(long ptr, int xRes, int yRes, int bytesPerPixel)
	{
		this.ptr = ptr;
		this.xRes = xRes;
		this.yRes = yRes;
		this.bytesPerPixel = bytesPerPixel;
	}
	
	public long getNativePtr()
	{
		return this.ptr;
	}
	
	public void setNativePtr(long ptr)
	{
		this.ptr = ptr;
	}
	
	protected ByteBuffer createByteBuffer()
	{
		int size = this.xRes * this.yRes * this.bytesPerPixel;
		ByteBuffer buffer = ByteBuffer.allocateDirect(size);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		NativeMethods.copyToBuffer(buffer, this.ptr, size);
		return buffer;
	}
	
	/**
	 * Copies the data to a preallocated buffer.
	 * 
	 * @param buffer preallocated ByteBuffer with size greater than or equal to 
	 *     the number of bytes need to be copied.
	 * @param size number of bytes to copy.
	 */
	public void copyToBuffer(ByteBuffer buffer, int size) {
	  NativeMethods.copyToBuffer(buffer, this.ptr, size);
	}

	protected long getPixelPtr(int x, int y) 
	{ 
		return this.ptr + (y * this.xRes + x) * this.bytesPerPixel; 
	} 

	public int getXRes() {
		return xRes;
	}

	public void setXRes(int xRes) {
		this.xRes = xRes;
	}

	public int getYRes() {
		return yRes;
	}

	public void setYRes(int yRes) {
		this.yRes = yRes;
	}

	public int getBytesPerPixel() {
		return bytesPerPixel;
	}

	protected void setBytesPerPixel(int bytesPerPixel) {
		this.bytesPerPixel = bytesPerPixel;
	}

	protected long ptr;
	protected int xRes;
	protected int yRes;
	protected int bytesPerPixel;
}
