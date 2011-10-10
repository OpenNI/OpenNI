/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
#ifndef __XN_MODULE_LOADER_H__
#define __XN_MODULE_LOADER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnModuleInterfaceContainers.h"
#include <XnHash.h>
#include <XnStringsHash.h>

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
	XnModuleLoader(XnContext* pContext);
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
	XnStatus Enumerate(XnProductionNodeType Type, XnNodeInfoList* pList, XnEnumerationErrors* pErrors);
	XnStatus CreateRootNode(XnNodeInfo* pTree, XnModuleInstance** ppInstance);
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
		static XnHashValue Hash(XnProductionNodeDescription const& key);
		static XnInt32 Compare(XnProductionNodeDescription const& key1, XnProductionNodeDescription const& key2);
	};

	XN_DECLARE_DEFAULT_VALUE_TRANSLATOR(XnProductionNodeDescription, XnGeneratorDescriptionTranslator);
	XN_DECLARE_DEFAULT_VALUE_TRANSLATOR(XnLoadedGenerator, XnLoadedGeneratorTranslator);
	XN_DECLARE_HASH(XnProductionNodeDescription, XnLoadedGenerator, XnLoadedGeneratorsHash, XnGeneratorDescriptionTranslator, XnLoadedGeneratorTranslator, XnDescriptionKeyManager);
	XN_DECLARE_STRINGS_HASH(XnProductionNodeType, ExtendedNodeTypesHash);

#if !XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
	typedef struct RegisteredModule
	{
		XnOpenNIModuleInterface* pInterface;
		const XnChar* strConfigDir;
		const XnChar* strName;
	} RegisteredModule;
	XN_DECLARE_LIST(RegisteredModule, RegisteredModulesList);
	static RegisteredModulesList sm_modulesList;
#endif

	XnLoadedGeneratorsHash m_AllGenerators;
	ExtendedNodeTypesHash m_ExtendedNodeTypesHash;
	XnContext* m_pContext;
	LoadingMode m_loadingMode;
};

#endif // __XN_MODULE_LOADER_H__