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
#ifndef __XN_MODULE_LOADER_H__
#define __XN_MODULE_LOADER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnModuleInterfaceContainers.h"
#include <XnHashT.h>
#include <XnStringsHashT.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnLoadedGenerator
{
	XnProductionNodeDescription Description;
	XnModuleExportedProductionNodeInterface ExportedInterface;
	XnProductionNodeInterfaceContainer* pInterface;
	const XnChar* strConfigDir;
} XnLoadedGenerator;

typedef struct XnModuleInstance
{
	XnLoadedGenerator* pLoaded;
	XnModuleNodeHandle hNode;
} XnModuleInstance;

class XnModuleLoader
{
public:
	XnModuleLoader();
	~XnModuleLoader();

	typedef enum
	{
		LOADING_MODE_LOAD,
		LOADING_MODE_PRINT,
		LOADING_MODE_VERIFY
	} LoadingMode;

	void SetLoadingMode(LoadingMode mode);

	XnStatus Init();
	XnStatus AddModule(XnOpenNIModuleInterface* pInterface, const XnChar* strConfigDir, const XnChar* strName);
	XnStatus AddExportedNode(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pInterface, const XnChar* strConfigDir);
	XnStatus Enumerate(XnContext* pContext, XnProductionNodeType Type, XnNodeInfoList* pList, XnEnumerationErrors* pErrors);
	XnStatus CreateRootNode(XnContext* pContext, XnNodeInfo* pTree, XnModuleInstance** ppInstance);
	void DestroyModuleInstance(XnModuleInstance* pInstance);

#if !XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
	static XnStatus RegisterModule(XnOpenNIModuleInterface* pInterface, const XnChar* strConfigDir, const XnChar* strName);
#endif

private:
	XnStatus LoadAllModules();
	XnStatus LoadModule(const XnChar* strFileName, const XnChar* strConfigDir);
	XnStatus AddModuleGenerators(const XnChar* strModuleFile, XN_LIB_HANDLE hLib, const XnChar* strConfigDir);
	XnStatus AddOpenNIGenerators();
	XnStatus LoadSpecificInterface(XnVersion& moduleOpenNIVersion, XnProductionNodeType Type, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadDeviceNode(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadDepthGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadImageGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadIRGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadGestureGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadUserGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadHandsGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadSceneAnalyzer(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadAudioGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadRecorder(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadPlayer(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadDeprecatedPlayer(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadCodec(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadScriptNode(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadProductionNode(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadMapGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);

	XnStatus ValidateProductionNodeInterface(XnVersion& moduleOpenNIVersion, XnModuleProductionNodeInterface* pInterface);
	XnStatus ValidateDeviceInterface(XnVersion& moduleOpenNIVersion, XnModuleDeviceInterface* pInterface);
	XnStatus ValidateGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleGeneratorInterface* pInterface);
	XnStatus ValidateMapGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleMapGeneratorInterface* pInterface);
	XnStatus ValidateDepthGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleDepthGeneratorInterface* pInterface);
	XnStatus ValidateImageGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleImageGeneratorInterface* pInterface);
	XnStatus ValidateIRGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleIRGeneratorInterface* pInterface);
	XnStatus ValidateGestureGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleGestureGeneratorInterface* pInterface);
	XnStatus ValidateUserGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleUserGeneratorInterface* pInterface);
	XnStatus ValidateHandsGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleHandsGeneratorInterface* pInterface);
	XnStatus ValidateSceneAnalyzerInterface(XnVersion& moduleOpenNIVersion, XnModuleSceneAnalyzerInterface* pInterface);
	XnStatus ValidateAudioGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleAudioGeneratorInterface* pInterface);
	XnStatus ValidateRecorderInterface(XnVersion& moduleOpenNIVersion, XnModuleRecorderInterface* pInterface);
	XnStatus ValidatePlayerInterface(XnVersion& moduleOpenNIVersion, XnModulePlayerInterface* pInterface);
	XnStatus ValidateCodecInterface(XnVersion& moduleOpenNIVersion, XnModuleCodecInterface* pInterface);
	XnStatus ValidateScriptNodeInterface(XnVersion& moduleOpenNIVersion, XnModuleScriptNodeInterface* pInterface);
	XnStatus ValidateNodeNotifications(XnVersion& moduleOpenNIVersion, XnNodeNotifications* pNodeNotifications);

	XnStatus ValidateFunctionGroup(const XnChar* strName, void* aFunctions[], XnUInt32 nSize);

	class XnDescriptionKeyManager
	{
	public:
		static XnHashCode Hash(XnProductionNodeDescription const& key);
		static XnInt32 Compare(XnProductionNodeDescription const& key1, XnProductionNodeDescription const& key2);
	};

	typedef XnHashT<XnProductionNodeDescription, XnLoadedGenerator, XnDescriptionKeyManager> XnLoadedGeneratorsHash;
	typedef XnStringsHashT<XnProductionNodeType> ExtendedNodeTypesHash;

#if !XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
	typedef struct RegisteredModule
	{
		XnOpenNIModuleInterface* pInterface;
		const XnChar* strConfigDir;
		const XnChar* strName;
	} RegisteredModule;
	typedef XnListT<RegisteredModule> RegisteredModulesList;
	static RegisteredModulesList sm_modulesList;
#endif

	XnLoadedGeneratorsHash m_AllGenerators;
	ExtendedNodeTypesHash m_ExtendedNodeTypesHash;
	LoadingMode m_loadingMode;
};

#endif // __XN_MODULE_LOADER_H__