package org.OpenNI;

public class AntiFlickerCapability extends CapabilityBase
{
	public AntiFlickerCapability(ProductionNode node) throws StatusException
	{
		super(node);

		this.powerLineFrequencyChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToPowerLineFrequencyChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromPowerLineFrequencyChange(toNative(), hCallback);
			}
		};
	}
	
	public PowerLineFrequency getPowerLineFrequency()
	{
		return PowerLineFrequency.fromNative(NativeMethods.xnGetPowerLineFrequency(toNative()));
	}
	
	public void setPowerLineFrequency(PowerLineFrequency frequency) throws StatusException
	{
		int status = NativeMethods.xnSetPowerLineFrequency(toNative(), frequency.toNative());
		WrapperUtils.throwOnError(status);
	}

	public IStateChangedObservable getPowerLineFrequencyChangedEvent() { return this.powerLineFrequencyChanged; }

	private StateChangedObservable powerLineFrequencyChanged;
}
