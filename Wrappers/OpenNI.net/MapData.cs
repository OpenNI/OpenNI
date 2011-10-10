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
	internal static class MapUtils<T>
	{
		public static T GetAt(IntPtr dataBuff, int index)
		{
			IntPtr ptr = new IntPtr(dataBuff.ToInt64() + index * Marshal.SizeOf(typeof(T)));
			return (T)Marshal.PtrToStructure(ptr, typeof(T));
		}

		public static void SetAt(IntPtr dataBuff, int index, T value)
		{
			IntPtr ptr = new IntPtr(dataBuff.ToInt64() + index * Marshal.SizeOf(typeof(T)));
			Marshal.StructureToPtr(value, ptr, false);
		}

		public static T GetAt(IntPtr dataBuff, int xRes, int x, int y)
		{
			return GetAt(dataBuff, y * xRes + x);
		}

		public static void SetAt(IntPtr dataBuff, int xRes, int x, int y, T value)
		{
			SetAt(dataBuff, y * xRes + x, value);
		}
	}

	public class MapData<T> : IMapData<T>
	{
		public MapData(int xRes, int yRes, IntPtr dataBuff)
		{
			this.xRes = xRes;
			this.yRes = yRes;
			this.dataBuff = dataBuff;
		}

		public int XRes
		{
			get { return this.xRes; }
		}

		public int YRes
		{
			get { return this.yRes; }
		}

		public T this[int index]
		{
			get	{ return MapUtils<T>.GetAt(this.dataBuff, index); }
			set { MapUtils<T>.SetAt(this.dataBuff, index, value); }
		}

		public T this[int x, int y]
		{
			get	{ return MapUtils<T>.GetAt(this.dataBuff, this.xRes, x, y); }
			set { MapUtils<T>.SetAt(this.dataBuff, this.xRes, x, y, value); }
		}

		private int xRes;
		private int yRes;
		private IntPtr dataBuff;
	}

	public class UInt16MapData : IMapData<int>
	{
		public UInt16MapData(int xRes, int yRes, IntPtr dataBuff)
		{
			this.actualMap = new MapData<UInt16>(xRes, yRes, dataBuff);
		}

		public int XRes
		{
			get { return this.actualMap.XRes; }
		}

		public int YRes
		{
			get { return this.actualMap.YRes; }
		}

		public int this[int index]
		{
			get { return (int)this.actualMap[index]; }
			set { this.actualMap[index] = (UInt16)value; }
		}

		public int this[int x, int y]
		{
			get { return (int)this.actualMap[x, y]; }
			set { this.actualMap[x, y] = (UInt16)value; }
		}

		private MapData<UInt16> actualMap;
	}
}
