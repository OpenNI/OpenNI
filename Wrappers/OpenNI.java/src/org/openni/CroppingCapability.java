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
 * Allows a Map Generator to output only a portion of a frame. <BR><BR>
 * 
 * The Cropping capability allows a Map Generator to output a 
 * selected area of the frame instead of the entire frame. When cropping is turned on, 
 * the data buffer looks just like resolution is smaller, meaning data is packed. For 
 * example if the MapGenerator is working in VGA resolution (640x480) and the application 
 * chose a cropping of 300x200, then after first 300 pixels the next pixel row begins. 
 * Pixels outside the cropping area are not returned in the buffer (and not just being blacked out). 
 * This is very useful for performance boost. 
 *
 */
public class CroppingCapability extends CapabilityBase
{
	/**
	 * Constructor to create a new cropping capability, given a production node that supports
	 * this capability
	 * @param node Node must be a production node type that supports the cropping capability
	 * @throws StatusException Required since this function attempts to make changes to sensor hardware
	 */
	public CroppingCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		this.croppingChanged = new StateChangedObservable()
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToCroppingChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromCroppingChange(toNative(), hCallback);
			}
		};
	}
	
	/**
	 * Sets the current cropping parameters to the values contained in the input cropping object
	 * @param cropping A cropping object preloaded with all desired parameters
	 * @throws StatusException Required since this function attempts to make changes to sensor hardware
	 */
	public void setCropping(Cropping cropping) throws StatusException
	{
		int status = NativeMethods.xnSetCropping(toNative(), cropping.getXOffset(), cropping.getYOffset(), cropping.getXSize(), cropping.getYSize(), cropping.isEnabled());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Retrieves the current cropping settings and loads them into an object of type Cropping
	 * @return An object of type Cropping with all members preset to current cropping settings
	 * @throws StatusException Required since this function attempts to read values from sensor hardware
	 */
	public Cropping getCropping() throws StatusException
	{
		OutArg<Integer> xOffset = new OutArg<Integer>();
		OutArg<Integer> yOffset = new OutArg<Integer>();
		OutArg<Integer> xSize = new OutArg<Integer>();
		OutArg<Integer> ySize = new OutArg<Integer>();
		OutArg<Boolean> isEnabled = new OutArg<Boolean>();
		int status = NativeMethods.xnGetCropping(toNative(), xOffset, yOffset, xSize, ySize, isEnabled);
		WrapperUtils.throwOnError(status);
		return new Cropping(xOffset.value, yOffset.value, xSize.value, ySize.value, isEnabled.value);
	}
	
	/**
	 * Checks to see if any cropping change events have occurred
	 * @return
	 */
	public IStateChangedObservable getCroppingChangedEvent() { return this.croppingChanged; }

	private StateChangedObservable croppingChanged;
}
