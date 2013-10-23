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
 * Used to filter the results of searching an OpenNI Context. <BR><BR>
 * 
 * Most types of production nodes that can be searched for in an OpenNI context have
 * many possible parameters.  This object stores search parameters to limit searches of 
 * the context for a given node type
 *
 */
public class Query extends ObjectWrapper
{
	/**
	 * Default Constructor, creates a new empty Query
	 * @throws GeneralException If underlying native code returns errors, a General Exception will be generated
	 */
	public Query() throws GeneralException 
	{
		super(allocate());
	}
	
	/**
	 * Sets the desired vendor for the node search
	 * @param vendor Vendor name
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
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
