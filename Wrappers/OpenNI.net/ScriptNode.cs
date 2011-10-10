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
	public class ScriptNode : ProductionNode
	{
		internal ScriptNode(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
		}

		public ScriptNode(Context context, string format) :
			this(context, Create(context, format), false)
		{
		}

		public string Format
		{
			get
			{
				return Marshal.PtrToStringAnsi(SafeNativeMethods.xnScriptNodeGetSupportedFormat(this.InternalObject));
			}
		}

		public void LoadScriptFromFile(string fileName)
		{
			int status = SafeNativeMethods.xnLoadScriptFromFile(this.InternalObject, fileName);
			WrapperUtils.ThrowOnError(status);
		}

		public void LoadScriptFromString(string script)
		{
			int status = SafeNativeMethods.xnLoadScriptFromString(this.InternalObject, script);
			WrapperUtils.ThrowOnError(status);
		}

		public void Run()
		{
			int status = SafeNativeMethods.xnScriptNodeRun(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		private static IntPtr Create(Context context, string format)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnCreateScriptNode(context.InternalObject, format, out nodeHandle);
			WrapperUtils.ThrowOnError(status);
			return nodeHandle;
		}
	}
}