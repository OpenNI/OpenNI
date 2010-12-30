using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
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

 		public ProductionNodeDescription Description
		{
            get
            {
                return OpenNIImporter.xnNodeInfoGetDescription(this.InternalObject);
            }
		}

		public string InstanceName
		{
            get
            {
                return OpenNIImporter.xnNodeInfoGetInstanceName(this.InternalObject);
            }
            set
            {
                Status.ThrowOnFail(OpenNIImporter.xnNodeInfoSetInstanceName(this.InternalObject, value));
			    
            }
		}

		public string CreationInfo
		{
            get
            {
                return OpenNIImporter.xnNodeInfoGetCreationInfo(this.InternalObject);
            }
		}

		public NodeInfoList NeededNodes
		{
            get
            {
                return new NodeInfoList(OpenNIImporter.xnNodeInfoGetNeededNodes(this.InternalObject));
            }
		}

		public ProductionNode Instance
		{
            get
            {
                NodeSafeHandle handle = OpenNIImporter.xnNodeInfoGetHandle(this.InternalObject);
                if (handle.IsInvalid)
                    return null;
                else
                    return ProductionNode.FromNative(handle);
            }
		}
	}
}