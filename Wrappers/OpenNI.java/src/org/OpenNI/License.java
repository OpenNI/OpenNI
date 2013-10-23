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
 * Holds a pair of values, representing a vendor and key. <BR><BR>
 * 
 * This class stores an associated vendor and key value, both as strings.  This 
 * information is intended to be used as part of the infrastructure to support
 * a license system for middleware.  Objects of this class are read-only once
 * they have been generated.<BR><BR>
 * 
 * The details of how this information is utilized is dependent on the middleware 
 * package that chooses to make use of it.  Contact your middleware vendor for more 
 * details if you are having license difficulties.
 *
 */
public class License
{
	/**
	 * Constructor, creates a new license object with specified vendor and key values
	 * @param vendor String value indicating the vendor
	 * @param key String value storing a licence key
	 */
	public License(String vendor, String key)
	{
		this.vendor = vendor;
		this.key = key;
	}
	
	/** 
	 * Getter function for the Vendor value
	 * @return The vendor value stored by the license object
	 */
	public String getVendor()
	{
		return this.vendor;
	}
	
	/**
	 * Getter function for the key value
	 * @return The key value stored by the license object
	 */
	public String getKey()
	{
		return this.key;
	}
	
	private final String vendor;
	private final String key;
}
