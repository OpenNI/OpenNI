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
