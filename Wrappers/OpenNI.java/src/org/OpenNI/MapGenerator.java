package org.OpenNI;

public class MapGenerator extends Generator 
{
	MapGenerator(Context context, long nodeHandle, boolean addRef) throws GeneralException 
	{
		super(context, nodeHandle, addRef);
		
		this.mapOutputModeChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToMapOutputModeChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromMapOutputModeChange(toNative(), hCallback);
			}
		}; 
	}

	public MapOutputMode[] getSupportedMapOutputModes() throws StatusException
	{
		int count = NativeMethods.xnGetSupportedMapOutputModesCount(this.toNative());
		MapOutputMode[] supportedModes = new MapOutputMode[count];
		int status = NativeMethods.xnGetSupportedMapOutputModes(this.toNative(), supportedModes);
		WrapperUtils.throwOnError(status);
		return supportedModes;
	}

	public MapOutputMode getMapOutputMode() throws StatusException
	{
		OutArg<Integer> xRes = new OutArg<Integer>();
		OutArg<Integer> yRes = new OutArg<Integer>();
		OutArg<Integer> FPS = new OutArg<Integer>();
		int status = NativeMethods.xnGetMapOutputMode(this.toNative(), xRes, yRes, FPS);
		WrapperUtils.throwOnError(status);
		return new MapOutputMode(xRes.value, yRes.value, FPS.value);
	}
	
	public void setMapOutputMode(MapOutputMode mode) throws StatusException
	{
		int status = NativeMethods.xnSetMapOutputMode(this.toNative(), mode.getXRes(), mode.getYRes(), mode.getFPS());
		WrapperUtils.throwOnError(status);
	}

	public IStateChangedObservable getMapOutputModeChangedEvent() { return this.mapOutputModeChanged; }

	public CroppingCapability getCroppingCapability() throws StatusException
	{
		return new CroppingCapability(this);
	}

	public GeneralIntCapability getBrightnessCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Brightness);
	}

	public GeneralIntCapability getContrastCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Contrast);
	}

	public GeneralIntCapability getSaturationCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Saturation);
	}

	public GeneralIntCapability getHueCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Hue);
	}

	public GeneralIntCapability getSharpnessCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Sharpness);
	}

	public GeneralIntCapability getGammaCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Gamma);
	}

	public GeneralIntCapability getWhiteBalanceCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.WhiteBalance);
	}

	public GeneralIntCapability getBacklightCompensationCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.BacklightCompensation);
	}

	public GeneralIntCapability getGainCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Gain);
	}

	public GeneralIntCapability getPanCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Pan);
	}

	public GeneralIntCapability getTiltCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Tilt);
	}

	public GeneralIntCapability getRollCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Roll);
	}

	public GeneralIntCapability getZoomCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Zoom);
	}

	public GeneralIntCapability getExposureCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Exposure);
	}

	public GeneralIntCapability getIrisCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Iris);
	}

	public GeneralIntCapability getFocusCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.Focus);
	}

	public GeneralIntCapability getLowLightCompensationCapability() throws StatusException
	{
		return new GeneralIntCapability(this, Capability.LowLightCompensation);
	}

	public AntiFlickerCapability getAntiFlickerCapability() throws StatusException
	{
		return new AntiFlickerCapability(this);
	}

	private StateChangedObservable mapOutputModeChanged;
}
