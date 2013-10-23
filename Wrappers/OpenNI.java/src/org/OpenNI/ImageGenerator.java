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
 * Generates RGB Images. <BR><BR>
 *
 * This generator is used to obtain the data from an RGB camera.  
 * 
 * This class defines the following events:
 * pixelFormatChanged: Triggered when the pixel format of this generator changes
 */
public class ImageGenerator extends MapGenerator
{
	/**
	 * Constructor, creates a new ImageGenerator in the given context
	 * @param context OpenNI context
	 * @param nodeHandle Native pointer to this object
	 * @param addRef Whether to register this object
	 * @throws GeneralException If underlying native code returns errors, a General Exception will be generated
	 */
	ImageGenerator(Context context, long nodeHandle, boolean addRef) throws GeneralException
	{
		super(context, nodeHandle, addRef);
		
		this.pixelFormatChanged = new StateChangedObservable() 
		{
			@Override
			protected int registerNative(String cb, OutArg<Long> phCallback) 
			{
				return NativeMethods.xnRegisterToPixelFormatChange(toNative(), this, cb, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromPixelFormatChange(toNative(), hCallback);
			}
		};
	}

	/** Creates an ImageGenerator in the given context, filtered by a given query, storing any errors in the given object
	 * @param context OpenNI context for the generator
	 * @param query Query object to filter the results
	 * @param errors Place to store any errors generated
	 * @return Resulting ImageGenerator object
	 * @throws GeneralException If underlying native code returns errors, a General Exception will be generated
	 */
	public static ImageGenerator create(Context context, Query query, EnumerationErrors errors) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateImageGenerator(context.toNative(), handle,
			query == null ? 0 : query.toNative(),
			errors == null ? 0 : errors.toNative());
		WrapperUtils.throwOnError(status);
		ImageGenerator result = (ImageGenerator)context.createProductionNodeObject(handle.value, NodeType.IMAGE);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	/** Creates an ImageGenerator in the given context, filtered by the given query, discarding any errors generated
	 * @param context OpenNI context for the generator
	 * @param query Query object to filter the results
	 * @return Resulting ImageGenerator object
	 * @throws GeneralException If underlying native code returns errors, a General Exception will be generated
	 */
	public static ImageGenerator create(Context context, Query query) throws GeneralException
	{
		return create(context, query, null);
	}

	/**
	 * Creates an ImageGenerator in the given context
	 * @param context OpenNI context for the generator
	 * @return Resulting ImageGenerator object
	 * @throws GeneralException If underlying native code returns errors, a General Exception will be generated
	 */
	public static ImageGenerator create(Context context) throws GeneralException
	{
		return create(context, null, null);
	}
	
	/**
	 * Queries whether a given pixel format is supported by this generator
	 * @param format Format to query
	 * @return TRUE if the format is supported, FALSE if it is not
	 */
	public boolean isPixelFormatSupported(PixelFormat format)
	{
		return NativeMethods.xnIsPixelFormatSupported(toNative(), format.toNative());
	}
	
	/**
	 * Sets the pixel format for this generator
	 * @param format Desired new format for this generator
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public void setPixelFormat(PixelFormat format) throws StatusException
	{
		int status = NativeMethods.xnSetPixelFormat(toNative(), format.toNative());
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Provides access to the current pixel format of this generator
	 * @return Current pixel format setting for this generator
	 */
	public PixelFormat getPixelFormat()
	{
		return PixelFormat.fromNative(NativeMethods.xnGetPixelFormat(toNative()));
	}
	
	/**
	 * Provides access to the current ImageMap produced by this generator
	 * @return Current image map
	 * @throws GeneralException If underlying native code returns errors, a General Exception will be generated
	 */
	public ImageMap getImageMap() throws GeneralException
	{
		int frameID = getFrameID();
		
		if ((this.currImageMap == null) || (this.currImageMapFrameID != frameID))
		{
			long ptr = NativeMethods.xnGetImageMap(toNative());
			MapOutputMode mode = getMapOutputMode();
			this.currImageMap = new ImageMap(ptr, mode.getXRes(), mode.getYRes(), NativeMethods.xnGetBytesPerPixel(toNative()));
			this.currImageMapFrameID = frameID; 
		}

		return this.currImageMap;
	}
	
	/**
	 * Provides access to the Pixel Format Changed event.
	 * @return
	 */
	public IStateChangedObservable getPixelFormatChangedEvent() { return this.pixelFormatChanged; }
	
	/**
	 * Provides the current map data for this generator, wrapped in a Meta Data, and 
	 * copies this into a given ImageMetaData object
	 * @param ImageMD Place to copy the data
	 */
	public void getMetaData(ImageMetaData ImageMD)
	{
		NativeMethods.xnGetImageMetaData(this.toNative(), ImageMD);
	}

	/**
	 * Provides access to this generators most current ImageMetaData object
	 * @return Most current ImageMetaData object for this generator
	 */
	public ImageMetaData getMetaData()
	{
		ImageMetaData ImageMD = new ImageMetaData();
		getMetaData(ImageMD);
		return ImageMD;
	}

	private ImageMap currImageMap;
	private int currImageMapFrameID;
	private StateChangedObservable pixelFormatChanged;
}
