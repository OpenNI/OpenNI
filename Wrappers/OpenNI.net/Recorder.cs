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
	public class Recorder : ProductionNode
	{
		internal Recorder(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
		}

		public Recorder(Context context, string formatName) :
			this(context, Create(context, formatName), false)
		{
		}

		public Recorder(Context context) :
			this(context, null)
		{
		}

		public void SetDestination(RecordMedium medium, string destination)
		{
			int status = SafeNativeMethods.xnSetRecorderDestination(this.InternalObject, medium, destination);
			WrapperUtils.ThrowOnError(status);
		}

		public void AddNodeToRecording(ProductionNode node, CodecID codec)
		{
			int status = SafeNativeMethods.xnAddNodeToRecording(this.InternalObject, node.InternalObject, codec.InternalValue);
			WrapperUtils.ThrowOnError(status);
		}

		public void AddNodeToRecording(ProductionNode node)
		{
			AddNodeToRecording(node, CodecID.Null);
		}

		public void RemoveNodeFromRecording(ProductionNode node)
		{
			int status = SafeNativeMethods.xnRemoveNodeFromRecording(this.InternalObject, node.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void Record()
		{
			int status = SafeNativeMethods.xnRecord(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		private static IntPtr Create(Context context, string formatName)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnCreateRecorder(context.InternalObject, formatName, out nodeHandle);
			WrapperUtils.ThrowOnError(status);
			return nodeHandle;
		}
	}
}