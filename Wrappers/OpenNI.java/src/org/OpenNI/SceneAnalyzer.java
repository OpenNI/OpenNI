package org.OpenNI;

public class SceneAnalyzer extends MapGenerator
{

	SceneAnalyzer(Context context, long nodeHandle, boolean addRef)
			throws GeneralException
	{
		super(context, nodeHandle, addRef);
	}

	public static SceneAnalyzer create(Context context, Query query, EnumerationErrors errors) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateSceneAnalyzer(context.toNative(), handle,
			query == null ? 0 : query.toNative(),
			errors == null ? 0 : errors.toNative());
		WrapperUtils.throwOnError(status);
		SceneAnalyzer result = (SceneAnalyzer)context.createProductionNodeObject(handle.value, NodeType.SCENE);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	public static SceneAnalyzer create(Context context, Query query) throws GeneralException
	{
		return create(context, query, null);
	}

	public static SceneAnalyzer create(Context context) throws GeneralException
	{
		return create(context, null, null);
	}

	public SceneMap getSceneMap() throws GeneralException
	{
		int frameID = getFrameID();
		
		if ((this.currSceneMap == null) || (this.currSceneMapFrameID != frameID))
		{
			long ptr = NativeMethods.xnGetLabelMap(toNative());
			MapOutputMode mode = getMapOutputMode();
			this.currSceneMap = new SceneMap(ptr, mode.getXRes(), mode.getYRes());
			this.currSceneMapFrameID = frameID; 
		}

		return this.currSceneMap;
	}
	public void getMetaData(SceneMetaData sceneMD)
	{
		NativeMethods.xnGetSceneMetaData(this.toNative(), sceneMD);
	}

	public SceneMetaData getMetaData()
	{
		SceneMetaData sceneMD = new SceneMetaData();
		getMetaData(sceneMD);
		return sceneMD;
	}

	public Plane3D getFloor() throws StatusException
	{
		OutArg<Point3D> planeNormal = new OutArg<Point3D>();
		OutArg<Point3D> planePoint = new OutArg<Point3D>();
		
		int status = NativeMethods.xnGetFloor(toNative(), planeNormal, planePoint);
		WrapperUtils.throwOnError(status);
		return new Plane3D(planeNormal.value, planePoint.value);
	}

	private SceneMap currSceneMap;
	private int currSceneMapFrameID;
}
