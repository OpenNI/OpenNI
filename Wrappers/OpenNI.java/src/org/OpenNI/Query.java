package org.OpenNI;

public class Query extends ObjectWrapper
{
	public Query() throws GeneralException 
	{
		super(allocate());
	}
	
	public void setVendor(String vendor) throws StatusException
	{
		int status = NativeMethods.xnNodeQuerySetVendor(toNative(), vendor);
		WrapperUtils.throwOnError(status);
	}

	public void setName(String name) throws StatusException
	{
		int status = NativeMethods.xnNodeQuerySetName(toNative(), name);
		WrapperUtils.throwOnError(status);
	}

	public void setMinVersion(Version version) throws StatusException
	{
		int status = NativeMethods.xnNodeQuerySetMinVersion(toNative(), version.getMajor(), version.getMinor(), version.getMaintenance(), version.getBuild());
		WrapperUtils.throwOnError(status);
	}

	public void setMaxVersion(Version version) throws StatusException
	{
		int status = NativeMethods.xnNodeQuerySetMaxVersion(toNative(), version.getMajor(), version.getMinor(), version.getMaintenance(), version.getBuild());
		WrapperUtils.throwOnError(status);
	}

	public void addSupportedCapability(Capability capability) throws StatusException
	{
		int status = NativeMethods.xnNodeQueryAddSupportedCapability(toNative(), capability.getName());
		WrapperUtils.throwOnError(status);
	}

	public void addSupportedMapOutputMode(MapOutputMode mode) throws StatusException
	{
		int status = NativeMethods.xnNodeQueryAddSupportedMapOutputMode(toNative(), mode.getXRes(), mode.getYRes(), mode.getFPS());
		WrapperUtils.throwOnError(status);
	}

	public void addSupportedMinUserPositions(int count) throws StatusException
	{
		int status = NativeMethods.xnNodeQuerySetSupportedMinUserPositions(toNative(), count);
		WrapperUtils.throwOnError(status);
	}

	public void setExistingNodeOnly(boolean existingOnly) throws StatusException
	{
		int status = NativeMethods.xnNodeQuerySetExistingNodeOnly(toNative(), existingOnly);
		WrapperUtils.throwOnError(status);
	}

	public void setNonExistingNodeOnly(boolean nonExistingOnly) throws StatusException
	{
		int status = NativeMethods.xnNodeQuerySetNonExistingNodeOnly(toNative(), nonExistingOnly);
		WrapperUtils.throwOnError(status);
	}
	
	public void addNeededNode(ProductionNode node) throws StatusException
	{
		int status = NativeMethods.xnNodeQueryAddNeededNode(toNative(), node.getName());
		WrapperUtils.throwOnError(status);
	}
	
	public void setCreationInfo(String creationInfo) throws StatusException
	{
		int status = NativeMethods.xnNodeQuerySetCreationInfo(toNative(), creationInfo);
		WrapperUtils.throwOnError(status);
	}

	@Override
	protected void freeObject(long ptr) 
	{
		NativeMethods.xnNodeQueryFree(ptr);
	}
	
	private static long allocate() throws StatusException
	{
		OutArg<Long> pQuery = new OutArg<Long>();
		int status = NativeMethods.xnNodeQueryAllocate(pQuery);
		WrapperUtils.throwOnError(status);
		return pQuery.value;
	}

}
