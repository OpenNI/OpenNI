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
 * Allows recording of the output from another Production Node. <BR><BR>
 *
 * OpenNI's recording functionality can be used to record the output of a Production Node for later
 * playback.  This can be used for creating applications that record and playback image, depth, IR, and 
 * audio data.  It is also useful for algorithmic testing, since it allows for repeated testing of different
 * algorithms with identical input data sets.  It also allows for the simulation of a sensor when actual
 * hardware is not available for whatever reason.
 * 
 * The recorder can record the output of a specific node, or any subset of all the nodes in the current context.
 *
 */
public class Recorder extends ProductionNode
{
	Recorder(Context context, long nodeHandle, boolean addRef) throws StatusException
	{
		super(context, nodeHandle, addRef);
	}

	public static Recorder create(Context context, String formatName) throws GeneralException
	{
		OutArg<Long> phRecorder = new OutArg<Long>();
		int status = NativeMethods.xnCreateRecorder(context.toNative(), formatName, phRecorder);
		WrapperUtils.throwOnError(status);
		Recorder recorder = (Recorder)context.createProductionNodeObject(phRecorder.value, NodeType.RECORDER);
		NativeMethods.xnProductionNodeRelease(phRecorder.value);
		return recorder;
	}
	
	public String getFormat()
	{
		return NativeMethods.xnGetRecorderFormat(toNative());
	}
	
	public void setDestination(RecordMedium medium, String name) throws StatusException
	{
		int status = NativeMethods.xnSetRecorderDestination(toNative(), medium.toNative(), name);
		WrapperUtils.throwOnError(status);
	}
	
	public RecordMedium getDestinationMedium() throws StatusException
	{
		OutArg<Integer> pMedium = new OutArg<Integer>();
		OutArg<String> pDest = new OutArg<String>();
		int status = NativeMethods.xnGetRecorderDestination(toNative(), pMedium, pDest);
		WrapperUtils.throwOnError(status);
		return RecordMedium.fromNative(pMedium.value);
	}

	public String getDestination() throws StatusException
	{
		OutArg<Integer> pMedium = new OutArg<Integer>();
		OutArg<String> pDest = new OutArg<String>();
		int status = NativeMethods.xnGetRecorderDestination(toNative(), pMedium, pDest);
		WrapperUtils.throwOnError(status);
		return pDest.value;
	}
	
	/** Specifies that the given node should be included while recording, using the specified codec
	 * @param node Production Node to be recorded
	 * @param codec Codec to be used to record data from the node
	 * @throws StatusException Potentially triggered by disk access and hardware sensor access
	 */
	public void addNodeToRecording(ProductionNode node, CodecID codec) throws StatusException
	{
		int status = NativeMethods.xnAddNodeToRecording(toNative(), node.toNative(), codec.toNative());
		WrapperUtils.throwOnError(status);
	}

	public void addNodeToRecording(ProductionNode node) throws StatusException
	{
		addNodeToRecording(node, CodecID.Null);
	}

	public void removeNodeToRecording(ProductionNode node) throws StatusException
	{
		int status = NativeMethods.xnRemoveNodeFromRecording(toNative(), node.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Starts recording.
	 * @throws StatusException We are potentially accessing disk and hardware, so exceptions are possible
	 */
	public void Record() throws StatusException
	{
		int status = NativeMethods.xnRecord(toNative());
		WrapperUtils.throwOnError(status);
	}
}
