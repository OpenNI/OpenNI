package org.OpenNI;

public class IRMetaData extends MapMetaData
{
	public IRMetaData()
	{
		super(PixelFormat.GRAYSCALE_16BIT, new IRMap());
	}
	
	public IRMap getData()
	{
		return (IRMap)super.getData();
	}
}
