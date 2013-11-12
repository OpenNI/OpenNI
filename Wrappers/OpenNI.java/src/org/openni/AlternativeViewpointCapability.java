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
 * Transforms image output to simulate a change in camera position. <BR><BR>
 * 
 * This capability allows for one map generator to transform its data to appear
 * as if it was taken from the point of view of a different map generator.<BR><BR>
 * 
 * This is generally most useful when a device generates two types of image data
 * from two different physical cameras, where the geometric relationship between
 * those cameras is known.  Two cameras viewing the same scene from different positions
 * will see a slightly different image.  Applying this transformation to one of those 
 * images to be superimposed on the other.<BR><BR>
 * 
 * With the PrimeSense sensor family, this capability is used primarily to allow RGB
 * and depth images to correspond with each other.  The math required to do this
 * transformation is hardware accelerated in the PrimeSense sensor family.  This functionality
 * is sometimes referred to as "Registration", or "Image registration"
 *
 */
public class AlternativeViewpointCapability extends CapabilityBase
{
	/**
	 * Constructor attaches this object to a given production node
	 * @param node Production node that we want to activate this capability in
	 * @throws StatusException Required because this function will communicate with HW
	 */
	public AlternativeViewpointCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		this.viewPointChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToViewPointChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromViewPointChange(toNative(), hCallback);
			}
		};
	}
	
	/**
	 * Tests to see if the production node that is using this capability supports
	 * transforming its point of view to be equivalent to some other production node
	 * @param other The production node that we want to test for support
	 * @return
	 */
	public boolean isViewpointSupported(ProductionNode other)
	{
		return NativeMethods.xnIsViewPointSupported(toNative(), other.toNative());
	}
	
	/**
	 * Actually sets the point of view of the node using this capability to be 
	 * equivalent to a different production node
	 * @param other The production node that we wish to simulate a view of
	 * @throws StatusException Required since this function actually changes hardware parameters
	 */
	public void setViewpoint(ProductionNode other) throws StatusException
	{
		int status = NativeMethods.xnSetViewPoint(toNative(), other.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Resets this production node to use its default viewpoint, ie stops transforming 
	 * the image
	 * @throws StatusException Required since this function actually communicates with sensor hardware
	 */
	public void resetViewpoint() throws StatusException
	{
		int status = NativeMethods.xnResetViewPoint(toNative());
		WrapperUtils.throwOnError(status);
	}
	
	/** Tests whether the viewpoint of this node is currently set to simulate that of
	 * another given node
	 * @param other The node that we wish to test
	 * @return
	 */
	public boolean isViewpointAs(ProductionNode other)
	{
		return NativeMethods.xnIsViewPointAs(toNative(), other.toNative());
	}
	
	/**
	 * This function allows you to transform a set of X,Y coordinates into equivalent X,Y coordinates in 
	 * another viewpoint
	 * @param viewPoint The Production Node whose viewpoint you wish to transform your coordinates into
	 * @param x The X coordinate, from the viewpoint of the production node that has this capability enabled
	 * @param y The Y coordinate, from the viewpoint of the production node that has this capability enabled
	 * @return The input X,Y coordinates, transformed to the viewpoint of Production Node given in ViewPoint
	 * @throws StatusException Required since we may use sensor hardware to accelerate this function
	 */
	public XYCoordinates getPixelCoordinatesInViewpoint(ProductionNode viewPoint, int x, int y) throws StatusException
	{
		OutArg<Integer> altX = new OutArg<Integer>();
		OutArg<Integer> altY = new OutArg<Integer>();
		int status = NativeMethods.xnGetPixelCoordinatesInViewPoint(toNative(), viewPoint.toNative(), x, y, altX, altY);
		WrapperUtils.throwOnError(status);
		return new XYCoordinates(altX.value, altY.value);
	}

	/** This function is used to see View Point Changed Events.
	 * @return
	 */
	public IStateChangedObservable getViewPointChangedEvent() { return this.viewPointChanged; }

	private StateChangedObservable viewPointChanged;
}
