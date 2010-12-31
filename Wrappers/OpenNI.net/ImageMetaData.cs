using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class ImageMetadata : MapMetadata
	{
		public ImageMetadata()
		{
		}

		public new PixelFormat PixelFormat
		{
			get { return ((MapMetadata)this).PixelFormat; }
			set { this.PixelFormatInternal = value; }
		}

		public MapData<RGB24Pixel> GetRgb24ImageMap()
		{
			return new MapData<RGB24Pixel>(XRes, YRes, this.image.pData);
		}

		public MapData<byte> GetGrayscale8ImageMap()
		{
			return new MapData<byte>(XRes, YRes, this.image.pData);
		}

		public MapData<UInt16> GetGrayscale16ImageMap()
		{
			return new MapData<UInt16>(XRes, YRes, this.image.pData);
		}

		public IntPtr ImageMapPtr
		{
			get { return this.image.pData; }
		}

		internal new IMarshaler GetMarshaler(bool passOut)
		{
			return new ImageMetadataMarshaler(this, passOut);
		}

		internal SafeNativeMethods.XnImageMetadata image = new SafeNativeMethods.XnImageMetadata();

		private class ImageMetadataMarshaler : Marshaler<SafeNativeMethods.XnImageMetadata>
		{
			public ImageMetadataMarshaler(ImageMetadata obj, bool marshalOut) :
				base(obj.image, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(ImageMetadata obj, bool marshalOut)
			{
				IMarshaler inner = ((MapMetadata)obj).GetMarshaler(marshalOut);
				obj.image.pMap = inner.Native;
				return inner;
			}
		}
	}
}
