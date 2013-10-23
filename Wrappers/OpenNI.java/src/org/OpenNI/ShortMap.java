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

import java.nio.ShortBuffer;

/**
 * Base class of all maps that store their data as shorts. <BR><BR>
 * 
 * Currently used as a base class for Depth, Scene and IR maps.
 *
 */
public class ShortMap extends Map 
{
	/**
	 * Default constructor.  Sets the bytes per pixel equal to the size of a short map, and
	 * does nothing else
	 */
	ShortMap()
	{
		super();
		this.setBytesPerPixel(BYTES_PER_PIXEL);
	}
	
	/**
	 * Calls parent constructor to create a ShortMap of specified x and y resolution, with
	 * specified pointer value, and each pixel with the same bit depth as a short int
	 * @param ptr Pointer to the resulting map
	 * @param xRes Horizontal resolution of the map, in pixels
	 * @param yRes Vertical resolution of the map, in pixels
	 */
	ShortMap(long ptr, int xRes, int yRes)
	{
		super(ptr, xRes, yRes, BYTES_PER_PIXEL);
	}
	
	/**
	 * Provides access to the value of a single pixel by passing that data from native code.
	 * @param x X coordinate of desired pixel
	 * @param y Y coordinate of the desired pixel
	 * @return value stored in that pixel
	 */ 
	public short readPixel(int x, int y)
	{
		return NativeMethods.readShort(getPixelPtr(x, y));
	}
	
	/**
	 * Creates a buffer suitable for storing a ShortMap
	 * @return The buffer created
	 */
	public ShortBuffer createShortBuffer()
	{
		return createByteBuffer().asShortBuffer();
	}
	
	private static final int BYTES_PER_PIXEL = Short.SIZE / 8;
}
