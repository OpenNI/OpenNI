using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class Codec : ProductionNode
	{
		internal Codec(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
		}

		public Codec(Context context, CodecID codecID, ProductionNode initializer) :
			this(Create(context, codecID, initializer), false)
		{
		}

		public CodecID GetCodecID()
		{
			return new CodecID(OpenNIImporter.xnGetCodecID(this.InternalObject));
		}

		public UInt32 EncodeData(IntPtr source, UInt32 sourceSize, IntPtr dest, UInt32 destSize)
		{
			UInt32 written;
			Status.ThrowOnFail(OpenNIImporter.xnEncodeData(this.InternalObject, source, sourceSize, dest, destSize, out written));
			return written;
		}

		public UInt32 DecodeData(IntPtr source, UInt32 sourceSize, IntPtr dest, UInt32 destSize)
		{
			UInt32 written;
			Status.ThrowOnFail(OpenNIImporter.xnDecodeData(this.InternalObject, source, sourceSize, dest, destSize, out written));
			return written;
		}

		private static NodeSafeHandle Create(Context context, CodecID codecID, ProductionNode initializer)
		{
            NodeSafeHandle nodeHandle;
            Status.ThrowOnFail(OpenNIImporter.xnCreateCodec(context.InternalObject, codecID.InternalValue, initializer.InternalObject, out nodeHandle));
			return nodeHandle;
		}
	}
}