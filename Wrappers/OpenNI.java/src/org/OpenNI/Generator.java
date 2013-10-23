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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Base class for all generator node types. <BR><BR>
 * 
 * Generally this class would not be instantiated by application code, though
 * some of its member functions may be called by users of its descendant classes.<BR><BR>
 * 
 * This class defines the following Observable Events:
 * 
 * generationRunningChanged -- Triggered when generation is started or stopped
 * newDataAvailable -- Triggered when the generator has fresh data available
 * 
 *
 */
public class Generator extends ProductionNode 
{
	/**
	 * Constructor -- creates a new generator, given an OpenNI context
	 * @param context OpenNI context of this generator
	 * @param nodeHandle Pointer to this generator
	 * @param addRef Indicates whether a reference to this generator should be stored
	 * @throws GeneralException If underlying native code returns errors, a General Exception will be generated
	 */
	Generator(Context context, long nodeHandle, boolean addRef) throws GeneralException 
	{
		super(context, nodeHandle, addRef);
		
		this.generationRunningChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToGenerationRunningChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromGenerationRunningChange(toNative(), hCallback);
			}
		};
		
		this.newDataAvailable = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToNewDataAvailable(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromNewDataAvailable(toNative(), hCallback);
			}
		};		
	}

	/**
	 * Starts data generation from this node
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void startGenerating() throws StatusException
	{
		int status = NativeMethods.xnStartGenerating(this.toNative());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Checks whether this node is currently generating data
	 * @return TRUE if this node is generating data, FALSE otherwise
	 */
	public boolean isGenerating()
	{
		return NativeMethods.xnIsGenerating(this.toNative());
	}

	/**
	 * Stops data generation from this node
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void stopGenerating() throws StatusException
	{
		int status = NativeMethods.xnStopGenerating(this.toNative());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Provides access to the Running Changed event type
	 * @return
	 */
	public IStateChangedObservable getGenerationRunningChangedEvent()
	{
		return this.generationRunningChanged;
	}

	/**
	 * Checks whether new data is available from this node
	 * @return TRUE if new data is available, FALSE otherwise
	 */
	public boolean isNewDataAvailable()
	{
		OutArg<Long> timestamp = new OutArg<Long>();
		return NativeMethods.xnIsNewDataAvailable(this.toNative(), timestamp);
	}

	/**
	 * Returns the time stamp of the data currently available
	 * @return Timestamp of current data available
	 */
	public long getAvailableTimestamp()
	{
		OutArg<Long> timestamp = new OutArg<Long>();
		NativeMethods.xnIsNewDataAvailable(this.toNative(), timestamp);
		return timestamp.value;
	}

	/**
	 * Provides access to the NewDataAvailable event
	 * @return
	 */
	public IStateChangedObservable getNewDataAvailableEvent()
	{
		return this.newDataAvailable;
	}

	/**
	 * Goes to sleep until data is available
	 * @throws StatusException If underlying native code returns non-zero status, Status Exception is thrown by this function
	 */
	public void waitAndUpdateData() throws StatusException
	{
		int status = NativeMethods.xnWaitAndUpdateData(this.toNative());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Checks whether the data available is new
	 * @return TRUE if new, FALSE otherwise
	 */
	public boolean isDataNew()
	{
		return NativeMethods.xnIsDataNew(this.toNative());
	}

	/**
	 * Provides the size of the currently available data
	 * @return Size of available data in bytes
	 */
	public int getDataSize()
	{
		return NativeMethods.xnGetDataSize(this.toNative());
	}
	
	/**
	 * Provides native pointer to this nodes data
	 * @return Pointer to this nodes data, stored as a long integer
	 */
	public long getDataPtr()
	{
		return NativeMethods.xnGetData(this.toNative());
	}

	/**
	 * Creates a buffer of appropriate size to hold this node's data, and 
	 * copies this data into that buffer
	 * @return The buffer created
	 */
	public ByteBuffer createDataByteBuffer()
	{
		int size = getDataSize();
		ByteBuffer buffer = ByteBuffer.allocateDirect(size);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		NativeMethods.copyToBuffer(buffer, getDataPtr(), size);
		return buffer;
	}
	
	/**
	 * Copies the current data from this node into a given buffer
	 * @param buffer Buffer to copy the data into
	 * @param size Size of data to be copied
	 */
	public void copyDataToBuffer(ByteBuffer buffer, int size)
	{
		NativeMethods.copyToBuffer(buffer, getDataPtr(), size);
	} 	

	/**
	 * Gets the timestamp of the current data
	 * @return Timestamp of the current data
	 */
	public long getTimestamp()
	{
		return NativeMethods.xnGetTimestamp(this.toNative());
	}

	/**
	 * Gets the FrameID of the current data
	 * @return FrameID of the current data
	 */
	public int getFrameID()
	{
		return NativeMethods.xnGetFrameID(this.toNative());
	}

	/**
	 * Requests a Mirror Capability from this generator
	 * @return MirrorCapability object -- this is the standard way for an application to access this capability.
	 * @throws StatusException Function may communicate with hardware, so exception is possible
	 */
	public MirrorCapability getMirrorCapability() throws StatusException
	{
		return new MirrorCapability(this);
	}

	/**
	 * Requests an AlternativeViewpointCapability
	 * @return Alternative Viewpoint object -- this is the standard way for an application to access this capability
	 * @throws StatusException Function may communicate with hardware, so exceptions are possible
	 */
	public AlternativeViewpointCapability getAlternativeViewpointCapability() throws StatusException
	{
		return new AlternativeViewpointCapability(this);
	}

	/**
	 * Requests a FrameSyncCapability 
	 * @return FrameSyncCapability object -- this is the standard way for an application to access this capability
	 * @throws StatusException Function may communicate with hardware, so exceptions are possible
	 */
	public FrameSyncCapability getFrameSyncCapability() throws StatusException
	{
		return new FrameSyncCapability(this);
	}

	private StateChangedObservable generationRunningChanged;
	private StateChangedObservable newDataAvailable;
}
