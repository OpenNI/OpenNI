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
﻿using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class OutputMetaData
	{
		public Int64 Timestamp
		{
			get { return (Int64)this.data.nTimestamp; }
			set { this.data.nTimestamp = (UInt64)value; }
		}

		public int FrameID
		{
			get { return (int)this.data.nFrameID; }
			set { this.data.nFrameID = (UInt32)value; }
		}

		public int DataSize
		{
			get { return (int)this.data.nDataSize; }
			set { this.data.nDataSize = (UInt32)value; }
		}

		public bool IsDataNew
		{
			get { return this.data.bIsNew; }
			set { this.data.bIsNew = value; }
		}

		internal IMarshaler GetMarshaler(bool passOut)
		{
			return new OutputMetaDataMarshaler(this, passOut);
		}

		internal class OutputMetaDataMarshaler : Marshaler<SafeNativeMethods.XnOutputMetaData>
		{
			public OutputMetaDataMarshaler(OutputMetaData output, bool marshalOut) :
				base(output.data, marshalOut)
			{
			}
		}

		internal SafeNativeMethods.XnOutputMetaData data = new SafeNativeMethods.XnOutputMetaData();
	}
}
