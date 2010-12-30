using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public abstract class MapMetaData : OutputMetaData
	{
		public int XRes
		{
			get { return (int)this.map.Res.X; }
			set { this.map.Res.X = (uint)value; }
		}

		public int YRes
		{
			get { return (int)this.map.Res.Y; }
			set { this.map.Res.Y = (uint)value; }
		}

		public int XOffset
		{
			get { return (int)this.map.Offset.X; }
			set { this.map.Offset.X = (uint)value; }
		}

		public int YOffset
		{
			get { return (int)this.map.Offset.Y; }
			set { this.map.Offset.Y = (uint)value; }
		}

		public int FullXRes
		{
			get { return (int)this.map.FullRes.X; }
			set { this.map.FullRes.X = (uint)value; }
		}

		public int FullYRes
		{
			get { return (int)this.map.FullRes.Y; }
			set { this.map.FullRes.Y = (uint)value; }
		}

		public int FPS
		{
			get { return (int)this.map.nFPS; }
			set { this.map.nFPS = (uint)value; }
		}

		public PixelFormat PixelFormat
		{
			get { return this.map.PixelFormat; }
		}

		public int BytesPerPixel
		{
			get
			{
				switch (PixelFormat)
				{
					case PixelFormat.Rgb24:
						return 3;
					case PixelFormat.Yuv422:
						return 2;
					case PixelFormat.Grayscale8Bit:
						return 1;
					case PixelFormat.Grayscale16Bit:
						return 2;
					default:
						throw new NotImplementedException();
				}
			}
		}

		internal new IMarshaler GetMarshaler(bool passOut)
		{
			return new MapMetaDataMarshaler(this, passOut);
		}

		protected PixelFormat PixelFormatInternal
		{
			set { this.map.PixelFormat = value; }
		}

		private class MapMetaDataMarshaler : Marshaler<SafeNativeMethods.XnMapMetaData>
		{
			public MapMetaDataMarshaler(MapMetaData obj, bool marshalOut) :
				base(obj.map, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(MapMetaData obj, bool marshalOut)
			{
				IMarshaler inner = ((OutputMetaData)obj).GetMarshaler(marshalOut);
				obj.map.pOutput = inner.Native;
				return inner;
			}
		}

		internal SafeNativeMethods.XnMapMetaData map = new SafeNativeMethods.XnMapMetaData();
	}

	public abstract class MapMetaData<T> : MapMetaData
	{
        // TODO: is there a performance penalty for these?
        //public abstract T this[int index] { get; set; }
        //public abstract T this[int x, int y] { get; set; }

		protected T this[int index, IntPtr buff]
		{
			get
			{
				buff = new IntPtr(buff.ToInt64() + index * BytesPerPixel);
				return (T)Marshal.PtrToStructure(buff, typeof(T));
			}
			set
			{
				buff = new IntPtr(buff.ToInt64() + index * BytesPerPixel);
				Marshal.StructureToPtr(value, buff, false);
			}
		}

		protected T this[int x, int y, IntPtr buff]
		{
			get
			{
				buff = new IntPtr(buff.ToInt64() + (y * XRes + x) * BytesPerPixel);
				return (T)Marshal.PtrToStructure(buff, typeof(T));
			}
			set
			{
				buff = new IntPtr(buff.ToInt64() + (y * XRes + x) * BytesPerPixel);
				Marshal.StructureToPtr(value, buff, false);
			}
		}
	}
}
