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
 * Stores the orientation and position of a skeleton joint. <BR><BR>
 *
 */
public class SkeletonJointTransformation
{
	/**
	 * Creates a SkeletonJointTransformation with the given Orientation and Position.
	 * @param position Position of the joint
	 * @param orientation Orientation of the joint
	 */
	public SkeletonJointTransformation(SkeletonJointPosition position, SkeletonJointOrientation orientation)
	{
		this.position = position;
		this.orientation = orientation;
	}
	
	/**
	 * Provides access to the current joint position.
	 * @return Current joint position
	 */
	public SkeletonJointPosition getPosition()
	{
		return position;
	}
	/**
	 * Provides access to the current joint orientation
	 * @return Current joint orientation
	 */
	public SkeletonJointOrientation getOrientation()
	{
		return orientation;
	}
	
	private SkeletonJointPosition position;
	private SkeletonJointOrientation orientation;

}
