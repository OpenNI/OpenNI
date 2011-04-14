using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class Codec : ProductionNode
	{
		internal Codec(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
		}

		public Codec(Context context, CodecID codecID, ProductionNode initializer) :
			this(context, Create(context, codecID, initializer), false)
		{
		}

		public CodecID CodecID
		{
			get
			{
				return new CodecID(SafeNativeMethods.xnGetCodecID(this.InternalObject));
			}
		}

		public int EncodeData(IntPtr source, int sourceSize, IntPtr destination, int destinationSize)
		{
			UInt32 written;
			int status = SafeNativeMethods.xnEncodeData(this.InternalObject, source, (UInt32)sourceSize, destination, (UInt32)destinationSize, out written);
			WrapperUtils.ThrowOnError(status);
			return (int)written;
		}

		public int DecodeData(IntPtr source, int sourceSize, IntPtr destination, int destinationSize)
		{
			UInt32 written;
			int status = SafeNativeMethods.xnDecodeData(this.InternalObject, source, (UInt32)sourceSize, destination, (UInt32)destinationSize, out written);
			WrapperUtils.ThrowOnError(status);
			return (int)written;
		}

		private static IntPtr Create(Context context, CodecID codecID, ProductionNode initializer)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnCreateCodec(context.InternalObject, codecID.InternalValue, initializer.InternalObject, out nodeHandle);
			WrapperUtils.ThrowOnError(status);
			return nodeHandle;
		}
	}
}