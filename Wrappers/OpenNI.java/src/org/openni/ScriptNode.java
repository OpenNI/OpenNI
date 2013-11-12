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

/**
 * Production node that loads and runs scripts from XML files. <BR><BR>
 *
 */
public class ScriptNode extends ProductionNode
{
	/**
	 * Create a new script node in the given context
	 * @param context OpenNI context for this production node
	 * @param nodeHandle Native pointer to this object
	 * @param addRef Whether to register this object with OpenNI
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	ScriptNode(Context context, long nodeHandle, boolean addRef) throws StatusException
	{
		super(context, nodeHandle, addRef);
	}

	/**
	 * Creates a new ScriptNode with specified context and format
	 * @param context OpenNI Context
	 * @param formatName format of the script
	 * @return Resulting ScriptNode
	 * @throws GeneralException If underlying native code returns errors, a General Exception will be generated
	 */
	public static ScriptNode create(Context context, String formatName) throws GeneralException
	{
		OutArg<Long> phScriptNode = new OutArg<Long>();
		int status = NativeMethods.xnCreateScriptNode(context.toNative(), formatName, phScriptNode);
		WrapperUtils.throwOnError(status);
		ScriptNode ScriptNode = (ScriptNode)context.createProductionNodeObject(phScriptNode.value, NodeType.SCRIPT_NODE);
		NativeMethods.xnProductionNodeRelease(phScriptNode.value);
		return ScriptNode;
	}

	/**
	 * Provides access to the supported format of this node
	 * @return Supported format name
	 */
	public String getSupportedFormat()
	{
		return NativeMethods.xnScriptNodeGetSupportedFormat(toNative());
	}
	
	/**
	 * Loads a script from a given file
	 * @param fileName Location of file to load script from
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public void loadScriptFromFile(String fileName) throws StatusException
	{
		int status = NativeMethods.xnLoadScriptFromFile(toNative(), fileName);
		WrapperUtils.throwOnError(status);
	}

	/**
	 * Loads a script from a string
	 * @param script String containing the script
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public void loadScriptFromString(String script) throws StatusException
	{
		int status = NativeMethods.xnLoadScriptFromString(toNative(), script);
		WrapperUtils.throwOnError(status);
	}
	
	/**
	 * Runs a script, storing errors in the provided object
	 * @param errors Place to store any run time errors that are generated
	 * @throws StatusException If underlying native code returns errors, a Status Exception will be generated
	 */
	public void Run(EnumerationErrors errors) throws StatusException
	{
		int status = NativeMethods.xnScriptNodeRun(toNative(), errors.toNative());
		WrapperUtils.throwOnError(status);
	}
}
