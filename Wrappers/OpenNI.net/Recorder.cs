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
			Status.ThrowOnFail(OpenNIImporter.xnSetRecorderDestination(this.InternalObject, medium, dest));
		}

		public void AddNodeToRecording(ProductionNode node, CodecID codec)
		{
			Status.ThrowOnFail(OpenNIImporter.xnAddNodeToRecording(this.InternalObject, node.InternalObject, codec.InternalValue));
		}

		public void AddNodeToRecording(ProductionNode node)
		{
			AddNodeToRecording(node, CodecID.Null);
		}

		public void RemoveNodeFromRecording(ProductionNode node)
		{
			Status.ThrowOnFail(OpenNIImporter.xnRemoveNodeFromRecording(this.InternalObject, node.InternalObject));
		}

		public void Record()
		{
			Status.ThrowOnFail(OpenNIImporter.xnRecord(this.InternalObject));
		}

		private static NodeSafeHandle Create(Context context, string formatName)
		{
            NodeSafeHandle nodeHandle;
            Status.ThrowOnFail(OpenNIImporter.xnCreateRecorder(context.InternalObject, formatName, out nodeHandle));
			return nodeHandle;
		}
	}
}