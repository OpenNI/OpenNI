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
 * Provides string names values for all pixel format codes. <BR><BR>
 *
 */
public enum PixelFormat 
{
	RGB24 (1),
	YUV422 (2),
	GRAYSCALE_8BIT (3),
	GRAYSCALE_16BIT (4);
	
	PixelFormat(int val)
	{
		this.val = val;
	}
	
	public int toNative() { return this.val; }
	
	public int getBytesPerPixel()
	{
		return NativeMethods.xnGetBytesPerPixelForPixelFormat(this.val);
	}
	
	public static PixelFormat fromNative(int value)
	{
		for (PixelFormat type : PixelFormat.values()) 
		{
			if (type.val == value)
				return type;
		}
		
		throw new NoSuchElementException();
	}
	
	private int val;
}
