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
	public class Codec : ProductionNode
	{
		internal Codec(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
		}

		public Codec(Context context, CodecID codecID, ProductionNode initializer) :
			this(context, Create(context, codecID, initializer), false)
		{
		}

		public CodecID CodecID
		{
			get
			{
				return new CodecID(SafeNativeMethods.xnGetCodecID(this.InternalObject));
			}
		}

		public int EncodeData(IntPtr source, int sourceSize, IntPtr destination, int destinationSize)
		{
			UInt32 written;
			int status = SafeNativeMethods.xnEncodeData(this.InternalObject, source, (UInt32)sourceSize, destination, (UInt32)destinationSize, out written);
			WrapperUtils.ThrowOnError(status);
			return (int)written;
		}

		public int DecodeData(IntPtr source, int sourceSize, IntPtr destination, int destinationSize)
		{
			UInt32 written;
			int status = SafeNativeMethods.xnDecodeData(this.InternalObject, source, (UInt32)sourceSize, destination, (UInt32)destinationSize, out written);
			WrapperUtils.ThrowOnError(status);
			return (int)written;
		}

		private static IntPtr Create(Context context, CodecID codecID, ProductionNode initializer)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnCreateCodec(context.InternalObject, codecID.InternalValue, initializer.InternalObject, out nodeHandle);
			WrapperUtils.ThrowOnError(status);
			return nodeHandle;
		}
	}
}