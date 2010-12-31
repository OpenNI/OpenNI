using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class Recorder : ProductionNode
	{
		internal Recorder(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
		}

		public Recorder(Context context, string formatName) :
			this(Create(context, formatName), false)
		{
		}

		public Recorder(Context context) :
			this(context, null)
		{
		}

		public void SetDestination(RecordMedium medium, string dest)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnSetRecorderDestination(this.InternalObject, medium, dest));
		}

		public void AddNodeToRecording(ProductionNode node, CodecId codec)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnAddNodeToRecording(this.InternalObject, node.InternalObject, codec.InternalValue));
		}

		public void AddNodeToRecording(ProductionNode node)
		{
			AddNodeToRecording(node, CodecId.Null);
		}

		public void RemoveNodeFromRecording(ProductionNode node)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnRemoveNodeFromRecording(this.InternalObject, node.InternalObject));
		}

		public void Record()
		{
			Status.ThrowOnFail(SafeNativeMethods.xnRecord(this.InternalObject));
		}

		private static NodeSafeHandle Create(Context context, string formatName)
		{
            NodeSafeHandle nodeHandle;
            Status.ThrowOnFail(SafeNativeMethods.xnCreateRecorder(context.InternalObject, formatName, out nodeHandle));
			return nodeHandle;
		}
	}
}