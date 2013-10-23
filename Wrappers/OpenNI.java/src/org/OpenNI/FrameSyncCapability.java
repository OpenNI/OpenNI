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
 * Enables two sensors producing frame data to synchronize their frames so that they arrive at the same time. <BR><BR>
 * 
 * The purpose of this capability is to synchronize the arrival time of the frames from two different sensors.  This 
 * prevents a situation where two sensors are creating frames at the same rate, but out of phase with each other.  For 
 * example, if a sensor produces depth and image frames at 30fps, the worst case scenario with FrameSync 
 * disabled is that each corresponding depth and image frames are up to 17ms apart from each other in time.  Enabling FrameSync
 * reduces this maximum to some value (exact max depends on sensor type in question).<BR><BR>
 * 
 * This capability is normally accessed by using Generator::GetFrameSyncCapability()<BR><BR>
 * 
 * This class defines the following events:<BR><BR>
 * FrameSyncChanged: Triggered when the frame sync settings for this node change
 *
 */
public class FrameSyncCapability extends CapabilityBase
{
	/**
	 * Creates a new FrameSyncCapability object for the given node
	 * @param node Node to create a capability for
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public FrameSyncCapability(ProductionNode node)
			throws StatusException
	{
		super(node);
		
		this.frameSyncChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToFrameSyncChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromFrameSyncChange(toNative(), hCallback);
			}
		};
	}
	
	/**
	 * Tests whether the node associated with this capability can be frame synced with 
	 * a given node
	 * @param other Node to test
	 * @return TRUE if the two nodes can be synced, FALSE otherwise
	 */
	public boolean canFrameSyncWith(Generator other)
	{
		return NativeMethods.xnCanFrameSyncWith(toNative(), other.toNative());
	}
	
	/**
	 * Turns on frame sync between this node and a specified node
	 * @param other Node for this node to be synchronized with
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void frameSyncWith(Generator other) throws StatusException
	{
		int status = NativeMethods.xnFrameSyncWith(toNative(), other.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Turns off the frame sync between this node and the specified other node
	 * @param other Node to turn off frame sync with
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void stopFrameSyncWith(Generator other) throws StatusException
	{
		int status = NativeMethods.xnStopFrameSyncWith(toNative(), other.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Tests whether this node is currently frame synced with a given other node
	 * @param other Node to test against
	 * @return TRUE if these two nodes are synchronized, FALSE otherwise
	 */
	public boolean isFrameSyncedWith(Generator other)
	{
		return NativeMethods.xnIsFrameSyncedWith(toNative(), other.toNative());
	}

	/**
	 * Provides access to the Frame Sync Changed event
	 * @return
	 */
	public IStateChangedObservable getFrameSyncChangedEvent() { return this.frameSyncChanged; }

	private StateChangedObservable frameSyncChanged;
}
