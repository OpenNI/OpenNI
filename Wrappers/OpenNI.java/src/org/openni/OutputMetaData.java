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
 * Base class for all generator output types. <BR><BR>
 * 
 * This class serves as the base class for generator output meta data types.
 * It encapsulates the output parameters that are common to all output types.
 *
 */
public class OutputMetaData 
{

	/**
	 * Getter function for this frame's timestamp
	 * @return Timestamp of this frame
	 */
	public long getTimestamp() {
		return timestamp;
	}
	
	/**
	 * Setter function for this frame's timestamp
	 * @param timestamp Desired new timestamp for this frame
	 */
	public void setTimestamp(long timestamp) {
		this.timestamp = timestamp;
	}
	
	/**
	 * Getter function for this frame's FrameID
	 * @return This frame's frameID
	 */
	public int getFrameID() {
		return frameID;
	}
	
	/**
	 * Setter function for this frame's frameID
	 * @param frameID desired new FrameID for this frame
	 */
	public void setFrameID(int frameID) {
		this.frameID = frameID;
	}
	
	/**
	 * Getter function for this frame's data size
	 * @return Size of the data in this frame, measured in bytes
	 */
	public int getDataSize() {
		return dataSize;
	}
	
	/**
	 * Setter function for this frame's data size
	 * @param dataSize Size of the data in this frame, measured in bytes
	 */
	public void setDataSize(int dataSize) {
		this.dataSize = dataSize;
	}
	
	/**
	 * Getter function for the flag that specifies whether frame data is new
	 * @return Flag that indicates whether frame is new
	 */
	public boolean getIsNew() {
		return isNew;
	}
	
	/** Setter function for the flag that specified whether the frame data is new
	 * @param isNew Desired new value for the flag
	 */
	public void setIsNew(boolean isNew) {
		this.isNew = isNew;
	}

	/**
	 * Getter function for the native pointer to the data in this frame
	 * @return Pointer to the data, stored as a long integer value
	 */
	public long getDataPtr() {
		return this.dataPtr;
	}
	
	/**
	 * Setter function for the native pointer to the data in this frame
	 * @param ptr Desired new value for the pointer to the data
	 */
	public void setDataPtr(long ptr) {
		this.dataPtr = ptr;
	}

	private long timestamp;
	private int frameID;
	private int dataSize;
	private boolean isNew;
	private long dataPtr;
}
