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
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace OpenNI
{
	public class Player : ProductionNode
	{
		internal Player(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
			this.endOfFileReachedEvent = new StateChangedEvent(this,
				SafeNativeMethods.xnRegisterToEndOfFileReached,
				SafeNativeMethods.xnUnregisterFromEndOfFileReached);
		}

		public Player(Context context, string formatName) :
			this(context, Create(context, formatName), false)
		{
		}

		public void SetRepeat(bool repeat)
		{
			int status = SafeNativeMethods.xnSetPlayerRepeat(this.InternalObject, repeat);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetSource(RecordMedium medium, string source)
		{
			int status = SafeNativeMethods.xnSetPlayerSource(this.InternalObject, medium, source);
			WrapperUtils.ThrowOnError(status);
		}

		public string SourceName
		{
			get
			{
				RecordMedium medium;
				return GetSourceImpl(out medium);
			}
		}

		public RecordMedium Medium
		{
			get
			{
				RecordMedium medium;
				GetSourceImpl(out medium);
				return medium;
			}
		}

		public void ReadNext()
		{
			int status = SafeNativeMethods.xnPlayerReadNext(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void SeekToTimestamp(Int64 timeOffset, PlayerSeekOrigin origin)
		{
			int status = SafeNativeMethods.xnSeekPlayerToTimeStamp(this.InternalObject, timeOffset, origin);
			WrapperUtils.ThrowOnError(status);
		}

		public void SeekToFrame(string nodeName, Int32 frameOffset, PlayerSeekOrigin origin)
		{
			int status = SafeNativeMethods.xnSeekPlayerToFrame(this.InternalObject, nodeName, frameOffset, origin);
			WrapperUtils.ThrowOnError(status);
		}

		public void SeekToFrame(ProductionNode node, Int32 frameOffset, PlayerSeekOrigin origin)
		{
			SeekToFrame(node.Name, frameOffset, origin);
		}

		public Int64 TellTimestamp()
		{
			UInt64 timestamp;
			int status = SafeNativeMethods.xnTellPlayerTimestamp(this.InternalObject, out timestamp);
			WrapperUtils.ThrowOnError(status);
			return (Int64)timestamp;
		}

		public Int32 TellFrame(string nodeName)
		{
			UInt32 frame;
			int status = SafeNativeMethods.xnTellPlayerFrame(this.InternalObject, nodeName, out frame);
			WrapperUtils.ThrowOnError(status);
			return (Int32)frame;
		}

		public Int32 TellFrame(ProductionNode node)
		{
			return TellFrame(node.Name);
		}

		public Int32 GetNumFrames(string nodeName)
		{
			UInt32 frames;
			int status = SafeNativeMethods.xnGetPlayerNumFrames(this.InternalObject, nodeName, out frames);
			WrapperUtils.ThrowOnError(status);
			return (Int32)frames;
		}

		public Int32 GetNumFrames(ProductionNode node)
		{
			return GetNumFrames(node.Name);
		}

		public string SupportedFormat
		{
			get
			{
				return Marshal.PtrToStringAnsi(SafeNativeMethods.xnGetPlayerSupportedFormat(this.InternalObject));
			}
		}

		public NodeInfoList EnumerateNodes()
		{
			IntPtr pList;
			int status = SafeNativeMethods.xnEnumeratePlayerNodes(this.InternalObject, out pList);
			WrapperUtils.ThrowOnError(status);

			return new NodeInfoList(pList);
		}

		public bool IsEOF
		{
			get
			{
				return SafeNativeMethods.xnIsPlayerAtEOF(this.InternalObject);
			}
		}

		public event EventHandler EndOfFileReached
		{
			add { this.endOfFileReachedEvent.Event += value; }
			remove { this.endOfFileReachedEvent.Event -= value; }
		}

		public double PlaybackSpeed
		{
			get
			{
				return SafeNativeMethods.xnGetPlaybackSpeed(this.InternalObject);
			}
			set
			{
				int status = SafeNativeMethods.xnSetPlaybackSpeed(this.InternalObject, value);
				WrapperUtils.ThrowOnError(status);
			}
		}

		private static IntPtr Create(Context context, string formatName)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnCreatePlayer(context.InternalObject, formatName, out nodeHandle);
			WrapperUtils.ThrowOnError(status);
			return nodeHandle;
		}

		private string GetSourceImpl(out RecordMedium medium)
		{
			const int size = 512;
			StringBuilder sb = new StringBuilder(size);
			int status = SafeNativeMethods.xnGetPlayerSource(this.InternalObject, out medium, sb, size);
			WrapperUtils.ThrowOnError(status);
			return sb.ToString();
		}

		private StateChangedEvent endOfFileReachedEvent;

	}
}