package org.OpenNI;

public class OutputMetaData 
{
	public long getTimestamp() {
		return timestamp;
	}
	
	public void setTimestamp(long timestamp) {
		this.timestamp = timestamp;
	}
	
	public int getFrameID() {
		return frameID;
	}
	
	public void setFrameID(int frameID) {
		this.frameID = frameID;
	}
	
	public int getDataSize() {
		return dataSize;
	}
	
	public void setDataSize(int dataSize) {
		this.dataSize = dataSize;
	}
	
	public boolean getIsNew() {
		return isNew;
	}
	
	public void setIsNew(boolean isNew) {
		this.isNew = isNew;
	}

	public long getDataPtr() {
		return this.dataPtr;
	}
	
	public void setDataPtr(long ptr) {
		this.dataPtr = ptr;
	}

	private long timestamp;
	private int frameID;
	private int dataSize;
	private boolean isNew;
	private long dataPtr;
}
