using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
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

		public IntPtr ImageMapPtr
		{
            get
            {
                return OpenNIImporter.xnGetImageMap(this.InternalObject);
            }
		}

		public MapData<RGB24Pixel> RGB24ImageMap
		{
            get
            {
                return GetMapData<RGB24Pixel>(OpenNIImporter.xnGetRGB24ImageMap(this.InternalObject));
            }
		}

		public MapData<byte> Grayscale8ImageMap
		{
            get
            {
                return GetMapData<byte>(OpenNIImporter.xnGetGrayscale8ImageMap(this.InternalObject));
            }
		}

		public MapData<UInt16> Grayscale16ImageMap
		{
            get
            {
                return GetMapData<UInt16>(OpenNIImporter.xnGetGrayscale16ImageMap(this.InternalObject));
            }
		}

		public bool IsPixelFormatSupported(PixelFormat format)
		{
			return OpenNIImporter.xnIsPixelFormatSupported(this.InternalObject, format);
		}

		public PixelFormat PixelFormat
		{
            get
            {
                return OpenNIImporter.xnGetPixelFormat(this.InternalObject);
            }
            set
            {
                Status.ThrowOnFail(OpenNIImporter.xnSetPixelFormat(this.InternalObject, value));
			    
            }
		}

		public void GetMetaData(ImageMetaData imageMD)
		{
			using (IMarshaler marsh = imageMD.GetMarshaler(true))
			{
				OpenNIImporter.xnGetImageMetaData(this.InternalObject, marsh.Native);
			}
		}

		public ImageMetaData MetaData
		{
            get
            {
                ImageMetaData imageMD = new ImageMetaData();
                GetMetaData(imageMD);
                return imageMD;
            }
		}

		public event StateChangedHandler PixelFormatChanged
		{
			add { this.pixelFormatChanged.Event += value; }
			remove { this.pixelFormatChanged.Event -= value; }
		}

		private static NodeSafeHandle Create(Context context, Query query, EnumerationErrors errors)
		{
            NodeSafeHandle handle;
            Status.ThrowOnFail(OpenNIImporter.xnCreateImageGenerator(context.InternalObject, out handle,
				query == null ? QuerySafeHandle.Zero : query.InternalObject,
                errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
			
			return handle;
		}

		private StateChangedEvent pixelFormatChanged;
	}
}