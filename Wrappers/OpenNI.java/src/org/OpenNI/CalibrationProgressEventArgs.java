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
 * Holds the data created by a Skeleton Calibration Progress Event. <BR><BR>
 * 
 * Indicates which user is being calibrated, and which state (from a list) that that 
 * calibration process is currently in.  This is a read only data type once it is 
 * created.  
 *
 */
public class CalibrationProgressEventArgs extends EventArgs
{
	/**
	 * Creates a new object with specified userID and calibration state
	 * @param user User ID of the user being calibrated
	 * @param state Current calibration state code of the calibration process
	 */
	public CalibrationProgressEventArgs(int user, CalibrationProgressStatus state)
	{
		this.user = user;
		this.state = state;
	}
	
	/** 
	 * Getter function for the user ID
	 * @return User ID of the user this event refers to
	 */
	public int getUser()
	{
		return this.user;
	}
	/**
	 * Getter function for calibration state
	 * @return Code for the current state of calibration
	 */
	public CalibrationProgressStatus getStatus()
	{
		return this.state;
	}
	
	private final int user;
	private final CalibrationProgressStatus state;
}
