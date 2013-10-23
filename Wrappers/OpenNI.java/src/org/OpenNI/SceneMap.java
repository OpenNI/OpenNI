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
 * Map type that holds the results of SceneAnalysis. <BR><BR>
 * 
 * This data type holds the output map from a SceneAnalyzer.  It is very similar to its
 * parent class (ShortMap)
 *
 */
public class SceneMap extends ShortMap
{
	/**
	 * Default constructor, simply calls the parent class default.  This sets the bit depth equal to the 
	 * number of bits in a short int on this system, and does nothing else.
	 */
	SceneMap()
	{
		super();
	}
	/** Creates a SceneMap (essentially the same as a short map) with specified x and y resolution, and the 
	 * specified pointer
	 * @param ptr Pointer to the map created
	 * @param xres Horizontal resolution of the map
	 * @param yres Vertical resolution of the map
	 */
	SceneMap(long ptr, int xres, int yres)
	{
		super(ptr, xres, yres);
	}

}
