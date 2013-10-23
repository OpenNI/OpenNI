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
 * Represents a Depth Generator Production Node. <BR><BR>
 * 
 * Creates the following events:
 * FieldOfViewChanged: Triggered if the field of view of the generator changes
 *
 */
public class DepthGenerator extends MapGenerator 
{
	/**
	 * Constructor, creates new DepthGenerator for given context
	 * @param context OpenNI Context of the generator
	 * @param nodeHandle Native pointer to the object created
	 * @param addRef Indicates whether to register the object created 
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	DepthGenerator(Context context, long nodeHandle, boolean addRef) throws GeneralException 
	{
		super(context, nodeHandle, addRef);
		
		this.fovChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback)
			{
				return NativeMethods.xnRegisterToDepthFieldOfViewChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromDepthFieldOfViewChange(toNative(), hCallback);
			}
		};
	}
	
	/**
	 * Creates a new depth generator from given context, using a query to filter results, and storing errors
	 * @param context OpenNI context to create generator from
	 * @param query Query object to filter results
	 * @param errors Place to store any enumeration errors that occur
	 * @return resulting DepthGenerator object created
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public static DepthGenerator create(Context context, Query query, EnumerationErrors errors) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateDepthGenerator(context.toNative(), handle,
			query == null ? 0 : query.toNative(),
			errors == null ? 0 : errors.toNative());
		WrapperUtils.throwOnError(status);
		DepthGenerator result = (DepthGenerator)context.createProductionNodeObject(handle.value, NodeType.DEPTH);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	/**
	 * Creates a new depth generator from given context, using a query to filter results, discarding errors
	 * @param context OpenNI context to create the generator from 
	 * @param query Query object to filter results
	 * @return resulting DepthGenerator object created
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public static DepthGenerator create(Context context, Query query) throws GeneralException
	{
		return create(context, query, null);
	}

	/**
	 * Creates a new depth generator object in the given context	
	 * @param context OpenNI context 
	 * @return New DepthGenerator object created 
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public static DepthGenerator create(Context context) throws GeneralException
	{
		return create(context, null, null);
	}

	/**
	 * Returns the current depth map created by this object
	 * @return Most recent depth map
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public DepthMap getDepthMap() throws GeneralException
	{
		int frameID = getFrameID();
		
		if ((this.currDepthMap == null) || (this.currDepthMapFrameID != frameID))
		{
			long ptr = NativeMethods.xnGetDepthMap(toNative());
			MapOutputMode mode = getMapOutputMode();
			this.currDepthMap = new DepthMap(ptr, mode.getXRes(), mode.getYRes());
			this.currDepthMapFrameID = frameID; 
		}

		return this.currDepthMap;
	}
	
	/**
	 * Returns the maximum depth value that this generator can produce
	 * @return Maximum depth value
	 */
	public int getDeviceMaxDepth()
	{
		return NativeMethods.xnGetDeviceMaxDepth(this.toNative());
	}

	/**
	 * Get the field of view of this depth generator
	 * @return Field of view of this generator
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public FieldOfView getFieldOfView() throws StatusException
	{
		OutArg<Double> hFOV = new OutArg<Double>();
		OutArg<Double> vFOV = new OutArg<Double>();
		int status = NativeMethods.xnGetDepthFieldOfView(this.toNative(), hFOV, vFOV);
		WrapperUtils.throwOnError(status);
		return new FieldOfView(hFOV.value, vFOV.value);
	}
	
	/**
	 * Provides access to the Field Of View Changed event
	 * @return
	 */
	public IStateChangedObservable getFieldOfViewChangedEvent() { return this.fovChanged; }

    /** 
     * Convert an array of points that is currently in Projective format to Real World format.<BR><BR>
     * 
     * Note that all input points must be in Real World format for this function to work properly, as there is no 
     * format information stored as part of the Point3D class.<BR><BR>
     * 
     * This function is part of the Depth Generator class because converting between real world and projective 
     * coordinates requires information specific to the hardware creating the depth information, such as resolution
     * and field of view angles. <BR><BR>
     * 
     * Note that the entire input array is copied and populated, the input array is not altered.
     * 
     * @param projectivePoints Array of input points to be converted, all points should be in Projective format
     * @return Array of points with same length as input array, and each point converted to Real World coordinates
     * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
     */
	public Point3D[] convertProjectiveToRealWorld(Point3D[] projectivePoints) throws StatusException
	{
		Point3D[] realWorld = new Point3D[projectivePoints.length];
		int status = NativeMethods.xnConvertProjectiveToRealWorld(this.toNative(), projectivePoints, realWorld);
		WrapperUtils.throwOnError(status);
		return realWorld;
	}

    /**
     * Convert a point that is currently in Projective format to Real World format.<BR><BR>
     * 
     * Note that the input point must be in Projective format for this function to work properly, as there is no 
     * format information stored as part of the Point3D class.<BR><BR>
     * 
     * This function is part of the Depth Generator class because converting between real world and projective 
     * coordinates requires information specific to the hardware creating the depth information, such as resolution
     * and field of view angles. <BR><BR>
     * 
     * @param projectivePoint Input point to be converted, should be a set of coordinates in Real World format.
     * @return Real World representation of the input point
     * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
     */
    public Point3D convertProjectiveToRealWorld(Point3D projectivePoint) throws StatusException
    {
        Point3D[] projectivePoints = new Point3D[1];
        projectivePoints[0] = projectivePoint;

        return convertProjectiveToRealWorld(projectivePoints)[0];
    }

    
    /** 
     * Convert an array of points that is currently in Real-World format to Projective format.<BR><BR>
     * 
     * Note that all input points must be in Real World format for this function to work properly, as there is no 
     * format information stored as part of the Point3D class.<BR><BR>
     * 
     * This function is part of the Depth Generator class because converting between real world and projective 
     * coordinates requires information specific to the hardware creating the depth information, such as resolution
     * and field of view angles. <BR><BR>
     * 
     * Note that the entire input array is copied and populated, the input array is not altered.
     * 
     * @param realWorldPoints Array of input points to be converted, all points should be in Real World format
     * @return Array of points with same length as input array, and each point converted to projective coordinates
     * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
     */
    public Point3D[] convertRealWorldToProjective(Point3D[] realWorldPoints) throws StatusException
    {
        Point3D[] projective = new Point3D[realWorldPoints.length];
        int status = NativeMethods.xnConvertRealWorldToProjective(this.toNative(), realWorldPoints, projective);
        WrapperUtils.throwOnError(status);
        return projective;
    }
    
    
    /**
     * Convert a point that is currently in Real-World format to Projective format.<BR><BR>
     * 
     * Note that the input point must be in Real World format for this function to work properly, as there is no 
     * format information stored as part of the Point3D class.<BR><BR>
     * 
     * This function is part of the Depth Generator class because converting between real world and projective 
     * coordinates requires information specific to the hardware creating the depth information, such as resolution
     * and field of view angles. <BR><BR>
     * 
     * @param realWorldPoint Input point to be converted, should be a set of coordinates in Real World format.
     * @return Projective representation of the input point
     * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
     */
    public Point3D convertRealWorldToProjective(Point3D realWorldPoint) throws StatusException
	{
        Point3D[] realWorldPoints = new Point3D[1];
        realWorldPoints[0] = realWorldPoint;

        return convertRealWorldToProjective(realWorldPoints)[0];
    }

	/**
	 * Provides access to the UserPositionCapability of this generator
	 * @return UserPositionCapability object tied to this generator
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public UserPositionCapability getUserPositionCapability() throws StatusException
	{
		return new UserPositionCapability(this);
	}

	/**
	 * Get the current data from this generator, wrapped in a MetaData object
	 * @param depthMD DepthMetaData object to store the results in
	 */
	public void getMetaData(DepthMetaData depthMD)
	{
		NativeMethods.xnGetDepthMetaData(this.toNative(), depthMD);
	}

	/**
	 * Returns the current depth frame data, plus associated meta data
	 * @return Object of type DepthMetaData with depth data from current frame
	 */
	public DepthMetaData getMetaData()
	{
		DepthMetaData depthMD = new DepthMetaData();
		getMetaData(depthMD);
		return depthMD;
	}

	private StateChangedObservable fovChanged;
	private DepthMap currDepthMap;
	private int currDepthMapFrameID;
}
