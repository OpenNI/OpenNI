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
 * Plays back recorded production node information from a file. <BR><BR>
 * 
 * The player is a production node that reads recorded data from a file, and 
 * plays back that data as if from a live node.
 *
 */
public class Player extends ProductionNode 
{
	Player(Context context, long nodeHandle, boolean addRef) throws StatusException 
	{
		super(context, nodeHandle, addRef);
		
		this.eofReached = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToEndOfFileReached(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromEndOfFileReached(toNative(), hCallback);
			}
		};
	}

	public static Player create(Context context, String formatName) throws GeneralException
	{
		OutArg<Long> phPlayer = new OutArg<Long>();
		int status = NativeMethods.xnCreatePlayer(context.toNative(), formatName, phPlayer);
		WrapperUtils.throwOnError(status);
		Player player = (Player)context.createProductionNodeObject(phPlayer.value, NodeType.PLAYER);
		NativeMethods.xnProductionNodeRelease(phPlayer.value);
		return player;
	}

	public String getFormat()
	{
		return NativeMethods.xnGetPlayerSupportedFormat(toNative());
	}
	
	public void setSource(RecordMedium medium, String name) throws StatusException
	{
		int status = NativeMethods.xnSetPlayerSource(toNative(), medium.toNative(), name);
		WrapperUtils.throwOnError(status);
	}
	
	public RecordMedium getSourceMedium() throws StatusException
	{
		OutArg<Integer> pMedium = new OutArg<Integer>();
		OutArg<String> pDest = new OutArg<String>();
		int status = NativeMethods.xnGetPlayerSource(toNative(), pMedium, pDest);
		WrapperUtils.throwOnError(status);
		return RecordMedium.fromNative(pMedium.value);
	}

	public String getSource() throws StatusException
	{
		OutArg<Integer> pMedium = new OutArg<Integer>();
		OutArg<String> pDest = new OutArg<String>();
		int status = NativeMethods.xnGetPlayerSource(toNative(), pMedium, pDest);
		WrapperUtils.throwOnError(status);
		return pDest.value;
	}

	/**
	 * Sets the value of the repeat flag.  If repeat is set, the recorded data will
	 * be played in an infinite loop.  If not set, playback will stop at the end of 
	 * the recording.
	 * @param repeat Desired value for repeat flag, TRUE = enable repeat, FALSE = disable repeat
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public void setRepeat(boolean repeat) throws StatusException
	{
		int status = NativeMethods.xnSetPlayerRepeat(toNative(), repeat);
		WrapperUtils.throwOnError(status);
	}
	
	public void readNext() throws StatusException
	{
		int status = NativeMethods.xnPlayerReadNext(toNative());
		WrapperUtils.throwOnError(status);
	}
	
	public void seekToTimestamp(PlayerSeekOrigin origin, long offset) throws StatusException
	{
		int status = NativeMethods.xnSeekPlayerToTimeStamp(toNative(), offset, origin.toNative());
		WrapperUtils.throwOnError(status);
	}

	public void seekToFrame(ProductionNode node, PlayerSeekOrigin origin, int offset) throws StatusException
	{
		int status = NativeMethods.xnSeekPlayerToFrame(toNative(), node.getName(), offset, origin.toNative());
		WrapperUtils.throwOnError(status);
	}

	public long tellTimestamp() throws StatusException
	{
		OutArg<Long> pnTimestamp = new OutArg<Long>();
		int status = NativeMethods.xnTellPlayerTimestamp(toNative(), pnTimestamp);
		WrapperUtils.throwOnError(status);
		return pnTimestamp.value;
	}

	public int tellFrame(ProductionNode node) throws StatusException
	{
		OutArg<Integer> pnFrameID = new OutArg<Integer>();
		int status = NativeMethods.xnTellPlayerFrame(toNative(), node.getName(), pnFrameID);
		WrapperUtils.throwOnError(status);
		return pnFrameID.value;
	}
	
	/**
	 * Returns the number of frames that the current recording contains from a given production node
	 * @param node Production node to count frames for
	 * @return Number of frames recorded for that production node
	 * @throws StatusException
	 */
	public int getNumberOfFrames(ProductionNode node) throws StatusException
	{
		OutArg<Integer> pnFrames = new OutArg<Integer>();
		int status = NativeMethods.xnGetPlayerNumFrames(toNative(), node.getName(), pnFrames);
		WrapperUtils.throwOnError(status);
		return pnFrames.value;
	}

	public NodeInfoList enumerateNodes() throws StatusException
	{
		OutArg<Long> ppList = new OutArg<Long>();
		int status = NativeMethods.xnEnumeratePlayerNodes(toNative(), ppList);
		WrapperUtils.throwOnError(status);
		return new NodeInfoList(ppList.value);
	}
	
	/**
	 * Queries whether player is at the end of file for this recording
	 * @return TRUE if player is at end of file, false otherwise
	 */
	public boolean isEOF()
	{
		return NativeMethods.xnIsPlayerAtEOF(toNative());
	}
	
	public IStateChangedObservable getEOFReachedEvent() { return this.eofReached; }
	
	/**
	 * Returns the current playback speed
	 * @return Current playback speed
	 */
	public double getPlaybackSpeed()
	{
		return NativeMethods.xnGetPlaybackSpeed(toNative());
	}
	
	/**
	 * Sets the playback speed
	 * @param speed Desired new playback speed
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public void setPlaybackSpeed(double speed) throws StatusException
	{
		int status = NativeMethods.xnSetPlaybackSpeed(toNative(), speed);
		WrapperUtils.throwOnError(status);
	}
	
	private StateChangedObservable eofReached;
}
