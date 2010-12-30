using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class MockDepthGenerator : DepthGenerator
	{
		internal MockDepthGenerator(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
		}

		public MockDepthGenerator(Context context, string name) :
			this(Create(context, name), false)
		{
		}

		public MockDepthGenerator(Context context) :
			this(context, null)
		{
		}

		public MockDepthGenerator(DepthGenerator basedOn, string name) :
			this(CreateBasedOn(basedOn, name), false)
		{
		}

		public MockDepthGenerator(DepthGenerator basedOn) :
			this(basedOn, null)
		{
		}

		public void SetData(UInt32 frameID, UInt64 timestamp, UInt32 dataSize, IntPtr buffer)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnMockDepthSetData(this.InternalObject, frameID, timestamp, dataSize, buffer));
			
		}

		public void SetData(DepthMetaData depthMD, UInt32 frameID, UInt64 timestamp)
		{
			SetData(frameID, timestamp, depthMD.DataSize, depthMD.DepthMapPtr);
		}

		public void SetData(DepthMetaData depthMD)
		{
			SetData(depthMD, depthMD.FrameId, depthMD.Timestamp);
		}

        private static NodeSafeHandle Create(Context context, string name)
		{
            NodeSafeHandle handle;
            Status.ThrowOnFail(SafeNativeMethods.xnCreateMockNode(context.InternalObject, NodeType.Depth, name, out handle));
			
			return handle;
		}

        private static NodeSafeHandle CreateBasedOn(DepthGenerator basedOn, string name)
		{
            NodeSafeHandle handle;
			Status.ThrowOnFail(SafeNativeMethods.xnCreateMockNodeBasedOn(basedOn.GetContext().InternalObject,
                basedOn.InternalObject, name, out handle));
			
			return handle;
		}
	}
}