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

public class Cropping 
{
	public Cropping(int xOffset, int yOffset, int xSize, int ySize, boolean enabled) 
	{
		super();
		this.xOffset = xOffset;
		this.yOffset = yOffset;
		this.xSize = xSize;
		this.ySize = ySize;
		this.enabled = enabled;
	}

	public int getXOffset() 
	{
		return xOffset;
	}
	public void setXOffset(int xOffset) 
	{
		this.xOffset = xOffset;
	}
	public int getYOffset() 
	{
		return yOffset;
	}
	public void setYOffset(int yOffset) 
	{
		this.yOffset = yOffset;
	}
	public int getXSize() 
	{
		return xSize;
	}
	public void setXSize(int xSize) 
	{
		this.xSize = xSize;
	}
	public int getYSize() 
	{
		return ySize;
	}
	public void setYSize(int ySize) 
	{
		this.ySize = ySize;
	}
	public boolean isEnabled() 
	{
		return enabled;
	}
	public void setEnabled(boolean enabled) 
	{
		this.enabled = enabled;
	}
	
	private int xOffset;
	private int yOffset;
	private int xSize;
	private int ySize;
	private boolean enabled;
}
