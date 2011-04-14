using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class IRMetaData : UInt16MapMetaData
	{
		public IRMetaData()
		{
		}

		public int this[int index]
		{
			get { return GetAt(this.ir.pData, index); }
			set { SetAt(this.ir.pData, index, value); }
		}

		public int this[int x, int y]
		{
			get { return GetAt(this.ir.pData, x, y); }
			set { SetAt(this.ir.pData, x, y, value); }
		}

		public UInt16MapData GetIRMap()
		{
			return new UInt16MapData(XRes, YRes, this.ir.pData);
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
