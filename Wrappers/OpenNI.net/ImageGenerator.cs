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
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class ImageGenerator : MapGenerator
	{
		internal ImageGenerator(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
			this.pixelFormatChanged = new StateChangedEvent(this,
				SafeNativeMethods.xnRegisterToPixelFormatChange,
				SafeNativeMethods.xnUnregisterFromPixelFormatChange);
		}

		public ImageGenerator(Context context, Query query, EnumerationErrors errors) :
			this(context, Create(context, query, errors), false)
		{
		}

		public ImageGenerator(Context context, Query query) :
			this(context, query, null)
		{
		}

		public ImageGenerator(Context context) :
			this(context, null, null)
		{
		}

		public IntPtr ImageMapPtr
		{
			get
			{
				return SafeNativeMethods.xnGetImageMap(this.InternalObject);
			}
		}

		public MapData<RGB24Pixel> GetRGB24ImageMap()
		{
			return GetMapData<RGB24Pixel>(SafeNativeMethods.xnGetRGB24ImageMap(this.InternalObject));
		}

		public MapData<byte> GetGrayscale8ImageMap()
		{
			return GetMapData<byte>(SafeNativeMethods.xnGetGrayscale8ImageMap(this.InternalObject));
		}

		public UInt16MapData GetGrayscale16ImageMap()
		{
			MapOutputMode mode = this.MapOutputMode;
			return new UInt16MapData(mode.XRes, mode.YRes, SafeNativeMethods.xnGetGrayscale16ImageMap(this.InternalObject));
		}

		public bool IsPixelFormatSupported(PixelFormat format)
		{
			return SafeNativeMethods.xnIsPixelFormatSupported(this.InternalObject, format);
		}

		public PixelFormat PixelFormat
		{
			get
			{
				return SafeNativeMethods.xnGetPixelFormat(this.InternalObject);
			}
			set
			{
				int status = SafeNativeMethods.xnSetPixelFormat(this.InternalObject, value);
				WrapperUtils.ThrowOnError(status);
			}
		}

		public void GetMetaData(ImageMetaData imageMD)
		{
			using (IMarshaler marsh = imageMD.GetMarshaler(true))
			{
				SafeNativeMethods.xnGetImageMetaData(this.InternalObject, marsh.Native);
			}
		}

		public ImageMetaData GetMetaData()
		{
			ImageMetaData imageMD = new ImageMetaData();
			GetMetaData(imageMD);
			return imageMD;
		}

		public event EventHandler PixelFormatChanged
		{
			add { this.pixelFormatChanged.Event += value; }
			remove { this.pixelFormatChanged.Event -= value; }
		}

		private static IntPtr Create(Context context, Query query, EnumerationErrors errors)
		{
			IntPtr handle;
			int status = SafeNativeMethods.xnCreateImageGenerator(context.InternalObject, out handle,
				query == null ? IntPtr.Zero : query.InternalObject,
				errors == null ? IntPtr.Zero : errors.InternalObject);
			WrapperUtils.ThrowOnError(status);
			return handle;
		}

		private StateChangedEvent pixelFormatChanged;
	}
}