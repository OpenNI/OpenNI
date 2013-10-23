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
 * Provides string names for all available output resolution codes. <BR><BR>
 *
 */
public enum Resolution 
{
	CUSTOM (0),
	QQVGA (1),
	CGA (2),
	QVGA (3),
	VGA (4),
	SVGA (5),
	XGA (6),
	P720 (7),
	SXGA (8),
	UXGA (9),
	P1080 (10);
	
	Resolution(int val)
	{
		this.val = val;
		this.xRes = NativeMethods.xnResolutionGetXRes(val);
		this.yRes = NativeMethods.xnResolutionGetYRes(val);
		this.name = NativeMethods.xnResolutionGetName(val);
	}
	
	public int getxRes()
	{
		return this.xRes;
	}

	public int getyRes()
	{
		return this.yRes;
	}

	public String getName()
	{
		return this.name;
	}
	
	public int toNative() { return this.val; }
	
	public static Resolution fromNative(int value)
	{
		for (Resolution type : Resolution.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	public static Resolution fromName(String name)
	{
		return fromNative(NativeMethods.xnResolutionGetFromName(name));
	}
	
	public static Resolution fromXYRes(int xRes, int yRes)
	{
		return fromNative(NativeMethods.xnResolutionGetFromXYRes(xRes, yRes));
	}
	
	private final int val;
	private final int xRes;
	private final int yRes;
	private final String name;
}
