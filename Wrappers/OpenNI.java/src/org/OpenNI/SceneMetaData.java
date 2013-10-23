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
 * Stores a SceneMap, along with associated metadata. <BR><BR>
 * 
 * Intended to store a SceneMap along with any required metadata about that map.  Currently,
 * there is no data stored other than the map itself and its pixel depth
 *
 */
public class SceneMetaData extends MapMetaData
{
	/**
	 * Default Constructor -- Creates a new SceneMetaData object, including initiating a SceneMap,
	 * and storing the appropriate pixel format.
	 */
	public SceneMetaData() 
	{
		super(PixelFormat.GRAYSCALE_16BIT, new SceneMap());
	}
	/**
	 * Returns the scene map associated with this SceneMetaData object
	 * @return The SceneMap stored by the SceneMetaData
	 */
	public SceneMap getData()
	{
		return (SceneMap)super.getData();
	}

}
