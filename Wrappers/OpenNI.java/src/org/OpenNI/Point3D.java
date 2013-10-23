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
 * Represents a single 3D point. <BR><BR>
 * 
 * Coordinates may be in either Projective or Real World format.
 * There is no internal representation of coordinate type -- it is the responsibility of the application
 * to keep track of what format the point is in.  The Point3D object simply stores the coordinate values 
 * as numbers, with no attempt made to interpret them according to type.<BR><BR>
 * 
 * The common conventions for storing coordinates is as follows:<BR><BR>
 * 
 * If using Projective coordinates (the default for reading depth map from the PrimeSense sensor) X and Y represent 
 * pixels.  X is the horizontal pixel number, with far left pixel counted as Zero.  Y is vertical pixel with 
 * Top pixel counted as zero.  Z is distance from sensor to observed object, along a line that passes through
 * the pixel represented by X and Y.<BR><BR>
 * 
 * If using real world coordinates, X is distance from the sensor in mm left is increasingly positive (from sensor perspective),
 * Y is distance up or down from sensor in mm, with up being increasingly positive.  Z is distance in mm on a line perpendicular to X/Y,
 * with 0 representing contact with the sensor, and moving away from the sensor represented as increasingly positive.
 */
public class Point3D
{
	/**
	 * Constructor to create new object from 3 arbitrary floating point numbers.
	 * @param x X coordinate of point to be created
	 * @param y Y coordinate of point to be created
	 * @param z Z coordinate of point to be created
	 */
	public Point3D(float x, float y, float z)
	{
		this.X = x;
		this.Y = y;
		this.Z = z;
	}
	/**
	 * Default constructor -- creates a point with all three coordinates set to 0.0
	 */
	public Point3D()
	{
		this(0f,0f,0f);
	}
	/**
	 * Sets all coordinates for this point to values given
	 * @param x New X coordinate of the point
	 * @param y New Y coordinate of the point
	 * @param z New Z coordinate of the point
	 */
	public void setPoint(float x, float y, float z)
	{
		this.X = x;
		this.Y = y;
		this.Z = z;
	}

	/**
	 * Getter function for the X coordinate
	 * @return Current value of the X coordinate
	 */
	public float getX()
	{
		return X;
	}
	/**
	 * Getter function for the Y coordinate
	 * @return Current value of the Y coordinate
	 */
	public float getY()
	{
		return Y;
	}
	/** 
	 * Getter function for the Z coordinate
	 * @return Current value of the Z coordinate
	 */
	public float getZ()
	{
		return Z;
	}
	
	
	private float X;
	private float Y;
	private float Z;
}
