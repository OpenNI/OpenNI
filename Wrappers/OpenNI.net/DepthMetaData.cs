using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class DepthMetaData : MapMetaData<UInt16>
	{
		public DepthMetaData()
		{
		}

		public UInt16 ZRes
		{
			get { return this.depth.nZRes; }
			set { this.depth.nZRes = value; }
		}

		public UInt16 this[int index]
		{
			get { return base[index, this.depth.pData]; }
			set { base[index, this.depth.pData] = value; }
		}

		public UInt16 this[int x, int y]
		{
			get { return base[x, y, this.depth.pData]; }
			set { base[x, y, this.depth.pData] = value; }
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
			return new DepthMetaDataMarshaler(this, passOut);
		}

		private OpenNIImporter.XnDepthMetaData depth = new OpenNIImporter.XnDepthMetaData();

		private class DepthMetaDataMarshaler : Marshaler<OpenNIImporter.XnDepthMetaData>
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
