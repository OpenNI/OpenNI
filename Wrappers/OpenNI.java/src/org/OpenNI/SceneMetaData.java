package org.OpenNI;

public class SceneMetaData extends MapMetaData
{
	public SceneMetaData() 
	{
		super(PixelFormat.GRAYSCALE_16BIT, new SceneMap());
	}
	public SceneMap getData()
	{
		return (SceneMap)super.getData();
	}

}
