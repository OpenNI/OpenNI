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
 * This class holds the data created when a Gesture Recognized Event is generated. <BR><BR>
 * 
 * This data consists of the name of the gesture detected, the location of the user, and the end point where that
 * gesture was detected.  This event type is associated with a user completely performing the given gesture type.
 *
 */
public class GestureRecognizedEventArgs extends EventArgs
{
	/**
	 * Constructor -- Creates a new object with specified gesture type, user position, and end position
	 * @param gesture The name of the gesture detected
	 * @param idPosition The position of the user that performed the gesture
	 * @param endPosition The place in space where the gesture ended
	 */
	public GestureRecognizedEventArgs(String gesture, Point3D idPosition, Point3D endPosition)
	{
		this.gesture = gesture;
		this.idPosition = idPosition;
		this.endPosition = endPosition;
	}
	
	/**
	 * Getter function for the gesture type
	 * @return String name of the gesture type detected
	 */
	public String getGesture()
	{
		return this.gesture;
	}
	/** Getter function for the ID Position of the gesture
	 * @return Location in space of the user that performed the gesture
	 */
	public Point3D getIdPosition()
	{
		return this.idPosition;
	}
	/** Getter function for the end position of the detected gesture
	 * @return Point in space where the gesture was completed
	 */
	public Point3D getEndPosition()
	{
		return this.endPosition;
	}
	
	private final String gesture;
	private final Point3D idPosition;
	private final Point3D endPosition;

}
