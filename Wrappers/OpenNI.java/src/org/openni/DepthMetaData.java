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
 * Stores a single frame DepthMap along with relevant meta data. <BR><BR>
 * 
 * Note that despite its name, this function stores the actual depth data
 * as well as associated meta data -- it does not just store meta data.
 *
 */
public class DepthMetaData extends MapMetaData 
{
	/**
	 * Default constructor, creates an empty DepthMap 
	 */
	public DepthMetaData() 
	{
		super(PixelFormat.GRAYSCALE_16BIT, new DepthMap());
	}
	
	/**
	 * Returns the Z resolution of the device - defined as the number
	 * of discrete depth values the device is capable of displaying.
	 * Should be equal to DepthGenerator::GetDeviceMaxDepth()
	 * @return
	 */
	public int getZRes() {
		return zRes;
	}

	/**
	 * Stores a new value in the zRes parameter of this class.  Note that 
	 * this does NOT reconfigure the device -- this is a reference value 
	 * used for calculations.  This value should only be set to the current 
	 * value returned by DepthGenerator::GetDeviceMaxDepth()
	 * @param zRes
	 */
	public void setZRes(int zRes) {
		this.zRes = zRes;
	}
	
	/**
	 * Returns a copy of the actual depth data for this frame
	 * @return Copy of the depth map
	 */
	public DepthMap getData()
	{
		return (DepthMap)super.getData();
	}
	
	private int zRes;
}
