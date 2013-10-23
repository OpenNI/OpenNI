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
 * This class stores a version value. <BR><BR>
 * 
 * Values are set at object construction; data is read only after that.<BR><BR>
 * 
 * Normally, this would be used internally by the SDK to report its version.  It
 * could be reused by applications to track their own versions, though.<BR><BR>
 * 
 * The format uses four numbers to store a version -- Major, Minor, Maintenance, and Build.
 * For example Note 1.5.2.23, would be Major Version 1, Minor Version 5, Maintenance release 2, 
 * build 23.
 *
 */
public class Version 
{
	/**
	 * Constructor to create a new version object with specified major, minor, maintenance,
	 * and build numbers.  Note the differing data types of each value.
	 * @param major Major version number
	 * @param minor Minor version number
	 * @param maintenance Maintenance release number
	 * @param build Build number
	 */
	public Version(byte major, byte minor, short maintenance, int build)
	{
		this.major = major;
		this.minor = minor;
		this.maintenance = maintenance;
		this.build = build;
	}

	/**
	 * Getter function for the Major version number
	 * @return Major version number
	 */
	public byte getMajor()
	{
		return this.major;
	}
	/**
	 * Getter function for the Minor version number
	 * @return Minor version number
	 */
	public byte getMinor()
	{
		return this.minor;
	}
	/**
	 * Getter function for the Maintenance release number
	 * @return Maintenance release number
	 */
	public short getMaintenance()
	{
		return this.maintenance;
	}
	/**
	 * Getter function for the Build number
	 * @return Build number
	 */
	public int getBuild()
	{
		return this.build;
	}
	
	private final byte major;
	private final byte minor;
	private final short maintenance;
	private final int build;
}
