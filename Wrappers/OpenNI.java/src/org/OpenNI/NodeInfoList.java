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

import java.util.Iterator;
import java.util.NoSuchElementException;

/**
 * Stores a list of NodeInfo objects. <BR><BR>
 *
 */
public class NodeInfoList extends ObjectWrapper implements Iterable<NodeInfo>
{
	/**
	 * Constructor, creates a new NodeInfoList with given native pointer and 'own' value
	 * @param ptr Native pointer to this object
	 * @param own Initial value of the "own" variable
	 */
	NodeInfoList(long ptr, boolean own)
	{
		super(ptr);
		this.own = own;
	}
	
	/**
	 * Constructor, creates a new NodeInfoList with the given native pointer, and sets 'own' to TRUE
	 * @param ptr Native pointer to this object
	 */
	NodeInfoList(long ptr)
	{
		super(ptr);
		this.own = true;
	}
	
	/**
	 * Adds a node to the NodeInfoList with given description, creationInfo, and neededNodes 
	 * @param description Description of the node added to the list
	 * @param creationInfo Creation info string for the node added to the list
	 * @param neededNodes Nodes needed by the node added to the list
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public void add(ProductionNodeDescription description, String creationInfo, NodeInfoList neededNodes) throws StatusException
	{
		long pNativeDesc = description.createNative();
		int status = NativeMethods.xnNodeInfoListAdd(toNative(), pNativeDesc, creationInfo,
				neededNodes == null ? 0 : neededNodes.toNative());
		ProductionNodeDescription.freeNative(pNativeDesc);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Adds a NodeInfo object to the list
	 * @param nodeInfo Node to be added to the list
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public void addNode(NodeInfo nodeInfo) throws StatusException
	{
		int status = NativeMethods.xnNodeInfoListAddNode(toNative(), nodeInfo.toNative());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Adds a node to this list by iterating from another list
	 * @param current Node to be added
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public void addNodeFromList(Iterator<NodeInfo> current) throws StatusException
	{
		NodeInfoListIterator iter = (NodeInfoListIterator)current;
		int status = NativeMethods.xnNodeInfoListAddNodeFromList(toNative(), iter.it);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * @param location
	 * @throws StatusException
	 */
	public void remove(Iterator<NodeInfo> location) throws StatusException
	{
		NodeInfoListIterator iter = (NodeInfoListIterator)location;
		int status = NativeMethods.xnNodeInfoListRemove(toNative(), iter.it);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Clears the list
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public void clear() throws StatusException
	{
		int status = NativeMethods.xnNodeInfoListClear(toNative());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Appends another NodeInfoList to this one
	 * @param other NodeInfoList to be appended
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public void append(NodeInfoList other) throws StatusException
	{
		int status = NativeMethods.xnNodeInfoListAppend(toNative(), other.toNative());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Tests whether this list is empty
	 * @return TRUE if this list is empty, false otherwise
	 */
	public boolean isEmpty()
	{
		return NativeMethods.xnNodeInfoListIsEmpty(toNative());
	}
	
	public void filter(Query query, Context context) throws StatusException
	{
		int status = NativeMethods.xnNodeQueryFilterList(context.toNative(), query.toNative(), toNative());
		WrapperUtils.throwOnError(status);
	}

	@Override
	public Iterator<NodeInfo> iterator() {
		return new NodeInfoListIterator(toNative());
	}

	@Override
	protected void freeObject(long ptr) 
	{
		if (this.own)
		{
			NativeMethods.xnNodeInfoListFree(ptr);
		}
	}
	
	private class NodeInfoListIterator implements Iterator<NodeInfo>
	{
		NodeInfoListIterator(long pNodeInfoList)
		{
			this.it = NativeMethods.xnNodeInfoListGetFirst(pNodeInfoList);
		}
		
		@Override
		public boolean hasNext() 
		{
			return NativeMethods.xnNodeInfoListIteratorIsValid(this.it);
		}

		@Override
		public NodeInfo next() 
		{
			if (!hasNext())
				throw new NoSuchElementException();
			
			NodeInfo next = null;
			
			try 
			{
				next = new NodeInfo(NativeMethods.xnNodeInfoListGetCurrent(this.it));
			} 
			catch (GeneralException e) 
			{
				// can't happen
			}
			
			this.it = NativeMethods.xnNodeInfoListGetNext(this.it);
			return next;
		}

		@Override
		public void remove() 
		{
			throw new UnsupportedOperationException();
		}

		protected long it;
	}
	
	private boolean own;
}
