using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class Recorder : ProductionNode
	{
		internal Recorder(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
		}

		public Recorder(Context context, string formatName) :
			this(context, Create(context, formatName), false)
		{
		}

		public Recorder(Context context) :
			this(context, null)
		{
		}

		public void SetDestination(RecordMedium medium, string destination)
		{
			int status = SafeNativeMethods.xnSetRecorderDestination(this.InternalObject, medium, destination);
			WrapperUtils.ThrowOnError(status);
		}

		public void AddNodeToRecording(ProductionNode node, CodecID codec)
		{
			int status = SafeNativeMethods.xnAddNodeToRecording(this.InternalObject, node.InternalObject, codec.InternalValue);
			WrapperUtils.ThrowOnError(status);
		}

		public void AddNodeToRecording(ProductionNode node)
		{
			AddNodeToRecording(node, CodecID.Null);
		}

		public void RemoveNodeFromRecording(ProductionNode node)
		{
			int status = SafeNativeMethods.xnRemoveNodeFromRecording(this.InternalObject, node.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void Record()
		{
			int status = SafeNativeMethods.xnRecord(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		private static IntPtr Create(Context context, string formatName)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnCreateRecorder(context.InternalObject, formatName, out nodeHandle);
			WrapperUtils.ThrowOnError(status);
			return nodeHandle;
		}
	}
}