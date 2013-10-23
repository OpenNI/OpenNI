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
 *  Defines various values needed to interact with Skeleton Profiles
 */

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
