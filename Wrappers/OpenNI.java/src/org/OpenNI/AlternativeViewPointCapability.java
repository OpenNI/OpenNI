package org.OpenNI;

public class AlternativeViewPointCapability extends CapabilityBase
{
	public AlternativeViewPointCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		this.viewPointChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToViewPointChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromViewPointChange(toNative(), hCallback);
			}
		};
	}
	
	boolean isViewpointSupported(ProductionNode other)
	{
		return NativeMethods.xnIsViewPointSupported(toNative(), other.toNative());
	}
	
	void setViewpoint(ProductionNode other) throws StatusException
	{
		int status = NativeMethods.xnSetViewPoint(toNative(), other.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	void resetViewpoint() throws StatusException
	{
		int status = NativeMethods.xnResetViewPoint(toNative());
		WrapperUtils.throwOnError(status);
	}
	
	boolean isViewpointAs(ProductionNode other)
	{
		return NativeMethods.xnIsViewPointAs(toNative(), other.toNative());
	}

	public IStateChangedObservable getViewPointChangedEvent() { return this.viewPointChanged; }

	private StateChangedObservable viewPointChanged;
}
