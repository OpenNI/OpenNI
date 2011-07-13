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