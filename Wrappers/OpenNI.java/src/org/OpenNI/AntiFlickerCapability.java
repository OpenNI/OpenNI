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
 * Alters Map Generator timing to reduce power line noise. <BR><BR>
 * 
 * This capability allows a Map Generator to time its frame in such a manner that
 * interference from power line frequency (ie electric mains) noise is minimized
 * or eliminated.<BR><BR>
 * 
 * Common AC power lines emit electromagnetic radiation at their base frequency (usually 50-60 Hz), as
 * well as creating power supply noise in any electronics using AC power.  This noise spike is the 
 * source of the familiar 50 or 60 Hertz "hum" heard near power transformers.  This noise can also induce
 * artifacts in image sensors.  This capability is intended to inform a device of the dominant AC power frequency
 * in its current area of operation.  Some devices can use this information to optimize timing of internal 
 * image sensors to minimize the effects of this noise.<BR><BR>  
 * 
 * Available nominal power line frequencies are 50 hertz, 60 hertz, or 0 hertz (capability off).  This 
 * is intended to cover the most common AC frequencies used by the power utilities in 
 * most countries.<BR><BR>
 * 
 * Power in North America, most of South America, Taiwan and Korea is 60 hertz.  Europe, Australia, China and 
 * the Middle East are mostly 50 hertz. Most but not all African countries use 50 hertz.  Japan uses both.
 * Antarctica remains mostly uncovered by an electric grid, though isolated systems can be found using
 * both 50 Hz and 60 Hz.  
 */
public class AntiFlickerCapability extends CapabilityBase
{
	/** Constructor activates this capability for the given Production Node
	 * @param node
	 * @throws StatusException
	 */
	public AntiFlickerCapability(ProductionNode node) throws StatusException
	{
		super(node);

		this.powerLineFrequencyChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToPowerLineFrequencyChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromPowerLineFrequencyChange(toNative(), hCallback);
			}
		};
	}
	
	/**
	 * Returns the current power line frequency setting.
	 * @return Value will either be 0 if the capability is disabled, 50 Hertz, or 60 Hertz
	 */
	public PowerLineFrequency getPowerLineFrequency()
	{
		return PowerLineFrequency.fromNative(NativeMethods.xnGetPowerLineFrequency(toNative()));
	}
	
	/**
	 * @param frequency Desired power line frequency, either 50 Hz, 60 Hz, or 0 Hz (for capability off)
	 * @throws StatusException Needed since this function will attempt to change sensor settings
	 */
	public void setPowerLineFrequency(PowerLineFrequency frequency) throws StatusException
	{
		int status = NativeMethods.xnSetPowerLineFrequency(toNative(), frequency.toNative());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * This function is used to observe Power Line Frequency Changed events.
	 * @return
	 */
	public IStateChangedObservable getPowerLineFrequencyChangedEvent() { return this.powerLineFrequencyChanged; }

	private StateChangedObservable powerLineFrequencyChanged;
}
