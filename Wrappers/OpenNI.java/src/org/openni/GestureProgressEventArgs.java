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
 * Stores the data created when a Gesture Progress Event is generated. <BR><BR>
 * 
 * This data consists of the gesture detected, where it was detected, and how close
 * that gesture is to completion.  This is a read only data type once it is created.
 * Generally, application code will not instantiate this object, but may use the 
 * member functions to read the data it is carrying.<BR><BR>
 * 
 * This event type is most often used to implement feedback mechanisms to help the
 * user understand that he has successfully started a gesture, and to provide feedback
 * as the user completes the gesture.
 *
 */
public class GestureProgressEventArgs extends EventArgs
{
	/**
	 * Constructor -- creates a new object with specified gesture type, position where that
	 * gesture was detected, and the progress towards completion of that gesture
	 * @param gesture Name of gesture detected
	 * @param position Position in space where gesture was detected
	 * @param progress Progress user has made towards completion of gesture
	 */
	public GestureProgressEventArgs(String gesture, Point3D position , float progress)
	{
		this.gesture = gesture;
		this.position = position;
		this.progress = progress;
	}
	
	/**
	 * Getter function for the gesture
	 * @return Name of the gesture detected
	 */
	public String getGesture()
	{
		return this.gesture;
	}
	/**
	 * Getter function for the position of the gesture
	 * @return Where in space the gesture was detected
	 */
	public Point3D getPosition()
	{
		return this.position;
	}
	/**
	 * Getter function for the progress of the gesture
	 * @return Progress towards completion of the gesture
	 */
	public float getProgress()
	{
		return this.progress;
	}
	
	private final String gesture;
	private final Point3D position;
	private final float progress;
}
