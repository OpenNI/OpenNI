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
 * Stores the data created when a Node Destroyed Event is generated. <BR><BR>
 * 
 * This data consists of the string name of the node that was destroyed.
 *
 */
public class NodeDestroyedEventArgs extends EventArgs
{
	/**
	 * Create a new object with the specified Node Name
	 * @param destroyedNodeName string name of the node that was destroyed
	 */
	public NodeDestroyedEventArgs(String destroyedNodeName)
	{
		this.destroyedNodeName = destroyedNodeName;
	}
	
	/** Getter function for the destroyed node name
	 * @return String name of the node that was destroyed to trigger this event
	 */
	public String getDestroyedNodeName()
	{
		return destroyedNodeName;
	}

	private String destroyedNodeName;
}
