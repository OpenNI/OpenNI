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
 * Stores various property information about a production node. <BR><BR>
 *
 */
public class ProductionNodeDescription 
{
	/**
	 * Create a new ProductionNodeDescription with specified type, vendor, name and version strings
	 * @param type NodeType of this node
	 * @param vendor Vendor string for this node
	 * @param name Name string for this node
	 * @param version Version of this node
	 */
	public ProductionNodeDescription(NodeType type, String vendor, String name, Version version)
	{
		this.type = type;
		this.vendor = vendor;
		this.name = name;
		this.version = version;
	}

	@SuppressWarnings("unused")
	private ProductionNodeDescription(int type, String vendor, String name, Version version)
	{
		this(new NodeType(type), vendor, name, version);
	}

	/**
	 * Provides access to the type value
	 * @return NodeType of this node
	 */
	public NodeType getType() { return this.type; }
	/**
	 * Provides access to the vendor string
	 * @return Vendor string for this node
	 */
	public String getVendor() { return this.vendor; }
	/**
	 * Provides access to the name string
	 * @return Name string for this node
	 */
	public String getName() { return this.name; }
	/**
	 * Provides access to version information
	 * @return Version of this node
	 */
	public Version getVersion() { return this.version; }
	
	protected long createNative()
	{
		return NativeMethods.createProductionNodeDescription(type.toNative(), vendor, name, version.getMajor(), version.getMinor(), version.getMaintenance(), version.getBuild());
	}
	
	protected static void freeNative(long pNative)
	{
		NativeMethods.freeProductionNodeDescription(pNative);
	}

	private NodeType type;
	private String vendor;
	private String name;
	private Version version; 
}
