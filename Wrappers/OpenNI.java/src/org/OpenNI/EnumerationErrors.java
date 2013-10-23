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
 * Stores enumeration errors generated when initializing a device. <BR><BR>
 *
 */
public class EnumerationErrors extends ObjectWrapper
{
	/**
	 * Default constructor
	 * @throws StatusException
	 */
	public EnumerationErrors() throws StatusException
	{
		this(create());
	}
	
	/**
	 * Checks to see whether this object is empty.  Useful to see if any
	 * enumeration errors exist when making status checks.
	 * @return TRUE if this object is empty, FALSE otherwise
	 */
	public boolean isEmpty()
	{
		long first = NativeMethods.xnEnumerationErrorsGetFirst(toNative());
		return !NativeMethods.xnEnumerationErrorsIteratorIsValid(first);
	}
	
	/**
	 * Extracts error messages in string format
	 * @return Error messages, stored as a string
	 */
	@Override
	public String toString() 
	{
		OutArg<String> result = new OutArg<String>();
		NativeMethods.xnEnumerationErrorsToString(toNative(), result);
		return result.value;
	}
	
	/**
	 * Frees memory occupied by an underlying enumeration errors object.
	 * @param ptr Long integer, representing a native point to an EnumerationErrors object
	 */
	@Override
	protected void freeObject(long ptr) 
	{
		NativeMethods.xnEnumerationErrorsFree(ptr);
	}

	private EnumerationErrors(long ptr) 
	{
		super(ptr);
	}
	
	private static long create() throws StatusException
	{
		OutArg<Long> pErrors = new OutArg<Long>();
		int status = NativeMethods.xnEnumerationErrorsAllocate(pErrors);
		WrapperUtils.throwOnError(status);
		return pErrors.value;
	}
}
