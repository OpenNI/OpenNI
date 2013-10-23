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
 * Base class for much of the OpenNI Java wrapper. <BR><BR>
 * 
 * Used by the API to facilitate Java wrapping of the the Native C code that makes
 * up OpenNI.  These functions are unlikely to be directly useful to application
 * programmers.  This function serves as the base class to a large percentage
 * of the classes that make up the OpenNI Java wrapper.
 *
 */
public abstract class ObjectWrapper
{
	/** Constructor that creates an object and sets its internal pointer value to the given 
	 * value.
	 * @param ptr Desired pointer value for this object.
	 */
	public ObjectWrapper(long ptr)
	{
		if (ptr == 0)
		{
			throw new NullPointerException("JavaWrapper: Trying to wrap a null object!");
		}
		this.ptr = ptr;
	}
	
	/** Retrieves a pointer value to this object that is usable in native code
	 * @return long integer that points to this object
	 */
	public long toNative()
	{
		return this.ptr;
	}

	/**
	 * Provides an alternate name (but identical functionality) to the dispose() function.
	 */
	protected void finalize()
	{
		dispose();
	}
	
	/**
	 *  Frees the objects memory and sets its pointer to NULL
	 */
	public void dispose()
	{
		if (this.ptr != 0)
		{
			freeObject(this.ptr);
			this.ptr = 0;
		}
	}
	
	/**
	 * Abstract function -- implemented to allow for each type of Object descended
	 * from ObjectWrapper to free its own memory.
	 * @param ptr Pointer to the object to be freed.
	 */
	protected abstract void freeObject(long ptr);
	
	private long ptr;
}
