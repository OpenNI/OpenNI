/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
package org.openni;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Base class of Depth, IR, Image, and Scene maps. <BR><BR>
 * 
 * This is the base class of all two dimensional image type maps.  This
 * class would not generally be instantiated on its own, but its member
 * functions might be called by users of its descendants.
 *
 */
public class Map 
{
	/**
	 * Default constructor.  Does nothing
	 */
	Map()
	{
	}
	
	/** 
	 * Constructor to create map with desired pointer value, resolution, and bit depth
	 * @param ptr Desired pointer value to point to this map
	 * @param xRes Desired horizontal resolution of the map
	 * @param yRes Desired vertical resolution of the map
	 * @param bytesPerPixel Desired bit depth in bytes per pixel for this map
	 */
	Map(long ptr, int xRes, int yRes, int bytesPerPixel)
	{
		this.ptr = ptr;
		this.xRes = xRes;
		this.yRes = yRes;
		this.bytesPerPixel = bytesPerPixel;
	}
	
	/** 
	 * Getter function for the native pointer to this object
	 * @return Pointer to this object suitable for use in native code
	 */
	public long getNativePtr()
	{
		return this.ptr;
	}
	
	/**
	 * Setter function for the native pointer to this object
	 * @param ptr Desired native pointer value for this object
	 */
	public void setNativePtr(long ptr)
	{
		this.ptr = ptr;
	}
	
	/**
	 * Allocates a buffer large enough to hold this map, taking into account
	 * bit depth and resolution.  Copies the actual data for this map from
	 * native code into this buffer.
	 * @return The created buffer
	 */
	protected ByteBuffer createByteBuffer()
	{
		int size = this.xRes * this.yRes * this.bytesPerPixel;
		ByteBuffer buffer = ByteBuffer.allocateDirect(size);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		NativeMethods.copyToBuffer(buffer, this.ptr, size);
		return buffer;
	}
	
	/**
	 * Copies data from native code to a buffer available in Java
	 * @param buffer The buffer to copy into 
	 * @param size Number of bytes to copy
	 */
	public void copyToBuffer(ByteBuffer buffer, int size)
	{
		NativeMethods.copyToBuffer(buffer, this.ptr, size);
	} 	
	
	/** Calculates a pointer to a given pixel, taking into account
	 * the bit depth and resolution of the function
	 * @param x X coordinate of desired pixel
	 * @param y Y coordinate of desired pixel
	 * @return Pointer to pixel suitable for use in native code, represented as a long integer
	 */
	protected long getPixelPtr(int x, int y) 
	{ 
		return this.ptr + (y * this.xRes + x) * this.bytesPerPixel; 
	} 

	/**
	 * Getter function for the horizontal resolution of this map
	 * @return Current vertical resolution of this map in pixels
	 */
	public int getXRes() {
		return xRes;
	}

	/**
	 * Setter function for the horizontal resolution of this map
	 * @param xRes Desired new horizontal resolution setting in pixels
	 */
	public void setXRes(int xRes) {
		this.xRes = xRes;
	}

	/**
	 * Getter function for vertical resolution of this map
	 * @return Current vertical resolution of this map
	 */
	public int getYRes() {
		return yRes;
	}

	/**
	 * Setter function for vertical resolution of this map
	 * @param yRes Desired new vertical resolution setting in pixels
	 */
	public void setYRes(int yRes) {
		this.yRes = yRes;
	}

	/**
	 * Getter function for bytes per pixel, usually representing color depth or
	 * depth resolution
	 * @return Current bytes per pixel setting
	 */
	public int getBytesPerPixel() {
		return bytesPerPixel;
	}

	/**
	 * Setter function for bytes per pixel, usually representing color depth or
	 * depth resolution.
	 * @param bytesPerPixel Desired number of bytes per pixel for this map
	 */
	protected void setBytesPerPixel(int bytesPerPixel) {
		this.bytesPerPixel = bytesPerPixel;
	}

	protected long ptr;
	protected int xRes;
	protected int yRes;
	protected int bytesPerPixel;
}
