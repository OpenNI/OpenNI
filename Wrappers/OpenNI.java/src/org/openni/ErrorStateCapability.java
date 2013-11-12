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
 * Enables a node to report that it is in "Error" status. <BR><BR>
 * 
 * This capability is normally accessed by requesting it with the GetErrorStateCapability(), a 
 * member function of the ProductionNode class. <BR><BR>
 * 
 * Provides the following events:
 * errorStateChanged: Triggered when the error state of the node changes
 *
 */
public class ErrorStateCapability extends CapabilityBase
{
	/**
	 * Creates a new ErrorStateCapability object for a given node
	 * @param node Node to create the capability for 
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public ErrorStateCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		this.errorStateChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToNodeErrorStateChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromNodeErrorStateChange(toNative(), hCallback);
			}
		};
	}
	
	/**
	 * Provides access to the current error state of the node associated with this capability
	 * @return Current error state
	 */
	public String getErrorState()
	{
		int status = NativeMethods.xnGetNodeErrorState(toNative());
		if (status == 0)
		{
			return null;
		}
		else
		{
			return NativeMethods.xnGetStatusString(status);
		}
	}
	
	/**
	 * Provides access to the Error State Changed event
	 * @return
	 */
	public IStateChangedObservable getErrorStateChangedEvent() { return this.errorStateChanged; }

	private StateChangedObservable errorStateChanged;
}
