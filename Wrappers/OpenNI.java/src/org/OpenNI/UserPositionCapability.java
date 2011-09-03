package org.OpenNI;

public class UserPositionCapability extends CapabilityBase
{
	public UserPositionCapability(ProductionNode node) throws StatusException
	{
		super(node);
		
		this.userPositionChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToUserPositionChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromUserPositionChange(toNative(), hCallback);
			}
		};
	}

	public int getSupportedCount()
	{
		return NativeMethods.xnGetSupportedUserPositionsCount(toNative());
	}
	
	public void setUserPosition(int index, BoundingBox3D position) throws StatusException
	{
		Point3D leftBottomNear = position.getLeftBottomNear();
		Point3D rightTopFar = position.getRightTopFar();
		int status = NativeMethods.xnSetUserPosition(toNative(), index, leftBottomNear.getX(), leftBottomNear.getY(), leftBottomNear.getZ(), rightTopFar.getX(), rightTopFar.getY(), rightTopFar.getZ());
		WrapperUtils.throwOnError(status);
	}
	
	public BoundingBox3D getUserPosition(int index) throws StatusException
	{
		OutArg<BoundingBox3D> position = new OutArg<BoundingBox3D>();
		int status = NativeMethods.xnGetUserPosition(toNative(), index, position);
		WrapperUtils.throwOnError(status);
		return position.value;
	}
	
	public IStateChangedObservable getUserPositionChangedEvent() { return this.userPositionChanged; }
	
	private StateChangedObservable userPositionChanged;
}
