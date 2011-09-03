package org.OpenNI;

public class NodeInfo extends ObjectWrapper 
{
	public ProductionNodeDescription getDescription()
	{
		return NativeMethods.xnNodeInfoGetDescription(toNative());
	}
	
	public String getInstanceName()
	{
		return NativeMethods.xnNodeInfoGetInstanceName(toNative());
	}
	
	public String getCreationInfo()
	{
		return NativeMethods.xnNodeInfoGetCreationInfo(toNative());
	}
	
	public NodeInfoList getNeededNodes() throws GeneralException
	{
		return new NodeInfoList(NativeMethods.xnNodeInfoGetNeededNodes(toNative()));
	}
	
	public ProductionNode getInstance() throws GeneralException
	{
		long hNode = NativeMethods.xnNodeInfoGetRefHandle(toNative());
		return Context.createProductionNodeFromNative(hNode);
	}
	
	protected NodeInfo(long pNodeInfo) throws GeneralException 
	{
		super(pNodeInfo);
	}
	
	@Override
	protected void freeObject(long ptr) 
	{
		// no need to free
	}
}
