package org.OpenNI;

public class DepthMetaData extends MapMetaData 
{
	public DepthMetaData() 
	{
		super(PixelFormat.GRAYSCALE_16BIT, new DepthMap());
	}
	
	public int getZRes() {
		return zRes;
	}

	public void setZRes(int zRes) {
		this.zRes = zRes;
	}
	
	public DepthMap getData()
	{
		return (DepthMap)super.getData();
	}
	
	private int zRes;
}
