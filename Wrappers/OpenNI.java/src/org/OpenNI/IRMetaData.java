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
 * Holds an IRMap along with all relevant meta data about that map. <BR><BR>
 * 
 * Currently, the only additional data beside the map itself is the pixel format.
 *
 */
public class IRMetaData extends MapMetaData
{
	/**
	 * Default constructor.  Creates an empty IRMetaData object, including an empty IRMap, and 
	 * sets the pixel format to the appropriate value
	 */
	public IRMetaData()
	{
		super(PixelFormat.GRAYSCALE_16BIT, new IRMap());
	}
	
	/**
	 * Getter function for the IRMap.  Provides access to the IRMap contained by this object.
	 * @return IRMap stored by this object
	 */
	public IRMap getData()
	{
		return (IRMap)super.getData();
	}
}
