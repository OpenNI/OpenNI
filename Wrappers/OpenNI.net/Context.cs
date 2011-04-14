using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class ErrorStateEventArgs : EventArgs
	{
		public ErrorStateEventArgs(Int32 status)
		{
			if (status == 0)
			{
				this.currError = null;
			}
			else
			{
				this.currError = WrapperUtils.GetErrorMessage(status);
			}
		}
		
		public string CurrentError
		{
			get { return currError; }
			set { currError = value; }
		}

		private string currError;
	}

	public class Context : ObjectWrapper
	{
		private Context(IntPtr pContext) : 
			base(pContext)
		{
			this.errorStateChangedHandler = this.ErrorStateChangedCallback;

			lock (Context.staticLock)
			{
				if (Context.allContexts.ContainsKey(pContext))
				{
					throw new GeneralException("C# wrapper: creating a Context object wrapping an already wrapped object!");
				}
				Context.allContexts.Add(pContext, this);
			}
		}

		public Context() :
			this(Init())
		{
		}

		public Context(string xmlFile) :
			this(InitFromXml(xmlFile))
		{
		}

		/// <summary>
		/// Creates a managed Context object to wrap a native one.
		/// </summary>
		/// <param name="pContext">A pointer to the native object</param>
		/// <returns>A managed Context object</returns>
		static public Context FromNative(IntPtr pContext)
		{
			lock (Context.staticLock)
			{
				if (Context.allContexts.ContainsKey(pContext))
				{
					return Context.allContexts[pContext];
				}
				else
				{
					return new Context(pContext);
				}
			}
		}

		static public ProductionNode CreateProductionNodeFromNative(IntPtr nodeHandle)
		{
			IntPtr pContext = SafeNativeMethods.xnGetContextFromNodeHandle(nodeHandle);
			Context context = Context.FromNative(pContext);
			return context.CreateProductionNodeObject(nodeHandle);
		}

		public void RunXmlScript(string xml)
		{
			EnumerationErrors errors = new EnumerationErrors();
			int status = SafeNativeMethods.xnContextRunXmlScript(this.InternalObject, xml, errors.InternalObject);
			WrapperUtils.CheckEnumeration(status, errors);
		}

		public void RunXmlScriptFromFile(string xmlFile)
		{
			EnumerationErrors errors = new EnumerationErrors();
			int status = SafeNativeMethods.xnContextRunXmlScriptFromFile(this.InternalObject, xmlFile, errors.InternalObject);
			WrapperUtils.CheckEnumeration(status, errors);
		}

		public void OpenFileRecording(string fileName)
		{
			int status = SafeNativeMethods.xnContextOpenFileRecording(this.InternalObject, fileName);
			WrapperUtils.ThrowOnError(status);
		}

		public void Shutdown()
		{
			Dispose();
		}

		public void AddLicense(License license)
		{
			int status = SafeNativeMethods.xnAddLicense(this.InternalObject, license);
			WrapperUtils.ThrowOnError(status);
		}

		public License[] EnumerateLicenses()
		{
			IntPtr pArray;
			uint size;
			int status = SafeNativeMethods.xnEnumerateLicenses(this.InternalObject, out pArray, out size);
			WrapperUtils.ThrowOnError(status);

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
				SafeNativeMethods.xnFreeLicensesList(pArray);
			}

			return result;
		}

		public NodeInfoList EnumerateProductionTrees(NodeType type, Query query)
		{
			IntPtr resultList;

			using (EnumerationErrors errors = new EnumerationErrors())
			{
				int status = SafeNativeMethods.xnEnumerateProductionTrees(this.InternalObject, type,
					query == null ? IntPtr.Zero : query.InternalObject,
					out resultList,
					errors.InternalObject);
				WrapperUtils.CheckEnumeration(status, errors);
			}

			return new NodeInfoList(resultList);
		}

		public ProductionNode CreateAnyProductionTree(NodeType type, Query query)
		{
			IntPtr nodeHandle = CreateAnyProductionTreeImpl(type, query);
			return CreateProductionNodeObject(nodeHandle, type);
		}

		public ProductionNode CreateProductionTree(NodeInfo nodeInfo)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnCreateProductionTree(this.InternalObject, nodeInfo.InternalObject, out nodeHandle);
			WrapperUtils.ThrowOnError(status);
			return CreateProductionNodeObject(nodeHandle, nodeInfo.Description.Type);
		}

		public NodeInfoList EnumerateExistingNodes()
		{
			IntPtr pList;
			int status = SafeNativeMethods.xnEnumerateExistingNodes(this.InternalObject, out pList);
			WrapperUtils.ThrowOnError(status);
			return new NodeInfoList(pList);
		}

		public NodeInfoList EnumerateExistingNodes(NodeType type)
		{
			IntPtr pList;
			int status = SafeNativeMethods.xnEnumerateExistingNodesByType(this.InternalObject, type, out pList);
			WrapperUtils.ThrowOnError(status);
			return new NodeInfoList(pList);
		}

		public ProductionNode FindExistingNode(NodeType type)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnFindExistingNodeByType(this.InternalObject, type, out nodeHandle);
			WrapperUtils.ThrowOnError(status);
			return CreateProductionNodeObject(nodeHandle, type);
		}

		public ProductionNode GetProductionNodeByName(string name)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnGetNodeHandleByName(this.InternalObject, name, out nodeHandle);
			WrapperUtils.ThrowOnError(status);
			return CreateProductionNodeObject(nodeHandle);
		}

		public NodeInfo GetProductionNodeInfoByName(string name)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnGetNodeHandleByName(this.InternalObject, name, out nodeHandle);
			WrapperUtils.ThrowOnError(status);
			IntPtr nodeInfo = SafeNativeMethods.xnGetNodeInfo(nodeHandle);
			return new NodeInfo(nodeInfo);
		}

		public void StartGeneratingAll()
		{
			int status = SafeNativeMethods.xnStartGeneratingAll(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void StopGeneratingAll()
		{
			int status = SafeNativeMethods.xnStopGeneratingAll(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public bool GlobalMirror
		{
			get
			{
				return SafeNativeMethods.xnGetGlobalMirror(this.InternalObject);
			}
			set
			{
				int status = SafeNativeMethods.xnSetGlobalMirror(this.InternalObject, value);
				WrapperUtils.ThrowOnError(status);
			}
		}

		/// <summary>
		/// Gets the global error state.
		/// </summary>
		/// <returns>null if all is OK, a description of the error otherwise</returns>
		public string GlobalErrorState
		{
			get
			{
				int state = SafeNativeMethods.xnGetGlobalErrorState(this.InternalObject);
				if (state == 0)
				{
					return null;
				}
				else
				{
					return WrapperUtils.GetErrorMessage(state);
				}
			}
		}

		public event EventHandler<ErrorStateEventArgs> ErrorStateChanged
		{
			add
			{
				if (this.errorStateChanged == null)
				{
					int status = SafeNativeMethods.xnRegisterToGlobalErrorStateChange(this.InternalObject, this.errorStateChangedHandler, IntPtr.Zero, out this.errorStateCallbackHandle);
					WrapperUtils.ThrowOnError(status);
				}

				this.errorStateChanged += value;
			}
			remove
			{
				this.errorStateChanged -= value;

				if (this.errorStateChanged == null)
				{
					SafeNativeMethods.xnUnregisterFromGlobalErrorStateChange(this.InternalObject, this.errorStateCallbackHandle);
				}
			}
		}

		public void WaitAndUpdateAll()
		{
			int status = SafeNativeMethods.xnWaitAndUpdateAll(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void WaitAnyUpdateAll()
		{
			int status = SafeNativeMethods.xnWaitAnyUpdateAll(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void WaitOneUpdateAll(Generator node)
		{
			int status = SafeNativeMethods.xnWaitOneUpdateAll(this.InternalObject, node.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		public void WaitNoneUpdateAll()
		{
			int status = SafeNativeMethods.xnWaitNoneUpdateAll(this.InternalObject);
			WrapperUtils.ThrowOnError(status);
		}

		internal IntPtr CreateAnyProductionTreeImpl(NodeType type, Query query)
		{
			IntPtr nodeHandle;
			using (EnumerationErrors errors = new EnumerationErrors())
			{
				int status = SafeNativeMethods.xnCreateAnyProductionTree(this.InternalObject, type,
					query == null ? IntPtr.Zero : query.InternalObject,
					out nodeHandle, errors.InternalObject);
				WrapperUtils.CheckEnumeration(status, errors);
			}

			return nodeHandle;
		}

		protected override void FreeObject(IntPtr ptr, bool disposing)
		{
			if (disposing)
			{
				foreach (ProductionNode node in this.allNodes.Values)
				{
					node.MarkAlreadyFreed();
				}

				// remove it from the list
				lock (Context.allContexts)
				{
					Context.allContexts.Remove(ptr);
				}
			}

			SafeNativeMethods.xnShutdown(ptr);
		}

		private static IntPtr Init()
		{
			IntPtr pContext;
			int status = SafeNativeMethods.xnInit(out pContext);
			WrapperUtils.ThrowOnError(status);
			return pContext;
		}

		private static IntPtr InitFromXml(string xmlFile)
		{
			IntPtr pContext;
			EnumerationErrors errors = new EnumerationErrors();
			int status = SafeNativeMethods.xnInitFromXmlFile(xmlFile, out pContext, errors.InternalObject);
			WrapperUtils.CheckEnumeration(status, errors);
			return pContext;
		}

		private ProductionNode CreateProductionNodeObject(IntPtr nodeHandle, NodeType? type)
		{
			lock (this)
			{
				if (!this.allNodes.ContainsKey(nodeHandle))
				{
					if (type == null)
					{
						IntPtr pNodeInfo = SafeNativeMethods.xnGetNodeInfo(nodeHandle);
						type = SafeNativeMethods.xnNodeInfoGetDescription(pNodeInfo).Type;
					}

					ProductionNode node;

					switch (type)
					{
						case NodeType.Audio:
							node = new AudioGenerator(this, nodeHandle, true);
							break;
						case NodeType.Codec:
							node = new Codec(this, nodeHandle, true);
							break;
						case NodeType.Depth:
							node = new DepthGenerator(this, nodeHandle, true);
							break;
						case NodeType.Device:
							node = new Device(this, nodeHandle, true);
							break;
						case NodeType.Gesture:
							node = new GestureGenerator(this, nodeHandle, true);
							break;
						case NodeType.Hands:
							node = new HandsGenerator(this, nodeHandle, true);
							break;
						case NodeType.Image:
							node = new ImageGenerator(this, nodeHandle, true);
							break;
						case NodeType.IR:
							node = new IRGenerator(this, nodeHandle, true);
							break;
						case NodeType.Player:
							node = new Player(this, nodeHandle, true);
							break;
						case NodeType.Recorder:
							node = new Recorder(this, nodeHandle, true);
							break;
						case NodeType.Scene:
							node = new SceneAnalyzer(this, nodeHandle, true);
							break;
						case NodeType.User:
							node = new UserGenerator(this, nodeHandle, true);
							break;
						default:
							throw new NotImplementedException("C# wrapper: Unknown generator type!");
					}
					this.allNodes[nodeHandle] = node;
				}

				return this.allNodes[nodeHandle];
			} // lock
		}

		private ProductionNode CreateProductionNodeObject(IntPtr nodeHandle)
		{
			return CreateProductionNodeObject(nodeHandle, null);
		}

		private void ErrorStateChangedCallback(Int32 status, IntPtr cookie)
		{
			EventHandler<ErrorStateEventArgs> handlers = this.errorStateChanged;
			if (handlers != null)
			{
				ErrorStateEventArgs args = new ErrorStateEventArgs(status);
				handlers(this, args);
			}
		}

		private IntPtr errorStateCallbackHandle;
		private event EventHandler<ErrorStateEventArgs> errorStateChanged;
		private SafeNativeMethods.XnErrorStateChangedHandler errorStateChangedHandler;
		private Dictionary<IntPtr, ProductionNode> allNodes = new Dictionary<IntPtr, ProductionNode>();

		private static object staticLock = new object();
		private static Dictionary<IntPtr, Context> allContexts = new Dictionary<IntPtr, Context>();
	}
}