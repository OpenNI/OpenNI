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
 * Enables a user generator to recognize when the user is posed in a specific position. <BR><BR>
 * 
 * This is used most commonly to recognize when a user has entered a "calibration pose" for the skeleton.  It
 * is used less frequently if a no-calibration skeleton is available.
 *
 */
public class PoseDetectionCapability extends CapabilityBase
{
	public PoseDetectionCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		// Events
		poseDetectedEvent = new Observable<PoseDetectionEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterToPoseDetected(toNative(), this, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromPoseDetected(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(String pose, int user)
			{
				notify(new PoseDetectionEventArgs(pose, user));
			}
		};
		outOfPoseEvent = new Observable<PoseDetectionEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterToOutOfPose(toNative(), this, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromOutOfPose(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(String pose, int user)
			{
				notify(new PoseDetectionEventArgs(pose, user));
			}
		};
		poseDetectionInProgressEvent = new Observable<PoseDetectionInProgressEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterToPoseDetectionInProgress(toNative(), this, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromPoseDetectionInProgress(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(String pose, int user, int status)
			{
				notify(new PoseDetectionInProgressEventArgs(pose, user, PoseDetectionStatus.fromNative(status)));
			}
		};
	}

	public int getNumberOfPoses()
	{
		return NativeMethods.xnGetNumberOfPoses(toNative());
	}
	
	public boolean isPoseSupported(String pose)
	{
		return NativeMethods.xnIsPoseSupported(toNative(),pose);
	}
	
	public void getPoseStatus(int user, String pose, OutArg<Long> poseTime, OutArg<PoseDetectionStatus> eStatus, OutArg<PoseDetectionState> eState ) throws StatusException
	{
		OutArg<Integer> eInnerStatus = new OutArg<Integer>();
		OutArg<Integer> eInnerState = new OutArg<Integer>();
		int status = NativeMethods.xnGetPoseStatus(toNative(), user, pose, poseTime, eInnerStatus,eInnerState);
		eStatus.value = PoseDetectionStatus.fromNative(eInnerStatus.value);
		eState.value = PoseDetectionState.fromNative(eInnerState.value);
		WrapperUtils.throwOnError(status);
	}
	
	public String[] getAllAvailablePoses() throws StatusException
	{
		OutArg<String[]> poses = new OutArg<String[]>();
		int status = NativeMethods.xnGetAllAvailablePoses(toNative(), poses);
		WrapperUtils.throwOnError(status);
		return poses.value;
	}
	/**
	 * @deprecated Out of date. Use startPoseDetection() instead.
	 */ 
	@Deprecated
	 public void StartPoseDetection(String pose, int user) throws StatusException
	{
		int status = NativeMethods.xnStartPoseDetection(toNative(), pose, user);
		WrapperUtils.throwOnError(status);
	}
	/**
	 * @deprecated Out of date. Use stopPoseDetection() instead.
	 */ 
	@Deprecated
	public void StopPoseDetection(int user) throws StatusException
	{
		int status = NativeMethods.xnStopPoseDetection(toNative(), user);
		WrapperUtils.throwOnError(status);
	}
	public void startPoseDetection(String pose, int user) throws StatusException
	{
		int status = NativeMethods.xnStartPoseDetection(toNative(), pose, user);
		WrapperUtils.throwOnError(status);
	}
	public void stopPoseDetection(int user) throws StatusException
	{
		int status = NativeMethods.xnStopPoseDetection(toNative(), user);
		WrapperUtils.throwOnError(status);
	}
	public void stopSinglePoseDetection(int user, String pose) throws StatusException
	{
		int status = NativeMethods.xnStopSinglePoseDetection(toNative(), user, pose);
		WrapperUtils.throwOnError(status);
	}
	// Events
	public IObservable<PoseDetectionEventArgs> getPoseDetectedEvent()
	{
		return poseDetectedEvent;
	}
	public IObservable<PoseDetectionEventArgs> getOutOfPoseEvent()
	{
		return outOfPoseEvent;
	}
	public IObservable<PoseDetectionInProgressEventArgs> getPoseDetectionInProgressEvent()
	{
		return poseDetectionInProgressEvent;
	}
	
	private Observable<PoseDetectionEventArgs> poseDetectedEvent;
	private Observable<PoseDetectionEventArgs> outOfPoseEvent;
	private Observable<PoseDetectionInProgressEventArgs> poseDetectionInProgressEvent;
}
