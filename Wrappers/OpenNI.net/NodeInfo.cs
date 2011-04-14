using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class NodeInfo : ObjectWrapper
	{
		internal NodeInfo(IntPtr pNodeInfo)
			: base(pNodeInfo)
		{
		}

		/// <summary>
		/// Creates a managed NodeInfo object to wrap a native one.
		/// </summary>
		/// <param name="pNodeInfo">The native node info pointer</param>
		/// <returns>A managed NodeInfo object</returns>
		static public NodeInfo FromNative(IntPtr pNodeInfo)
		{
			return new NodeInfo(pNodeInfo);
		}

		public ProductionNodeDescription Description
		{
			get
			{
				return SafeNativeMethods.xnNodeInfoGetDescription(this.InternalObject);
			}
		}

		public string InstanceName
		{
			get
			{
				return Marshal.PtrToStringAnsi(SafeNativeMethods.xnNodeInfoGetInstanceName(this.InternalObject));
			}
			set
			{
				int status = SafeNativeMethods.xnNodeInfoSetInstanceName(this.InternalObject, value);
				WrapperUtils.ThrowOnError(status);
			}
		}

		public string CreationInfo
		{
			get
			{
				return Marshal.PtrToStringAnsi(SafeNativeMethods.xnNodeInfoGetCreationInfo(this.InternalObject));
			}
		}

		public NodeInfoList NeededNodes
		{
			get
			{
				return new NodeInfoList(SafeNativeMethods.xnNodeInfoGetNeededNodes(this.InternalObject));
			}
		}

		public ProductionNode Instance
		{
			get
			{
				IntPtr handle = SafeNativeMethods.xnNodeInfoGetHandle(this.InternalObject);
				if (handle == IntPtr.Zero)
					return null;
				else
					return ProductionNode.FromNative(handle);
			}
		}

		protected override void FreeObject(IntPtr ptr, bool disposing)
		{
			// no need to free anything
		}
	}
}