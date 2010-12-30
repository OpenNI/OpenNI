using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class MockIRGenerator : IRGenerator
	{
		internal MockIRGenerator(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
		}

		public MockIRGenerator(Context context, string name) :
			this(Create(context, name), false)
		{
		}

		public MockIRGenerator(Context context) :
			this(context, null)
		{
		}

		public MockIRGenerator(IRGenerator basedOn, string name) :
			this(CreateBasedOn(basedOn, name), false)
		{
		}

		public MockIRGenerator(IRGenerator basedOn) :
			this(basedOn, null)
		{
		}

		public void SetData(UInt32 frameID, UInt64 timestamp, UInt32 dataSize, IntPtr buffer)
		{
            Status.ThrowOnFail(OpenNIImporter.xnMockIRSetData(this.InternalObject, frameID, timestamp, dataSize, buffer));
			
		}

		public void SetData(IRMetaData irMD, UInt32 frameID, UInt64 timestamp)
		{
			SetData(frameID, timestamp, irMD.DataSize, irMD.IRMapPtr);
		}

		public void SetData(IRMetaData irMD)
		{
			SetData(irMD, irMD.FrameID, irMD.Timestamp);
		}

		private static NodeSafeHandle Create(Context context, string name)
		{
            NodeSafeHandle handle;
            Status.ThrowOnFail(OpenNIImporter.xnCreateMockNode(context.InternalObject, NodeType.IR, name, out handle));
			
			return handle;
		}

        private static NodeSafeHandle CreateBasedOn(IRGenerator basedOn, string name)
		{
            NodeSafeHandle handle;
			Status.ThrowOnFail(OpenNIImporter.xnCreateMockNodeBasedOn(basedOn.GetContext().InternalObject,
                basedOn.InternalObject, name, out handle));
			
			return handle;
		}
	}
}