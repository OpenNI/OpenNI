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
 * Base class for all integer based capabilities. <BR><BR>
 * 
 * This class serves to implement a family of classes that are controlled by setting a single
 * integer property in the underlying node.<BR><BR>
 * 
 * These capabilities are accessed via the appropriate function for the specific capability desired.
 * All functions for specific GeneralInt capabilities can be found in the MapGenerator class.
 *
 */
public class GeneralIntCapability extends CapabilityBase
{
	/**
	 * Produce a new GeneralInt capability on the specified node, with the specified capability type
	 * @param node Node to create the capability on
	 * @param capName capability type
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public GeneralIntCapability(ProductionNode node, Capability capName) throws StatusException
	{
		super(node);
		this.capName = capName.getName();
		
		OutArg<Integer> pMin = new OutArg<Integer>();
		OutArg<Integer> pMax = new OutArg<Integer>();
		OutArg<Integer> pStep = new OutArg<Integer>();
		OutArg<Integer> pDefault = new OutArg<Integer>();
		OutArg<Boolean> pAutoSupported = new OutArg<Boolean>();
		
		int status = NativeMethods.xnGetGeneralIntRange(toNative(), getCapName(), pMin, pMax, pStep, pDefault, pAutoSupported);
		WrapperUtils.throwOnError(status);
		
		this.min = pMin.value;
		this.max = pMax.value;
		this.step = pStep.value;
		this.defaultVal = pDefault.value;
		this.autoSupported = pAutoSupported.value;
		
		this.valueChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToGeneralIntValueChange(toNative(), getCapName(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromNodeErrorStateChange(toNative(), hCallback);
			}
		};
	}
	
	/**
	 * Provides access to the minimum value that this capability can accept
	 * @return Minimum possible value
	 */
	public int getMin()
	{
		return this.min;
	}

	/**
	 * Provides access to the maximum value that this capability can accept
	 * @return Maximum possible value
	 */
	public int getMax()
	{
		return this.max;
	}

	/**
	 * Provides access to the step size of the capability, ie the distance between
	 * sequential available values
	 * @return Step size for this capability
	 */
	public int getStep()
	{
		return this.step;
	}

	/**
	 * Provides access to the default value taken by this capability
	 * @return Default value
	 */
	public int getDefault()
	{
		return this.defaultVal;
	}

	/**
	 * Queries whether the value of this capability can be automatically controlled
	 * @return TRUE if yes, FALSE if automatic control is not available
	 */
	public boolean isAutoSupported()
	{
		return this.autoSupported;
	}
	
	/**
	 * Gets the current value set in this capability
	 * @return current value
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public int getValue() throws StatusException
	{
		OutArg<Integer> val = new OutArg<Integer>();
		int status = NativeMethods.xnGetGeneralIntValue(toNative(), this.capName, val);
		WrapperUtils.throwOnError(status);
		return val.value;
	}
	
	/**
	 * Sets the value of this capability to a new value
	 * @param value Desired new value
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void setValue(int value) throws StatusException
	{
		int status = NativeMethods.xnSetGeneralIntValue(toNative(), this.capName, value);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Provides access to the Value Changed event
	 * @return
	 */
	public IStateChangedObservable getValueChangedEvent() { return this.valueChanged; }
	
	/**
	 * Provides access to the name of this capability
	 * @return Name of this capability
	 */
	String getCapName() { return this.capName; }

	private StateChangedObservable valueChanged;
	private final String capName;
	private int min;
	private int max;
	private int step;
	private int defaultVal;
	private boolean autoSupported;
}
