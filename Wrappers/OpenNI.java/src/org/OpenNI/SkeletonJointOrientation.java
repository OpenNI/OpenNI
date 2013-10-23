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
 * Represents the orientation of a skeleton joint as two vectors with a common origin. <BR><BR>
 * 
 * One point is given as the origin.  Two more points each define a vector when joined with the first point.  
 * A confidence value is also stored enabling skeleton middleware to quantify uncertainty in the values.
 *
 */
public class SkeletonJointOrientation
{
	/** Create a new skeleton joint orientation, given 3 points in 3-space, and a confidence value
	 * @param x1 value for x1
	 * @param y1 value for y1
	 * @param z1 value for z1
	 * @param x2 value for x2
	 * @param y2 value for y2
	 * @param z2 value for z2
	 * @param x3 value for x3
	 * @param y3 value for y3
	 * @param z3 value for z3
	 * @param confidence Confidence value
	 */
	public SkeletonJointOrientation(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float confidence)
	{
		this.x1 = x1;
		this.y1 = y1;
		this.z1 = z1;
		this.x2 = x2;
		this.y2 = y2;
		this.z2 = z2;
		this.x3 = x3;
		this.y3 = y3;
		this.z3 = z3;
		this.confidence = confidence;
	}
	
	/** 
	 * Getter function for X1
	 * @return Current value of X1
	 */
	public float getX1()
	{
		return this.x1;
	}
	/**
	 * Getter function for Y1
	 * @return Current value for Y1
	 */
	public float getY1()
	{
		return this.y1;
	}
	/** 
	 * Getter function for Z1
	 * @return Current value for Z1
	 */
	public float getZ1()
	{
		return this.z1;
	}
	/** 
	 * Getter function for X2
	 * @return Current value for X2
	 */
	public float getX2()
	{
		return this.x2;
	}
	/**
	 * Getter function for Y2
	 * @return Current value for Y2
	 */
	public float getY2()
	{
		return this.y2;
	}
	/**
	 * Getter function for Z2
	 * @return Current value for Z2
	 */
	public float getZ2()
	{
		return this.z2;
	}
	/** 
	 * Getter function for X3
	 * @return Current value for X3
	 */
	public float getX3()
	{
		return this.x3;
	}
	/**
	 * Getter function for Y3
	 * @return Current value for Y3
	 */
	public float getY3()
	{
		return this.y3;
	}
	/**
	 * Getter function for Z3
	 * @return Current value for Z3
	 */
	public float getZ3()
	{
		return this.z3;
	}
	/**
	 * Confidence value for the calculation of this joint's orientation.
	 * The exact scale of this value is defined by the skeleton middleware's
	 * implementer
	 * @return
	 */
	public float getConfidence()
	{
		return this.confidence;
	}

	private float x1;
	private float y1;
	private float z1;
	private float x2;
	private float y2;
	private float z2;
	private float x3;
	private float y3;
	private float z3;
	private float confidence;
}
