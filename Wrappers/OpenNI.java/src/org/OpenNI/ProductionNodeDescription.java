package org.OpenNI;

public class ProductionNodeDescription 
{
	public ProductionNodeDescription(NodeType type, String vendor, String name, Version version)
	{
		this.type = type;
		this.vendor = vendor;
		this.name = name;
		this.version = version;
	}

	@SuppressWarnings("unused")
	private ProductionNodeDescription(int type, String vendor, String name, Version version)
	{
		this(new NodeType(type), vendor, name, version);
	}

	public NodeType getType() { return this.type; }
	public String getVendor() { return this.vendor; }
	public String getName() { return this.name; }
	public Version getVersion() { return this.version; }
	
	protected long createNative()
	{
		return NativeMethods.createProductionNodeDescription(type.toNative(), vendor, name, version.getMajor(), version.getMinor(), version.getMaintenance(), version.getBuild());
	}
	
	protected static void freeNative(long pNative)
	{
		NativeMethods.freeProductionNodeDescription(pNative);
	}

	private NodeType type;
	private String vendor;
	private String name;
	private Version version; 
}
