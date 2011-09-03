package org.OpenNI;

public class ScriptNode extends ProductionNode
{
	ScriptNode(Context context, long nodeHandle, boolean addRef) throws StatusException
	{
		super(context, nodeHandle, addRef);
	}

	public static ScriptNode create(Context context, String formatName) throws GeneralException
	{
		OutArg<Long> phScriptNode = new OutArg<Long>();
		int status = NativeMethods.xnCreateScriptNode(context.toNative(), formatName, phScriptNode);
		WrapperUtils.throwOnError(status);
		ScriptNode ScriptNode = (ScriptNode)context.createProductionNodeObject(phScriptNode.value, NodeType.SCRIPT_NODE);
		NativeMethods.xnProductionNodeRelease(phScriptNode.value);
		return ScriptNode;
	}

	public String getSupportedFormat()
	{
		return NativeMethods.xnScriptNodeGetSupportedFormat(toNative());
	}
	
	public void loadScriptFromFile(String fileName) throws StatusException
	{
		int status = NativeMethods.xnLoadScriptFromFile(toNative(), fileName);
		WrapperUtils.throwOnError(status);
	}

	public void loadScriptFromString(String script) throws StatusException
	{
		int status = NativeMethods.xnLoadScriptFromString(toNative(), script);
		WrapperUtils.throwOnError(status);
	}
	
	public void Run(EnumerationErrors errors) throws StatusException
	{
		int status = NativeMethods.xnScriptNodeRun(toNative(), errors.toNative());
		WrapperUtils.throwOnError(status);
	}
}
