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
 * Detects when a hand is at the edge of the sensor FOV. <BR><BR>
 * 
 * This capability detects when a hand is at the edge of the available field
 * of view.  It is normally accessed by using the function
 * GetHandTouchingFOVEdgeCapability, a member function of the HandsGenerator
 * class.<BR><BR>
 * 
 * This class defines the following events:
 * handTouchingFOVEdgeEvent: triggered when a hand is touching the FOV edge
 *
 */
public class HandTouchingFOVEdgeCapability extends CapabilityBase
{
	/**
	 * Creates this capability for the specified node
	 * @param node Node to have capability added
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public HandTouchingFOVEdgeCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		// Events
		handTouchingFOVEdgeEvent = new Observable<ActiveHandDirectionEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterToHandTouchingFOVEdge(toNative(), this, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromHandTouchingFOVEdge(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(int id, Point3D position, float time, int direction)
			{
				notify(new ActiveHandDirectionEventArgs(id, position, time, Direction.fromNative(direction)));
			}
		};
	}

	// Events
	/**
	 * Provides access to the Hand Touching FOV Edge event
	 * @return
	 */
	public IObservable<ActiveHandDirectionEventArgs> getHandTouchingFOVEdgeEvent()
	{
		return handTouchingFOVEdgeEvent;
	}
	
	private Observable<ActiveHandDirectionEventArgs> handTouchingFOVEdgeEvent;
}
