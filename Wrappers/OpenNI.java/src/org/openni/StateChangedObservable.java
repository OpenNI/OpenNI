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
 * Extends Observable to handle state change events. <BR><BR>
 *
 */
public abstract class StateChangedObservable extends Observable<EventArgs> implements IStateChangedObservable
{
	/**
	 * Registers a callback function with native code
	 * @param phCallback Pointer to callback function
	 * @return Result code from the native call
	 */
	@Override
	protected int registerNative(OutArg<Long> phCallback) 
	{
		return registerNative("callback", phCallback);
	}

	protected abstract int registerNative(String cb, OutArg<Long> phCallback);

	@SuppressWarnings("unused")
	private void callback()
	{
		notify(null);
	}
}
