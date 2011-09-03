package org.OpenNI;

public class FrameSyncCapability extends CapabilityBase
{
	public FrameSyncCapability(ProductionNode node)
			throws StatusException
	{
		super(node);
		
		this.frameSyncChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToFrameSyncChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromFrameSyncChange(toNative(), hCallback);
			}
		};
	}
	
	boolean canFrameSyncWith(Generator other)
	{
		return NativeMethods.xnCanFrameSyncWith(toNative(), other.toNative());
	}
	
	void frameSyncWith(Generator other) throws StatusException
	{
		int status = NativeMethods.xnFrameSyncWith(toNative(), other.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	void stopFrameSyncWith(Generator other) throws StatusException
	{
		int status = NativeMethods.xnStopFrameSyncWith(toNative(), other.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	boolean isFrameSyncedWith(Generator other)
	{
		return NativeMethods.xnIsFrameSyncedWith(toNative(), other.toNative());
	}

	public IStateChangedObservable getFrameSyncChangedEvent() { return this.frameSyncChanged; }

	private StateChangedObservable frameSyncChanged;
}
