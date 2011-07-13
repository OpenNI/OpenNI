package org.OpenNI;

public class MirrorCapability extends CapabilityBase
{
	public MirrorCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		this.mirrorChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToMirrorChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromMirrorChange(toNative(), hCallback);
			}
		};
	}
	
	public boolean isMirrored()
	{
		return NativeMethods.xnIsMirrored(toNative());
	}
	
	public void setMirror(boolean isMirrored) throws StatusException
	{
		int status = NativeMethods.xnSetMirror(toNative(), isMirrored);
		WrapperUtils.throwOnError(status);
	}

	public IStateChangedObservable getMirrorChangedEvent() { return this.mirrorChanged; }

	private StateChangedObservable mirrorChanged;
}
