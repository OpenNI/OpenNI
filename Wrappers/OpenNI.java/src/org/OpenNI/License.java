package org.OpenNI;

public class License
{
	public License(String vendor, String key)
	{
		this.vendor = vendor;
		this.key = key;
	}
	
	public String getVendor()
	{
		return this.vendor;
	}
	
	public String getKey()
	{
		return this.key;
	}
	
	private final String vendor;
	private final String key;
}
