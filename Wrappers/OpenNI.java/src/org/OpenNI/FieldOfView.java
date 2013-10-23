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
 * Stores a sensor's Field of View angles in radians. <BR><BR>
 * 
 * These values are used to properly calculate Real World to Projective coordinate conversions <BR><BR>
 * 
 * Note that this class is intended to store the FOV for a given hardware type.  Setting the FoV 
 * via the members of this class does change the actual FoV settings of any associated hardware.
 *
 */
public class FieldOfView 
{
	/**
	 * Constructor.  Creates a new FieldOfView object with specified FOV values
	 * @param hFOV Horizontal field of view of the sensor, measured in radians
	 * @param vFOV Vertical field of view of the sensor, measured in radians
	 */
	public FieldOfView(double hFOV, double vFOV) {
		super();
		this.hFOV = hFOV;
		this.vFOV = vFOV;
	}
	
	/**
	 * Getter function for the horizontal field of view value
	 * @return Horizontal field of view value in Radians
	 */
	public double getHFOV() {
		return hFOV;
	}
	
	/**
	 * Setter function for the horizontal field of view value
	 * @param hFOV Horizontal field of view value in Radians
	 */
	public void setHFOV(double hFOV) {
		this.hFOV = hFOV;
	}
	
	/**
	 * Getter function for the vertical field of view value
	 * @return Vertical field of view in Radians
	 */
	public double getVFOV() {
		return vFOV;
	}
	
	/**
	 * Setter function for the vertical field of view value
	 * @param vFOV Vertical field of view in Radians
	 */
	public void setVFOV(double vFOV) {
		this.vFOV = vFOV;
	}
	
	private double hFOV;
	private double vFOV;
}
