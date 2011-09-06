package org.OpenNI;

public class CroppingCapability extends CapabilityBase
{
	public CroppingCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		this.croppingChanged = new StateChangedObservable()
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToCroppingChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromCroppingChange(toNative(), hCallback);
			}
		};
	}
	
	public void setCropping(Cropping cropping) throws StatusException
	{
		int status = NativeMethods.xnSetCropping(toNative(), cropping.getXOffset(), cropping.getYOffset(), cropping.getXSize(), cropping.getYSize(), cropping.isEnabled());
		WrapperUtils.throwOnError(status);
	}
	
	public Cropping getCropping() throws StatusException
	{
		OutArg<Integer> xOffset = new OutArg<Integer>();
		OutArg<Integer> yOffset = new OutArg<Integer>();
		OutArg<Integer> xSize = new OutArg<Integer>();
		OutArg<Integer> ySize = new OutArg<Integer>();
		OutArg<Boolean> isEnabled = new OutArg<Boolean>();
		int status = NativeMethods.xnGetCropping(toNative(), xOffset, yOffset, xSize, ySize, isEnabled);
		WrapperUtils.throwOnError(status);
		return new Cropping(xOffset.value, yOffset.value, xSize.value, ySize.value, isEnabled.value);
	}
	
	public IStateChangedObservable getCroppingChangedEvent() { return this.croppingChanged; }

	private StateChangedObservable croppingChanged;
}
