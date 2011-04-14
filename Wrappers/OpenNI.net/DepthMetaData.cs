using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class DepthMetaData : UInt16MapMetaData
	{
		public DepthMetaData()
		{
		}

		public int ZRes
		{
			get { return this.depth.nZRes; }
			set { this.depth.nZRes = (UInt16)value; }
		}

		public int this[int index]
		{
			get { return GetAt(this.depth.pData, index); }
			set { SetAt(this.depth.pData, index, value); }
		}

		public int this[int x, int y]
		{
			get { return GetAt(this.depth.pData, x, y); }
			set { SetAt(this.depth.pData, x, y, value); }
		}

		public UInt16MapData GetDepthMap()
		{
			return new UInt16MapData(XRes, YRes, this.depth.pData);
		}

		public IntPtr DepthMapPtr
		{
			get { return this.depth.pData; }
		}

		internal new IMarshaler GetMarshaler(bool passOut)
		{
			return new DepthMetaDataMarshaler(this, passOut);
		}

		private SafeNativeMethods.XnDepthMetaData depth = new SafeNativeMethods.XnDepthMetaData();

		private class DepthMetaDataMarshaler : Marshaler<SafeNativeMethods.XnDepthMetaData>
		{
			public DepthMetaDataMarshaler(DepthMetaData obj, bool marshalOut) :
				base(obj.depth, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(DepthMetaData obj, bool marshalOut)
			{
				IMarshaler inner = ((MapMetaData)obj).GetMarshaler(marshalOut);
				obj.depth.pMap = inner.Native;
				return inner;
			}
		}
	}
}
