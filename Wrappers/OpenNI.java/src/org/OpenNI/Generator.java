package org.OpenNI;

public class Generator extends ProductionNode 
{
	Generator(Context context, long nodeHandle, boolean addRef) throws GeneralException 
	{
		super(context, nodeHandle, addRef);
		
		this.generationRunningChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToGenerationRunningChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromGenerationRunningChange(toNative(), hCallback);
			}
		};
		
		this.newDataAvailable = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToNewDataAvailable(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromNewDataAvailable(toNative(), hCallback);
			}
		};		
	}

	public void startGenerating() throws StatusException
	{
		int status = NativeMethods.xnStartGenerating(this.toNative());
		WrapperUtils.throwOnError(status);
	}

	public boolean isGenerating()
	{
		return NativeMethods.xnIsGenerating(this.toNative());
	}

	public void stopGenerating() throws StatusException
	{
		int status = NativeMethods.xnStopGenerating(this.toNative());
		WrapperUtils.throwOnError(status);
	}

	public IStateChangedObservable getGenerationRunningChangedEvent()
	{
		return this.generationRunningChanged;
	}

	public boolean isNewDataAvailable()
	{
		OutArg<Long> timestamp = new OutArg<Long>();
		return NativeMethods.xnIsNewDataAvailable(this.toNative(), timestamp);
	}

	public long getAvailableTimestamp()
	{
		OutArg<Long> timestamp = new OutArg<Long>();
		NativeMethods.xnIsNewDataAvailable(this.toNative(), timestamp);
		return timestamp.value;
	}

	public IStateChangedObservable getNewDataAvailableEvent()
	{
		return this.newDataAvailable;
	}

	public void waitAndUpdateData() throws StatusException
	{
		int status = NativeMethods.xnWaitAndUpdateData(this.toNative());
		WrapperUtils.throwOnError(status);
	}

	public boolean isDataNew()
	{
		return NativeMethods.xnIsDataNew(this.toNative());
	}

	public int getDataSize()
	{
		return NativeMethods.xnGetDataSize(this.toNative());
	}

	public long getTimestamp()
	{
		return NativeMethods.xnGetTimestamp(this.toNative());
	}

	public int getFrameID()
	{
		return NativeMethods.xnGetFrameID(this.toNative());
	}

	// TODO: implmenet
	/*
	public MirrorCapability MirrorCapability
	{
		get
		{
			return new MirrorCapability(this);
		}
	}

	public AlternativeViewpointCapability AlternativeViewpointCapability
	{
		get
		{
			return new AlternativeViewpointCapability(this);
		}
	}

	public FrameSyncCapability FrameSyncCapability
	{
		get
		{
			return new FrameSyncCapability(this);
		}
	}
	*/

	private StateChangedObservable generationRunningChanged;
	private StateChangedObservable newDataAvailable;
}
