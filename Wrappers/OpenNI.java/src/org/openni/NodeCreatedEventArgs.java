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
 * Stores the data created when a Node Created Event is generated. <BR><BR>
 * 
 * This data consists only of a reference to the production node that was just created.
 *
 */
public class NodeCreatedEventArgs extends EventArgs
{
	/**
	 * Constructor -- Creates a new object with the specified production node
	 * @param createdNode
	 */
	public NodeCreatedEventArgs(ProductionNode createdNode)
	{
		this.createdNode = createdNode;
	}
	
	/**
	 * Getter function for the node that was created
	 * @return The Production Node that was created to trigger the event.
	 */
	public ProductionNode getCreatedNode()
	{
		return this.createdNode;
	}

	private ProductionNode createdNode;
}
