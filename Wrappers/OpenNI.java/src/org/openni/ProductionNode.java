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
 * Parent class of all production node types. <BR><BR>
 * 
 * Factors common functionality into a single place.  Normally, application code would not 
 * instantiate this class, though users of child classes may need to refer to functions here.<BR><BR>
 * 
 * Capabilities common to all Production Nodes are accessed via calls to this class.
 *
 */
public class ProductionNode extends NodeWrapper
{
	/**
	 * Instantiates a production node.  Generally only called by constructors of classes
	 * that extend ProductionNode
	 * @param context An OpenNI context
	 * @param nodeHandle Handle used to refer to this particular node
	 * @param addRef 
	 * @throws StatusException Required as this function will directly access sensor hardware
	 */
	ProductionNode(Context context, long nodeHandle, boolean addRef) throws StatusException
	{
		super(context, nodeHandle, addRef);
	}
	
	/** 
	 * Constructor to create a production node given a pointer to a node already
	 * created by Native Code.  Generally only called by descendant classes.
	 * @param hNode
	 * @return
	 * @throws GeneralException Required because this function talks to hardware
	 */
	public static ProductionNode fromNative(long hNode) throws GeneralException
	{
		return Context.createProductionNodeFromNative(hNode);
	}
	
	/**
	 * Requests a NodeInfo object containing available info about this production node.
	 * @return Object containing all available NodeInfo
	 * @throws GeneralException
	 */
	public NodeInfo getInfo() throws GeneralException
	{
		return new NodeInfo(NativeMethods.xnGetNodeInfo(toNative()));
	}
	
	/**
	 * Specifies that a given node is required by this node
	 * @param needed Node required by this node
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void addNeededNode(ProductionNode needed) throws StatusException
	{
		int status = NativeMethods.xnAddNeededNode(toNative(), needed.toNative());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Removes the indication that a given node is required by this node
	 * @param needed Node no longer required by this node
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void removeNeededNode(ProductionNode needed) throws StatusException
	{
		int status = NativeMethods.xnRemoveNeededNode(toNative(), needed.toNative());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Checks whether a given capability is supported by this node
	 * @param capabilityName Capability to be tested
	 * @return Whether capability is supported (TRUE for yes, FALSE for no)
	 */
	public boolean isCapabilitySupported(String capabilityName)
	{
		return NativeMethods.xnIsCapabilitySupported(this.toNative(), capabilityName);
	}

	/**
	 * Sets an integer property of this node, specified by its string name
	 * @param propName String name of the property to be set
	 * @param value New integer value for the property specified
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void setIntProperty(String propName, long value) throws StatusException
	{
		int status = NativeMethods.xnSetIntProperty(this.toNative(), propName, value);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Sets a floating point valued property of this node, specified by its string name
	 * @param propName String name of the property to be set
	 * @param value New value for the property specified
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void setRealProperty(String propName, double value) throws StatusException
	{
		int status = NativeMethods.xnSetRealProperty(this.toNative(), propName, value);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Sets a string property of this node, specified by its string name
	 * @param propName String name of the property to be set
	 * @param value New value for the property specified
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void setStringProperty(String propName, String value) throws StatusException
	{
		int status = NativeMethods.xnSetStringProperty(this.toNative(), propName, value);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Sets a general value of the underlying node.  Value is passed as a specified number
	 * of bytes to be stored.
	 * @param propName String name of the property to be set
	 * @param size Size of the value to be stored, in bytes
	 * @param buff Native pointer to the value to be stored, stored as a long integer
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void setGeneralProperty(String propName, int size, long buff) throws StatusException
	{
		int status = NativeMethods.xnSetGeneralProperty(this.toNative(), propName, size, buff);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Sets a general value of the underlying node.  Value is passed as an array of bytes
	 * @param propName String value of the property to be set
	 * @param buffer Array of bytes containing the value to be set
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void setGeneralProperty(String propName, byte[] buffer) throws StatusException
	{
		int status = NativeMethods.xnSetGeneralPropertyArray(this.toNative(), propName, buffer);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Provides access to an integer property of the underlying node
	 * @param propName String name of the property value to be accessed
	 * @return Current value stored in that property
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public long getIntProperty(String propName) throws StatusException
	{
		OutArg<Long> value = new OutArg<Long>();
		int status = NativeMethods.xnGetIntProperty(this.toNative(), propName, value);
		WrapperUtils.throwOnError(status);
		return value.value.longValue();
	}

	/**
	 * Provides access to real value property of the underlying node
	 * @param propName String name of the property value to be accessed
	 * @return Current value stored in that property
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public double getRealProperty(String propName) throws StatusException
	{
		OutArg<Double> value = new OutArg<Double>();
		int status = NativeMethods.xnGetRealProperty(this.toNative(), propName, value);
		WrapperUtils.throwOnError(status);
		return value.value.doubleValue();
	}

	/**
	 * Provides access to a string valued property stored by the underlying node
	 * @param propName String name of the property to be accessed
	 * @return Current value stored in that property
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public String getStringProperty(String propName) throws StatusException
	{
		OutArg<String> value = new OutArg<String>();
		int status = NativeMethods.xnGetStringProperty(this.toNative(), propName, value);
		WrapperUtils.throwOnError(status);
		return value.value;
	}
	
	/**
	 * Provides access to a general valued property stored in the underlying node.  This 
	 * function returns the value as a pointer and size value
	 * @param propName String name of the property to be accessed
	 * @param size Size of the property to be accessed, in bytes
	 * @param buff Native pointer to the property to be accessed
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void getGeneralProperty(String propName, int size, long buff) throws StatusException
	{
		int status = NativeMethods.xnGetGeneralProperty(this.toNative(), propName, size, buff);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Provides access to a general valued property stored in the underlying node.  This 
	 * function returns the value as an array
	 * @param propName String name of the property to be accessed
	 * @param buffer Byte array to place the return value in 
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void getGeneralProperty(String propName, byte[] buffer) throws StatusException
	{
		int status = NativeMethods.xnGetGeneralPropertyArray(this.toNative(), propName, buffer);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Locks the node so that changes to properties can be made
	 * @return Lock handle for the node, used later to unlock the node
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public LockHandle lockForChanges() throws StatusException
	{
		OutArg<Integer> handle = new OutArg<Integer>();
		int status = NativeMethods.xnLockNodeForChanges(this.toNative(), handle);
		WrapperUtils.throwOnError(status);
		return new LockHandle(handle.value.intValue());
	}

	/**
	 * Unlock a node previously locked for changes
	 * @param lockHandle The lock handle created when the node was locked
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void unlockForChanges(LockHandle lockHandle) throws StatusException
	{
		int status = NativeMethods.xnUnlockNodeForChanges(this.toNative(), lockHandle.toNative());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Triggers changes on a locked node
	 * @param lockHandle Lock handle obtained when node was locked
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void lockedNodeStartChanges(LockHandle lockHandle) throws StatusException
	{
		int status = NativeMethods.xnLockedNodeStartChanges(this.toNative(), lockHandle.toNative());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Ends the change process on a locked node
	 * @param lockHandle Lock handle obtained when the node was locked
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void lockedNodeEndChanges(LockHandle lockHandle) throws StatusException
	{
		int status = NativeMethods.xnLockedNodeEndChanges(this.toNative(), lockHandle.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Provides access to the Error State Capability of this node by returning the appropriate capability 
	 * object
	 * @return Error state capability object, connected to this node
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public ErrorStateCapability getErrorStateCapability() throws StatusException
	{
		return new ErrorStateCapability(this);
	}

	/**
	 * Provides access to a GeneralIntCapability of this node
	 * @param capability The capability type requested
	 * @return GeneralIntCapability object for the capability requested
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public GeneralIntCapability getGeneralIntCapability(Capability capability) throws StatusException
	{
		return new GeneralIntCapability(this, capability);
	}
}
