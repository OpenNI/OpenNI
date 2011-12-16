/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
package org.OpenNI;

import java.util.NoSuchElementException;

public enum SkeletonProfile
{
	/** No joints at all **/
	NONE(1),

	/** All joints **/
	ALL(2),
	
	/** All the joints in the upper body (torso and upwards) **/
	UPPER_BODY(3),
	
	/** All the joints in the lower body (torso and downwards) **/
	LOWER_BODY(4),
	
	/** The head and the hands **/
	HEAD_HANDS(5);
	
	SkeletonProfile(int val)
	{
		this.val = val;
	}
	
	public int toNative() { return this.val; }
	
	public static SkeletonProfile fromNative(int value)
	{
		for (SkeletonProfile type : SkeletonProfile.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	private final int val;
}
