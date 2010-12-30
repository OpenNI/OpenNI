using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
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
			UInt32 status = OpenNIImporter.xnEncodeData(this.InternalObject, source, sourceSize, dest, destSize, out written);
			WrapperUtils.CheckStatus(status);
			return written;
		}

		public UInt32 DecodeData(IntPtr source, UInt32 sourceSize, IntPtr dest, UInt32 destSize)
		{
			UInt32 written;
			UInt32 status = OpenNIImporter.xnDecodeData(this.InternalObject, source, sourceSize, dest, destSize, out written);
			WrapperUtils.CheckStatus(status);
			return written;
		}

		private static NodeSafeHandle Create(Context context, CodecID codecID, ProductionNode initializer)
		{
            NodeSafeHandle nodeHandle;
            UInt32 status = OpenNIImporter.xnCreateCodec(context.InternalObject, codecID.InternalValue, initializer.InternalObject, out nodeHandle);
			WrapperUtils.CheckStatus(status);
			return nodeHandle;
		}
	}
}