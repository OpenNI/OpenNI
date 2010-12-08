/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




#ifndef __XN_MODULE_LOADER_H__
#define __XN_MODULE_LOADER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnModuleInterfaceContainers.h"
#include <XnHash.h>

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
	XnStatus AddGenerator(XnModuleExportedProductionNodeInterface* pInterface, const XnChar* strConfigDir);
	XnStatus Enumerate(XnProductionNodeType Type, XnNodeInfoList* pList, XnEnumerationErrors* pErrors);
	XnStatus CreateRootNode(XnNodeInfo* pTree, XnModuleInstance** ppInstance);
	void DestroyModuleInstance(XnModuleInstance* pInstance);

private:
	XnStatus LoadAllModules();
	XnStatus LoadModule(const XnChar* strFileName, const XnChar* strConfigDir);
	XnStatus AddModuleGenerators(const XnChar* strModuleFile, XN_LIB_HANDLE hLib, const XnChar* strConfigDir);
	XnStatus LoadSpecificInterface(XnProductionNodeType Type, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadDeviceNode(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadDepthGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadImageGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadIRGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadGestureGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadUserGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadHandsGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadSceneAnalyzer(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadAudioGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadRecorder(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadPlayer(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadDeprecatedPlayer(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);
	XnStatus LoadCodec(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer);

	XnStatus ValidateProductionNodeInterface(XnModuleProductionNodeInterface* pInterface);
	XnStatus ValidateDeviceInterface(XnModuleDeviceInterface* pInterface);
	XnStatus ValidateGeneratorInterface(XnModuleGeneratorInterface* pInterface);
	XnStatus ValidateMapGeneratorInterface(XnModuleMapGeneratorInterface* pInterface);
	XnStatus ValidateDepthGeneratorInterface(XnModuleDepthGeneratorInterface* pInterface);
	XnStatus ValidateImageGeneratorInterface(XnModuleImageGeneratorInterface* pInterface);
	XnStatus ValidateIRGeneratorInterface(XnModuleIRGeneratorInterface* pInterface);
	XnStatus ValidateGestureGeneratorInterface(XnModuleGestureGeneratorInterface* pInterface);
	XnStatus ValidateUserGeneratorInterface(XnModuleUserGeneratorInterface* pInterface);
	XnStatus ValidateHandsGeneratorInterface(XnModuleHandsGeneratorInterface* pInterface);
	XnStatus ValidateSceneAnalyzerInterface(XnModuleSceneAnalyzerInterface* pInterface);
	XnStatus ValidateAudioGeneratorInterface(XnModuleAudioGeneratorInterface* pInterface);
	XnStatus ValidateRecorderInterface(XnModuleRecorderInterface* pInterface);
	XnStatus ValidatePlayerInterface(XnModulePlayerInterface* pInterface);
	XnStatus ValidateCodecInterface(XnModuleCodecInterface* pInterface);
	XnStatus ValidateNodeNotifications(XnNodeNotifications* pNodeNotifications);

	class XnDescriptionKeyManager
	{
	public:
		static XnHashValue Hash(XnProductionNodeDescription const& key);
		static XnInt32 Compare(XnProductionNodeDescription const& key1, XnProductionNodeDescription const& key2);
	};

	XN_DECLARE_DEFAULT_VALUE_TRANSLATOR(XnProductionNodeDescription, XnGeneratorDescriptionTranslator);
	XN_DECLARE_DEFAULT_VALUE_TRANSLATOR(XnLoadedGenerator, XnLoadedGeneratorTranslator);
	XN_DECLARE_HASH(XnProductionNodeDescription, XnLoadedGenerator, XnLoadedGeneratorsHash, XnGeneratorDescriptionTranslator, XnLoadedGeneratorTranslator, XnDescriptionKeyManager);

	XnLoadedGeneratorsHash m_AllGenerators;
	XnContext* m_pContext;
	LoadingMode m_loadingMode;
};

#endif // __XN_MODULE_LOADER_H__