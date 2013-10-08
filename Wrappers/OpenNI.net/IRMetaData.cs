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
	public class IRMetaData : UInt16MapMetaData
	{
		public IRMetaData()
		{
		}

		public int this[int index]
		{
			get { return GetAt(this.ir.pData, index); }
			set { SetAt(this.ir.pData, index, value); }
		}

		public int this[int x, int y]
		{
			get { return GetAt(this.ir.pData, x, y); }
			set { SetAt(this.ir.pData, x, y, value); }
		}

		public UInt16MapData GetIRMap()
		{
			return new UInt16MapData(XRes, YRes, this.ir.pData);
		}

		public IntPtr IRMapPtr
		{
			get { return this.ir.pData; }
		}

		internal new IMarshaler GetMarshaler(bool passOut)
		{
			return new IRMetaDataMarshaler(this, passOut);
		}

		internal SafeNativeMethods.XnIRMetaData ir = new SafeNativeMethods.XnIRMetaData();

		private class IRMetaDataMarshaler : Marshaler<SafeNativeMethods.XnIRMetaData>
		{
			public IRMetaDataMarshaler(IRMetaData obj, bool marshalOut) :
				base(obj.ir, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(IRMetaData obj, bool marshalOut)
			{
				IMarshaler inner = ((MapMetaData)obj).GetMarshaler(marshalOut);
				obj.ir.pMap = inner.Native;
				return inner;
			}
		}
	}
}
