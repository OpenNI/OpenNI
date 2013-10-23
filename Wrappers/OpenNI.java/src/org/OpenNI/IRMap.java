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
 * Stores a 2D Infrared Image. <BR><BR>
 *
 */
public class IRMap extends ShortMap
{
	/**
	 * Default constructor, creates a new empty IR map
	 */
	IRMap()
	{
		super();
	}
	
	/**
	 * Constructor, creates a new IR map with given native pointer and resolution
	 * @param ptr Native pointer of this object
	 * @param xRes Horizontal resolution of this map
	 * @param yRes Vertical resolution of this map
	 */
	IRMap(long ptr, int xRes, int yRes)
	{
		super(ptr, xRes, yRes);
	}
}
