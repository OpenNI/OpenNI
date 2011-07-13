package org.OpenNI;

public class ImageGenerator extends MapGenerator
{
	ImageGenerator(Context context, long nodeHandle, boolean addRef) throws GeneralException
	{
		super(context, nodeHandle, addRef);
		
		this.pixelFormatChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToPixelFormatChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromPixelFormatChange(toNative(), hCallback);
			}
		};
	}

	public static ImageGenerator create(Context context, Query query, EnumerationErrors errors) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateImageGenerator(context.toNative(), handle,
			query == null ? 0 : query.toNative(),
			errors == null ? 0 : errors.toNative());
		WrapperUtils.throwOnError(status);
		ImageGenerator result = (ImageGenerator)context.createProductionNodeObject(handle.value, NodeType.IMAGE);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	public static ImageGenerator create(Context context, Query query) throws GeneralException
	{
		return create(context, query, null);
	}

	public static ImageGenerator create(Context context) throws GeneralException
	{
		return create(context, null, null);
	}
	
	public boolean isPixelFormatSupported(PixelFormat format)
	{
		return NativeMethods.xnIsPixelFormatSupported(toNative(), format.toNative());
	}
	
	public void setPixelFormat(PixelFormat format) throws StatusException
	{
		int status = NativeMethods.xnSetPixelFormat(toNative(), format.toNative());
		WrapperUtils.throwOnError(status);
	}

	public PixelFormat getPixelFormat()
	{
		return PixelFormat.fromNative(NativeMethods.xnGetPixelFormat(toNative()));
	}
	
	public ImageMap getImageMap() throws GeneralException
	{
		int frameID = getFrameID();
		
		if ((this.currImageMap == null) || (this.currImageMapFrameID != frameID))
		{
			long ptr = NativeMethods.xnGetImageMap(toNative());
			MapOutputMode mode = getMapOutputMode();
			this.currImageMap = new ImageMap(ptr, mode.getXRes(), mode.getYRes(), NativeMethods.xnGetBytesPerPixel(toNative()));
			this.currImageMapFrameID = frameID; 
		}

		return this.currImageMap;
	}
	
	public IStateChangedObservable getPixelFormatChangedEvent() { return this.pixelFormatChanged; }
	
	public void getMetaData(ImageMetaData ImageMD)
	{
		NativeMethods.xnGetImageMetaData(this.toNative(), ImageMD);
	}

	public ImageMetaData getMetaData()
	{
		ImageMetaData ImageMD = new ImageMetaData();
		getMetaData(ImageMD);
		return ImageMD;
	}

	private ImageMap currImageMap;
	private int currImageMapFrameID;
	private StateChangedObservable pixelFormatChanged;
}
