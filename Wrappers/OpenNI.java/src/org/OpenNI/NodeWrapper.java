package org.OpenNI;

public class NodeWrapper extends ObjectWrapper
{
	NodeWrapper(Context context, long hNode, boolean addRef) throws StatusException
	{
		super(hNode);
		
		this.context = context;
		
		if (addRef)
		{
			WrapperUtils.throwOnError(NativeMethods.xnProductionNodeAddRef(hNode));
		}
	}
	
	public Context getContext()
	{
		return this.context;
	}
	
	public String getName()
	{
		return NativeMethods.xnGetNodeName(toNative());
	}

	protected void freeObject(long ptr)
	{
		NativeMethods.xnProductionNodeRelease(ptr);
	}
	
	private Context context;
}
