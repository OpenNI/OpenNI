/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
package org.OpenNI;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class AudioMetaData extends OutputMetaData
{
	public int getSampleRate()
	{
		return this.sampleRate;
	}
	public void setSampleRate(int sampleRate)
	{
		this.sampleRate = sampleRate;
	}
	public short getBitsPerSample()
	{
		return this.bitsPerSample;
	}
	public void setBitsPerSample(short bitsPerSample)
	{
		this.bitsPerSample = bitsPerSample;
	}
	public byte getNumberOfChannels()
	{
		return this.numberOfChannels;
	}
	public void setNumberOfChannels(byte numberOfChannels)
	{
		this.numberOfChannels = numberOfChannels;
	}
	
	public ByteBuffer createByteBuffer()
	{
		int size = getDataSize();
		ByteBuffer buffer = ByteBuffer.allocateDirect(size);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		NativeMethods.copyToBuffer(buffer, getDataPtr(), size);
		return buffer;
	}

	private int sampleRate;
	private short bitsPerSample;
	private byte numberOfChannels;
}
