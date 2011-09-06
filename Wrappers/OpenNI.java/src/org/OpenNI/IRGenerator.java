package org.OpenNI;

public class IRGenerator extends MapGenerator
{
	IRGenerator(Context context, long nodeHandle, boolean addRef) throws GeneralException
	{
		super(context, nodeHandle, addRef);
		// TODO Auto-generated constructor stub
	}

	public static IRGenerator create(Context context, Query query, EnumerationErrors errors) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateIRGenerator(context.toNative(), handle,
			query == null ? 0 : query.toNative(),
			errors == null ? 0 : errors.toNative());
		WrapperUtils.throwOnError(status);
		IRGenerator result = (IRGenerator)context.createProductionNodeObject(handle.value, NodeType.IR);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	public static IRGenerator create(Context context, Query query) throws GeneralException
	{
		return create(context, query, null);
	}

	public static IRGenerator create(Context context) throws GeneralException
	{
		return create(context, null, null);
	}
	
	public IRMap getIRMap() throws GeneralException
	{
		int frameID = getFrameID();
		
		if ((this.currIRMap == null) || (this.currIRMapFrameID != frameID))
		{
			long ptr = NativeMethods.xnGetIRMap(toNative());
			MapOutputMode mode = getMapOutputMode();
			this.currIRMap = new IRMap(ptr, mode.getXRes(), mode.getYRes());
			this.currIRMapFrameID = frameID; 
		}

		return this.currIRMap;
	}
	
	public void getMetaData(IRMetaData IRMD)
	{
		NativeMethods.xnGetIRMetaData(this.toNative(), IRMD);
	}

	public IRMetaData getMetaData()
	{
		IRMetaData IRMD = new IRMetaData();
		getMetaData(IRMD);
		return IRMD;
	}

	private IRMap currIRMap;
	private int currIRMapFrameID;
}
