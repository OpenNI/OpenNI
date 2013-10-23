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
 * Stores the data created when an Inactive Hand Event is generated. <BR><BR>
 * 
 * This data consists of a handpoint ID of the hand that has become inactive, and the 
 * timestamp on the frame where this event was first detected.  This is a read only data type
 * after it is created.  In general, this class will not be instantiated by application code.  
 * The various member functions will be used to read the data it is providing.
 * 
 */
public class InactiveHandEventArgs extends EventArgs
{
	/**
	 * Constructor -- Creates a new object with specified ID and time stamp
	 * @param id Hand point ID of the hand that went inactive
	 * @param time Time when this occurred
	 */
	public InactiveHandEventArgs(int id, float time)
	{
		this.id = id;
		this.time = time;
	}
	
	/**
	 * Getter function for the ID
	 * @return Handpoint ID of the hand that is no longer active
	 */
	public int getId()
	{
		return this.id;
	}
	/**
	 * Getter function for the time
	 * @return Timestamp on the frame where this event was first detected
	 */
	public float getTime()
	{
		return this.time;
	}
	
	private final int id;
	private final float time;
}
