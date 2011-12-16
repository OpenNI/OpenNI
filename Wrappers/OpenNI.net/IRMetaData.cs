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
