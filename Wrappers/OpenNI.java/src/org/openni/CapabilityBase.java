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
 * This is the base class for all capability types. <BR><BR>
 * 
 * There would usually be no reason to use this class directly.  It is provided
 * primarily to factor out common parts of the constructor for each capability.
 *
 */
public class CapabilityBase extends NodeWrapper
{
	/**
	 * This is a constructor that factors out common functionality of the various 
	 * capability classes that extend CapabilityBase.  Normally this function 
	 * would only be called by the constructors of its descendants.
	 * @param node Production node that this capability will be attached to
	 * @throws StatusException Required since we may be talking to hardware when we 
	 * access the node
	 */
	public CapabilityBase(ProductionNode node) throws StatusException
	{
		super(node.getContext(), node.toNative(), true);
		this.node = node;
	}

	private ProductionNode node;
}
