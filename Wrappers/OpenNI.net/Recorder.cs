using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
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
			UInt32 status = OpenNIImporter.xnSetRecorderDestination(this.InternalObject, medium, dest);
			WrapperUtils.CheckStatus(status);
		}

		public void AddNodeToRecording(ProductionNode node, CodecID codec)
		{
			UInt32 status = OpenNIImporter.xnAddNodeToRecording(this.InternalObject, node.InternalObject, codec.InternalValue);
			WrapperUtils.CheckStatus(status);
		}

		public void AddNodeToRecording(ProductionNode node)
		{
			AddNodeToRecording(node, CodecID.Null);
		}

		public void RemoveNodeFromRecording(ProductionNode node)
		{
			UInt32 status = OpenNIImporter.xnRemoveNodeFromRecording(this.InternalObject, node.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void Record()
		{
			UInt32 status = OpenNIImporter.xnRecord(this.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		private static NodeSafeHandle Create(Context context, string formatName)
		{
            NodeSafeHandle nodeHandle;
            UInt32 status = OpenNIImporter.xnCreateRecorder(context.InternalObject, formatName, out nodeHandle);
			WrapperUtils.CheckStatus(status);
			return nodeHandle;
		}
	}
}