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
	public class MockDepthGenerator : DepthGenerator
	{
		internal MockDepthGenerator(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
		}

		public MockDepthGenerator(Context context, string name) :
			this(context, Create(context, name), false)
		{
		}

		public MockDepthGenerator(Context context) :
			this(context, (string)null)
		{
		}

		public MockDepthGenerator(Context context, DepthGenerator basedOn, string name) :
			this(context, CreateBasedOn(basedOn, name), false)
		{
		}

		public MockDepthGenerator(Context context, DepthGenerator basedOn) :
			this(context, basedOn, null)
		{
		}

		public void SetData(Int32 frameID, Int64 timestamp, Int32 dataSize, IntPtr buffer)
		{
			int status = SafeNativeMethods.xnMockDepthSetData(this.InternalObject, (UInt32)frameID, (UInt64)timestamp, (UInt32)dataSize, buffer);
			WrapperUtils.ThrowOnError(status);
		}

		public void SetData(DepthMetaData depthMD, Int32 frameID, Int64 timestamp)
		{
			SetData(frameID, timestamp, depthMD.DataSize, depthMD.DepthMapPtr);
		}

		public void SetData(DepthMetaData depthMD)
		{
			SetData(depthMD, depthMD.FrameID, depthMD.Timestamp);
		}

		private static IntPtr Create(Context context, string name)
		{
			IntPtr handle;
			int status = SafeNativeMethods.xnCreateMockNode(context.InternalObject, NodeType.Depth, name, out handle);
			WrapperUtils.ThrowOnError(status);
			return handle;
		}

		private static IntPtr CreateBasedOn(DepthGenerator basedOn, string name)
		{
			IntPtr handle;
			int status = SafeNativeMethods.xnCreateMockNodeBasedOn(basedOn.Context.InternalObject, 
				basedOn.InternalObject, name, out handle);
			WrapperUtils.ThrowOnError(status);
			return handle;
		}
	}
}