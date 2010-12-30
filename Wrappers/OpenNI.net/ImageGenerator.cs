using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class ImageGenerator : MapGenerator
	{
		internal ImageGenerator(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
			this.pixelFormatChanged = new StateChangedEvent(this,
				OpenNIImporter.xnRegisterToPixelFormatChange,
				OpenNIImporter.xnUnregisterFromPixelFormatChange);
		}

		public ImageGenerator(Context context, Query query, EnumerationErrors errors) :
			this(Create(context, query, errors), false)
		{
		}

		public ImageGenerator(Context context, Query query) :
			this(context, query, null)
		{
		}

		public ImageGenerator(Context context) :
			this(context, null, null)
		{
		}

		public IntPtr GetImageMapPtr()
		{
			return OpenNIImporter.xnGetImageMap(this.InternalObject);
		}

		public MapData<RGB24Pixel> GetRGB24ImageMap()
		{
			return GetMapData<RGB24Pixel>(OpenNIImporter.xnGetRGB24ImageMap(this.InternalObject));
		}

		public MapData<byte> GetGrayscale8ImageMap()
		{
			return GetMapData<byte>(OpenNIImporter.xnGetGrayscale8ImageMap(this.InternalObject));
		}

		public MapData<UInt16> GetGrayscale16ImageMap()
		{
			return GetMapData<UInt16>(OpenNIImporter.xnGetGrayscale16ImageMap(this.InternalObject));
		}

		public bool IsPixelFormatSupported(PixelFormat format)
		{
			return OpenNIImporter.xnIsPixelFormatSupported(this.InternalObject, format);
		}

		public void SetPixelFormat(PixelFormat format)
		{
			UInt32 status = OpenNIImporter.xnSetPixelFormat(this.InternalObject, format);
			WrapperUtils.CheckStatus(status);
		}

		public PixelFormat GetPixelFormat()
		{
			return OpenNIImporter.xnGetPixelFormat(this.InternalObject);
		}

		public void GetMetaData(ImageMetaData imageMD)
		{
			using (IMarshaler marsh = imageMD.GetMarshaler(true))
			{
				OpenNIImporter.xnGetImageMetaData(this.InternalObject, marsh.Native);
			}
		}

		public ImageMetaData GetMetaData()
		{
			ImageMetaData imageMD = new ImageMetaData();
			GetMetaData(imageMD);
			return imageMD;
		}

		public event StateChangedHandler PixelFormatChanged
		{
			add { this.pixelFormatChanged.Event += value; }
			remove { this.pixelFormatChanged.Event -= value; }
		}

		private static NodeSafeHandle Create(Context context, Query query, EnumerationErrors errors)
		{
            NodeSafeHandle handle;
            UInt32 status = OpenNIImporter.xnCreateImageGenerator(context.InternalObject, out handle,
				query == null ? QuerySafeHandle.Zero : query.InternalObject,
				errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject);
			WrapperUtils.CheckStatus(status);
			return handle;
		}

		private StateChangedEvent pixelFormatChanged;
	}
}