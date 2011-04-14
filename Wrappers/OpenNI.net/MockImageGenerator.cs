using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class MockImageGenerator : ImageGenerator
	{
		internal MockImageGenerator(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
		}

		public MockImageGenerator(Context context, string name) :
			this(context, Create(context, name), false)
		{
		}

		public MockImageGenerator(Context context) :
			this(context, (string)null)
		{
		}

		public MockImageGenerator(Context context, ImageGenerator basedOn, string name) :
			this(context, CreateBasedOn(basedOn, name), false)
		{
		}

		public MockImageGenerator(Context context, ImageGenerator basedOn) :
			this(context, basedOn, null)
		{
		}

		public void SetData(Int32 frameID, Int64 timestamp, Int32 dataSize, IntPtr buffer)
		{
			int status = SafeNativeMethods.xnMockImageSetData(this.InternalObject, (UInt32)frameID, (UInt64)timestamp, (UInt32)dataSize, buffer);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetData(ImageMetaData imageMD, Int32 frameID, Int64 timestamp)
		{
			SetData(frameID, timestamp, imageMD.DataSize, imageMD.ImageMapPtr);
		}

		public void SetData(ImageMetaData imageMD)
		{
			SetData(imageMD, imageMD.FrameID, imageMD.Timestamp);
		}

		private static IntPtr Create(Context context, string name)
		{
			IntPtr handle;
			int status = SafeNativeMethods.xnCreateMockNode(context.InternalObject, NodeType.Image, name, out handle);
			WrapperUtils.ThrowOnError(status);
			return handle;
		}

		private static IntPtr CreateBasedOn(ImageGenerator basedOn, string name)
		{
			IntPtr handle;
			int status = SafeNativeMethods.xnCreateMockNodeBasedOn(basedOn.Context.InternalObject,
				basedOn.InternalObject, name, out handle);
			WrapperUtils.ThrowOnError(status);
			return handle;
		}
	}
}