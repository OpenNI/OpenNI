using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace OpenNI
{
	public class Player : ProductionNode
	{
        internal Player(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
			this.endOfFileReachedEvent = new StateChangedEvent(this,
				OpenNIImporter.xnRegisterToEndOfFileReached,
				OpenNIImporter.xnUnregisterFromEndOfFileReached);
		}

		public Player(Context context, string formatName) :
			this(Create(context, formatName), false)
		{
		}

		public void SetRepeat(bool repeat)
		{
			Status.ThrowOnFail(OpenNIImporter.xnSetPlayerRepeat(this.InternalObject, repeat));
		}

		public void SetSource(RecordMedium medium, string source)
		{
			Status.ThrowOnFail(OpenNIImporter.xnSetPlayerSource(this.InternalObject, medium, source));
		}

		public string GetSource(out RecordMedium medium)
		{
			const int size = 512;
			StringBuilder sb = new StringBuilder(size);
			Status.ThrowOnFail(OpenNIImporter.xnGetPlayerSource(this.InternalObject, out medium, sb, size));
			return sb.ToString();
		}

		public void ReadNext()
		{
			Status.ThrowOnFail(OpenNIImporter.xnPlayerReadNext(this.InternalObject));
		}

		public void SeekToTimestamp(Int64 timeOffset, PlayerSeekOrigin origin)
		{
			Status.ThrowOnFail(OpenNIImporter.xnSeekPlayerToTimeStamp(this.InternalObject, timeOffset, origin));
		}

		public void SeekToFrame(string nodeName, Int32 frameOffset, PlayerSeekOrigin origin)
		{
			Status.ThrowOnFail(OpenNIImporter.xnSeekPlayerToFrame(this.InternalObject, nodeName, frameOffset, origin));
		}

		public void SeekToFrame(ProductionNode node, Int32 frameOffset, PlayerSeekOrigin origin)
		{
			SeekToFrame(node.Name, frameOffset, origin);
		}

		public UInt64 TellTimestamp()
		{
			UInt64 timestamp;
			Status.ThrowOnFail(OpenNIImporter.xnTellPlayerTimestamp(this.InternalObject, out timestamp));
			return timestamp;
		}

		public UInt32 TellFrame(string nodeName)
		{
			UInt32 frame;
			Status.ThrowOnFail(OpenNIImporter.xnTellPlayerFrame(this.InternalObject, nodeName, out frame));
			return frame;
		}

		public UInt32 TellFrame(ProductionNode node)
		{
			return TellFrame(node.Name);
		}

		public UInt32 GetNumFrames(string nodeName)
		{
			UInt32 frames;
			Status.ThrowOnFail(OpenNIImporter.xnGetPlayerNumFrames(this.InternalObject, nodeName, out frames));
			return frames;
		}

		public UInt32 GetNumFrames(ProductionNode node)
		{
			return GetNumFrames(node.Name);
		}

		public string GetSupportedFormat()
		{
			return OpenNIImporter.xnGetPlayerSupportedFormat(this.InternalObject);
		}

		public NodeInfoList EnumerateNodes()
		{
			NodeInfoListSafeHandle pList;
			Status.ThrowOnFail(OpenNIImporter.xnEnumeratePlayerNodes(this.InternalObject, out pList));

			return new NodeInfoList(pList);
		}

		public bool IsEOF
		{
            get
            {
                return OpenNIImporter.xnIsPlayerAtEOF(this.InternalObject);
            }
		}

		public event EventHandler<StateChangedArgs> EndOfFileReached
		{
			add { this.endOfFileReachedEvent.Event += value; }
			remove { this.endOfFileReachedEvent.Event -= value; }
		}

		public double GetPlaybackSpeed
		{
            get
            {
                return OpenNIImporter.xnGetPlaybackSpeed(this.InternalObject);
            }
            set
            {
			    Status.ThrowOnFail(OpenNIImporter.xnSetPlaybackSpeed(this.InternalObject, value));
            }
		}

        private static NodeSafeHandle Create(Context context, string formatName)
		{
            NodeSafeHandle nodeHandle;
			Status.ThrowOnFail(OpenNIImporter.xnCreatePlayer(context.InternalObject, formatName, out nodeHandle));
			return nodeHandle;
		}

		private StateChangedEvent endOfFileReachedEvent;

	}
}