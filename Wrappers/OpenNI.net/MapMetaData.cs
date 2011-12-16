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
	public class MapMetaData : OutputMetaData
	{
		public MapMetaData()
		{
		}

		public int XRes
		{
			get { return this.map.Res.X; }
			set { this.map.Res.X = value; }
		}

		public int YRes
		{
			get { return this.map.Res.Y; }
			set { this.map.Res.Y = value; }
		}

		public int XOffset
		{
			get { return this.map.Offset.X; }
			set { this.map.Offset.X = value; }
		}

		public int YOffset
		{
			get { return this.map.Offset.Y; }
			set { this.map.Offset.Y = value; }
		}

		public int FullXRes
		{
			get { return this.map.FullRes.X; }
			set { this.map.FullRes.X = value; }
		}

		public int FullYRes
		{
			get { return this.map.FullRes.Y; }
			set { this.map.FullRes.Y = value; }
		}

		public int FPS
		{
			get { return (int)this.map.nFPS; }
			set { this.map.nFPS = (uint)value; }
		}

		public PixelFormat PixelFormat
		{
			get { return this.map.PixelFormat; }
		}

		public int BytesPerPixel
		{
			get
			{
				switch (PixelFormat)
				{
					case PixelFormat.RGB24:
						return 3;
					case PixelFormat.YUV422:
						return 2;
					case PixelFormat.Grayscale8Bit:
						return 1;
					case PixelFormat.Grayscale16Bit:
						return 2;
					default:
						throw new NotImplementedException();
				}
			}
		}

		internal new IMarshaler GetMarshaler(bool passOut)
		{
			return new MapMetaDataMarshaler(this, passOut);
		}

		protected void SetPixelFormat(PixelFormat format)
		{
			this.map.PixelFormat = format;
		}

		private class MapMetaDataMarshaler : Marshaler<SafeNativeMethods.XnMapMetaData>
		{
			public MapMetaDataMarshaler(MapMetaData obj, bool marshalOut) :
				base(obj.map, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(MapMetaData obj, bool marshalOut)
			{
				IMarshaler inner = ((OutputMetaData)obj).GetMarshaler(marshalOut);
				obj.map.pOutput = inner.Native;
				return inner;
			}
		}

		internal SafeNativeMethods.XnMapMetaData map = new SafeNativeMethods.XnMapMetaData();
	}

	public class UInt16MapMetaData : MapMetaData
	{
		protected int GetAt(IntPtr dataBuff, int index)
		{
			return MapUtils<UInt16>.GetAt(dataBuff, index);
		}

		protected void SetAt(IntPtr dataBuff, int index, int value)
		{
			MapUtils<UInt16>.SetAt(dataBuff, index, (UInt16)value);
		}

		protected int GetAt(IntPtr dataBuff, int x, int y)
		{
			return MapUtils<UInt16>.GetAt(dataBuff, XRes, x, y);
		}

		protected void SetAt(IntPtr dataBuff, int x, int y, int value)
		{
			MapUtils<UInt16>.SetAt(dataBuff, XRes, x, y, (UInt16)value);
		}
	}
}
