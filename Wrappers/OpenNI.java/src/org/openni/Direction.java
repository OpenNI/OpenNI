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

import java.util.NoSuchElementException;

/**
 * Provides string names for all six cardinal directions in 3-space. <BR><BR>
 * 
 * This is useful, for example, when looking at the direction a HandPoint is traveling.
 *
 */
public enum Direction 
{
	ILLEGAL(0),
	LEFT(1),
	RIGHT(2),
	UP(3),
	DOWN(4),
	FORWARD(5),
	BACKWARD(6);
	
	Direction(int val)
	{
		this.val = val;
	}
	
	public int toNative() { return this.val; }
	
	public static Direction fromNative(int value)
	{
		for (Direction type : Direction.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	private final int val;

}
