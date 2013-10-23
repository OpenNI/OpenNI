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
 * Stores the data created when a Pose Detection In Progress event is generated. <BR><BR>
 * 
 * This data consists of the pose that was detected, the user ID of the user performing the pose,
 * and the status of the pose detection process.  This event is triggered when the system is still
 * in the process of detecting the pose, and is most useful for providing feedback to the user to 
 * continue performing the pose until it is fully detected.
 *
 */
public class PoseDetectionInProgressEventArgs extends EventArgs
{
	/**
	 * Constructor -- creates an object with the specified pose type, user ID, and status
	 * @param pose String name of the pose type detected
	 * @param user User ID of the user performing the pose
	 * @param status Status code that shows how far along the system is in detecting the pose
	 */
	public PoseDetectionInProgressEventArgs(String pose, int user, PoseDetectionStatus status)
	{
		this.pose = pose;
		this.user = user;
		this.status = status;
	}
	
	/**
	 * Getter function for the pose type
	 * @return Returns the string name of the pose detected
	 */
	public String getPose()
	{
		return this.pose;
	}
	/** Getter function for the user ID
	 * @return User ID of the user performing the pose
	 */
	public int getUser()
	{
		return this.user;
	}
	/** Getter function for the status code
	 * @return Status code indicating how much of the pose has been performed
	 */
	public PoseDetectionStatus getStatus()
	{
		return this.status;
	}
	
	private final String pose;
	private final int user;
	private final PoseDetectionStatus status;
}
