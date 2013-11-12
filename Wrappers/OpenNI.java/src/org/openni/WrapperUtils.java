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
 * Contains miscellaneous functions that are useful in the Java wrapper. <BR><BR>
 * 
 * Normally, none of these functions would be called directly by an application.  They
 * are used primarily by internal functions of the OpenNI Java wrapper to translate
 * OpenNI function output codes into appropriate exceptions and error messages.
 *
 */
public class WrapperUtils
{
	/**
	 * This function examines the integers status outputs from many native OpenNI functions,
	 * and throws the appropriate StatusException if the value is non-zero.
	 * @param status The status output from some function that might trigger StatusExceptions
	 * @throws StatusException
	 */
	public static void throwOnError(int status) throws StatusException
	{
		if (status != 0)
		{
			throw new StatusException(status);
		}
	}
	
	/**
	 * This function examines the integer status output from some native OpenNI functions.  If 
	 * status is non-zero, and there are errors, then an appropriate GeneralException is thrown.  If
	 * status is non-zero, but there are no errors, then an appropriate StatusException is thrown.
	 * @param status
	 * @param errors
	 * @throws GeneralException
	 */
	public static void checkEnumeration(int status, EnumerationErrors errors) throws GeneralException
	{
		if (status != 0)
		{
			if (errors != null && !errors.isEmpty())
			{
				throw new GeneralException(errors.toString());
			}
			else
			{
				throw new StatusException(status);
			}
		}
	}
	
	/**
	 * This function converts integer status codes into human readable string values.
	 * @param status Status code to be translated into a string value
	 * @return
	 */
	public static String getErrorMessage(int status)
	{
		String message = NativeMethods.xnGetStatusString(status);
		return message;
	}
}
