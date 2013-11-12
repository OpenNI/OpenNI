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
 * Enables mirroring of the data produced by a generator. <BR><BR> 
 * 
 * Mirroring is useful if the sensor is placed in front of the user, as the image 
 * captured by the sensor is mirrored, so the right hand appears as the left hand of the 
 * mirrored figure.  This class is normally accessed by using GetMirrorCapability(), 
 * a member function of class Generator.<BR><BR>
 * 
 * This class defines the following events:
 * MirrorChanged: Triggered when the value of the mirror flag changes
 *
 */
public class MirrorCapability extends CapabilityBase
{
	/**
	 * Creates a new mirror capability for the given node.
	 * @param node Node to create the capability for.
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public MirrorCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		this.mirrorChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToMirrorChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromMirrorChange(toNative(), hCallback);
			}
		};
	}
	
	/**
	 * Checks whether this node is currently mirroring its output
	 * @return TRUE if yes, FALSE if no
	 */
	public boolean isMirrored()
	{
		return NativeMethods.xnIsMirrored(toNative());
	}
	
	/**
	 * Turns mirroring on or off
	 * @param isMirrored Desired new value: TRUE to enable mirroring, FALSE to disable it
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public void setMirror(boolean isMirrored) throws StatusException
	{
		int status = NativeMethods.xnSetMirror(toNative(), isMirrored);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Used to access the Mirror Changed event
	 * @return
	 */
	public IStateChangedObservable getMirrorChangedEvent() { return this.mirrorChanged; }

	private StateChangedObservable mirrorChanged;
}
