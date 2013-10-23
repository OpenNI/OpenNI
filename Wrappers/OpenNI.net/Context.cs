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

    public class NodeCreatedEventArgs : EventArgs
    {
        public NodeCreatedEventArgs(ProductionNode createdNode)
        {
            this.createdNode = createdNode;
        }

        public ProductionNode CreatedNode
        {
            get { return createdNode; }
            set { createdNode = value; }
        }

        private ProductionNode createdNode;
    }

    public class NodeDestroyedEventArgs : EventArgs
    {
        public NodeDestroyedEventArgs(string destroyedNodeName)
        {
            this.destroyedNodeName = destroyedNodeName;
        }

        public string DestroyedNodeName
        {
            get { return destroyedNodeName; }
            set { destroyedNodeName = value; }
        }

        private string destroyedNodeName;
    }

	public class Context : ObjectWrapper
	{
		private Context(IntPtr pContext, bool addRef) : 
			base(pContext)
		{
			this.errorStateChangedHandler = this.ErrorStateChangedCallback;
            this.nodeCreationHandler = this.NodeCreationCallback;
            this.nodeDestructionHandler = this.NodeDestructionCallback;

			lock (Context.staticLock)
			{
				if (Context.allContexts.ContainsKey(pContext))
				{
					throw new GeneralException("C# wrapper: creating a Context object wrapping an already wrapped object!");
				}
				Context.allContexts.Add(pContext, this);
			}

			if (addRef)
			{
				WrapperUtils.ThrowOnError(SafeNativeMethods.xnContextAddRef(pContext));
			}

			this.shutdownHandler = OnContextShuttingDown;

			WrapperUtils.ThrowOnError(SafeNativeMethods.xnContextRegisterForShutdown(pContext, this.shutdownHandler, IntPtr.Zero, out this.shutdownCallbackHandle));
		}

		private Context(IntPtr pContext) : this(pContext, true) { }

		public Context() :
			this(Init(), false)
		{
		}

		[Obsolete("use Context.CreateFromXmlFile() instead")]
		public Context(string xmlFile) :
			this(InitFromXml(xmlFile), false)
		{
			this.usingDeprecatedAPI = true;
		}

		public static Context CreateFromXmlFile(string xmlFile, out ScriptNode scriptNode)
		{
			IntPtr hScriptNode;
			IntPtr pContext = InitFromXmlEx(xmlFile, out hScriptNode);
			Context context = new Context(pContext, false);
			scriptNode = new ScriptNode(context, hScriptNode, false);
			return context;
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
			IntPtr pContext = SafeNativeMethods.xnGetRefContextFromNodeHandle(nodeHandle);
			Context context = Context.FromNative(pContext);
			SafeNativeMethods.xnContextRelease(pContext);
			return context.CreateProductionNodeObject(nodeHandle);
		}

		[Obsolete("Use RunXmlScriptEx() instead")]
		public void RunXmlScript(string xml)
		{
			this.usingDeprecatedAPI = true;
			EnumerationErrors errors = new EnumerationErrors();
			int status = SafeNativeMethods.xnContextRunXmlScript(this.InternalObject, xml, errors.InternalObject);
			WrapperUtils.CheckEnumeration(status, errors);
		}

		public ScriptNode RunXmlScriptEx(string xml)
		{
			EnumerationErrors errors = new EnumerationErrors();
			IntPtr hScriptNode;
			int status = SafeNativeMethods.xnContextRunXmlScriptEx(this.InternalObject, xml, errors.InternalObject, out hScriptNode);
			WrapperUtils.CheckEnumeration(status, errors);
			return new ScriptNode(this, hScriptNode, false);
		}

		[Obsolete("Use RunXmlScriptFromFileEx() instead")]
		public void RunXmlScriptFromFile(string xmlFile)
		{
			this.usingDeprecatedAPI = true;
			EnumerationErrors errors = new EnumerationErrors();
			int status = SafeNativeMethods.xnContextRunXmlScriptFromFile(this.InternalObject, xmlFile, errors.InternalObject);
			WrapperUtils.CheckEnumeration(status, errors);
		}

		public ProductionNode RunXmlScriptFromFileEx(string xmlFile)
		{
			EnumerationErrors errors = new EnumerationErrors();
			IntPtr hScriptNode;
			int status = SafeNativeMethods.xnContextRunXmlScriptFromFileEx(this.InternalObject, xmlFile, errors.InternalObject, out hScriptNode);
			WrapperUtils.CheckEnumeration(status, errors);
			return new ScriptNode(this, hScriptNode, false);
		}

		[Obsolete("Use OpenFileRecordingEx() instead")]
		public void OpenFileRecording(string fileName)
		{
			this.usingDeprecatedAPI = true;
			int status = SafeNativeMethods.xnContextOpenFileRecording(this.InternalObject, fileName);
			WrapperUtils.ThrowOnError(status);
		}

		public ProductionNode OpenFileRecordingEx(string fileName)
		{
			IntPtr hPlayer;
            int status = SafeNativeMethods.xnContextOpenFileRecordingEx(this.InternalObject, fileName, out hPlayer);
			WrapperUtils.ThrowOnError(status);
            return new Player(this, hPlayer, false);
		}

		[Obsolete("Do not use this function. You may use Release() instead, or count on GC.")]
		public void Shutdown()
		{
            // take the original context pointer (it will soon be erased)
            IntPtr pContext = this.InternalObject;
            // shutdown (this will destroy the native context, and cause the shutdown event to be raised, 
            // and our OnContextShuttingDown callback will be called)
			SafeNativeMethods.xnShutdown(this.InternalObject);
            // remove it from the static list
            RemoveContext(pContext);

            // (no need to dispose. Already done as part of OnContextShuttingDown callback)
		}

		public void Release()
		{
			Dispose();
		}

		public void AddLicense(License license)
		{
            IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf(license));
            try
            {
                int status = SafeNativeMethods.xnAddLicense(this.InternalObject, ptr);
                WrapperUtils.ThrowOnError(status);
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }
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

        public NodeInfoList EnumerateProductionTrees(NodeType type)
        {
            return EnumerateProductionTrees(type, null);
        }

		public ProductionNode CreateAnyProductionTree(NodeType type, Query query)
		{
			IntPtr nodeHandle = CreateAnyProductionTreeImpl(type, query);
			ProductionNode result = CreateProductionNodeObject(nodeHandle, type);
			// release the handle (result now owns it)
			SafeNativeMethods.xnProductionNodeRelease(nodeHandle);
			return result;
		}

		public ProductionNode CreateProductionTree(NodeInfo nodeInfo)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnCreateProductionTree(this.InternalObject, nodeInfo.InternalObject, out nodeHandle);
			WrapperUtils.ThrowOnError(status);
			ProductionNode node = CreateProductionNodeObject(nodeHandle, nodeInfo.Description.Type);
            SafeNativeMethods.xnProductionNodeRelease(nodeHandle);
            return node;
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
			int status = SafeNativeMethods.xnFindExistingRefNodeByType(this.InternalObject, type, out nodeHandle);
            if (status != 0)
            {
                return null;
            }

			ProductionNode node = CreateProductionNodeObject(nodeHandle, type);

			// release the handle
			SafeNativeMethods.xnProductionNodeRelease(nodeHandle);

			return node;
		}

		public ProductionNode GetProductionNodeByName(string name)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnGetRefNodeHandleByName(this.InternalObject, name, out nodeHandle);
			WrapperUtils.ThrowOnError(status);

			ProductionNode node = CreateProductionNodeObject(nodeHandle);

			// release the handle
			SafeNativeMethods.xnProductionNodeRelease(nodeHandle);

			return node;
		}

		public NodeInfo GetProductionNodeInfoByName(string name)
		{
			IntPtr nodeHandle;
			int status = SafeNativeMethods.xnGetRefNodeHandleByName(this.InternalObject, name, out nodeHandle);
			WrapperUtils.ThrowOnError(status);
			IntPtr nodeInfo = SafeNativeMethods.xnGetNodeInfo(nodeHandle);
			SafeNativeMethods.xnProductionNodeRelease(nodeHandle);
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

        public event EventHandler<NodeCreatedEventArgs> NodeCreated
        {
            add
            {
                if (this.nodeCreated == null)
                {
                    int status = SafeNativeMethods.xnRegisterToNodeCreation(this.InternalObject, this.nodeCreationHandler, IntPtr.Zero, out this.nodeCreationCallbackHandle);
                    WrapperUtils.ThrowOnError(status);
                }

                this.nodeCreated += value;
            }
            remove
            {
                this.nodeCreated -= value;

                if (this.nodeCreated == null)
                {
                    SafeNativeMethods.xnUnregisterFromNodeCreation(this.InternalObject, this.nodeCreationCallbackHandle);
                }
            }
        }

        public event EventHandler<NodeDestroyedEventArgs> NodeDestroyed
        {
            add
            {
                if (this.nodeDestroyed == null)
                {
                    int status = SafeNativeMethods.xnRegisterToNodeDestruction(this.InternalObject, this.nodeDestructionHandler, IntPtr.Zero, out this.nodeDestructionCallbackHandle);
                    WrapperUtils.ThrowOnError(status);
                }

                this.nodeDestroyed += value;
            }
            remove
            {
                this.nodeDestroyed -= value;

                if (this.nodeDestroyed == null)
                {
                    SafeNativeMethods.xnUnregisterFromNodeDestruction(this.InternalObject, this.nodeDestructionCallbackHandle);
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
				// remove it from the list
                RemoveContext(ptr);
			}

			if (this.usingDeprecatedAPI)
			{
				SafeNativeMethods.xnForceShutdown(ptr);
			}
			else
			{
				SafeNativeMethods.xnContextUnregisterFromShutdown(ptr, this.shutdownCallbackHandle);
				SafeNativeMethods.xnContextRelease(ptr);
			}
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
			#pragma warning disable 612
			int status = SafeNativeMethods.xnInitFromXmlFile(xmlFile, out pContext, errors.InternalObject);
			#pragma warning restore 612
			WrapperUtils.CheckEnumeration(status, errors);
			return pContext;
		}

		private static IntPtr InitFromXmlEx(string xmlFile, out IntPtr hScriptNode)
		{
			IntPtr pContext;
			EnumerationErrors errors = new EnumerationErrors();
			int status = SafeNativeMethods.xnInitFromXmlFileEx(xmlFile, out pContext, errors.InternalObject, out hScriptNode);
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
                        type = NodeInfo.FromNative(pNodeInfo).Description.Type;
					}

					ProductionNode node;

                   	// start with concrete types
                    if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.Device))
                    {
                        node = new Device(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.Depth))
                    {
                        node = new DepthGenerator(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.Image))
                    {
                        node = new ImageGenerator(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.Audio))
                    {
                        node = new AudioGenerator(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.IR))
                    {
                        node = new IRGenerator(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.User))
                    {
                        node = new UserGenerator(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.Recorder))
                    {
                        node = new Recorder(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.Player))
                    {
                        node = new Player(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.Gesture))
                    {
                        node = new GestureGenerator(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.Scene))
                    {
                        node = new SceneAnalyzer(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.Hands))
                    {
                        node = new HandsGenerator(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.Codec))
                    {
                        node = new Codec(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.ScriptNode))
                    {
                        node = new ScriptNode(this, nodeHandle, true);
                    }
                    // move on to abstract types
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.MapGenerator))
                    {
                        node = new MapGenerator(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.Generator))
                    {
                        node = new Generator(this, nodeHandle, true);
                    }
                    else if (SafeNativeMethods.xnIsTypeDerivedFrom(type.Value, NodeType.ProductionNode))
                    {
                        node = new ProductionNode(this, nodeHandle, true);
                    }
                    else
                    {
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

        private void NodeCreationCallback(IntPtr pContext, IntPtr hCreatedNode, IntPtr pCookie)
        {
            EventHandler<NodeCreatedEventArgs> handlers = this.nodeCreated;
            if (handlers != null)
            {
                ProductionNode createdNode = Context.CreateProductionNodeFromNative(hCreatedNode);
                NodeCreatedEventArgs args = new NodeCreatedEventArgs(createdNode);
                handlers(this, args);
            }
        }

        private void NodeDestructionCallback(IntPtr pContext, string strDestroyedNodeName, IntPtr pCookie)
        {
            EventHandler<NodeDestroyedEventArgs> handlers = this.nodeDestroyed;
            if (handlers != null)
            {
                NodeDestroyedEventArgs args = new NodeDestroyedEventArgs(strDestroyedNodeName);
                handlers(this, args);
            }
        }

        private void OnContextShuttingDown(IntPtr pContext, IntPtr pCookie)
		{
			// context is shutting down. This object is no longer valid
			// no need to unregister from event, the event is destroyed anyway
			UnsafeReplaceInternalObject(IntPtr.Zero);

			Dispose();
		}

        private static void RemoveContext(IntPtr pContext)
        {
            lock (Context.allContexts)
            {
                Context.allContexts.Remove(pContext);
            }
        }

		private bool usingDeprecatedAPI;

        private IntPtr errorStateCallbackHandle;
		private event EventHandler<ErrorStateEventArgs> errorStateChanged;
		private SafeNativeMethods.XnErrorStateChangedHandler errorStateChangedHandler;

        private IntPtr nodeCreationCallbackHandle;
        private event EventHandler<NodeCreatedEventArgs> nodeCreated;
        private SafeNativeMethods.XnNodeCreationHandler nodeCreationHandler;

        private IntPtr nodeDestructionCallbackHandle;
        private event EventHandler<NodeDestroyedEventArgs> nodeDestroyed;
        private SafeNativeMethods.XnNodeDestructionHandler nodeDestructionHandler;

        private IntPtr shutdownCallbackHandle;
		private SafeNativeMethods.XnContextShuttingDownHandler shutdownHandler;
		private Dictionary<IntPtr, ProductionNode> allNodes = new Dictionary<IntPtr, ProductionNode>();

		private static object staticLock = new object();
		private static Dictionary<IntPtr, Context> allContexts = new Dictionary<IntPtr, Context>();
	}
}