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
 * Production node that represents a physical device. <BR><BR>
 * 
 * This class provides a ProductionNode that can be tied to a physical sensing device. 
 * It enables application code to connect to a device, and obtain the required capabilities
 * to properly identify that device
 *
 */
public class Device extends ProductionNode 
{
	/**
	 * Creates a new device object
	 * @param context OpenNI context
	 * @param nodeHandle
	 * @param addRef
	 * @throws StatusException Function potentially communicates with hardware, so status exception possible
	 */
	public Device(Context context, long nodeHandle, boolean addRef) throws StatusException 
	{
		super(context, nodeHandle, addRef);
	}
	
	/**
	 * Creates a device, storing query and error information
	 * @param context OpenNI Context
	 * @param query Used to store the query information
	 * @param errors Used to store any enumeration errors
	 * @return
	 * @throws GeneralException Talks to hardware, so General Exceptions may occur and should be handled
	 */
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

	/**
	 * Creates a device.  Stores query information, but discards error information
	 * @param context OpenNI Context
	 * @param query Used to store query information
	 * @return
	 * @throws GeneralException Talks to hardware, so General Exceptions may occur and should be handled
	 */
	public static Device create(Context context, Query query) throws GeneralException
	{
		return create(context, query, null);
	}

	/**
	 * Creates a device, but does not store query or context information
	 * @param context OpenNI Context
	 * @return
	 * @throws GeneralException Talks to hardware, so General Exceptions may occur and should be handled
	 */
	public static Device create(Context context) throws GeneralException
	{
		return create(context, null, null);
	}
	
	/**
	 * Requests a Device Identification Capability.  This is the usual way to obtain this capability
	 * @return Device Identification Capability for this device
	 * @throws StatusException Function communicates with hardware, so status exceptions are possible
	 */
	public DeviceIdentificationCapability getDeviceIdentificationCapability() throws StatusException
	{
		return new DeviceIdentificationCapability(this);
	}
}
