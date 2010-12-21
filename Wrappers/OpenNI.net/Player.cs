using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace xn
{
	public class Player : ProductionNode
	{
		internal Player(IntPtr nodeHandle) :
			base(nodeHandle)
		{
			this.endOfFileReachedEvent = new StateChangedEvent(this,
				OpenNIImporter.xnRegisterToEndOfFileReached,
				OpenNIImporter.xnUnregisterFromEndOfFileReached);
		}

		public Player(Context context, string formatName) :
			this(Create(context, formatName))
		{
		}

		public void SetRepeat(bool repeat)
		{
			UInt32 status = OpenNIImporter.xnSetPlayerRepeat(this.InternalObject, repeat);
			WrapperUtils.CheckStatus(status);
		}

		public void SetSource(RecordMedium medium, string source)
		{
			UInt32 status = OpenNIImporter.xnSetPlayerSource(this.InternalObject, medium, source);
			WrapperUtils.CheckStatus(status);
		}

		public string GetSource(out RecordMedium medium)
		{
			const int size = 512;
			StringBuilder sb = new StringBuilder(size);
			UInt32 status = OpenNIImporter.xnGetPlayerSource(this.InternalObject, out medium, sb, size);
			WrapperUtils.CheckStatus(status);
			return sb.ToString();
		}

		public void ReadNext()
		{
			UInt32 status = OpenNIImporter.xnPlayerReadNext(this.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void SeekToTimestamp(Int64 timeOffset, PlayerSeekOrigin origin)
		{
			UInt32 status = OpenNIImporter.xnSeekPlayerToTimeStamp(this.InternalObject, timeOffset, origin);
			WrapperUtils.CheckStatus(status);
		}

		public void SeekToFrame(string nodeName, Int32 frameOffset, PlayerSeekOrigin origin)
		{
			UInt32 status = OpenNIImporter.xnSeekPlayerToFrame(this.InternalObject, nodeName, frameOffset, origin);
			WrapperUtils.CheckStatus(status);
		}

		public void SeekToFrame(ProductionNode node, Int32 frameOffset, PlayerSeekOrigin origin)
		{
			SeekToFrame(node.GetName(), frameOffset, origin);
		}

		public UInt64 TellTimestamp()
		{
			UInt64 timestamp;
			UInt32 status = OpenNIImporter.xnTellPlayerTimestamp(this.InternalObject, out timestamp);
			WrapperUtils.CheckStatus(status);
			return timestamp;
		}

		public UInt32 TellFrame(string nodeName)
		{
			UInt32 frame;
			UInt32 status = OpenNIImporter.xnTellPlayerFrame(this.InternalObject, nodeName, out frame);
			WrapperUtils.CheckStatus(status);
			return frame;
		}

		public UInt32 TellFrame(ProductionNode node)
		{
			return TellFrame(node.GetName());
		}

		public UInt32 GetNumFrames(string nodeName)
		{
			UInt32 frames;
			UInt32 status = OpenNIImporter.xnGetPlayerNumFrames(this.InternalObject, nodeName, out frames);
			WrapperUtils.CheckStatus(status);
			return frames;
		}

		public UInt32 GetNumFrames(ProductionNode node)
		{
			return GetNumFrames(node.GetName());
		}

		public string GetSupportedFormat()
		{
			return OpenNIImporter.xnGetPlayerSupportedFormat(this.InternalObject);
		}

		public NodeInfoList EnumerateNodes()
		{
			IntPtr pList;
			UInt32 status = OpenNIImporter.xnEnumeratePlayerNodes(this.InternalObject, out pList);
			WrapperUtils.CheckStatus(status);

			return new NodeInfoList(pList);
		}

		public bool IsEOF()
		{
			return OpenNIImporter.xnIsPlayerAtEOF(this.InternalObject);
		}

		public event StateChangedHandler EndOfFileReached
		{
			add { this.endOfFileReachedEvent.Event += value; }
			remove { this.endOfFileReachedEvent.Event -= value; }
		}

		public void SetPlaybackSpeed(double dSpeed)
		{
			UInt32 status = OpenNIImporter.xnSetPlaybackSpeed(this.InternalObject, dSpeed);
			WrapperUtils.CheckStatus(status);
		}

		public double GetPlaybackSpeed()
		{
			return OpenNIImporter.xnGetPlaybackSpeed(this.InternalObject);
		}

		private static IntPtr Create(Context context, string formatName)
		{
			IntPtr nodeHandle;
			UInt32 status = OpenNIImporter.xnCreatePlayer(context.InternalObject, formatName, out nodeHandle);
			WrapperUtils.CheckStatus(status);
			return nodeHandle;
		}

		private StateChangedEvent endOfFileReachedEvent;

	}
}