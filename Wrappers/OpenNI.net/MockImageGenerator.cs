using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
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

		public void SetData(UInt32 frameID, UInt64 timestamp, UInt32 dataSize, IntPtr buffer)
		{
			UInt32 status = OpenNIImporter.xnMockImageSetData(this.InternalObject, frameID, timestamp, dataSize, buffer);
			WrapperUtils.CheckStatus(status);
		}

		public void SetData(ImageMetaData imageMD, UInt32 frameID, UInt64 timestamp)
		{
			SetData(frameID, timestamp, imageMD.DataSize, imageMD.ImageMapPtr);
		}

		public void SetData(ImageMetaData imageMD)
		{
			SetData(imageMD, imageMD.FrameID, imageMD.Timestamp);
		}

		private static NodeSafeHandle Create(Context context, string name)
		{
            NodeSafeHandle handle;
            UInt32 status = OpenNIImporter.xnCreateMockNode(context.InternalObject, NodeType.Image, name, out handle);
			WrapperUtils.CheckStatus(status);
			return handle;
		}

        private static NodeSafeHandle CreateBasedOn(ImageGenerator basedOn, string name)
		{
            NodeSafeHandle handle;
			UInt32 status = OpenNIImporter.xnCreateMockNodeBasedOn(basedOn.GetContext().InternalObject,
                basedOn.InternalObject, name, out handle);
			WrapperUtils.CheckStatus(status);
			return handle;
		}
	}
}