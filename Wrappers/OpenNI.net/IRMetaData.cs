using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class IRMetaData : MapMetaData<UInt16>
	{
		public IRMetaData()
		{
		}

		public UInt16 this[int index]
		{
			get { return base[index, this.ir.pData]; }
			set { base[index, this.ir.pData] = value; }
		}

		public UInt16 this[int x, int y]
		{
			get { return base[x, y, this.ir.pData]; }
			set { base[x, y, this.ir.pData] = value; }
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
			return new IRMetaDataMarshaler(this, passOut);
		}

		internal SafeNativeMethods.XnIRMetaData ir = new SafeNativeMethods.XnIRMetaData();

		private class IRMetaDataMarshaler : Marshaler<SafeNativeMethods.XnIRMetaData>
		{
			public IRMetaDataMarshaler(IRMetaData obj, bool marshalOut) :
				base(obj.ir, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(IRMetaData obj, bool marshalOut)
			{
				IMarshaler inner = ((MapMetaData)obj).GetMarshaler(marshalOut);
				obj.ir.pMap = inner.Native;
				return inner;
			}
		}
	}
}
