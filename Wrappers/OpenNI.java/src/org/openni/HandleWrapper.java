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
 * Provides a Java object to wrap the integer handles used by native OpenNI code. <BR><BR>
 *
 */
public class HandleWrapper 
{
	/**
	 * Constructor, creates a new HandleWrapper, and stores the specified value
	 * @param handle The handle to be stored
	 */
	public HandleWrapper(int handle)
	{
		this.handle = handle;
	}
	
	/**
	 * Returns the stored HandleWrapper
	 * @return The stored HandleWrapper
	 */
	public int toNative() { return this.handle; }
	
	private int handle;
}
