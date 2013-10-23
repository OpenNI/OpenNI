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
 * Stores info about a ProductionNode. <BR><BR>
 * 
 * This class normally serves as a data member of a Production Node, and keeps track of 
 * various properties for the node that possesses it.
 *
 */
public class NodeInfo extends ObjectWrapper 
{
	/**
	 * Provides access to the description value for this production node.
	 * @return Node description
	 */
	public ProductionNodeDescription getDescription()
	{
		return NativeMethods.xnNodeInfoGetDescription(toNative());
	}
	
	/**
	 * Provides access to the Instance Name of this production node.
	 * @return Instance name
	 */
	public String getInstanceName()
	{
		return NativeMethods.xnNodeInfoGetInstanceName(toNative());
	}
	
	/**
	 * Provides access to the Creation Info string of a production node.
	 * @return Creation Info String
	 */
	public String getCreationInfo()
	{
		return NativeMethods.xnNodeInfoGetCreationInfo(toNative());
	}
	
	/**
	 * Provides a list of nodes needed by this node
	 * @return List of required nodes
	 * @throws GeneralException If underlying native code returns errors, a General Exception will be generated
	 */
	public NodeInfoList getNeededNodes() throws GeneralException
	{
		return new NodeInfoList(NativeMethods.xnNodeInfoGetNeededNodes(toNative()), false);
	}
	
	/**
	 * Gets an instance of a production node
	 * @return The production node that possesses this NodeInfo object
	 * @throws GeneralException If underlying native code returns errors, a General Exception will be generated
	 */
	public ProductionNode getInstance() throws GeneralException
	{
		long hNode = NativeMethods.xnNodeInfoGetRefHandle(toNative());
		return Context.createProductionNodeFromNative(hNode);
	}
	
	/**
	 * Converts this NodeInfo to a string
	 * @return String representation of this nodeInfo object
	 */
	@Override
	public String toString() 
	{
		OutArg<String> result = new OutArg<String>();
		NativeMethods.xnNodeInfoGetTreeStringRepresentation(toNative(), result);
		return result.value;
	}
	
	protected NodeInfo(long pNodeInfo) throws GeneralException 
	{
		super(pNodeInfo);
	}
	
	@Override
	protected void freeObject(long ptr) 
	{
		// no need to free
	}
}
