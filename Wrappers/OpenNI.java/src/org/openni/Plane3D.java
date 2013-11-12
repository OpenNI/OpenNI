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
 * Represents a two dimensional plane in three dimensional space. <BR><BR>
 * 
 * Stores the plane as a single point on the plane, plus a vector normal to 
 * that plane.<BR><BR>
 * 
 * This is a read only data type after it is created.
 *
 */
public class Plane3D
{
	/**
	 * Constructor that creates a new plane based on a point on that plane and a
	 * vector normal to the plane
	 * @param normal Vector normal to the plane, represented using a Point3D
	 * @param point Point on the plane
	 */
	public Plane3D(Point3D normal, Point3D point)
	{
		this.normal = normal;
		this.point = point;
	}
	
	/**
	 * Getter function for the normal to the plane
	 * @return Vector normal to the plane, represented as a Point3D
	 */
	public Point3D getNormal()
	{
		return this.normal;
	}
	/**
	 * Getter function for a point on the plane
	 * @return A point on the plane
	 */
	public Point3D getPoint()
	{
		return this.point;
	}
	
	
	private final Point3D normal;
	private final Point3D point;

}
