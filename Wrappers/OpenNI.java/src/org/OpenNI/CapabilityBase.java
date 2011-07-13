package org.OpenNI;

public class CapabilityBase extends NodeWrapper
{
	public CapabilityBase(ProductionNode node) throws StatusException
	{
		super(node.getContext(), node.toNative(), true);
		this.node = node;
	}

	private ProductionNode node;
}
