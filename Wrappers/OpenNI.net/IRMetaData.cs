using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class IRMetadata : MapMetadata<UInt16>
	{
		public IRMetadata()
		{
		}

		public UInt16 this[int index]
		{
			get 
            {
                Contract.Requires(index >= 0 && index < this.FullXRes * this.FullYRes);

                return base[index, this.ir.pData]; 
            }
			set 
            {
                Contract.Requires(index >= 0 && index < this.FullXRes * this.FullYRes);

                base[index, this.ir.pData] = value; 
            }
		}

		public UInt16 this[int x, int y]
		{
			get 
            {
                Contract.Requires(x >= 0 && x < this.FullXRes);
                Contract.Requires(y >= 0 && y < this.FullYRes);

                return base[x, y, this.ir.pData]; 
            }
			set 
            {
                Contract.Requires(x >= 0 && x < this.FullXRes);
                Contract.Requires(y >= 0 && y < this.FullYRes);

                base[x, y, this.ir.pData] = value; 
            }
		}

		public MapData<UInt16> GetIRMap()
		{
			return new MapData<UInt16>(XRes, YRes, this.ir.pData);
		}

		public IntPtr IRMapPtr
		{
			get { return this.ir.pData; }
		}

		internal new IMarshaler GetMarshaler(bool passOut)
		{
			return new IRMetadataMarshaler(this, passOut);
		}

		internal SafeNativeMethods.XnIRMetadata ir = new SafeNativeMethods.XnIRMetadata();

		private class IRMetadataMarshaler : Marshaler<SafeNativeMethods.XnIRMetadata>
		{
			public IRMetadataMarshaler(IRMetadata obj, bool marshalOut) :
				base(obj.ir, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(IRMetadata obj, bool marshalOut)
			{
				IMarshaler inner = ((MapMetadata)obj).GetMarshaler(marshalOut);
				obj.ir.pMap = inner.Native;
				return inner;
			}
		}
	}
}
