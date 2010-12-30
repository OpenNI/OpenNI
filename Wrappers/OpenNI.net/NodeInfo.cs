using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace xn
{
	public class NodeInfo : ObjectWrapper
	{
		internal NodeInfo(NodeInfoSafeHandle pNodeInfo)
			: base(pNodeInfo)
		{
		}

		/// <summary>
		/// Creates a managed NodeInfo object to wrap a native one.
		/// </summary>
		/// <param name="pNodeInfo">The native node info pointer</param>
		/// <returns>A managed NodeInfo object</returns>
        static internal NodeInfo FromNative(NodeInfoSafeHandle pNodeInfo)
		{
			return new NodeInfo(pNodeInfo);
		}

        internal new NodeInfoSafeHandle InternalObject
        {
            get { return (NodeInfoSafeHandle)base.InternalObject; }
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
			NodeSafeHandle handle = OpenNIImporter.xnNodeInfoGetHandle(this.InternalObject);
			if (handle.IsInvalid)
				return null;
			else
				return ProductionNode.FromNative(handle);
		}
	}
}