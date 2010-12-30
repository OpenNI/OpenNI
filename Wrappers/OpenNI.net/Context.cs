using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public delegate void ErrorStateChangedHandler(Status currError);

    /// <summary>
    /// Represents an OpenNI context object.
    /// </summary>
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

        /// <summary>
        /// Runs an XML script in the this Context.
        /// </summary>
        /// <param name="xml">A string representation of the XML script.</param>
        /// <exception cref="GeneralException"></exception>
        /// <exception cref="XnStatusException"></exception>
		public void RunXmlScript(string xml)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(xml));

			EnumerationErrors errors = new EnumerationErrors();
            var status = OpenNIImporter.xnContextRunXmlScript(this.InternalObject, xml, errors.InternalObject);
            Status.ThrowOnFail(status, errors);
		}

        /// <summary>
        /// Runs an XML script in the this Context.
        /// </summary>
        /// <param name="xmlFile">The file to open.</param>
		public void RunXmlScriptFromFile(string xmlFile)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(xmlFile));
            Contract.Requires(System.IO.File.Exists(xmlFile));
            
            EnumerationErrors errors = new EnumerationErrors();
            var status = OpenNIImporter.xnContextRunXmlScriptFromFile(this.InternalObject, xmlFile, errors.InternalObject);
            Status.ThrowOnFail(status, errors);
		}

        /// <summary>
        /// Opens a recording file, adding all nodes in it to the context. 
        /// </summary>
        /// <param name="fileName">The file to open.</param>
		public void OpenFileRecording(string fileName)
		{
            Contract.Requires(!string.IsNullOrWhiteSpace(fileName));

            Status.ThrowOnFail(OpenNIImporter.xnContextOpenFileRecording(this.InternalObject, fileName));
			
		}

		public void AddLicense(License license)
		{
            Contract.Requires(license != null);

            Status.ThrowOnFail(OpenNIImporter.xnAddLicense(this.InternalObject, license));
			
		}

		public License[] EnumerateLicenses()
		{
			IntPtr pArray;
			uint size;
            Status.ThrowOnFail(OpenNIImporter.xnEnumerateLicenses(this.InternalObject, out pArray, out size));
			

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
				var status = OpenNIImporter.xnEnumerateProductionTrees(this.InternalObject, type,
					query == null ? QuerySafeHandle.Zero : query.InternalObject,
					out resultList,
                    errors.InternalObject);
                Status.ThrowOnFail(status, errors);
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
            Status.ThrowOnFail(OpenNIImporter.xnCreateProductionTree(this.InternalObject, nodeInfo.InternalObject, out nodeHandle));
			
			return CreateProductionNodeObject(nodeHandle, nodeInfo.Description.Type);
		}

		public NodeInfoList EnumerateExistingNodes()
		{
            Contract.Ensures(Contract.Result<NodeInfoList>() != null);

            NodeInfoListSafeHandle pList;
            Status.ThrowOnFail(OpenNIImporter.xnEnumerateExistingNodes(this.InternalObject, out pList));
			
			return new NodeInfoList(pList);
		}

		public NodeInfoList EnumerateExistingNodes(NodeType type)
		{
            Contract.Ensures(Contract.Result<NodeInfoList>() != null);

            NodeInfoListSafeHandle pList;
            Status.ThrowOnFail(OpenNIImporter.xnEnumerateExistingNodesByType(this.InternalObject, type, out pList));
			
			return new NodeInfoList(pList);
		}

		public ProductionNode FindExistingNode(NodeType type)
		{
            Contract.Ensures(Contract.Result<ProductionNode>() != null);

            NodeSafeHandle nodeHandle;
            Status.ThrowOnFail(OpenNIImporter.xnFindExistingNodeByType(this.InternalObject, type, out nodeHandle));
			
			return CreateProductionNodeObject(nodeHandle, type);
		}

		public ProductionNode GetProductionNodeByName(string name)
		{
            Contract.Ensures(Contract.Result<ProductionNode>() != null);

            NodeSafeHandle nodeHandle;
            Status.ThrowOnFail(OpenNIImporter.xnGetNodeHandleByName(this.InternalObject, name, out nodeHandle));
			
			return CreateProductionNodeObject(nodeHandle);
		}

		public NodeInfo GetProductionNodeInfoByName(string name)
		{
            Contract.Requires(!String.IsNullOrEmpty(name));
            Contract.Ensures(Contract.Result<NodeInfo>() != null);

            NodeSafeHandle nodeHandle;
            Status.ThrowOnFail(OpenNIImporter.xnGetNodeHandleByName(this.InternalObject, name, out nodeHandle));
			
			NodeInfoSafeHandle nodeInfo = OpenNIImporter.xnGetNodeInfo(nodeHandle);
			return new NodeInfo(nodeInfo);
		}

		public void StartGeneratingAll()
		{
            Status.ThrowOnFail(OpenNIImporter.xnStartGeneratingAll(this.InternalObject));
			
		}

		public void StopGeneratingAll()
		{
            Status.ThrowOnFail(OpenNIImporter.xnStopGeneratingAll(this.InternalObject));
			
		}

		public bool GlobalMirror
        {
            get
            {
                return OpenNIImporter.xnGetGlobalMirror(this.InternalObject);
            }
            set
            {
                Status.ThrowOnFail(OpenNIImporter.xnSetGlobalMirror(this.InternalObject, value));
			    
            }
		}

		/// <summary>
		/// Gets the global error state.
		/// </summary>
		/// <returns>null if all is OK, a description of the error otherwise</returns>
		public Status GlobalErrorState
		{
            get
            {
                return OpenNIImporter.xnGetGlobalErrorState(this.InternalObject);
            }
		}

        public event EventHandler<ErrorStateChangedArgs> ErrorStateChanged
		{
			add
			{
				if (this.errorStateChanged == null)
				{
                    Status.ThrowOnFail(OpenNIImporter.xnRegisterToGlobalErrorStateChange(this.InternalObject, ErrorStateChangedCallback, IntPtr.Zero, out this.errorStateCallbackHandle));
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
            Status.ThrowOnFail(OpenNIImporter.xnWaitAndUpdateAll(this.InternalObject));
			
		}

		public void WaitAnyUpdateAll()
		{
            Status.ThrowOnFail(OpenNIImporter.xnWaitAnyUpdateAll(this.InternalObject));
			
		}

		public void WaitOneUpdateAll(Generator node)
		{
            Status.ThrowOnFail(OpenNIImporter.xnWaitOneUpdateAll(this.InternalObject, node.InternalObject));
			
		}

		public void WaitNoneUpdateAll()
		{
            Status.ThrowOnFail(OpenNIImporter.xnWaitNoneUpdateAll(this.InternalObject));
			
		}

        internal NodeSafeHandle CreateAnyProductionTreeImpl(NodeType type, Query query)
		{
            NodeSafeHandle nodeHandle;
			using (EnumerationErrors errors = new EnumerationErrors())
			{
				var status = (OpenNIImporter.xnCreateAnyProductionTree(this.InternalObject, type,
					query == null ? QuerySafeHandle.Zero : query.InternalObject,
                    out nodeHandle, errors.InternalObject));
                Status.ThrowOnFail(status, errors);
			}

			return nodeHandle;
		}

        public static Context Init()
		{
			ContextSafeHandle pContext;
            Status.ThrowOnFail(OpenNIImporter.xnInit(out pContext));
			
			return new Context(pContext);
		}

        public static Context InitFromXml(string xmlFile)
		{
            ContextSafeHandle pContext;
            EnumerationErrors errors = new EnumerationErrors();
            var status = OpenNIImporter.xnInitFromXmlFile(xmlFile, out pContext, errors.InternalObject);
            Status.ThrowOnFail(status, errors);
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

		private void ErrorStateChangedCallback(Status status, IntPtr cookie)
		{
            var handler = this.errorStateChanged;
            if (handler != null)
			{
                handler(this, new ErrorStateChangedArgs(status, cookie));
			}
		}

		private IntPtr errorStateCallbackHandle;
        private event EventHandler<ErrorStateChangedArgs> errorStateChanged;
	}

    /// <summary>
    /// Provides data for state change event.
    /// </summary>
    public class ErrorStateChangedArgs
        : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the StateChangedArgs class.
        /// </summary>
        /// <param name="cookie">The object that contains data about the Capability.</param>
        public ErrorStateChangedArgs(Status status, IntPtr cookie)
        {
            this.Status = status;
            this.Cookie = cookie;
        }

        /// <summary>
        /// Gets the status.
        /// </summary>
        public Status Status { get; private set; }

        /// <summary>
        /// Gets the object that contains data about the Capability.
        /// </summary>
        public IntPtr Cookie { get; private set; }
    }

}