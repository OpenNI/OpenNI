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
 * Stores the data created when a Gesture Position Event is generated. <BR><BR>
 * 
 * This data includes a code indicating which predefined focus gesture was detected,
 * and the position in space where this gesture was detected.<BR><BR>
 * 
 * This event type is generated when the system detects that the user has possibly 
 * started performing a recognized gesture.  This does NOT mean that the user has 
 * finished performing the gesture.  This is generally useful to provide immediate feedback
 * to the user as he starts performing the gesture.
 *
 */
public class GesturePositionEventArgs extends EventArgs
{
	/**
	 * Constructor - creates a new object with specified gesture code and position
	 * @param gesture Which gesture was detected
	 * @param position Where the gesture was detected
	 */
	public GesturePositionEventArgs(String gesture, Point3D position)
	{
		this.gesture = gesture;
		this.position = position;
	}
	
	/**
	 * Getter function for the gesture detected
	 * @return Name of the detected gesture
	 */
	public String getGesture()
	{
		return this.gesture;
	}
	/**
	 * Getter function for the gesture position
	 * @return Position of the detected gesture
	 */
	public Point3D getPosition()
	{
		return this.position;
	}
	
	private final String gesture;
	private final Point3D position;

}
