package org.OpenNI;

public class AudioGenerator extends Generator
{
	public AudioGenerator(Context context, long nodeHandle, boolean addRef) throws GeneralException
	{
		super(context, nodeHandle, addRef);
		
		this.waveOutputModeChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToWaveOutputModeChanges(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromWaveOutputModeChanges(toNative(), hCallback);
			}
		}; 
	}

	public static AudioGenerator create(Context context, Query query, EnumerationErrors errors) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateAudioGenerator(context.toNative(), handle,
			query == null ? 0 : query.toNative(),
			errors == null ? 0 : errors.toNative());
		WrapperUtils.throwOnError(status);
		AudioGenerator result = (AudioGenerator)context.createProductionNodeObject(handle.value, NodeType.AUDIO);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	public static AudioGenerator create(Context context, Query query) throws GeneralException
	{
		return create(context, query, null);
	}

	public static AudioGenerator create(Context context) throws GeneralException
	{
		return create(context, null, null);
	}
	
	public WaveOutputMode[] getSupportedMapOutputModes() throws StatusException
	{
		int count = NativeMethods.xnGetSupportedWaveOutputModesCount(this.toNative());
		WaveOutputMode[] supportedModes = new WaveOutputMode[count];
		int status = NativeMethods.xnGetSupportedWaveOutputModes(this.toNative(), supportedModes);
		WrapperUtils.throwOnError(status);
		return supportedModes;
	}

	public WaveOutputMode getWaveOutputMode() throws StatusException
	{
		OutArg<Integer> sampleRate = new OutArg<Integer>();
		OutArg<Short> bitsPerSample = new OutArg<Short>();
		OutArg<Byte> numberOfChannels = new OutArg<Byte>();
		int status = NativeMethods.xnGetWaveOutputMode(this.toNative(), sampleRate, bitsPerSample, numberOfChannels);
		WrapperUtils.throwOnError(status);
		return new WaveOutputMode(sampleRate.value, bitsPerSample.value, numberOfChannels.value);
	}
	
	public void setWaveOutputMode(WaveOutputMode mode) throws StatusException
	{
		int status = NativeMethods.xnSetWaveOutputMode(this.toNative(), mode.getSampleRate(), mode.getBitsPerSample(), mode.getNumberOfChannels());
		WrapperUtils.throwOnError(status);
	}

	public IStateChangedObservable getMapOutputModeChangedEvent() { return this.waveOutputModeChanged; }

	private StateChangedObservable waveOutputModeChanged;
}
