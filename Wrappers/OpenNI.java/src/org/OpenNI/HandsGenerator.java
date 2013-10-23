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
 * Locates and continuously tracks a user's hands. <BR><BR>
 * 
 * HandPoint generators generally make use of the data from a depth generator to
 * located and track a user's hands.  Exact requirements will vary depending on 
 * middleware implementation.
 * 
 * This generator implements the following events:
 * handCreateEvent -- Triggered when the system finds a new hand in a scene
 * handUpdateEvent -- Triggered whenever a previously located hand changes location
 * handDestroyEvent -- Triggered whenever a previously located hand is lost
 *
 */
public class HandsGenerator extends Generator
{

	/** 
	 * Constructor, creates a new hands generator form the given context
	 * @param context OpenNI context for this generator
	 * @param nodeHandle Native pointer to this object
	 * @param addRef Indicates whether this object should be registered
	 * @throws GeneralException If underlying native code returns errors, a General Exception is thrown by this function
	 */
	HandsGenerator(Context context, long nodeHandle, boolean addRef)
			throws GeneralException
	{
		super(context, nodeHandle, addRef);
		// TODO Auto-generated constructor stub
		
		handCreateEvent = new Observable<ActiveHandEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterHandCallbacks(toNative(), this, "callback", null, null, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterHandCallbacks(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(int id, Point3D point, float time)
			{
				notify(new ActiveHandEventArgs(id, point, time));
			}
		};
		handUpdateEvent = new Observable<ActiveHandEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterHandCallbacks(toNative(), this, null, "callback", null, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterHandCallbacks(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(int id, Point3D point, float time)
			{
				notify(new ActiveHandEventArgs(id, point, time));
			}
		};
		handDestroyEvent = new Observable<InactiveHandEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterHandCallbacks(toNative(), this, null, null, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterHandCallbacks(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(int id, float time)
			{
				notify(new InactiveHandEventArgs(id, time));
			}
		};
	}

	/**
	 * Creates a hands generator in a given context, filtered by a given query object, and stores any errors in a given
	 * EnumerationErrors object
	 * @param context OpenNI Context for this generator
	 * @param query Query object to filter results
	 * @param errors Place to store any errors generated
	 * @return Resulting HandsGenerator object
	 * @throws GeneralException If underlying native code returns errors, a General Exception is thrown by this function
	 */
	public static HandsGenerator create(Context context, Query query, EnumerationErrors errors) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateHandsGenerator(context.toNative(), handle,
			query == null ? 0 : query.toNative(),
			errors == null ? 0 : errors.toNative());
		WrapperUtils.throwOnError(status);
		HandsGenerator result = (HandsGenerator)context.createProductionNodeObject(handle.value, NodeType.HANDS);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	/** 	 
	 * Creates a hands generator in a given context, filtered by a given query object, discarding error data
	 * @param context OpenNI Context for this generator
	 * @param query Query object to filter results
	 * @return Resulting HandsGenerator object
	 * @throws GeneralException If underlying native code returns errors, a General Exception is thrown by this function
	 */
	public static HandsGenerator create(Context context, Query query) throws GeneralException
	{
		return create(context, query, null);
	}

	/**
	 * Creates a hands generator in a given context
	 * @param context OpenNI context for this generator
	 * @return Resulting HandsGenerator object
	 * @throws GeneralException If underlying native code returns errors, a General Exception is thrown by this function
	 */
	public static HandsGenerator create(Context context) throws GeneralException
	{
		return create(context, null, null);
	}

	/**
	 * Stops tracking a specific handpoint, specified by ID number
	 * @param id ID of the handpoint to stop tracking
	 * @throws StatusException The underlying native function (xnStopTracking)
	 * can throw status codes
	 */
	public void StopTracking(int id) throws StatusException
	{
		int status = NativeMethods.xnStopTracking(toNative(), id);
		WrapperUtils.throwOnError(status);
	}
	/**
	 * Turns off tracking for all active handpoints
	 * @throws StatusException The underlying native function (xnStopTrackingAll)
	 * can throw status codes
	 */
	public void StopTrackingAll() throws StatusException
	{
		int status = NativeMethods.xnStopTrackingAll(toNative());
		WrapperUtils.throwOnError(status);
	}
	/**
	 * Starts tracking the user's hand at the specified point in space.  Normally, a focus gesture
	 * is used to both indicate the user's intent to start tracking, and to help locate the user's hand
	 * in the scene.  This function can be called manually if the user's hand location is known, but
	 * if the location is not known, a focus gesture will be needed for accurate tracking.
	 * @param position Point in space where user's hand is located at start of tracking.
	 * @throws StatusException The underlying native function (xnStartTracking) can throw status codes.
	 */
	public void StartTracking(Point3D position) throws StatusException
	{
		int status = NativeMethods.xnStartTracking(toNative(), position.getX(), position.getY(), position.getZ());
		WrapperUtils.throwOnError(status);
	}
	/**
	 * Sets the number for a smoothing factor.  A simple smoothing algorithm is applied to average
	 * handpoint locations over several frames.  This produces a much less jittery hand point value, but
	 * at the expense of apparent responsiveness.  For most UI tasks, some smoothing will produce a better
	 * user experience.
	 * @param factor Smoothing factor number.  Higher values produce more smoothing.
	 * @throws StatusException The underlying native function being called
	 * (xnSetTrackingSmoothing)can throw status codes
	 */
	public void SetSmoothing(float factor) throws StatusException
	{
		int status = NativeMethods.xnSetTrackingSmoothing(toNative(), factor);
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Activates the HandTouchingFOVEdge capability for this node
	 * @return The resulting capability activated by this function
	 * @throws StatusException Required, since node might not be compatible with this capability
	 */
	public HandTouchingFOVEdgeCapability getHandTouchingFOVEdgeCapability() throws StatusException
	{
		return new HandTouchingFOVEdgeCapability(this);
	}

	// Events
	/** Provides access to the handCreateEvent
	 * @return
	 */
	public IObservable<ActiveHandEventArgs> getHandCreateEvent()
	{
		return handCreateEvent;
	}
	/**
	 * Provides access to the handUpdateEvent
	 * @return
	 */
	public IObservable<ActiveHandEventArgs> getHandUpdateEvent()
	{
		return handUpdateEvent;
	}
	/**
	 * Provides access to the handDestroyEvent
	 * @return
	 */
	public IObservable<InactiveHandEventArgs> getHandDestroyEvent()
	{
		return handDestroyEvent;
	}
	
	private Observable<ActiveHandEventArgs> handCreateEvent;
	private Observable<ActiveHandEventArgs> handUpdateEvent;
	private Observable<InactiveHandEventArgs> handDestroyEvent;
}
