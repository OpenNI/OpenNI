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
package org.openni;

import java.util.Hashtable;

/**
 * Stores state and settings for OpenNI SDK. <BR><BR>
 * 
 * This class is most commonly used to initialize settings from an XML file. <BR><BR>
 * 
 * Once initialized, this class is also used for determining which production nodes are available, 
 * for creating production nodes, and for updating the data in production nodes.<BR><BR>
 * 
 * Defines the following events:
 * errorStateChangedEvent: Triggered when the error state changes
 * nodeCreatedEvent: Triggered when a new node is created
 * nodeDestroyedEvent: Triggered when a node is deleted
 *
 */
public class Context extends ObjectWrapper
{
	/**
	 * Default constructor
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public Context() throws GeneralException
	{
		this(init(), false);
	}
	
	/**
	 * Creates a new OpenNI context using the settings in an XML file
	 * @param xmlFile Path to the XML file used to create context
	 * @param scriptNode
	 * @return Context created
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public static Context createFromXmlFile(String xmlFile, OutArg<ScriptNode> scriptNode) throws GeneralException
	{
		OutArg<Long> hScript = new OutArg<Long>();
		long pContext = Context.initFromXmlEx(xmlFile, hScript);
		Context context = new Context(pContext, false);
		scriptNode.value = new ScriptNode(context, hScript.value, false);
		return context;
	}
	
	/**
	 * Creates a new OpenNI context from an existing OpenNI context
	 * @param pContext pointer to a context to be copied
	 * @return New context created by this function
	 * @throws GeneralException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public static Context fromNative(long pContext) throws GeneralException
	{
		synchronized (Context.allContexts)
		{
			if (Context.allContexts.containsKey(pContext))
			{
				return Context.allContexts.get(pContext);
			}
			else
			{
				return new Context(pContext);
			}
		}
	}
	
	/**
	 * Checks the OpenNI version of this context
	 * @return Version 
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public static Version getVersion() throws StatusException
	{
		OutArg<Version> pVersion = new OutArg<Version>();
		int status = NativeMethods.xnGetVersion(pVersion);
		WrapperUtils.throwOnError(status);
		return pVersion.value;
	}
	
	/**
	 * Creates a new ScriptNode from an XML script file
	 * @param scriptFile Path to the XML script
	 * @return Script Node created
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public ScriptNode runXmlScriptFromFile(String scriptFile) throws GeneralException
	{
		EnumerationErrors errors = new EnumerationErrors();
		OutArg<Long> phScriptNode = new OutArg<Long>();
		int status = NativeMethods.xnContextRunXmlScriptFromFileEx(toNative(), scriptFile, errors.toNative(), phScriptNode);
		WrapperUtils.checkEnumeration(status, errors);
		return new ScriptNode(this, phScriptNode.value, false);
	}

	/**
	 * Creates a new ScriptNode from an XML script stored in a string
	 * @param script String storing the XML script
	 * @return Script node created
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public ScriptNode runXmlScript(String script) throws GeneralException
	{
		EnumerationErrors errors = new EnumerationErrors();
		OutArg<Long> phScriptNode = new OutArg<Long>();
		int status = NativeMethods.xnContextRunXmlScriptEx(toNative(), script, errors.toNative(), phScriptNode);
		WrapperUtils.checkEnumeration(status, errors);
		return new ScriptNode(this, phScriptNode.value, false);
	}

	/**
	 * Opens a recording stored as a file
	 * @param fileName Path to the recording
	 * @return Player object created from the recording
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public Player openFileRecordingEx(String fileName) throws GeneralException
	{
		OutArg<Long> phPlayer = new OutArg<Long>();
		int status = NativeMethods.xnContextOpenFileRecordingEx(toNative(), fileName, phPlayer);
		WrapperUtils.throwOnError(status);
		return (Player)createProductionNodeFromNative(phPlayer.value);
	}
	
	/**
	 * This is an alternate way to call the dispose() function 
	 */
	public void release()
	{
		dispose();
	}
	
	/**
	 * Adds a vendor/key pair from a license object
	 * @param license License object to pull vendor/key pair from
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void addLicense(License license) throws StatusException
	{
		int status = NativeMethods.xnAddLicense(toNative(), license.getVendor(), license.getKey());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Returns a list of all licenses in this context
	 * @return List of licenses
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public License[] enumerateLicenses() throws StatusException
	{
		OutArg<License[]> licenses = new OutArg<License[]>();
		int status = NativeMethods.xnEnumerateLicenses(toNative(), licenses);
		WrapperUtils.throwOnError(status);
		return licenses.value;
	}
	
	/**
	 * Creates a list of the production trees available in this context, filtered by a query object
	 * @param type Type of node to search for
	 * @param query A query object that can be used to filter results
	 * @return NodeInfoList containing the list of production trees containing the node type specified
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public NodeInfoList enumerateProductionTrees(NodeType type, Query query) throws GeneralException
	{
		EnumerationErrors errors = new EnumerationErrors();
		OutArg<Long> ppNodesList = new OutArg<Long>();
		int status = NativeMethods.xnEnumerateProductionTrees(toNative(), type.toNative(),
				query == null ? 0 : query.toNative(),
				ppNodesList,
				errors.toNative());
		WrapperUtils.checkEnumeration(status, errors);
		return new NodeInfoList(ppNodesList.value);
	}

	/**
	 * Creates a list of production trees available in this context with specified type, not filtered by a query object
	 * @param type Type of node to search for
	 * @return NodeInfoList containing the list of production trees containing the node type specified
	 * @throws GeneralException
	 */
	public NodeInfoList enumerateProductionTrees(NodeType type) throws GeneralException
	{
		return enumerateProductionTrees(type, null);
	}
	
	/**
	 * Creates a list of production trees available for a node type specified in a nodeInfo object
	 * @param nodeInfo nodeInfo object specifying production node type to create chain for
	 * @return Production node created by this process
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public ProductionNode createProductionTree(NodeInfo nodeInfo) throws GeneralException
	{
		OutArg<Long> phNode = new OutArg<Long>();
		int status = NativeMethods.xnCreateProductionTree(toNative(), nodeInfo.toNative(), phNode);
		WrapperUtils.throwOnError(status);
		return createProductionNodeObject(phNode.value);
	}
	
	/**
	 * Creates first possible production tree of given type, filtered by a query
	 * @param type Type of production node desired
	 * @param query Query object to filter results
	 * @return ProductionNode created by this command
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public ProductionNode createAnyProductionTree(NodeType type, Query query) throws GeneralException
	{
		EnumerationErrors errors = new EnumerationErrors();
		OutArg<Long> phNode = new OutArg<Long>();
		int status = NativeMethods.xnCreateAnyProductionTree(toNative(), type.toNative(),
				query == null ? 0 : query.toNative(),
				phNode,
				errors.toNative());
		WrapperUtils.checkEnumeration(status, errors);
		return createProductionNodeFromNative(phNode.value);
	}

	/**
	 * Creates a list of all nodes available in this context
	 * @return NodeInfoList containing list of all nodes in the context
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public NodeInfoList enumerateExistingNodes() throws GeneralException
	{
		OutArg<Long> ppList = new OutArg<Long>();
		int status = NativeMethods.xnEnumerateExistingNodes(this.toNative(), ppList);
		WrapperUtils.throwOnError(status);
		return new NodeInfoList(ppList.value);
	}

	/**
	 * Returns a list of all nodes in this context of a specified type
	 * @param type Type of node desired
	 * @return List of nodes of specified type
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public NodeInfoList enumerateExistingNodes(NodeType type) throws GeneralException
	{
		OutArg<Long> ppList = new OutArg<Long>();
		int status = NativeMethods.xnEnumerateExistingNodesByType(this.toNative(), type.toNative(), ppList);
		WrapperUtils.throwOnError(status);
		return new NodeInfoList(ppList.value);
	}

	/**
	 * Returns first node found of specified type
	 * @param type Type of node to find
	 * @return ProductionNode object wrapping the node found
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public ProductionNode findExistingNode(NodeType type) throws GeneralException
	{
		OutArg<Long> phNode = new OutArg<Long>();
		int status = NativeMethods.xnFindExistingRefNodeByType(this.toNative(), type.toNative(), phNode);
		WrapperUtils.throwOnError(status);
		ProductionNode node = createProductionNodeObject(phNode.value, type);
		// release the handle
		NativeMethods.xnProductionNodeRelease(phNode.value);

		return node;
	}

	/**
	 * Finds a production node by string type, and wraps it in a ProductionNode object
	 * @param name String name of the node to wrap
	 * @return New ProductionNode object wrapping the node found
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public ProductionNode getProductionNodeByName(String name) throws GeneralException
	{
		OutArg<Long> phNode = new OutArg<Long>();
		int status = NativeMethods.xnGetRefNodeHandleByName(this.toNative(), name, phNode);
		WrapperUtils.throwOnError(status);
		ProductionNode node = createProductionNodeObject(phNode.value);
		// release the handle
		NativeMethods.xnProductionNodeRelease(phNode.value);

		return node;
	}

	/**
	 * Finds a production node with the given name value, and obtains the nodeInfo from it
	 * @param name String name of the production node to be returned
	 * @return NodeInfo associated with the production node searched for 
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public NodeInfo getProductionNodeInfoByName(String name) throws GeneralException
	{
		return getProductionNodeByName(name).getInfo();
	}

	/**
	 * Tells all production nodes to start generating data
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void startGeneratingAll() throws StatusException
	{
		int status = NativeMethods.xnStartGeneratingAll(this.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Tells all production nodes to stop generating data
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void stopGeneratingAll() throws StatusException
	{
		int status = NativeMethods.xnStopGeneratingAll(this.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Provides the current value of the global mirror flag
	 * @return Current value of the global mirror flag
	 */
	public boolean getGlobalMirror()
	{
		return NativeMethods.xnGetGlobalMirror(toNative());
	}
	
	/**
	 * Sets a new value for the global mirror flag
	 * @param mirror Desired new value of the global mirror flag
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void setGlobalMirror(boolean mirror) throws StatusException
	{
		int status = NativeMethods.xnSetGlobalMirror(toNative(), mirror);
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Go to sleep until all nodes have new data
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void waitAndUpdateAll() throws StatusException
	{
		int status = NativeMethods.xnWaitAndUpdateAll(this.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Go to sleep until one specific node has new data, and then update the data from all nodes
	 * @param waitFor The node that triggers the update
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void waitOneUpdateAll(ProductionNode waitFor) throws StatusException
	{
		int status = NativeMethods.xnWaitOneUpdateAll(toNative(), waitFor.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Go to sleep until any node has new data, and then update all nodes
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void waitAnyUpdateAll() throws StatusException
	{
		int status = NativeMethods.xnWaitAnyUpdateAll(this.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Updates without waiting for any nodes to have new data
	 * @throws StatusException If underlying native code returns errors, Status Exception is thrown by this function
	 */
	public void waitNoneUpdateAll() throws StatusException
	{
		int status = NativeMethods.xnWaitNoneUpdateAll(this.toNative());
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Provides access to the Error State Changed event.
	 * @return
	 */
	public IObservable<ErrorStateEventArgs> getErrorStateChangedEvent()
	{
		return errorStateChangedEvent;
	}
	
	/**
	 * Provides access to the Node Created event
	 * @return
	 */
	public IObservable<NodeCreatedEventArgs> getNodeCreatedEvent()
	{
		return nodeCreatedEvent;
	}
	
	/**
	 * Provides access to the Node Destroyed event
	 * @return
	 */
	public IObservable<NodeDestroyedEventArgs> getNodeDestroyedEvent()
	{
		return nodeDestroyedEvent;
	}
	
	/**
	 * Creates a new ProductionNode object, and wraps it around a node specified by nodeHandle
	 * @param nodeHandle Handle of the node to be wrapped
	 * @return New ProductionNode object wrapping the specified node
	 * @throws GeneralException If underlying native code returns errors, General Exception is thrown by this function
	 */
	public static ProductionNode createProductionNodeFromNative(long nodeHandle) throws GeneralException
	{
		long pContext = NativeMethods.xnGetRefContextFromNodeHandle(nodeHandle);
		Context context = Context.fromNative(pContext);
		NativeMethods.xnContextRelease(pContext);
		return context.createProductionNodeObject(nodeHandle);
	}

	/**
	 * Frees the memory used by an object at the given pointer
	 * @param ptr Pointer to the object to be freed
	 */
	protected void freeObject(long ptr)
	{
		synchronized (Context.allContexts) 
		{
			Context.allContexts.remove(ptr);
		}
		NativeMethods.xnContextRelease(ptr);
	}
	
	private Context(long pContext, boolean addRef) throws GeneralException
	{
		super(pContext);
		
		errorStateChangedEvent = new Observable<ErrorStateEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterToGlobalErrorStateChange(toNative(), this, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromGlobalErrorStateChange(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(int status)
			{
				notify(new ErrorStateEventArgs(status));
			}
		};
		
		nodeCreatedEvent = new Observable<NodeCreatedEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterToNodeCreation(toNative(), this, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromNodeCreation(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(long hCreatedNode) throws GeneralException
			{
				ProductionNode createdNode = createProductionNodeObject(hCreatedNode);
				notify(new NodeCreatedEventArgs(createdNode));
			}
		};
		
		nodeDestroyedEvent = new Observable<NodeDestroyedEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterToNodeDestruction(toNative(), this, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromNodeDestruction(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(String destroyedNodeName) throws GeneralException
			{
				notify(new NodeDestroyedEventArgs(destroyedNodeName));
			}
		};
		
		synchronized (Context.allContexts) 
		{
			if (Context.allContexts.containsKey(pContext))
			{
				throw new GeneralException("Java wrapper: creating a Context object wrapping an already wrapped object!");
			}
			Context.allContexts.put(pContext, this);
		}
		
		if (addRef)
		{
			WrapperUtils.throwOnError(NativeMethods.xnContextAddRef(pContext));
		}
	}
	
	private Context(long pContext) throws GeneralException
	{
		this(pContext, true);
	}

	private static long init() throws StatusException
	{
		OutArg<Long> pContext = new OutArg<Long>();
		int status = NativeMethods.xnInit(pContext);
		WrapperUtils.throwOnError(status);
		return pContext.value;
	}
	
	private static long initFromXmlEx(String xmlFile, OutArg<Long> hScriptNode) throws StatusException
	{
		OutArg<Long> pContext = new OutArg<Long>();
		int status = NativeMethods.xnInitFromXmlFileEx(xmlFile, pContext, 0L, hScriptNode);
		WrapperUtils.throwOnError(status);
		return pContext.value;
	}
	
	ProductionNode createProductionNodeObject(long nodeHandle, NodeType type) throws GeneralException
	{
		synchronized (this.allNodes)
		{
			if (!this.allNodes.containsKey(nodeHandle))
			{
				// create it
				ProductionNode node;
				
				if (type.equals(NodeType.DEVICE))
					node = new Device(this, nodeHandle, true);
				else if (type.equals(NodeType.DEPTH))
					node = new DepthGenerator(this, nodeHandle, true);
				else if (type.equals(NodeType.IMAGE))
					node = new ImageGenerator(this, nodeHandle, true);
				else if (type.equals(NodeType.AUDIO))
					node = new AudioGenerator(this, nodeHandle, true);
				else if (type.equals(NodeType.IR))
					node = new IRGenerator(this, nodeHandle, true);
				else if (type.equals(NodeType.USER))
					node = new UserGenerator(this, nodeHandle, true);
				else if (type.equals(NodeType.RECORDER))
					node = new Recorder(this, nodeHandle, true);
				else if (type.equals(NodeType.PLAYER))
					node = new Player(this, nodeHandle, true);
				else if (type.equals(NodeType.GESTURE))
					node = new GestureGenerator(this, nodeHandle, true);
				else if (type.equals(NodeType.SCENE))
					node = new SceneAnalyzer(this, nodeHandle, true);
				else if (type.equals(NodeType.HANDS))
					node = new HandsGenerator(this, nodeHandle, true);
				else if (type.equals(NodeType.CODEC))
					node = new Codec(this, nodeHandle, true);
				else if (type.equals(NodeType.SCRIPT_NODE))
					node = new ScriptNode(this, nodeHandle, true);
				else if (type.equals(NodeType.PRODUCTION_NODE))
					node = new ProductionNode(this, nodeHandle, true);
				else if (type.equals(NodeType.GENERATOR))
					node = new Generator(this, nodeHandle, true);
				else if (type.equals(NodeType.MAP_GENERATOR))
					node = new MapGenerator(this, nodeHandle, true);
				else
					throw new GeneralException("java wrapper: Unknown generator type!");
				
				// add it to map
				this.allNodes.put(nodeHandle, node);
			} // create
			
			return this.allNodes.get(nodeHandle);
		} // synch
	}

	ProductionNode createProductionNodeObject(long hNode) throws GeneralException
	{
		long pNodeInfo = NativeMethods.xnGetNodeInfo(hNode);
		NodeType type = NativeMethods.xnNodeInfoGetDescription(pNodeInfo).getType();
		return createProductionNodeObject(hNode, type);
	}

	private Observable<ErrorStateEventArgs> errorStateChangedEvent;
	private Observable<NodeCreatedEventArgs> nodeCreatedEvent;
	private Observable<NodeDestroyedEventArgs> nodeDestroyedEvent;

	private Hashtable<Long, ProductionNode> allNodes = new Hashtable<Long, ProductionNode>();
	
	private static Hashtable<Long, Context> allContexts = new Hashtable<Long, Context>(); 
}
