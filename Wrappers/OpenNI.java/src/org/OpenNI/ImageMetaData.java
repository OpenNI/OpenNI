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
 * Bundles an ImageMap with appropriate meta data. <BR><BR>
 *
 */
public class ImageMetaData extends MapMetaData
{
	/**
	 * Default constructor, creates a new ImageMetaData object with 24 bits per pixel
	 */
	public ImageMetaData()
	{
		super(PixelFormat.RGB24, new ImageMap());
		setPixelFormat(PixelFormat.RGB24);
	}

	/**
	 * Sets the pixel format of this object
	 * @param format Desired new format
	 */
	public void setPixelFormat(PixelFormat format)
	{
		super.setPixelFormat(format);
	}
	
	/**
	 * Provides access to this objects map data
	 * @return ImageMap stored by this object
	 */
	public ImageMap getData()
	{
		return (ImageMap)super.getData();
	}
}
