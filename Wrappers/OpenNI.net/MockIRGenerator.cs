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
	public class MockIRGenerator : IRGenerator
	{
		internal MockIRGenerator(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
		}

		public MockIRGenerator(Context context, string name) :
			this(context, Create(context, name), false)
		{
		}

		public MockIRGenerator(Context context) :
			this(context, (string)null)
		{
		}

		public MockIRGenerator(Context context, IRGenerator basedOn, string name) :
			this(context, CreateBasedOn(basedOn, name), false)
		{
		}

		public MockIRGenerator(Context context, IRGenerator basedOn) :
			this(context, basedOn, null)
		{
		}

		public void SetData(Int32 frameID, Int64 timestamp, Int32 dataSize, IntPtr buffer)
		{
			int status = SafeNativeMethods.xnMockIRSetData(this.InternalObject, (UInt32)frameID, (UInt64)timestamp, (UInt32)dataSize, buffer);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetData(IRMetaData irMD, Int32 frameID, Int64 timestamp)
		{
			SetData(frameID, timestamp, irMD.DataSize, irMD.IRMapPtr);
		}

		public void SetData(IRMetaData irMD)
		{
			SetData(irMD, irMD.FrameID, irMD.Timestamp);
		}

		private static IntPtr Create(Context context, string name)
		{
			IntPtr handle;
			int status = SafeNativeMethods.xnCreateMockNode(context.InternalObject, NodeType.IR, name, out handle);
			WrapperUtils.ThrowOnError(status);
			return handle;
		}

		private static IntPtr CreateBasedOn(IRGenerator basedOn, string name)
		{
			IntPtr handle;
			int status = SafeNativeMethods.xnCreateMockNodeBasedOn(basedOn.Context.InternalObject, 
				basedOn.InternalObject, name, out handle);
			WrapperUtils.ThrowOnError(status);
			return handle;
		}
	}
}