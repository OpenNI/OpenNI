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
 * Represents a box in 3D space. <BR><BR>
 * 
 * Box is oriented parallel to the X, Y, and Z axis of the scene.  Dimensions and position of the box
 * are represented by storing two vertices of the box.  
 *
 */
public class BoundingBox3D
{
	/** 
	 * Constructor, creates a BoundingBox3D object from two opposite points
	 * @param leftBottomNear The bottom left vertex of the box, nearest to the camera
	 * @param rightTopFar The top right vertex of the box, furthest from the camera
	 */
	public BoundingBox3D(Point3D leftBottomNear, Point3D rightTopFar)
	{
		this.leftBottomNear = leftBottomNear;
		this.rightTopFar = rightTopFar;
	}

	/**
	 * Getter function, returns the vertex with the smallest X, Y, and Z coordinates
	 * @return Left bottom near vertex
	 */
	public Point3D getLeftBottomNear()
	{
		return this.leftBottomNear;
	}
	/**
	 * Getter function, returns the vertex with the largest X, Y, and Z coordinates
	 * @return Right top far vertex
	 */
	public Point3D getRightTopFar()
	{
		return this.rightTopFar;
	}
	
	/**
	 * Returns the vertex with the smallest X, Y, and Z coordinates<BR><BR>
	 * Calling this function is the same as calling getLeftBottomNear()
	 * @return Left bottom near vertex
	 */
	public Point3D getMins()
	{
		return getLeftBottomNear();
	}
	/**
	 * Returns the vertex with the largest X, Y, and Z coordinates<BR><BR>
	 * Calling this function is the same as calling getRightTopFar()
	 * @return Right top far vertex
	 */
	public Point3D getMaxs()
	{
		return getRightTopFar();
	}
	
	private final Point3D leftBottomNear;
	private final Point3D rightTopFar;
}
