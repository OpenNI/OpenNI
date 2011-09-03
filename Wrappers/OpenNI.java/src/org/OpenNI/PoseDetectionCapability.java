package org.OpenNI;

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
	public String[] getAllAvailablePoses() throws StatusException
	{
		OutArg<String[]> poses = new OutArg<String[]>();
		int status = NativeMethods.xnGetAllAvailablePoses(toNative(), poses);
		WrapperUtils.throwOnError(status);
		return poses.value;
	}
	public void StartPoseDetection(String pose, int user) throws StatusException
	{
		int status = NativeMethods.xnStartPoseDetection(toNative(), pose, user);
		WrapperUtils.throwOnError(status);
	}
	public void StopPoseDetection(int user) throws StatusException
	{
		int status = NativeMethods.xnStopPoseDetection(toNative(), user);
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
