package org.OpenNI;

public class ErrorStateCapability extends CapabilityBase
{
	public ErrorStateCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		this.errorStateChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToNodeErrorStateChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromNodeErrorStateChange(toNative(), hCallback);
			}
		};
	}
	
	public String getErrorState()
	{
		int status = NativeMethods.xnGetNodeErrorState(toNative());
		if (status == 0)
		{
			return null;
		}
		else
		{
			return NativeMethods.xnGetStatusString(status);
		}
	}
	
	public IStateChangedObservable getErrorStateChangedEvent() { return this.errorStateChanged; }

	private StateChangedObservable errorStateChanged;
}
