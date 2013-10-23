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
 * Identifies predefined gestures in a scene. <BR><BR>
 * 
 * This generator type is responsible for identifying specific predefined gestures when
 * performed by a person in the scene.  This will usually require a DepthGenerator to
 * produce the raw data that the gestures are recognized in.<BR><BR>
 * 
 * The most common use for the GestureGenerator is to create a "Focus" gesture that
 * is used to identify an active user.  Once this has been done, it is generally more
 * useful to track handpoints and operate on higher level objects such as those 
 * provided in the PrimeSense NiTE handpoint API. <BR><BR>
 * 
 * Events defined by this class:
 * 
 * gestureRecognizedEvent: triggered when a complete gesture is recognized.<BR>
 * gestureProgressEvent: triggered when a gesture is being performed.<BR>
 * gestureIntermediateStateCompletedEvent: triggered when one step of a multi-step gesture is completed.<BR>
 * gestureReadyForNextIntermediateStageEvent: triggered when new step of a multi-step gesture is started.<BR>
 * gestureChangedEvent: triggered when a gesture is changed.<BR>
 *
 */
public class GestureGenerator extends Generator
{
	/**
	 * Creates a new gesture generator
	 * 
	 * @param context OpenNI Context 
	 * @param nodeHandle Native pointer to this generator
	 * @param addRef Whether the generator should be registered
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	GestureGenerator(Context context, long nodeHandle, boolean addRef) throws GeneralException 
	{
		super(context, nodeHandle, addRef);
		
		gestureRecognizedEvent = new Observable<GestureRecognizedEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterGestureCallbacks(toNative(), this, "callback", null, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterHandCallbacks(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(String gesture, Point3D idPosition, Point3D endPosition)
			{
				notify(new GestureRecognizedEventArgs(gesture, idPosition, endPosition));
			}
		};
		gestureProgressEvent = new Observable<GestureProgressEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterGestureCallbacks(toNative(), this, null, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterHandCallbacks(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(String gesture, Point3D position, float progress)
			{
				notify(new GestureProgressEventArgs(gesture, position, progress));
			}
		};
		gestureIntermediateStageCompletedEvent = new Observable<GesturePositionEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterToGestureIntermediateStageCompleted(toNative(), this, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromGestureIntermediateStageCompleted(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(String gesture, Point3D position)
			{
				notify(new GesturePositionEventArgs(gesture, position));
			}
		};
		gestureReadyForNextIntermediateStageEvent = new Observable<GesturePositionEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterToGestureReadyForNextIntermediateStage(toNative(), this, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromGestureReadyForNextIntermediateStage(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(String gesture, Point3D position)
			{
				notify(new GesturePositionEventArgs(gesture, position));
			}
		};
		gestureChangedEvent = new StateChangedObservable()
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback)
			{
				return NativeMethods.xnRegisterToGestureChange(toNative(), this, cb, phCallback);
			}
			@Override
			protected void unregisterNative(long hCallback)
			{
				NativeMethods.xnUnregisterFromGestureChange(toNative(), hCallback);
			}
			
		};
	}
	
	/**
	 * Creates a new gesture generator, filtering by query and storing error information
	 * @param context OpenNI Context
	 * @param query Query object to filter results
	 * @param errors Used to store query information
	 * @return Resulting GestureGenerator
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public static GestureGenerator create(Context context, Query query, EnumerationErrors errors) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateGestureGenerator(context.toNative(), handle,
			query == null ? 0 : query.toNative(),
			errors == null ? 0 : errors.toNative());
		WrapperUtils.throwOnError(status);
		GestureGenerator result = (GestureGenerator)context.createProductionNodeObject(handle.value, NodeType.GESTURE);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	/** Creates a new gesture generator, filtering by query 
	 * @param context OpenNI context
	 * @param query Query object to filter results
	 * @return Resulting GestureGenerator
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public static GestureGenerator create(Context context, Query query) throws GeneralException
	{
		return create(context, query, null);
	}

	/**
	 * Creates a new gesture generator from the given context	
	 * @param context OpenNI context
	 * @return Resulting gesture generator
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public static GestureGenerator create(Context context) throws GeneralException
	{
		return create(context, null, null);
	}
	
	/**
	 * Adds a given gesture to the list of gestures to be detected
	 * @param gesture String name of the gesture to be detected
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void addGesture(String gesture) throws StatusException
	{
		int status = NativeMethods.xnAddGesture(toNative(), gesture);
		WrapperUtils.throwOnError(status);
	}
	/**
	 * Adds a given gesture to be detected, within a given bounding box
	 * @param gesture String name of the gesture to be detected
	 * @param area Area in space where this gesture is valid
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void addGesture(String gesture, BoundingBox3D area) throws StatusException
	{
		if (area == null)
		{
			addGesture(gesture);
			return;
		}
		int status = NativeMethods.xnAddGesture(toNative(), gesture, area.getMins().getX(), area.getMins().getY(), area.getMins().getZ(), area.getMaxs().getX(), area.getMaxs().getY(), area.getMaxs().getZ());
		WrapperUtils.throwOnError(status);
	}
	/**
	 * Removes a given gesture from the list of gestures to be detected
	 * @param gesture String name of the gesture to be removed from the list
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void removeGesture(String gesture) throws StatusException
	{
		int status = NativeMethods.xnRemoveGesture(toNative(), gesture);
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Queries the available number of gestures that can be detected
	 * @return Number of gestures that can potentially be detected by this generator
	 */
	public int getNumberOfAvailableGestures()
	{
		return NativeMethods.xnGetNumberOfAvailableGestures(toNative());
	}
	
	/**
	 * Queries whether a specific gesture is available from this generator
	 * @param gesture Gesture to be tested for
	 * @return TRUE if this gesture is available, FALSE otherwise
	 */
	public boolean isGestureAvailable(String gesture)
	{
		return NativeMethods.xnIsGestureAvailable(toNative(), gesture);
	}
	/**
	 * Queries whether a specific gesture supports gesture progress queries
	 * @param gesture Gesture to test for progress support
	 * @return TRUE if gesture supports Progress, FALSE otherwise
	 */
	public boolean isGestureProgressSupported(String gesture)
	{
		return NativeMethods.xnIsGestureProgressSupported(toNative(), gesture);
	}

	/**
	 * Enumerates all available gestures
	 * @return Array of strings, each containing the name of an available gesture
	 * @throws StatusException If underlying native code returns errors, a Status Exception is thrown by this function
	 */
	public String[] enumerateAllGestures() throws StatusException
	{
		OutArg<String[]> gestures = new OutArg<String[]>();
		int status = NativeMethods.xnEnumerateAllGestures(toNative(), gestures);
		WrapperUtils.throwOnError(status);
		return gestures.value;
	}
	
	/**
	 * Enumerates all currently active gestures
	 * @return Array of strings, each containing the name of an active gesture
	 * @throws StatusException If underlying native code returns errors, a Status Exception is thrown by this function
	 */
	public String[] getAllActiveGestures() throws StatusException
	{
		OutArg<String[]> gestures = new OutArg<String[]>();
		int status = NativeMethods.xnGetAllActiveGestures(toNative(), gestures);
		WrapperUtils.throwOnError(status);
		return gestures.value;
	}
	
	// Events
	/**
	 * Provides access to the Gesture Recognized event
	 * @return
	 */
	public IObservable<GestureRecognizedEventArgs> getGestureRecognizedEvent()
	{
		return gestureRecognizedEvent;
	}
	/**
	 * Provides access to the Gesture Progress event
	 * @return
	 */
	public IObservable<GestureProgressEventArgs> getGestureProgressEvent()
	{
		return gestureProgressEvent;
	}
	/** 
	 * Provides access to the Gesture Intermediate Stage Completed event
	 * @return
	 */
	public IObservable<GesturePositionEventArgs> getGestureIntermediateStageCompletedEvent()
	{
		return gestureIntermediateStageCompletedEvent;
	}
	/**
	 * Provides access to the Ready for Next Intermediate Stage event
	 * @return
	 */
	public IObservable<GesturePositionEventArgs> getGestureReadyForNextIntermediateStageEvent()
	{
		return gestureReadyForNextIntermediateStageEvent;
	}
	/**
	 * Provides access to the Gesture Changed event
	 * @return
	 */
	public IStateChangedObservable getGestureChangedEvent()
	{
		return gestureChangedEvent;
	}
	
	private Observable<GestureRecognizedEventArgs> gestureRecognizedEvent;
	private Observable<GestureProgressEventArgs> gestureProgressEvent;
	private Observable<GesturePositionEventArgs> gestureIntermediateStageCompletedEvent;
	private Observable<GesturePositionEventArgs> gestureReadyForNextIntermediateStageEvent;
	private StateChangedObservable gestureChangedEvent;
}
