package org.OpenNI;

public class Device extends ProductionNode 
{
	public Device(Context context, long nodeHandle, boolean addRef) throws StatusException 
	{
		super(context, nodeHandle, addRef);
	}
	
	public static Device create(Context context, Query query, EnumerationErrors errors) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateDevice(context.toNative(), handle,
			query == null ? 0 : query.toNative(),
			errors == null ? 0 : errors.toNative());
		WrapperUtils.throwOnError(status);
		Device result = (Device)context.createProductionNodeObject(handle.value, NodeType.DEVICE);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	public static Device create(Context context, Query query) throws GeneralException
	{
		return create(context, query, null);
	}

	public static Device create(Context context) throws GeneralException
	{
		return create(context, null, null);
	}
	
	public DeviceIdentificationCapability getDeviceIdentificationCapability() throws StatusException
	{
		return new DeviceIdentificationCapability(this);
	}
}
