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
	public class ImageMetaData : MapMetaData
	{
		public ImageMetaData()
		{
		}

		public new PixelFormat PixelFormat
		{
			get { return ((MapMetaData)this).PixelFormat; }
			set { this.SetPixelFormat(value); }
		}

		public MapData<RGB24Pixel> GetRGB24ImageMap()
		{
			return new MapData<RGB24Pixel>(XRes, YRes, this.image.pData);
		}

		public MapData<byte> GetGrayscale8ImageMap()
		{
			return new MapData<byte>(XRes, YRes, this.image.pData);
		}

		public UInt16MapData GetGrayscale16ImageMap()
		{
			return new UInt16MapData(XRes, YRes, this.image.pData);
		}

		public IntPtr ImageMapPtr
		{
			get { return this.image.pData; }
		}

		internal new IMarshaler GetMarshaler(bool passOut)
		{
			return new ImageMetaDataMarshaler(this, passOut);
		}

		internal SafeNativeMethods.XnImageMetaData image = new SafeNativeMethods.XnImageMetaData();

		private class ImageMetaDataMarshaler : Marshaler<SafeNativeMethods.XnImageMetaData>
		{
			public ImageMetaDataMarshaler(ImageMetaData obj, bool marshalOut) :
				base(obj.image, marshalOut,
					MarshalInner(obj, marshalOut))
			{
			}

			private static IMarshaler MarshalInner(ImageMetaData obj, bool marshalOut)
			{
				IMarshaler inner = ((MapMetaData)obj).GetMarshaler(marshalOut);
				obj.image.pMap = inner.Native;
				return inner;
			}
		}
	}
}
