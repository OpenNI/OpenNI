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
 * Stores type information about a Production Node. <BR><BR>
 *
 */
public class NodeType 
{
	public static NodeType INVALID = new NodeType(0);

	/** A device node **/
	public static NodeType DEVICE = new NodeType(1);

	/** A depth generator **/
	public static NodeType DEPTH = new NodeType(2);

	/** An image generator **/
	public static NodeType IMAGE = new NodeType(3);

	/** An audio generator **/
	public static NodeType AUDIO = new NodeType(4);

	/** An IR generator **/
	public static NodeType IR = new NodeType(5);

	/** A user generator **/
	public static NodeType USER = new NodeType(6);

	/** A recorder **/
	public static NodeType RECORDER = new NodeType(7);

	/** A player **/
	public static NodeType PLAYER = new NodeType(8);

	/** A gesture generator **/
	public static NodeType GESTURE = new NodeType(9);

	/** A scene analyzer **/
	public static NodeType SCENE = new NodeType(10);

	/** A hands generator **/
	public static NodeType HANDS = new NodeType(11);

	/** A Codec **/
	public static NodeType CODEC = new NodeType(12);

	/** A general production node **/
	public static NodeType PRODUCTION_NODE = new NodeType(13);

	/** A general generator **/
	public static NodeType GENERATOR = new NodeType(14);

	/** A general map generator **/
	public static NodeType MAP_GENERATOR = new NodeType(15);

	public static NodeType SCRIPT_NODE = new NodeType(16);
	
	/**
	 * Creates a new NodeType object with specified value
	 * @param val NodeType value for this node
	 */
	NodeType(int val)
	{
		this.val = val;
	}
	
	/**
	 * Creates a hash code for a give node value
	 * @return The hash code created
	 */
	@Override
	public int hashCode()
	{
		final int prime = 31;
		int result = 1;
		result = prime * result + this.val;
		return result;
	}

	/**
	 * Tests for equality between this object and a specified object
	 * @param obj Object to test against for equality
	 * @return TRUE if the objects are equal, FALSE otherwise
	 */
	@Override
	public boolean equals(Object obj)
	{
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		NodeType other = (NodeType) obj;
		if (this.val != other.val)
			return false;
		return true;
	}

	/**
	 * Translates the NodeType contained in this object to a native value
	 * @return Resulting native value
	 */
	public int toNative() { return this.val; }
	
	/**
	 * Translates the NodeType contained in this object to a string value
	 * @return Resulting string value
	 */
	@Override
	public String toString()
	{
		return NativeMethods.xnProductionNodeTypeToString(toNative());
	}
	
	/**
	 * Tests whether this node type is a generator
	 * @return TRUE if this node type is a generator, FALSE otherwise
	 */
	public boolean isGenerator()
	{
		return NativeMethods.xnIsTypeGenerator(toNative());
	}
	
	/**
	 * Test whether this node is derived from a given base node
	 * @param base Base node to test against
	 * @return TRUE if this node was derived from the given base node, FALSE otherwise
	 */
	public boolean isDerivedFrom(NodeType base)
	{
		return NativeMethods.xnIsTypeDerivedFrom(toNative(), base.toNative());
	}
	
	private final int val;
}
