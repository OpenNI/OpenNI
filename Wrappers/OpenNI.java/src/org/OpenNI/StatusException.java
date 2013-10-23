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
 * Extends the GeneralException to include an integer status value. <BR><BR>
 * 
 * This is used for OpenNI exceptions originating from Native code returning
 * non-zero status values.<BR><BR>
 * 
 * Translates the integer status code into an error message.
 *
 */
public class StatusException extends GeneralException
{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	/**
	 * Constructor - creates a new StatusException with the given integer code,
	 * and looks up the error message corresponding to that code.
	 * @param status
	 */
	public StatusException(int status)
	{
		super(WrapperUtils.getErrorMessage(status));
	}
}
