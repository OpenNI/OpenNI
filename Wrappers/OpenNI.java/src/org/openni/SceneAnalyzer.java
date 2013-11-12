/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
package org.openni;

/**
 * Performs Scene Analysis. <BR><BR>
 * 
 * A Scene Analyzer node is a Map Generator that performs scene analysis. It supports all Map Generator functions, and adds additional functions. 
 *
 */
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
