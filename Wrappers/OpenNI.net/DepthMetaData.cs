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
using System;
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
