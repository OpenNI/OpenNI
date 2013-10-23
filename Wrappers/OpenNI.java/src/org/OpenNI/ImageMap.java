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

/**
 * Stores an RGB Image. <BR><BR>
 *
 */
public class ImageMap extends Map
{
	/**
	 * Creates a new empty ImageMap
	 */
	public ImageMap()
	{
	}

	/**
	 * Creates a new ImageMap with given native pointer, resolution, and bit depth
	 * @param ptr Native pointer to this object
	 * @param xRes Horizontal resolution of this map, in pixels
	 * @param yRes Vertical resolution of this map, in pixels
	 * @param bytesPerPixel Bit depth of this map, in bytes per pixel
	 */
	public ImageMap(long ptr, int xRes, int yRes, int bytesPerPixel)
	{
		super(ptr, xRes, yRes, bytesPerPixel);
	}
	
	/**
	 * Creates a buffer suitable for the storage of the data in this map
	 * @return
	 */
	public ByteBuffer createByteBuffer()
	{
		return super.createByteBuffer();
	}
}
