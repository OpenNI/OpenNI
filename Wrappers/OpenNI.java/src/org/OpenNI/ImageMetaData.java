package org.OpenNI;

public class ImageMetaData extends MapMetaData
{
	public ImageMetaData()
	{
		super(PixelFormat.RGB24, new ImageMap());
	}

	public void setPixelFormat(PixelFormat format)
	{
		super.setPixelFormat(format);
	}
	
	public ImageMap getData()
	{
		return (ImageMap)super.getData();
	}
}
