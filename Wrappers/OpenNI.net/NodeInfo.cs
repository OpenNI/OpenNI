using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
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

		public void SetInstanceName(string strName)
		{
			UInt32 status = OpenNIImporter.xnNodeInfoSetInstanceName(this.InternalObject, strName);
			WrapperUtils.CheckStatus(status);
		}

		public ProductionNodeDescription GetDescription()
		{
			return OpenNIImporter.xnNodeInfoGetDescription(this.InternalObject);
		}

		public string GetInstanceName()
		{
			return OpenNIImporter.xnNodeInfoGetInstanceName(this.InternalObject);
		}

		public string GetCreationInfo()
		{
			return OpenNIImporter.xnNodeInfoGetCreationInfo(this.InternalObject);
		}

		public NodeInfoList GetNeededNodes()
		{
			return new NodeInfoList(OpenNIImporter.xnNodeInfoGetNeededNodes(this.InternalObject));
		}

		public ProductionNode GetInstance()
		{
			IntPtr handle = OpenNIImporter.xnNodeInfoGetHandle(this.InternalObject);
			if (handle == IntPtr.Zero)
				return null;
			else
				return ProductionNode.FromNative(handle);
		}

		protected override void FreeObject(IntPtr ptr)
		{
			// no need to free anything
		}
	}
}