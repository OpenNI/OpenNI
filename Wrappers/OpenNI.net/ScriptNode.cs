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