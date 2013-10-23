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
 * Holds the data generated when an Error State Event is generated. <BR><BR>
 * 
 * This data consists of a predefined string message associated with an error code.  Error
 * codes are generally created by native OpenNI code.
 *
 */
public class ErrorStateEventArgs extends EventArgs
{
	/**
	 * Constructor -- creates a new object with specified integer status code.  For all
	 * non-zero codes, the appropriate error message for that code is retrieved and stored
	 * @param status The integer error 
	 */
	public ErrorStateEventArgs(int status)
	{
		if (status == 0)
		{
			this.currError = null;
		}
		else
		{
			this.currError = WrapperUtils.getErrorMessage(status);
		}
	}
	
	/**
	 * Getter function for the current error value
	 * @return String appropriate to the error code associated with this event
	 */
	public String getCurrentError()
	{
		return currError;
	}

	private String currError;
}
