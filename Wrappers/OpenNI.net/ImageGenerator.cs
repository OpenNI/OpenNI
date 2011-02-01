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
				SafeNativeMethods.xnRegisterToPixelFormatChange,
				SafeNativeMethods.xnUnregisterFromPixelFormatChange);
		}

		public ImageGenerator(Context context, Query query, ErrorCollection errors) :
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
                return SafeNativeMethods.xnGetImageMap(this.InternalObject);
            }
		}

		public MapData<RGB24Pixel> RGB24ImageMap
		{
            get
            {
                return GetMapData<RGB24Pixel>(SafeNativeMethods.xnGetRGB24ImageMap(this.InternalObject));
            }
		}

		public MapData<byte> Grayscale8ImageMap
		{
            get
            {
                return GetMapData<byte>(SafeNativeMethods.xnGetGrayscale8ImageMap(this.InternalObject));
            }
		}

		public MapData<UInt16> Grayscale16ImageMap
		{
            get
            {
                return GetMapData<UInt16>(SafeNativeMethods.xnGetGrayscale16ImageMap(this.InternalObject));
            }
		}

		public bool IsPixelFormatSupported(PixelFormat format)
		{
			return SafeNativeMethods.xnIsPixelFormatSupported(this.InternalObject, format);
		}

		public PixelFormat PixelFormat
		{
            get
            {
                return SafeNativeMethods.xnGetPixelFormat(this.InternalObject);
            }
            set
            {
                Status.ThrowOnFail(SafeNativeMethods.xnSetPixelFormat(this.InternalObject, value));
			    
            }
		}

		public void GetMetadata(ImageMetadata imageMetadata)
		{
			using (IMarshaler marsh = imageMetadata.GetMarshaler(true))
			{
				SafeNativeMethods.xnGetImageMetaData(this.InternalObject, marsh.Native);
			}
		}

		public ImageMetadata Metadata
		{
            get
            {
                ImageMetadata imageMetadata = new ImageMetadata();
                GetMetadata(imageMetadata);
                return imageMetadata;
            }
		}

		public IObservable<StateChangedArgs> PixelFormatChanged
		{
            get { return this.pixelFormatChanged.StateChanged; }
		}

		private static NodeSafeHandle Create(Context context, Query query, ErrorCollection errors)
		{
            NodeSafeHandle handle;
            Status.ThrowOnFail(SafeNativeMethods.xnCreateImageGenerator(context.InternalObject, out handle,
				query == null ? QuerySafeHandle.Zero : query.InternalObject,
                errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
			
			return handle;
		}

		private StateChangedEvent pixelFormatChanged;
	}
}