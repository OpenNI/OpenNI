package org.OpenNI;

public class DeviceIdentificationCapability extends CapabilityBase
{
	public DeviceIdentificationCapability(ProductionNode node)
			throws StatusException
	{
		super(node);
	}

	public String getDeviceName() throws StatusException
	{
		OutArg<String> result = new OutArg<String>();
		int status = NativeMethods.xnGetDeviceName(toNative(), result);
		WrapperUtils.throwOnError(status);
		return result.value;
	}

	public String getVendorSpecificData() throws StatusException
	{
		OutArg<String> result = new OutArg<String>();
		int status = NativeMethods.xnGetVendorSpecificData(toNative(), result);
		WrapperUtils.throwOnError(status);
		return result.value;
	}

	public String getSerialNumber() throws StatusException
	{
		OutArg<String> result = new OutArg<String>();
		int status = NativeMethods.xnGetSerialNumber(toNative(), result);
		WrapperUtils.throwOnError(status);
		return result.value;
	}
}
