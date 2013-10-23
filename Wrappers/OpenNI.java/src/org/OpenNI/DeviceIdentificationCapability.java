/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
package org.openni;

/**
 * Allows identification of a hardware device. <BR><BR>
 * 
 * This capability is used to query identification strings stored in 
 * a hardware device.  
 *
 */
public class DeviceIdentificationCapability extends CapabilityBase
{
	/**
	 * Constructor to create a new Device Identification Capability
	 * This function will not normally be called directly by application code. 
	 * The GetDeviceIdentificationCapability() function in class Device should instead
	 * be used to create a new capability.
	 * @param node The device the capability will be attached to
	 * @throws StatusException Talks to hardware, so Status Exceptions are possible
	 */
	public DeviceIdentificationCapability(ProductionNode node)
			throws StatusException
	{
		super(node);
	}

	/**
	 * Requests the device name string
	 * @return String value of the Device Name stored in the hardware device
	 * @throws StatusException Talks to hardware, so Status Exceptions are possible
	 */
	public String getDeviceName() throws StatusException
	{
		OutArg<String> result = new OutArg<String>();
		int status = NativeMethods.xnGetDeviceName(toNative(), result);
		WrapperUtils.throwOnError(status);
		return result.value;
	}

	/**
	 * Requests any vendor specific data strings stored in the device
	 * @return String value of the vendor specific data stored in the device
	 * @throws StatusException Talks to hardware, so Status Exceptions are possible
	 */
	public String getVendorSpecificData() throws StatusException
	{
		OutArg<String> result = new OutArg<String>();
		int status = NativeMethods.xnGetVendorSpecificData(toNative(), result);
		WrapperUtils.throwOnError(status);
		return result.value;
	}

	/**
	 * Requests the serial number string stored in the device
	 * @return String value of the serial number stored in the device
	 * @throws StatusException Talks to hardware, so Status Exceptions are possible
	 */
	public String getSerialNumber() throws StatusException
	{
		OutArg<String> result = new OutArg<String>();
		int status = NativeMethods.xnGetSerialNumber(toNative(), result);
		WrapperUtils.throwOnError(status);
		return result.value;
	}
}
