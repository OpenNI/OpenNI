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
 * Stores a single frame of depth map data. <BR><BR>
 * 
 * This class is normally used as a member of the DepthMetaData class
 * in order to provide resolution information.  It is a good practice to
 * make use of the metadata information provided with each frame so that
 * the application is robust to changes in hardware output resolution.
 *
 */
public class DepthMap extends ShortMap 
{
	/**
	 * Default constructor -- creates a new empty DepthMap
	 */
	DepthMap()
	{
		super();
	}
	
	/**
	 * Creates a depth map with given native pointer, and resolution
	 * @param ptr native pointer to created depth map
	 * @param xRes horizontal resolution of created depth map
	 * @param yRes vertical resolution of created depth map
	 */
	DepthMap(long ptr, int xRes, int yRes)
	{
		super(ptr, xRes, yRes);
	}
}
