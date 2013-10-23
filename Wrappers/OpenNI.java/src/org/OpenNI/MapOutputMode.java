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
 * Represents the output settings for a map generator. <BR><BR>
 * 
 * This class is intended to contain the resolution and frame rate settings
 * used by a MapGenerator.  It is used when configuring a MapGenerator, 
 * querying available settings, and querying current settings.
 *
 */
public class MapOutputMode 
{
	/**
	 * Creates a new MapOutputMode object with specified Resolution and frame rate
	 * @param xRes Horizontal resolution in pixels
	 * @param yRes Vertical resolution in pixels
	 * @param FPS Frame rate, measured in frames per second
	 */
	public MapOutputMode(int xRes, int yRes, int FPS)
	{
		this.xRes = xRes;
		this.yRes = yRes;
		this.FPS = FPS;
	}
	
	/**
	 * Setter function for the horizontal resolution
	 * @param xRes Desired new horizontal resolution in pixels
	 */
	public void setXRes(int xRes)
	{
		this.xRes = xRes;
	}

	/**
	 * Setter function for the vertical resolution
	 * @param yRes Desired new vertical resolution in pixels
	 */
	public void setYRes(int yRes)
	{
		this.yRes = yRes;
	}

	/**
	 * Setter function for the frame rate
	 * @param fPS Desired new frame rate in frames per second
	 */
	public void setFPS(int fPS)
	{
		this.FPS = fPS;
	}

	/**
	 * Getter function for the horizontal resolution
	 * @return Current horizontal resolution in pixels
	 */
	public int getXRes() { return this.xRes; }
	/**
	 * Getter function for the vertical resolution
	 * @return Current vertical resolution in pixels
	 */
	public int getYRes() { return this.yRes; }
	/**
	 * Getter function for the frame rate
	 * @return Desired frame rate, in frames per second
	 */
	public int getFPS() { return this.FPS; }
	
	private int xRes;
	private int yRes;
	private int FPS;
}
