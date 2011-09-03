package org.OpenNI;

public abstract class MapMetaData extends OutputMetaData 
{
	MapMetaData(PixelFormat format, Map map)
	{
		this.pixelFormat = format;
		this.map = map;
	}
	
	public int getXRes() {
		return xRes;
	}
	public void setXRes(int xRes) {
		this.xRes = xRes;
		this.map.setXRes(xRes);
	}
	public int getYRes() {
		return yRes;
	}
	public void setYRes(int yRes) {
		this.yRes = yRes;
		this.map.setYRes(yRes);
	}
	public int getXOffset() {
		return xOffset;
	}
	public void setXOffset(int xOffset) {
		this.xOffset = xOffset;
	}
	public int getYOffset() {
		return yOffset;
	}
	public void setYOffset(int yOffset) {
		this.yOffset = yOffset;
	}
	public int getFullXRes() {
		return fullXRes;
	}
	public void setFullXRes(int fullXRes) {
		this.fullXRes = fullXRes;
	}
	public int getFullYRes() {
		return fullYRes;
	}
	public void setFullYRes(int fullYRes) {
		this.fullYRes = fullYRes;
	}
	public PixelFormat getPixelFormat() {
		return pixelFormat;
	}
	public int getFPS() {
		return FPS;
	}
	public void setFPS(int fPS) {
		FPS = fPS;
	}
	public Map getData() {
		return this.map;
	}
	
	@Override
	public void setDataPtr(long ptr) {
		super.setDataPtr(ptr);
		this.map.setNativePtr(ptr);
	}
	
	protected void setPixelFormat(PixelFormat format)
	{
		this.pixelFormat = format;
		this.map.setBytesPerPixel(format.getBytesPerPixel());
	}
	
	private int xRes;
	private int yRes;
	private int xOffset;
	private int yOffset;
	private int fullXRes;
	private int fullYRes;
	private PixelFormat pixelFormat;
	private int FPS;
	private Map map;
}
