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