package org.OpenNI;

public class ProductionNode extends NodeWrapper
{
	ProductionNode(Context context, long nodeHandle, boolean addRef) throws StatusException
	{
		super(context, nodeHandle, addRef);
	}
	
	public static ProductionNode fromNative(long hNode) throws GeneralException
	{
		return Context.createProductionNodeFromNative(hNode);
	}
	
	public NodeInfo getInfo() throws GeneralException
	{
		return new NodeInfo(NativeMethods.xnGetNodeInfo(toNative()));
	}
	
	public void addNeededNode(ProductionNode needed) throws StatusException
	{
		int status = NativeMethods.xnAddNeededNode(toNative(), needed.toNative());
		WrapperUtils.throwOnError(status);
	}

	public void removeNeededNode(ProductionNode needed) throws StatusException
	{
		int status = NativeMethods.xnRemoveNeededNode(toNative(), needed.toNative());
		WrapperUtils.throwOnError(status);
	}

	public boolean isCapabilitySupported(String capabilityName)
	{
		return NativeMethods.xnIsCapabilitySupported(this.toNative(), capabilityName);
	}

	public void setIntProperty(String propName, long value) throws StatusException
	{
		int status = NativeMethods.xnSetIntProperty(this.toNative(), propName, value);
		WrapperUtils.throwOnError(status);
	}

	public void setRealProperty(String propName, double value) throws StatusException
	{
		int status = NativeMethods.xnSetRealProperty(this.toNative(), propName, value);
		WrapperUtils.throwOnError(status);
	}

	public void setStringProperty(String propName, String value) throws StatusException
	{
		int status = NativeMethods.xnSetStringProperty(this.toNative(), propName, value);
		WrapperUtils.throwOnError(status);
	}

	public void setGeneralProperty(String propName, int size, long buff) throws StatusException
	{
		int status = NativeMethods.xnSetGeneralProperty(this.toNative(), propName, size, buff);
		WrapperUtils.throwOnError(status);
	}

	public long getIntProperty(String propName) throws StatusException
	{
		OutArg<Long> value = new OutArg<Long>();
		int status = NativeMethods.xnGetIntProperty(this.toNative(), propName, value);
		WrapperUtils.throwOnError(status);
		return value.value.longValue();
	}

	public double getRealProperty(String propName) throws StatusException
	{
		OutArg<Double> value = new OutArg<Double>();
		int status = NativeMethods.xnGetRealProperty(this.toNative(), propName, value);
		WrapperUtils.throwOnError(status);
		return value.value.doubleValue();
	}

	public String getStringProperty(String propName) throws StatusException
	{
		OutArg<String> value = new OutArg<String>();
		int status = NativeMethods.xnGetStringProperty(this.toNative(), propName, value);
		WrapperUtils.throwOnError(status);
		return value.value;
	}

	public void getGeneralProperty(String propName, int size, long buff) throws StatusException
	{
		int status = NativeMethods.xnGetGeneralProperty(this.toNative(), propName, size, buff);
		WrapperUtils.throwOnError(status);
	}

	public LockHandle lockForChanges() throws StatusException
	{
		OutArg<Integer> handle = new OutArg<Integer>();
		int status = NativeMethods.xnLockNodeForChanges(this.toNative(), handle);
		WrapperUtils.throwOnError(status);
		return new LockHandle(handle.value.intValue());
	}

	public void unlockForChanges(LockHandle lockHandle) throws StatusException
	{
		int status = NativeMethods.xnUnlockNodeForChanges(this.toNative(), lockHandle.toNative());
		WrapperUtils.throwOnError(status);
	}

	public void lockedNodeStartChanges(LockHandle lockHandle) throws StatusException
	{
		int status = NativeMethods.xnLockedNodeStartChanges(this.toNative(), lockHandle.toNative());
		WrapperUtils.throwOnError(status);
	}

	public void lockedNodeEndChanges(LockHandle lockHandle) throws StatusException
	{
		int status = NativeMethods.xnLockedNodeEndChanges(this.toNative(), lockHandle.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	public ErrorStateCapability getErrorStateCapability() throws StatusException
	{
		return new ErrorStateCapability(this);
	}

	public GeneralIntCapability getGeneralIntCapability(Capability capability) throws StatusException
	{
		return new GeneralIntCapability(this, capability);
	}
}
