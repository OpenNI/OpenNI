using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices;

namespace xn
{
	public delegate void ErrorStateChangedHandler(string currError);

	public class Context : ObjectWrapper
	{
		internal Context(ContextSafeHandle pContext) : 
			base(pContext)
		{
		}

		/// <summary>
		/// Creates a managed Context object to wrap a native one.
		/// </summary>
		/// <param name="pContext">A pointer to the native object</param>
		/// <returns>A managed Context object</returns>
		static internal Context FromNative(ContextSafeHandle pContext)
		{
            Contract.Requires(pContext != null);
            
            return new Context(pContext);
		}

        internal new ContextSafeHandle InternalObject
        {
            get { return (ContextSafeHandle)base.InternalObject; }
        }

        static internal ProductionNode CreateProductionNodeFromNative(NodeSafeHandle hNodeHandle)
		{
            Contract.Requires(!hNodeHandle.IsClosed);

            return CreateProductionNodeObject(hNodeHandle);
		}

		public void RunXmlScript(string xml)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(xml));

			EnumerationErrors errors = new EnumerationErrors();
			UInt32 status = OpenNIImporter.xnContextRunXmlScript(this.InternalObject, xml, errors.InternalObject);
			WrapperUtils.CheckEnumeration(status, errors);
		}

		public void RunXmlScriptFromFile(string xmlFile)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(xmlFile));
            
            EnumerationErrors errors = new EnumerationErrors();
			UInt32 status = OpenNIImporter.xnContextRunXmlScriptFromFile(this.InternalObject, xmlFile, errors.InternalObject);
			WrapperUtils.CheckEnumeration(status, errors);
		}

		public void OpenFileRecording(string fileName)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(fileName));

			UInt32 status = OpenNIImporter.xnContextOpenFileRecording(this.InternalObject, fileName);
			WrapperUtils.CheckStatus(status);
		}

        //public void Shutdown()
        //{
        //    Dispose();
        //}

		public void AddLicense(License license)
		{
            Contract.Requires(license != null);

			UInt32 status = OpenNIImporter.xnAddLicense(this.InternalObject, license);
			WrapperUtils.CheckStatus(status);
		}

		public License[] EnumerateLicenses()
		{
			IntPtr pArray;
			uint size;
			UInt32 status = OpenNIImporter.xnEnumerateLicenses(this.InternalObject, out pArray, out size);
			WrapperUtils.CheckStatus(status);

			License[] result = null;

			try
			{
				result = new License[size];
				IntPtr ptr = pArray;

				for (int i = 0; i < size; ++i)
				{
					result[i] = (License)Marshal.PtrToStructure(ptr, typeof(License));
					ptr = (IntPtr)(ptr.ToInt64() + Marshal.SizeOf(typeof(License)));
				}
			}
			finally
			{
				OpenNIImporter.xnFreeLicensesList(pArray);
			}

			return result;
		}

		public NodeInfoList EnumerateProductionTrees(NodeType type, Query query)
		{
            Contract.Requires(query != null);
            Contract.Ensures(Contract.Result<NodeInfoList>() != null);

            NodeInfoListSafeHandle resultList;

			using (EnumerationErrors errors = new EnumerationErrors())
			{
				UInt32 status = OpenNIImporter.xnEnumerateProductionTrees(this.InternalObject, type,
					query == null ? QuerySafeHandle.Zero : query.InternalObject,
					out resultList,
					errors.InternalObject);
				WrapperUtils.CheckEnumeration(status, errors);
			}

			return new NodeInfoList(resultList);
		}

		public ProductionNode CreateAnyProductionTree(NodeType type, Query query)
		{
            Contract.Requires(query != null);
            Contract.Ensures(Contract.Result<ProductionNode>() != null);
            
            NodeSafeHandle nodeHandle = CreateAnyProductionTreeImpl(type, query);
			return CreateProductionNodeObject(nodeHandle, type);
		}

		public ProductionNode CreateProductionTree(NodeInfo nodeInfo)
		{
            Contract.Requires(nodeInfo != null);
            Contract.Ensures(Contract.Result<ProductionNode>() != null);

            NodeSafeHandle nodeHandle;
			UInt32 status = OpenNIImporter.xnCreateProductionTree(this.InternalObject, nodeInfo.InternalObject, out nodeHandle);
			WrapperUtils.CheckStatus(status);
			return CreateProductionNodeObject(nodeHandle, nodeInfo.Description.Type);
		}

		public NodeInfoList EnumerateExistingNodes()
		{
            Contract.Ensures(Contract.Result<NodeInfoList>() != null);

            NodeInfoListSafeHandle pList;
			UInt32 status = OpenNIImporter.xnEnumerateExistingNodes(this.InternalObject, out pList);
			WrapperUtils.CheckStatus(status);
			return new NodeInfoList(pList);
		}

		public NodeInfoList EnumerateExistingNodes(NodeType type)
		{
            Contract.Ensures(Contract.Result<NodeInfoList>() != null);

            NodeInfoListSafeHandle pList;
			UInt32 status = OpenNIImporter.xnEnumerateExistingNodesByType(this.InternalObject, type, out pList);
			WrapperUtils.CheckStatus(status);
			return new NodeInfoList(pList);
		}

		public ProductionNode FindExistingNode(NodeType type)
		{
            Contract.Ensures(Contract.Result<ProductionNode>() != null);

            NodeSafeHandle nodeHandle;
			UInt32 status = OpenNIImporter.xnFindExistingNodeByType(this.InternalObject, type, out nodeHandle);
			WrapperUtils.CheckStatus(status);
			return CreateProductionNodeObject(nodeHandle, type);
		}

		public ProductionNode GetProductionNodeByName(string name)
		{
            Contract.Ensures(Contract.Result<ProductionNode>() != null);

            NodeSafeHandle nodeHandle;
			UInt32 status = OpenNIImporter.xnGetNodeHandleByName(this.InternalObject, name, out nodeHandle);
			WrapperUtils.CheckStatus(status);
			return CreateProductionNodeObject(nodeHandle);
		}

		public NodeInfo GetProductionNodeInfoByName(string name)
		{
            Contract.Requires(!String.IsNullOrEmpty(name));
            Contract.Ensures(Contract.Result<NodeInfo>() != null);

            NodeSafeHandle nodeHandle;
			UInt32 status = OpenNIImporter.xnGetNodeHandleByName(this.InternalObject, name, out nodeHandle);
			WrapperUtils.CheckStatus(status);
			NodeInfoSafeHandle nodeInfo = OpenNIImporter.xnGetNodeInfo(nodeHandle);
			return new NodeInfo(nodeInfo);
		}

		public void StartGeneratingAll()
		{
			UInt32 status = OpenNIImporter.xnStartGeneratingAll(this.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void StopGeneratingAll()
		{
			UInt32 status = OpenNIImporter.xnStopGeneratingAll(this.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public bool GlobalMirror
        {
            get
            {
                return OpenNIImporter.xnGetGlobalMirror(this.InternalObject);
            }
            set
            {
			    UInt32 status = OpenNIImporter.xnSetGlobalMirror(this.InternalObject, value);
			    WrapperUtils.CheckStatus(status);
            }
		}

		/// <summary>
		/// Gets the global error state.
		/// </summary>
		/// <returns>null if all is OK, a description of the error otherwise</returns>
		public string GetGlobalErrorState()
		{
			UInt32 state = OpenNIImporter.xnGetGlobalErrorState(this.InternalObject);
			if (state == 0)
			{
				return null;
			}
			else
			{
				return WrapperUtils.GetErrorMessage(state);
			}
		}

		public event ErrorStateChangedHandler ErrorStateChanged
		{
			add
			{
				if (this.errorStateChanged == null)
				{
					UInt32 status = OpenNIImporter.xnRegisterToGlobalErrorStateChange(this.InternalObject, ErrorStateChangedCallback, IntPtr.Zero, out this.errorStateCallbackHandle);
					WrapperUtils.CheckStatus(status);
				}

				this.errorStateChanged += value;
			}
			remove
			{
				this.errorStateChanged -= value;

				if (this.errorStateChanged == null)
				{
					OpenNIImporter.xnUnregisterFromGlobalErrorStateChange(this.InternalObject, this.errorStateCallbackHandle);
				}
			}
		}

		public void WaitAndUpdateAll()
		{
			UInt32 status = OpenNIImporter.xnWaitAndUpdateAll(this.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void WaitAnyUpdateAll()
		{
			UInt32 status = OpenNIImporter.xnWaitAnyUpdateAll(this.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void WaitOneUpdateAll(Generator node)
		{
			UInt32 status = OpenNIImporter.xnWaitOneUpdateAll(this.InternalObject, node.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

		public void WaitNoneUpdateAll()
		{
			UInt32 status = OpenNIImporter.xnWaitNoneUpdateAll(this.InternalObject);
			WrapperUtils.CheckStatus(status);
		}

        internal NodeSafeHandle CreateAnyProductionTreeImpl(NodeType type, Query query)
		{
            NodeSafeHandle nodeHandle;
			using (EnumerationErrors errors = new EnumerationErrors())
			{
				UInt32 status = OpenNIImporter.xnCreateAnyProductionTree(this.InternalObject, type,
					query == null ? QuerySafeHandle.Zero : query.InternalObject,
					out nodeHandle, errors.InternalObject);
				WrapperUtils.CheckEnumeration(status, errors);
			}

			return nodeHandle;
		}

        public static Context Init()
		{
			ContextSafeHandle pContext;
			UInt32 status = OpenNIImporter.xnInit(out pContext);
			WrapperUtils.CheckStatus(status);
			return new Context(pContext);
		}

        public static Context InitFromXml(string xmlFile)
		{
            ContextSafeHandle pContext;
            EnumerationErrors errors = new EnumerationErrors();
			UInt32 status = OpenNIImporter.xnInitFromXmlFile(xmlFile, out pContext, errors.InternalObject);
			WrapperUtils.CheckEnumeration(status, errors);
            return new Context(pContext);
		}

        private static ProductionNode CreateProductionNodeObject(NodeSafeHandle nodeHandle, NodeType? type)
		{
			if (type == null)
			{
				NodeInfoSafeHandle pNodeInfo = OpenNIImporter.xnGetNodeInfo(nodeHandle);
				type = OpenNIImporter.xnNodeInfoGetDescription(pNodeInfo).Type;
			}

			switch (type)
			{
				case NodeType.Audio:
					return new AudioGenerator(nodeHandle, true);
				case NodeType.Codec:
					return new Codec(nodeHandle, true);
				case NodeType.Depth:
					return new DepthGenerator(nodeHandle, true);
				case NodeType.Device:
					return new Device(nodeHandle, true);
				case NodeType.Gesture:
					return new GestureGenerator(nodeHandle, true);
				case NodeType.Hands:
					return new HandsGenerator(nodeHandle, true);
				case NodeType.Image:
					return new ImageGenerator(nodeHandle, true);
				case NodeType.IR:
					return new IRGenerator(nodeHandle, true);
				case NodeType.Player:
					return new Player(nodeHandle, true);
				case NodeType.Recorder:
					return new Recorder(nodeHandle, true);
				case NodeType.Scene:
					return new SceneAnalyzer(nodeHandle, true);
				case NodeType.User:
					return new UserGenerator(nodeHandle, true);
				default:
					throw new NotImplementedException("C# wrapper: Unknown generator type!");
			}
		}

		private static ProductionNode CreateProductionNodeObject(NodeSafeHandle nodeHandle)
		{
			return CreateProductionNodeObject(nodeHandle, null);
		}

		private void ErrorStateChangedCallback(UInt32 status, IntPtr cookie)
		{
			if (this.errorStateChanged != null)
			{
				if (status == 0)
				{
					this.errorStateChanged(null);
				}
				else
				{
					this.errorStateChanged(WrapperUtils.GetErrorMessage(status));
				}
			}
		}

		private IntPtr errorStateCallbackHandle;
		private event ErrorStateChangedHandler errorStateChanged;
	}
}