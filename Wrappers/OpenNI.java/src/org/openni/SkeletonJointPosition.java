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
 * Stores the position of one skeleton joint in 3-space. <BR><BR>
 * 
 * Also stores a  
 * confidence value that allows skeleton middleware writers to quantify the 
 * uncertainty of the calculation.
 *
 */
public class SkeletonJointPosition
{
	/**
	 * Constructor to create a new joint position at a given point and confidence
	 * @param position Position of the Joint
	 * @param confidence Confidence of this value
	 */
	public SkeletonJointPosition(Point3D position, float confidence)
	{
		this.position = position;
		this.confidence = confidence;
	}
	
	/** 
	 * Getter function for the joint position
	 * @return The position of the joint
	 */
	public Point3D getPosition()
	{
		return this.position;
	}
	/**
	 * Confidence value for this joint value.  The exact scale and meaning of this value
	 * is determined by the skeleton middleware implementer
	 * @return
	 */
	public float getConfidence()
	{
		return this.confidence;
	}
	
	private Point3D position;
	private float confidence;
}
