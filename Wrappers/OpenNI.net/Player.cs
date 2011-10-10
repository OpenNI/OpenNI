/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
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