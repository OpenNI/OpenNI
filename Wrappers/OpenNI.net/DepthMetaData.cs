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
	public class DepthMetaData : UInt16MapMetaData
	{
		public DepthMetaData()
		{
		}

		public int ZRes
		{
			get { return this.depth.nZRes; }
			set { this.depth.nZRes = (UInt16)value; }
		}

		public int this[int index]
		{
			get { return GetAt(this.depth.pData, index); }
			set { SetAt(this.depth.pData, index, value); }
		}

		public int this[int x, int y]
		{
			get { return GetAt(this.depth.pData, x, y); }
			set { SetAt(this.depth.pData, x, y, value); }
		}

		public UInt16MapData GetDepthMap()
		{
			return new UInt16MapData(XRes, YRes, this.depth.pData);
		}

		public IntPtr DepthMapPtr
		{
			get { return this.depth.pData; }
		}

		internal new IMarshaler GetMarshaler(bool passOut)
		{
			return new DepthMetaDataMarshaler(this, passOut);
		}

		private SafeNativeMethods.XnDepthMetaData depth = new SafeNativeMethods.XnDepthMetaData();

		private class DepthMetaDataMarshaler : Marshaler<SafeNativeMethods.XnDepthMetaData>
		{
			public DepthMetaDataMarshaler(DepthMetaData obj, bool marshalOut) :
				base(obj.depth, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(DepthMetaData obj, bool marshalOut)
			{
				IMarshaler inner = ((MapMetaData)obj).GetMarshaler(marshalOut);
				obj.depth.pMap = inner.Native;
				return inner;
			}
		}
	}
}
