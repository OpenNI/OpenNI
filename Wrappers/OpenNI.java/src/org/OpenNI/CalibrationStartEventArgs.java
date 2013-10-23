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
 * Holds the data generated when a Calibration Start Event is created. <BR><BR>
 * 
 * This data currently consists only of hte user ID of the user being calibrated.
 *
 */
public class CalibrationStartEventArgs extends EventArgs
{
	/**
	 * Constructor -- creates a new object with specified user id stored
	 * @param user
	 */
	CalibrationStartEventArgs(int user)
	{
		this.user = user;
	}
	
	/**
	 * Getter function for the user ID
	 * @return ID of the user that triggered this event
	 */
	public int getUser()
	{
		return this.user;
	}
	
	private int user;
	

}
