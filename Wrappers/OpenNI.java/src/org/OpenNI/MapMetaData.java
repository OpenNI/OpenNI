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

/**
 * Base class for Depth, IR, and Image metadata objects. <BR><BR>
 *
 */
public abstract class MapMetaData extends OutputMetaData 
{
	/**
	 * Constructor, creates a new MapMetaData from a map and given
	 * pixel format 
	 * @param format Desired format of the new object 
	 * @param map Map that this meta data object will wrap
	 */
	MapMetaData(PixelFormat format, Map map)
	{
		this.pixelFormat = format;
		this.map = map;
	}
	
	/**
	 * Provides the current x resolution of this object's map, measured in pixels
	 * @return X resolution in pixels
	 */
	public int getXRes() {
		return xRes;
	}
	/**
	 * Sets the stored horizontal resolution of this object's map 
	 * @param xRes Desired X Resolution in pixels
	 */
	public void setXRes(int xRes) {
		this.xRes = xRes;
		this.map.setXRes(xRes);
	}
	/**
	 * Provides the current y resolution of this object's map, measured in pixels
	 * @return Y resolution in pixels
	 */
	public int getYRes() {
		return yRes;
	}
	/**
	 * Sets the stored vertical resolution of this object's map
	 * @param yRes Desired Y resolution in pixels
	 */
	public void setYRes(int yRes) {
		this.yRes = yRes;
		this.map.setYRes(yRes);
	}
	/** 
	 * Provides the current horizontal offset of this object's map
	 * @return x offset of the map in pixels
	 */
	public int getXOffset() {
		return xOffset;
	}
	/**
	 * Sets the stored horizontal offset of this map
	 * @param xOffset Desired new X offset value in pixels
	 */
	public void setXOffset(int xOffset) {
		this.xOffset = xOffset;
	}
	/**
	 * Provides the current vertical offset of this map
	 * @return Y offset of the map in pixels
	 */
	public int getYOffset() {
		return yOffset;
	}
	/**
	 * Sets the current vertical offset of this map
	 * @param yOffset Desired new y offset value in pixels
	 */
	public void setYOffset(int yOffset) {
		this.yOffset = yOffset;
	}
	/**
	 * Provides the full, non-windowed horizontal resolution of this map
	 * @return X resolution in pixels
	 */
	public int getFullXRes() {
		return fullXRes;
	}
	/**
	 * Sets the full, non-windowed horizontal resolution stored for this map
	 * @param fullXRes Desired new X resolution value in pixels
	 */
	public void setFullXRes(int fullXRes) {
		this.fullXRes = fullXRes;
	}
	/**
	 * Gives the full, non-windowed vertical resolution stored for this map
	 * @return Y resolution in pixels
	 */
	public int getFullYRes() {
		return fullYRes;
	}
	/** 
	 * Sets the full, non-windowed Y resolution stored for this map
	 * @param fullYRes Desired new Y resolution value in pixels
	 */
	public void setFullYRes(int fullYRes) {
		this.fullYRes = fullYRes;
	}
	/**
	 * Provides the PixelFormat used by this object's map
	 * @return Current PixelFormat
	 */
	public PixelFormat getPixelFormat() {
		return pixelFormat;
	}
	/**
	 * Provides the current frame rate of the generator that made this map
	 * @return Current frame rate, in frames per second
	 */
	public int getFPS() {
		return FPS;
	}
	
	/**
	 * Sets the stored value for the frame rate of the generator that made this map
	 * @param fPS New frame rate value, measured in frames per second
	 */
	public void setFPS(int fPS) {
		FPS = fPS;
	}
	/** Provides the map stored by this object
	 * @return Current map
	 */
	public Map getData() {
		return this.map;
	}
	
	/**
	 * Provides the native pointer to this object
	 * @param ptr Pointer to the object, stored as a long integer
	 */
	@Override
	public void setDataPtr(long ptr) {
		super.setDataPtr(ptr);
		this.map.setNativePtr(ptr);
	}
	
	/**
	 * Sets a new pixel format for this object
	 * @param format New value for the pixel format
	 */
	protected void setPixelFormat(PixelFormat format)
	{
		this.pixelFormat = format;
		this.map.setBytesPerPixel(format.getBytesPerPixel());
	}
	
	private int xRes;
	private int yRes;
	private int xOffset;
	private int yOffset;
	private int fullXRes;
	private int fullYRes;
	private PixelFormat pixelFormat;
	private int FPS;
	private Map map;
}
