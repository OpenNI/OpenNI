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