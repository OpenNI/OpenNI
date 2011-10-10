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