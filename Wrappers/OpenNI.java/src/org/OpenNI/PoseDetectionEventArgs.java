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
 * Stores the arguments from a Pose Detection Event. <BR><BR>
 * 
 *
 */
public class PoseDetectionEventArgs extends EventArgs
{
	/**
	 * Creates a new PoseDetectionEvent with specified pose and user
	 * @param pose Pose value to store
	 * @param user User value to store
	 */
	public PoseDetectionEventArgs(String pose, int user)
	{
		this.pose = pose;
		this.user = user;
	}
	
	/**
	 * Returns the Pose value stored by this object
	 * @return Pose value
	 */
	public String getPose()
	{
		return this.pose;
	}
	/**
	 * Returns the UserID stored by this object
	 * @return User ID
	 */
	public int getUser()
	{
		return this.user;
	}
	
	private final String pose;
	private final int user;

}
