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
 * Optimizes the output depth map that is generated for a specific area of the scene. <BR><BR>
 * 
 * The exact nature of this optimization is left up to the implementation of the Depth Generator that
 * includes this capability.
 *
 */
public class UserPositionCapability extends CapabilityBase
{
	/**
	 * Activates this capability in the specified production node
	 * @param node Production node that includes this capability
	 * @throws StatusException Required, since we are communicating with a hardware sensor
	 */
	public UserPositionCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		this.userPositionChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToUserPositionChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromUserPositionChange(toNative(), hCallback);
			}
		};
	}

	/** 
	 * Counts the number of possible user positions supported by this depth generator.
	 * @return Number of supported positions that the user can occupy for depth optimization purposes
	 */
	public int getSupportedCount()
	{
		return NativeMethods.xnGetSupportedUserPositionsCount(toNative());
	}
	
	/**
	 * This function gives a programmatic means to tell the capability where the user is located
	 * @param index User ID of the user being located
	 * @param position A bounding box showing the rough location where the user is known to be located
	 * @throws StatusException Required, since we are making calls to talk to sensor hardware that could fail
	 */
	public void setUserPosition(int index, BoundingBox3D position) throws StatusException
	{
		Point3D leftBottomNear = position.getLeftBottomNear();
		Point3D rightTopFar = position.getRightTopFar();
		int status = NativeMethods.xnSetUserPosition(toNative(), index, leftBottomNear.getX(), leftBottomNear.getY(), leftBottomNear.getZ(), rightTopFar.getX(), rightTopFar.getY(), rightTopFar.getZ());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Returns the general position of the user specified by index
	 * @param index The UserID of the user we wish to locate
	 * @return A bounding box indicating the rough location of the user
	 * @throws StatusException Required, since this function might talk to hardware
	 */
	public BoundingBox3D getUserPosition(int index) throws StatusException
	{
		OutArg<BoundingBox3D> position = new OutArg<BoundingBox3D>();
		int status = NativeMethods.xnGetUserPosition(toNative(), index, position);
		WrapperUtils.throwOnError(status);
		return position.value;
	}
	
	/**
	 * Used to enable detection of the User Position Changed event type
	 * @return
	 */
	public IStateChangedObservable getUserPositionChangedEvent() { return this.userPositionChanged; }
	
	private StateChangedObservable userPositionChanged;
}
