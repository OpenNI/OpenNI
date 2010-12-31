using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class DepthMetadata : MapMetadata<UInt16>
	{
		public DepthMetadata()
		{
		}

		public UInt16 ZRes
		{
			get { return this.depth.nZRes; }
			set { this.depth.nZRes = value; }
		}

		public UInt16 this[int index]
		{
			get 
            {
                Contract.Requires(index >= 0 && index < this.FullXRes * this.FullYRes);

                return base[index, this.depth.pData]; 
            }
			set 
            {
                Contract.Requires(index >= 0 && index < this.FullXRes * this.FullYRes);

                base[index, this.depth.pData] = value; 
            }
		}

		public UInt16 this[int x, int y]
		{
			get 
            {
                Contract.Requires(x >= 0 && x < this.FullXRes);
                Contract.Requires(y >= 0 && y < this.FullYRes);

                return base[x, y, this.depth.pData]; 
            }
			set 
            {
                Contract.Requires(x >= 0 && x < this.FullXRes);
                Contract.Requires(y >= 0 && y < this.FullYRes);

                base[x, y, this.depth.pData] = value; 
            }
		}

		public MapData<UInt16> GetDepthMap()
		{
			return new MapData<UInt16>(XRes, YRes, this.depth.pData);
		}

		public IntPtr DepthMapPtr
		{
			get { return this.depth.pData; }
		}

		internal new IMarshaler GetMarshaler(bool passOut)
		{
			return new DepthMetadataMarshaler(this, passOut);
		}

		private SafeNativeMethods.XnDepthMetaData depth = new SafeNativeMethods.XnDepthMetaData();

		private class DepthMetadataMarshaler : Marshaler<SafeNativeMethods.XnDepthMetaData>
		{
			public DepthMetadataMarshaler(DepthMetadata obj, bool marshalOut) :
				base(obj.depth, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(DepthMetadata obj, bool marshalOut)
			{
				IMarshaler inner = ((MapMetadata)obj).GetMarshaler(marshalOut);
				obj.depth.pMap = inner.Native;
				return inner;
			}
		}
	}
}
