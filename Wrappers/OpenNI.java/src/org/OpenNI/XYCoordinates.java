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
 * Represents a pair of two dimensional integral coordinates. <BR><BR>  
 * 
 * Primarily useful to keep track of things measured in pixels.  This class doesn't include any kind
 * of internal consistency checking -- it is just a handy way to store two numbers.
 * 
 *
 */
public class XYCoordinates 
{
	/**
	 * Constructor to create a new object with specified initial coordinate values.
	 * @param x The desired X coordinate
	 * @param y The desired Y coordinate
	 */
	public XYCoordinates(int x, int y)
	{
		this.x = x;
		this.y = y;
	}
	
	/**
	 * Setter function for the X coordinate
	 * @param x Desired new value of the X coordinate
	 */
	public void setX(int x)
	{
		this.x = x;
	}

	/**
	 * Setter function for the Y coordinate
	 * @param y Desired new value of the Y coordinate
	 */
	public void setY(int y)
	{
		this.y = y;
	}

	/**
	 * Getter function for the X coordinate
	 * @return Current value of the X coordinate
	 */
	public int getX() { return this.x; }
	/**
	 * Getter function for the Y coordinate
	 * @return Current value of the Y coordinate
	 */
	public int getY() { return this.y; }
	
	private int x;
	private int y;
}
