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
 * Represents a specific Codec used for recording and playback of sensor data. <BR><BR>
 * 
 *
 */
public class Codec extends ProductionNode
{
	/**
	 * Creates a new codec object
	 * @param context OpenNI context for this codec
	 * @param nodeHandle Native pointer to this object
	 * @param addRef Whether to register the native pointer to this object
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	Codec(Context context, long nodeHandle, boolean addRef) throws StatusException
	{
		super(context, nodeHandle, addRef);
	}

	/**
	 * Create a new codec object
	 * @param context OpenNI context for this codec
	 * @param codecID CodecID of this codec
	 * @param initializer
	 * @return Codec created
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public static Codec create(Context context, CodecID codecID, ProductionNode initializer) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateCodec(context.toNative(), codecID.toNative(), initializer.toNative(), handle);
		WrapperUtils.throwOnError(status);
		Codec result = (Codec)context.createProductionNodeObject(handle.value, NodeType.CODEC);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	/**
	 * Provides access to the CodecID of this codec
	 * @return Current CodecID of this codec
	 */
	public CodecID getCodecID()
	{
		int codecID = NativeMethods.xnGetCodecID(toNative());
		return new CodecID(codecID);
	}
	
	/**
	 * Encodes data using this codec
	 * @param pSrcPtr Pointer to the data to be encoded
	 * @param nSrcSize Size of data to be encoded
	 * @param pDstPtr Pointer to where encoded data should be stored
	 * @param nDstSize Size of encoded data
	 * @return Number of bytes written to the destination buffer
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public int EncodeData(long pSrcPtr, int nSrcSize, long pDstPtr, int nDstSize) throws StatusException
	{
		OutArg<Integer> written = new OutArg<Integer>();
		int status = NativeMethods.xnEncodeData(toNative(), pSrcPtr, nSrcSize, pDstPtr, nDstSize, written);
		WrapperUtils.throwOnError(status);
		return written.value;
	}

	/**
	 * Decodes data using this codec
	 * @param pSrcPtr Pointer to the data to be decoded
	 * @param nSrcSize Size of the data to be decoded
	 * @param pDstPtr Pointer to where the decoded data should be stored
	 * @param nDstSize Size of the decoded data
	 * @return Number of bytes written to the destination buffer
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public int DecodeData(long pSrcPtr, int nSrcSize, long pDstPtr, int nDstSize) throws StatusException
	{
		OutArg<Integer> written = new OutArg<Integer>();
		int status = NativeMethods.xnDecodeData(toNative(), pSrcPtr, nSrcSize, pDstPtr, nDstSize, written);
		WrapperUtils.throwOnError(status);
		return written.value;
	}
}
