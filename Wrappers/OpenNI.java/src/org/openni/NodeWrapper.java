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
 * Based class for Production Nodes. <BR><BR>
 * 
 * This is the base class for many production node classes
 *
 */
public class NodeWrapper extends ObjectWrapper
{
	/**
	 * Create a new NodeWrapper for the given context, with given pointer
	 * @param context OpenNI context for this node
	 * @param hNode Pointer to the node for creation of native reference
	 * @param addRef Whether to create a native reference (TRUE for do create, FALSE for don't)
	 * @throws StatusException Underlying native code may trigger exceptions, so Status Exception should be handled
	 */
	NodeWrapper(Context context, long hNode, boolean addRef) throws StatusException
	{
		super(hNode);
		
		this.context = context;
		
		if (addRef)
		{
			WrapperUtils.throwOnError(NativeMethods.xnProductionNodeAddRef(hNode));
		}
	}
	
	/**
	 * Provides access to the OpenNI context for this node
	 * @return Context for this node
	 */
	public Context getContext()
	{
		return this.context;
	}
	
	/**
	 * Provides access to the string name of this node
	 * @return Name of this node, formatted as a string
	 */
	public String getName()
	{
		return NativeMethods.xnGetNodeName(toNative());
	}

	/* (non-Javadoc)
	 * @see org.openni.ObjectWrapper#freeObject(long)
	 */
	protected void freeObject(long ptr)
	{
		NativeMethods.xnProductionNodeRelease(ptr);
	}
	
	private Context context;
}
