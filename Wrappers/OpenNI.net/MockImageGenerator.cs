using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class MockImageGenerator : ImageGenerator
	{
		internal MockImageGenerator(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
		}

		public MockImageGenerator(Context context, string name) :
			this(Create(context, name), false)
		{
		}

		public MockImageGenerator(Context context) :
			this(context, null)
		{
		}

		public MockImageGenerator(ImageGenerator basedOn, string name) :
			this(CreateBasedOn(basedOn, name), false)
		{
		}

		public MockImageGenerator(ImageGenerator basedOn) :
			this(basedOn, null)
		{
		}

		public void SetData(UInt32 frameId, UInt64 timestamp, UInt32 dataSize, IntPtr buffer)
		{
            Status.ThrowOnFail(SafeNativeMethods.xnMockImageSetData(this.InternalObject, frameId, timestamp, dataSize, buffer));
			
		}

		public void SetData(ImageMetadata imageMetadata, UInt32 frameId, UInt64 timestamp)
		{
			SetData(frameId, timestamp, imageMetadata.DataSize, imageMetadata.ImageMapPtr);
		}

		public void SetData(ImageMetadata imageMetadata)
		{
			SetData(imageMetadata, imageMetadata.FrameId, imageMetadata.Timestamp);
		}

		private static NodeSafeHandle Create(Context context, string name)
		{
            NodeSafeHandle handle;
            Status.ThrowOnFail(SafeNativeMethods.xnCreateMockNode(context.InternalObject, NodeType.Image, name, out handle));
			
			return handle;
		}

        private static NodeSafeHandle CreateBasedOn(ImageGenerator basedOn, string name)
		{
            NodeSafeHandle handle;
			Status.ThrowOnFail(SafeNativeMethods.xnCreateMockNodeBasedOn(basedOn.Context.InternalObject,
                basedOn.InternalObject, name, out handle));
			
			return handle;
		}
	}
}