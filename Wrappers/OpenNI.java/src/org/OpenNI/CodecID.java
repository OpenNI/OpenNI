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
 * Stores the ID code of a codec. <BR><BR>
 *
 */
public class CodecID
{
	public static final CodecID Null = new CodecID((byte)0, (byte)0, (byte)0, (byte)0);
	public static final CodecID Uncompressed = new CodecID('N', 'O', 'N', 'E');
	public static final CodecID Jpeg = new CodecID('J', 'P', 'E', 'G');
	public static final CodecID Z16 = new CodecID('1', '6', 'z', 'P');
	public static final CodecID Z16WithTables = new CodecID('1', '6', 'z', 'T');
	public static final CodecID Z8 = new CodecID('I', 'm', '8', 'z');
	
	/**
	 * Stores a CodecID value provided by native code in this object
	 * @param nativeValue Value to be stored
	 */
	public CodecID(int nativeValue)
	{
		this.value = nativeValue;
	}
	
	/**
	 * Packs the four individual bytes of a codec ID and stores in this object
	 * @param byte1 First byte of ID
	 * @param byte2 Second byte of ID
	 * @param byte3 Third byte of ID
	 * @param byte4 Fourth byte of ID
	 */
	public CodecID(byte byte1, byte byte2, byte byte3, byte byte4)
	{
		this(byte4 << 24 | byte3 << 16 | byte2 << 8 | byte1);
	}
	
	/**
	 * Packs the four individual bytes of a codec ID and stores them,
	 * with the bytes provided to the function as characters
	 * @param char1 First byte of the ID
	 * @param char2 Second byte of the ID
	 * @param char3 Third byte of the ID
	 * @param char4 Fourth byte of the ID
	 */
	public CodecID(char char1, char char2, char char3, char char4)
	{
		this((byte)char1, (byte)char2, (byte)char3, (byte)char4);
	}
	
	/**
	 * Creates a hash value from the codec value
	 * @return Hash value created
	 */
	@Override
	public int hashCode()
	{
		final int prime = 31;
		int result = 1;
		result = prime * result + this.value;
		return result;
	}

	/**
	 * Tests this object for equality with another object
	 * @param obj Object to be compared with
	 * @return TRUE if the objects are equal, false otherwise
	 */
	@Override
	public boolean equals(Object obj)
	{
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		CodecID other = (CodecID) obj;
		if (this.value != other.value)
			return false;
		return true;
	}

	/**
	 * Translates the value stored by this object into a CodecID value
	 * suitable for use by native code
	 * @return Native value that was created
	 */
	public int toNative() { return this.value; }
	
	private int value;
}
