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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnModuleLoader.h"
#include "XnInternalDefs.h"
#include <XnOpenNI.h>
#include <XnLog.h>
#include "XnXml.h"
#include "XnTypeManager.h"
#include <XnArray.h>
#include <XnAlgorithms.h>
#include "xnInternalFuncs.h"

#if !XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
#include <XnModuleCFunctions.h>
#endif

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MODULE_LOADER_MAX_MODULES_COUNT			1000
#define XN_MODULE_LOADER_MAX_INTERFACE_PER_MODULE	20
#define XN_MASK_MODULE_LOADER						"ModuleLoader"
#define XN_MODULE_ELEMENT_NAME						"Module"

#define XN_VALIDATE_FUNC_NOT_NULL(pInterface, func)												\
	if ((pInterface)->func == NULL)																\
	{																							\
		xnLogWarning(XN_MASK_MODULE_LOADER, "Production Node does not have the %s function!",	\
			XN_STRINGIFY(func));																\
		return XN_STATUS_INVALID_GENERATOR;														\
	}

#define XN_VALIDATE_CAPABILITY_STRUCT(name, pStruct)	\
	{													\
		XnStatus nTempRetVal = ValidateFunctionGroup(	\
			XN_STRINGIFY(name),							\
			(void**)pStruct,							\
			sizeof(*pStruct)/sizeof(void*));			\
		XN_IS_STATUS_OK(nTempRetVal);					\
	}

#define XN_VALIDATE_CAPABILITY(pInterface, name)												\
	XN_VALIDATE_CAPABILITY_STRUCT(name, pInterface->p##name##Interface)

//---------------------------------------------------------------------------
// Backwards Compatibility Issues
//---------------------------------------------------------------------------
static XnVersion EXTENSIONS_VERSION = { 1, 1, 0, 0 };

typedef const void* (XN_CALLBACK_TYPE* GetDataPrototype)(XnModuleNodeHandle hGenerator);

static const void* XN_CALLBACK_TYPE GetDataNull(XnModuleNodeHandle /*hGenerator*/)
{
	return NULL;
}

typedef XnUInt32 (XN_CALLBACK_TYPE* GetBytesPerPixelPrototype)(XnModuleNodeHandle hGenerator);

static XnUInt32 XN_CALLBACK_TYPE GetDepthBytesPerPixel(XnModuleNodeHandle /*hNode*/)
{
	return sizeof(XnDepthPixel);
}

static XnUInt32 XN_CALLBACK_TYPE GetIRBytesPerPixel(XnModuleNodeHandle /*hNode*/)
{
	return sizeof(XnIRPixel);
}

static XnUInt32 XN_CALLBACK_TYPE GetSceneBytesPerPixel(XnModuleNodeHandle /*hNode*/)
{
	return sizeof(XnLabel);
}

static XnStatus XN_CALLBACK_TYPE UnimplementedGetPixelCoordinatesInViewPoint(XnModuleNodeHandle /*hGenerator*/, XnNodeHandle /*hOther*/, XnUInt32 /*x*/, XnUInt32 /*y*/, XnUInt32* /*pAltX*/, XnUInt32* /*pAltY*/)
{
	return XN_STATUS_NOT_IMPLEMENTED;
}

//---------------------------------------------------------------------------
// XnDescriptionKeyManager class
//---------------------------------------------------------------------------
XnHashCode XnModuleLoader::XnDescriptionKeyManager::Hash(XnProductionNodeDescription const& key)
{
	XnUInt32 nTotalCRC = 0;
	nTotalCRC += (key.Type * 19);

	XnUInt32 nTempCRC;
	xnOSStrCRC32(key.strVendor, &nTempCRC);
	nTotalCRC += nTempCRC;

	xnOSStrCRC32(key.strName, &nTempCRC);
	nTotalCRC += nTempCRC;

	xnOSStrNCRC32((XnUChar*)&key.Version, sizeof(key.Version), &nTempCRC);
	nTotalCRC += nTempCRC;

	// convert from UINT32 to XnHashValue
	return nTotalCRC % (1 << (sizeof(XnHashCode)*8)); 
}

XnInt32 XnModuleLoader::XnDescriptionKeyManager::Compare(XnProductionNodeDescription const& key1, XnProductionNodeDescription const& key2)
{
	XnInt32 nResult = key1.Type - key2.Type;

	if (nResult == 0)
	{
		nResult = strcmp(key1.strVendor, key2.strVendor);
	}

	if (nResult == 0)
	{
		nResult = strcmp(key1.strName, key2.strName);
	}

	if (nResult == 0)
	{
		nResult = xnVersionCompare(&key1.Version, &key2.Version);
	}

	return nResult;
}

XnStatus resolveModulesFile(XnChar* strFileName, XnUInt32 nBufSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnGetOpenNIConfFilesPath(strFileName, nBufSize);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSStrAppend(strFileName, "modules.xml", nBufSize);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus loadModulesFile(TiXmlDocument& doc)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strFileName[XN_FILE_MAX_PATH];

	nRetVal = resolveModulesFile(strFileName, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);
	
	XnBool bDoesExist = FALSE;
	nRetVal = xnOSDoesFileExist(strFileName, &bDoesExist);
	XN_IS_STATUS_OK(nRetVal);

	if (bDoesExist)
	{
		nRetVal = xnXmlLoadDocument(doc, strFileName);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		TiXmlElement root("Modules");
		doc.InsertEndChild(root);
		doc.SaveFile(strFileName);
	}
	
	return (XN_STATUS_OK);
}

XnStatus saveModulesFile(TiXmlDocument& doc)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strFileName[XN_FILE_MAX_PATH];

	nRetVal = resolveModulesFile(strFileName, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	if (!doc.SaveFile(strFileName))
	{
		return XN_STATUS_OS_FILE_WRITE_FAILED;
	}

	return (XN_STATUS_OK);
}

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnModuleLoader::XnModuleLoader() : m_loadingMode(LOADING_MODE_LOAD)
{
}

XnModuleLoader::~XnModuleLoader()
{
	// free memory
	for (XnLoadedGeneratorsHash::Iterator it = m_AllGenerators.Begin(); it != m_AllGenerators.End(); ++it)
	{
		xnOSFree(it->Value().strConfigDir);
		XN_DELETE(it->Value().pInterface);
	}
}

void XnModuleLoader::SetLoadingMode(LoadingMode mode)
{
	m_loadingMode = mode;
}

XnStatus XnModuleLoader::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

#if XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
	nRetVal = LoadAllModules();
	XN_IS_STATUS_OK(nRetVal);
#else
	for (RegisteredModulesList::Iterator it = sm_modulesList.Begin(); it != sm_modulesList.End(); ++it)
	{
		RegisteredModule& module = *it;
		nRetVal = AddModule(module.pInterface, module.strConfigDir, module.strName);
		XN_IS_STATUS_OK(nRetVal);
	}
#endif

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadAllModules()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// first load OpenNI itself
	nRetVal = AddOpenNIGenerators();
	XN_IS_STATUS_OK(nRetVal);

	// now load modules
	TiXmlDocument doc;
	nRetVal = loadModulesFile(doc);
	XN_IS_STATUS_OK(nRetVal);

	// try to load each
	TiXmlElement* pModule = doc.RootElement()->FirstChildElement(XN_MODULE_ELEMENT_NAME);
	while (pModule != NULL)
	{
		const XnChar* strModulePath = NULL;
		nRetVal = xnXmlReadStringAttribute(pModule, "path", &strModulePath);
		XN_IS_STATUS_OK(nRetVal);

		const XnChar* strConfigDir = pModule->Attribute("configDir");
#if XN_PLATFORM == XN_PLATFORM_ANDROID_ARM
		// Append full application data path to both module path and config directory
		XnChar strTemp[1024];
		XnChar strTemp2[1024];

		xnOSGetApplicationLibDir(strTemp, sizeof(strTemp));
		strncat(strTemp, strModulePath, sizeof(strTemp));
		strModulePath = strTemp;

		if (strConfigDir != NULL)
		{
			xnOSGetApplicationFilesDir(strTemp2, sizeof(strTemp2));
			strncat(strTemp2, strConfigDir, sizeof(strTemp2));
			strConfigDir = strTemp2;
		}
#endif
		nRetVal = LoadModule(strModulePath, strConfigDir);
		XN_IS_STATUS_OK(nRetVal);

		pModule = pModule->NextSiblingElement(XN_MODULE_ELEMENT_NAME);
	}

	if (m_loadingMode == LOADING_MODE_LOAD && m_AllGenerators.Size() == 0)
	{
		return (XN_STATUS_NO_MODULES_FOUND);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadModule(const XnChar* strFileName, const XnChar* strConfigDir)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	xnLogVerbose(XN_MASK_MODULE_LOADER, "Checking %s...", strFileName);

	if (m_loadingMode == LOADING_MODE_PRINT)
	{
		printf("%s ", strFileName);
	}

	XN_LIB_HANDLE hLib;
	nRetVal = xnOSLoadLibrary(strFileName, &hLib);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_MODULE_LOADER, "Failed to load '%s' - missing dependencies?", strFileName);
		return (XN_STATUS_OK);
	}

	nRetVal = AddModuleGenerators(strFileName, hLib, strConfigDir);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFreeLibrary(hLib);
		return (nRetVal);
	}

	if (m_loadingMode == LOADING_MODE_PRINT)
	{
		printf("\n");
	}

	return (XN_STATUS_OK);
}

inline static XnStatus FindFuncAddress(const XnChar* strModuleFile, XN_LIB_HANDLE hLib, const XnChar* funcName, XnFarProc* pFunc)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = xnOSGetProcAddress(hLib, funcName, pFunc);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_MODULE_LOADER, "'%s' is not a valid module: can't find '%s' function!", strModuleFile, funcName);
		return (nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::AddModuleGenerators(const XnChar* strModuleFile, XN_LIB_HANDLE hLib, const XnChar* strConfigDir)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnOpenNIModuleInterface openNIModule;

	// get the function pointers
	nRetVal = FindFuncAddress(strModuleFile, hLib, XN_STRINGIFY(XN_MODULE_LOAD), (XnFarProc*)&openNIModule.pLoadFunc);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FindFuncAddress(strModuleFile, hLib, XN_STRINGIFY(XN_MODULE_UNLOAD), (XnFarProc*)&openNIModule.pUnloadFunc);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FindFuncAddress(strModuleFile, hLib, XN_STRINGIFY(XN_MODULE_GET_EXPORTED_NODES_COUNT), (XnFarProc*)&openNIModule.pGetCountFunc);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FindFuncAddress(strModuleFile, hLib, XN_STRINGIFY(XN_MODULE_GET_EXPORTED_NODES_ENTRY_POINTS), (XnFarProc*)&openNIModule.pGetEntryPointsFunc);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FindFuncAddress(strModuleFile, hLib, XN_STRINGIFY(XN_MODULE_GET_OPEN_NI_VERSION), (XnFarProc*)&openNIModule.pGetVersionFunc);
	XN_IS_STATUS_OK(nRetVal);

	// add it
	nRetVal = AddModule(&openNIModule, strConfigDir, strModuleFile);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus XnModuleLoader::AddOpenNIGenerators()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnOpenNIModuleInterface* pOpenNIModule = GetOpenNIModuleInterface();

	// add it
	nRetVal = AddModule(pOpenNIModule, NULL, "OpenNI");
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus XnModuleLoader::AddModule(XnOpenNIModuleInterface* pInterface, const XnChar* strConfigDir, const XnChar* strName)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// get OpenNI Version
	XnVersion openNIVersion;
	pInterface->pGetVersionFunc(&openNIVersion);

	if (m_loadingMode == LOADING_MODE_PRINT)
	{
		XnChar strOpenNIVersion[100];
		xnVersionToString(&openNIVersion, strOpenNIVersion, 100);
		printf("(compiled with OpenNI %s):\n", strOpenNIVersion);
	}

	// load Module
	nRetVal = pInterface->pLoadFunc();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_MODULE_LOADER, "'%s' load function failed. Error code: 0x%x", strName, nRetVal);
		return (nRetVal);
	}

	// take the number of generators
	XnUInt32 nCount = pInterface->pGetCountFunc();

	// allocate entry points array
	XnModuleGetExportedInterfacePtr* aEntryPoints;
	XN_VALIDATE_CALLOC(aEntryPoints, XnModuleGetExportedInterfacePtr, nCount);

	// fill it
	nRetVal = pInterface->pGetEntryPointsFunc(aEntryPoints, nCount);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_MODULE_LOADER, "'%s' - failed to get exported nodes. Error code: 0x%x", strName, nRetVal);
		xnOSFree(aEntryPoints);
		return (nRetVal);
	}

	// now add every exported node
	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		// get exported interface
		XnModuleExportedProductionNodeInterface ExportedInterface;
		aEntryPoints[i](&ExportedInterface);

		nRetVal = AddExportedNode(openNIVersion, &ExportedInterface, strConfigDir);
		if (nRetVal == XN_STATUS_INVALID_GENERATOR)
		{
			// if it failed, then this specific generator is not loaded, but the rest should be loaded anyway.
			xnLogWarning(XN_MASK_MODULE_LOADER, "Failed to add generator %d from module '%s'", i, strName);
		}
		else if (nRetVal != XN_STATUS_OK)
		{
			xnOSFree(aEntryPoints);
			return (nRetVal);
		}
	}

	xnOSFree(aEntryPoints);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::AddExportedNode(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, const XnChar* strConfigDir)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate we have all mandatory functions
	XN_VALIDATE_FUNC_NOT_NULL(pExportedInterface, GetDescription);
	XN_VALIDATE_FUNC_NOT_NULL(pExportedInterface, EnumerateProductionTrees);
	XN_VALIDATE_FUNC_NOT_NULL(pExportedInterface, Create);
	XN_VALIDATE_FUNC_NOT_NULL(pExportedInterface, Destroy);
	XN_VALIDATE_FUNC_NOT_NULL(pExportedInterface, GetInterface.General);

	XnLoadedGenerator loaded;
	xnOSMemSet(&loaded, 0, sizeof(loaded));
	loaded.ExportedInterface = *pExportedInterface;

	// Get Description
	pExportedInterface->GetDescription(&loaded.Description);

	XnChar strDescription[512];
	xnProductionNodeDescriptionToString(&loaded.Description, strDescription, 512);

	xnLogVerbose(XN_MASK_MODULE_LOADER, "Found exported production node. %s", strDescription);
	if (m_loadingMode == LOADING_MODE_PRINT)
	{
		printf("\t%s\n", strDescription);
	}

	// make sure it's not in the list
	XnLoadedGeneratorsHash::ConstIterator it = m_AllGenerators.Find(loaded.Description);
	if (it != m_AllGenerators.End())
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_INVALID_GENERATOR, XN_MASK_MODULE_LOADER, "A Generator with the same description already exists!");
	}

	// Now load specific interface
	XnProductionNodeInterfaceContainer* pInterfaceContainer = NULL;

	nRetVal = LoadSpecificInterface(moduleOpenNIVersion, loaded.Description.Type, pExportedInterface, pInterfaceContainer);
	XN_IS_STATUS_OK(nRetVal);

	loaded.pInterface = pInterfaceContainer;

	if (strConfigDir != NULL)
	{
		loaded.strConfigDir = xnOSStrDup(strConfigDir);
	}

	// Add it to list
	if (m_loadingMode == LOADING_MODE_LOAD)
	{
		nRetVal = m_AllGenerators.Set(loaded.Description, loaded);
		if (nRetVal != XN_STATUS_OK)
		{
			xnOSFree(loaded.strConfigDir);
			XN_DELETE(pInterfaceContainer);
			return (nRetVal);
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadSpecificInterface(XnVersion& moduleOpenNIVersion, XnProductionNodeType Type, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	const XnBitSet* pHierarchy;
	nRetVal = TypeManager::GetInstance().GetTypeHierarchy(Type, pHierarchy);
	XN_IS_STATUS_OK(nRetVal);

	// start with concrete types
	if (pHierarchy->IsSet(XN_NODE_TYPE_DEVICE))
	{
		nRetVal = LoadDeviceNode(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_DEPTH))
	{
		nRetVal = LoadDepthGenerator(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_IMAGE))
	{
		nRetVal = LoadImageGenerator(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_IR))
	{
		nRetVal = LoadIRGenerator(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_GESTURE))
	{
		nRetVal = LoadGestureGenerator(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_USER))
	{
		nRetVal = LoadUserGenerator(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_HANDS))
	{
		nRetVal = LoadHandsGenerator(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_SCENE))
	{
		nRetVal = LoadSceneAnalyzer(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_AUDIO))
	{
		nRetVal = LoadAudioGenerator(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_RECORDER))
	{
		nRetVal = LoadRecorder(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_PLAYER))
	{
		nRetVal = LoadPlayer(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_CODEC))
	{
		nRetVal = LoadCodec(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_SCRIPT))
	{
		nRetVal = LoadScriptNode(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}

	// and now, some abstract types
	else if (pHierarchy->IsSet(XN_NODE_TYPE_MAP_GENERATOR))
	{
		nRetVal = LoadMapGenerator(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_GENERATOR))
	{
		nRetVal = LoadGenerator(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_PRODUCTION_NODE))
	{
		nRetVal = LoadProductionNode(moduleOpenNIVersion, pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_UNKNOWN_GENERATOR_TYPE, XN_MASK_MODULE_LOADER, "Unknown type: %u", Type);
	}

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadDeviceNode(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Device(&Interface.Device);

	// validate it
	nRetVal = ValidateDeviceInterface(moduleOpenNIVersion, &Interface.Device);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnDeviceInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnDeviceInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadDepthGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDepthGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Depth(&Interface.Depth);

	// fix BC issues
	if (xnVersionCompare(&moduleOpenNIVersion, &EXTENSIONS_VERSION) < 0)
	{
		Interface.Generator.GetData = (GetDataPrototype)Interface.Depth.GetDepthMap;
		Interface.Map.GetBytesPerPixel = GetDepthBytesPerPixel;
	}

	// validate it
	nRetVal = ValidateDepthGeneratorInterface(moduleOpenNIVersion, &Interface.Depth);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnDepthGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnDepthGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadImageGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnImageGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Image(&Interface.Image);

	// fix BC issues
	if (xnVersionCompare(&moduleOpenNIVersion, &EXTENSIONS_VERSION) < 0)
	{
		Interface.Generator.GetData = (GetDataPrototype)Interface.Image.GetImageMap;
		Interface.Map.GetBytesPerPixel = (GetBytesPerPixelPrototype)XN_SPECIAL_BC_BEHAVIOR;
	}

	// validate interface
	nRetVal = ValidateImageGeneratorInterface(moduleOpenNIVersion, &Interface.Image);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnImageGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnImageGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadIRGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnIRGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.IR(&Interface.IR);

	// fix BC issues
	if (xnVersionCompare(&moduleOpenNIVersion, &EXTENSIONS_VERSION) < 0)
	{
		Interface.Generator.GetData = (GetDataPrototype)Interface.IR.GetIRMap;
		Interface.Map.GetBytesPerPixel = GetIRBytesPerPixel;
	}

	// validate interface
	nRetVal = ValidateIRGeneratorInterface(moduleOpenNIVersion, &Interface.IR);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnIRGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnIRGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadGestureGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnGestureGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Gesture(&Interface.Gesture);

	// fix BC issues
	if (xnVersionCompare(&moduleOpenNIVersion, &EXTENSIONS_VERSION) < 0)
	{
		Interface.Generator.GetData = GetDataNull;
	}

	// validate interface
	nRetVal = ValidateGestureGeneratorInterface(moduleOpenNIVersion, &Interface.Gesture);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnGestureGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnGestureGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return XN_STATUS_OK;
}

XnStatus XnModuleLoader::LoadUserGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUserGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.User(&Interface.User);

	// fix BC issues
	if (xnVersionCompare(&moduleOpenNIVersion, &EXTENSIONS_VERSION) < 0)
	{
		Interface.Generator.GetData = GetDataNull;
	}

	// validate interface
	nRetVal = ValidateUserGeneratorInterface(moduleOpenNIVersion, &Interface.User);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnUserGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnUserGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return XN_STATUS_OK;
}

XnStatus XnModuleLoader::LoadHandsGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnHandsGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Hands(&Interface.Hands);

	// fix BC issues
	if (xnVersionCompare(&moduleOpenNIVersion, &EXTENSIONS_VERSION) < 0)
	{
		Interface.Generator.GetData = GetDataNull;
	}

	// validate interface
	nRetVal = ValidateHandsGeneratorInterface(moduleOpenNIVersion, &Interface.Hands);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnHandsGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnHandsGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return XN_STATUS_OK;
}

XnStatus XnModuleLoader::LoadSceneAnalyzer(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnSceneAnalyzerInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Scene(&Interface.Scene);

	// fix BC issues
	if (xnVersionCompare(&moduleOpenNIVersion, &EXTENSIONS_VERSION) < 0)
	{
		Interface.Generator.GetData = (GetDataPrototype)Interface.Scene.GetLabelMap;
		Interface.Map.GetBytesPerPixel = GetSceneBytesPerPixel;
	}

	// validate interface
	nRetVal = ValidateSceneAnalyzerInterface(moduleOpenNIVersion, &Interface.Scene);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnSceneAnalyzerInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnSceneAnalyzerInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return XN_STATUS_OK;
}

XnStatus XnModuleLoader::LoadAudioGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnAudioGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Audio(&Interface.Audio);

	// fix BC issues
	if (xnVersionCompare(&moduleOpenNIVersion, &EXTENSIONS_VERSION) < 0)
	{
		Interface.Generator.GetData = (GetDataPrototype)Interface.Audio.GetAudioBuffer;
	}

	// validate interface
	nRetVal = ValidateAudioGeneratorInterface(moduleOpenNIVersion, &Interface.Audio);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnAudioGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnAudioGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadRecorder(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnRecorderInterfaceContainer interface;

	// fill it up
	pExportedInterface->GetInterface.Recorder(&interface.recorder);

	// validate interface
	nRetVal = ValidateRecorderInterface(moduleOpenNIVersion, &interface.recorder);
	XN_IS_STATUS_OK(nRetVal);

	/*interface.recorder.pNodeNotifications points to interface.nodeNotifications,
	  so interface.nodeNotifications is already set. */
	nRetVal = ValidateNodeNotifications(moduleOpenNIVersion, &interface.nodeNotifications);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnRecorderInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnRecorderInterfaceContainer);
	*pContainer = interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadPlayer(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnPlayerInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Player(&Interface.Player);

	// validate interface
	nRetVal = ValidatePlayerInterface(moduleOpenNIVersion, &Interface.Player);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnPlayerInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnPlayerInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadCodec(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnCodecInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Codec(&Interface.Codec);

	// validate interface
	nRetVal = ValidateCodecInterface(moduleOpenNIVersion, &Interface.Codec);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnCodecInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnCodecInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadScriptNode(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnScriptNodeInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Script(&Interface.Script);

	// validate interface
	nRetVal = ValidateScriptNodeInterface(moduleOpenNIVersion, &Interface.Script);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnScriptNodeInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnScriptNodeInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadProductionNode(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnProductionNodeInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.ProductionNode(&Interface.ProductionNode);

	// validate interface
	nRetVal = ValidateProductionNodeInterface(moduleOpenNIVersion, &Interface.ProductionNode);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnProductionNodeInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnProductionNodeInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Generator(&Interface.Generator);

	// validate interface
	nRetVal = ValidateGeneratorInterface(moduleOpenNIVersion, &Interface.Generator);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadMapGenerator(XnVersion& moduleOpenNIVersion, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnMapGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.MapGenerator(&Interface.Map);

	// validate interface
	nRetVal = ValidateMapGeneratorInterface(moduleOpenNIVersion, &Interface.Map);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnMapGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnMapGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateProductionNodeInterface(XnVersion& /*moduleOpenNIVersion*/, XnModuleProductionNodeInterface* pInterface)
{
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, IsCapabilitySupported);

	// NOTE: we allow general set / get functions to be NULL, so no need to check them

	// validate extended serialization capability
	XN_VALIDATE_CAPABILITY(pInterface, ExtendedSerialization);

	// validate lock aware capability
	XN_VALIDATE_CAPABILITY(pInterface, LockAware);

	// validate error state capability
	XN_VALIDATE_CAPABILITY(pInterface, ErrorState);

	// validate general int capability
	XN_VALIDATE_CAPABILITY(pInterface, GeneralInt);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateDeviceInterface(XnVersion& moduleOpenNIVersion, XnModuleDeviceInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = ValidateProductionNodeInterface(moduleOpenNIVersion, pInterface->pProductionNode);
	XN_IS_STATUS_OK(nRetVal);

	// validate identification capability
	XN_VALIDATE_CAPABILITY(pInterface, DeviceIdentification);
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ValidateProductionNodeInterface(moduleOpenNIVersion, pInterface->pProductionNodeInterface);
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_FUNC_NOT_NULL(pInterface, StartGenerating);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, StopGenerating);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterToGenerationRunningChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterFromGenerationRunningChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterToNewDataAvailable);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterFromNewDataAvailable);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, IsNewDataAvailable);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UpdateData);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetData);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetDataSize);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetTimestamp);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetFrameID);

	// Fix BC issues
	if (pInterface->pAlternativeViewPointInterface->GetPixelCoordinatesInViewPoint == NULL)
	{
		pInterface->pAlternativeViewPointInterface->GetPixelCoordinatesInViewPoint = UnimplementedGetPixelCoordinatesInViewPoint;
	}

	// validate mirror capability
	XN_VALIDATE_CAPABILITY(pInterface, Mirror);

	// validate alternative view point capability
	XN_VALIDATE_CAPABILITY(pInterface, AlternativeViewPoint);

	// validate frame sync capability
	XN_VALIDATE_CAPABILITY(pInterface, FrameSync);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateMapGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleMapGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate base
	nRetVal = ValidateGeneratorInterface(moduleOpenNIVersion, pInterface->pGeneratorInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetSupportedMapOutputModes);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, SetMapOutputMode);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetMapOutputMode);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterToMapOutputModeChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterFromMapOutputModeChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetBytesPerPixel); // This function was only added in 1.0.0.30, but we already BC it in each LoadX() function.

	// validate Cropping interface
	XN_VALIDATE_CAPABILITY(pInterface, Cropping);

	// validate AntiFlicker interface
	XN_VALIDATE_CAPABILITY(pInterface, AntiFlicker);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateDepthGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleDepthGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// validate base
	nRetVal = ValidateMapGeneratorInterface(moduleOpenNIVersion, pInterface->pMapInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetDeviceMaxDepth);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetFieldOfView);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterToFieldOfViewChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterFromFieldOfViewChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetDepthMap);

	// now check capabilities
	XN_VALIDATE_CAPABILITY(pInterface, UserPosition);
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateImageGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleImageGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// validate base
	nRetVal = ValidateMapGeneratorInterface(moduleOpenNIVersion, pInterface->pMapInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetImageMap);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, IsPixelFormatSupported);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, SetPixelFormat);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetPixelFormat);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterToPixelFormatChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterFromPixelFormatChange);
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateIRGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleIRGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// validate base
	nRetVal = ValidateMapGeneratorInterface(moduleOpenNIVersion, pInterface->pMapInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetIRMap);
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateGestureGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleGestureGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate base
	nRetVal = ValidateGeneratorInterface(moduleOpenNIVersion, pInterface->pGeneratorInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, AddGesture);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RemoveGesture);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetActiveGestures);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, EnumerateGestures);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, IsGestureAvailable);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, IsGestureProgressSupported);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterGestureCallbacks);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterGestureCallbacks);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterToGestureChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterFromGestureChange);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateSceneAnalyzerInterface(XnVersion& moduleOpenNIVersion, XnModuleSceneAnalyzerInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate base
	nRetVal = ValidateMapGeneratorInterface(moduleOpenNIVersion, pInterface->pMapInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetLabelMap);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetFloor);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateUserGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleUserGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate base
	nRetVal = ValidateGeneratorInterface(moduleOpenNIVersion, pInterface->pGeneratorInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetNumberOfUsers);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetUsers);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetCoM);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetUserPixels);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterUserCallbacks);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterUserCallbacks);

	// now check Skeleton capability. NOTE: we don't check the entire struct. Only the first 28 
	// functions are mandatory. The rest were added in future versions
	nRetVal = ValidateFunctionGroup("Skeleton", (void**)pInterface->pSkeletonInterface, 28);
	XN_IS_STATUS_OK(nRetVal);

	// now check Skeleton capability. NOTE: we don't check the entire struct. Only the first 6 
	// functions are mandatory. The rest were added in future versions
	nRetVal = ValidateFunctionGroup("PoseDetection", (void**)pInterface->pPoseDetectionInterface, 6);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateHandsGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleHandsGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate base
	nRetVal = ValidateGeneratorInterface(moduleOpenNIVersion, pInterface->pGeneratorInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterHandCallbacks);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterHandCallbacks);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, StopTracking);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, StopTrackingAll);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, StartTracking);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, SetSmoothing);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateAudioGeneratorInterface(XnVersion& moduleOpenNIVersion, XnModuleAudioGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate base
	nRetVal = ValidateGeneratorInterface(moduleOpenNIVersion, pInterface->pGeneratorInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetAudioBuffer);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetSupportedWaveOutputModes);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, SetWaveOutputMode);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetWaveOutputMode);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterToWaveOutputModeChanges);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterFromWaveOutputModeChanges);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateRecorderInterface(XnVersion& moduleOpenNIVersion, XnModuleRecorderInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, SetOutputStream);

	// validate bases
	nRetVal = ValidateProductionNodeInterface(moduleOpenNIVersion, pInterface->pProductionNode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ValidateNodeNotifications(moduleOpenNIVersion, pInterface->pNodeNotifications);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidatePlayerInterface(XnVersion& moduleOpenNIVersion, XnModulePlayerInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	//validate functions
	nRetVal = ValidateProductionNodeInterface(moduleOpenNIVersion, pInterface->pProductionNode);
	XN_IS_STATUS_OK(nRetVal);
	
	//validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, SetInputStream);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, ReadNext);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, SetNodeNotifications);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, SetRepeat);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, SeekToTimeStamp);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, SeekToFrame);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, TellTimestamp);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, TellFrame);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetNumFrames);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetSupportedFormat);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, IsEOF);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterToEndOfFileReached);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterFromEndOfFileReached);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateCodecInterface(XnVersion& moduleOpenNIVersion, XnModuleCodecInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ValidateProductionNodeInterface(moduleOpenNIVersion, pInterface->pProductionNode);
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetCodecID);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, Init);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, CompressData);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, DecompressData);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateScriptNodeInterface(XnVersion& moduleOpenNIVersion, XnModuleScriptNodeInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ValidateProductionNodeInterface(moduleOpenNIVersion, pInterface->pProductionNode);
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetSupportedFormat);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, LoadScriptFromFile);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, LoadScriptFromString);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, Run);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateNodeNotifications(XnVersion& /*moduleOpenNIVersion*/, XnNodeNotifications* pNodeNotifications)
{
	XN_VALIDATE_FUNC_NOT_NULL(pNodeNotifications, OnNodeAdded);
	XN_VALIDATE_FUNC_NOT_NULL(pNodeNotifications, OnNodeRemoved);
	XN_VALIDATE_FUNC_NOT_NULL(pNodeNotifications, OnNodeIntPropChanged);
	XN_VALIDATE_FUNC_NOT_NULL(pNodeNotifications, OnNodeRealPropChanged);
	XN_VALIDATE_FUNC_NOT_NULL(pNodeNotifications, OnNodeStringPropChanged);
	XN_VALIDATE_FUNC_NOT_NULL(pNodeNotifications, OnNodeStateReady);
	XN_VALIDATE_FUNC_NOT_NULL(pNodeNotifications, OnNodeGeneralPropChanged);
	XN_VALIDATE_FUNC_NOT_NULL(pNodeNotifications, OnNodeNewData);
	
	return (XN_STATUS_OK);
}

static XnBool CompareGeneratorsByVersion(const XnLoadedGenerator*& arg1, const XnLoadedGenerator*& arg2)
{
	XnInt32 nCompareRes = strcmp(arg1->Description.strVendor, arg2->Description.strVendor);
	if (nCompareRes == 0)
	{
		nCompareRes = strcmp(arg1->Description.strName, arg2->Description.strName);
	}

	if (nCompareRes == 0)
	{
		nCompareRes = -xnVersionCompare(&arg1->Description.Version, &arg2->Description.Version);
	}

	return (nCompareRes < 0);
}

XnStatus XnModuleLoader::Enumerate(XnContext* pContext, XnProductionNodeType Type, XnNodeInfoList* pList, XnEnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnArray<const XnLoadedGenerator*> foundGenerators;
	foundGenerators.Reserve(50);

	for (XnLoadedGeneratorsHash::ConstIterator it = m_AllGenerators.Begin(); it != m_AllGenerators.End(); ++it)
	{
		const XnLoadedGenerator& LoadedGenerator = it->Value();
		
		// check if it's of the same type and it's not a mock node
		if (LoadedGenerator.Description.Type == Type)
		{
			nRetVal = foundGenerators.AddLast(&LoadedGenerator);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	// now sort the list, so that new versions of a specific generator (vendor + name) will appear first
	XnAlgorithms::BubbleSort(foundGenerators.GetData(), foundGenerators.GetSize(), CompareGeneratorsByVersion);

	// and now enumerate each one
	for (XnUInt32 i = 0; i < foundGenerators.GetSize(); ++i)
	{
		XnNodeInfoList* pGeneratorList = NULL;
		nRetVal = xnNodeInfoListAllocate(&pGeneratorList);
		XN_IS_STATUS_OK(nRetVal);

		const XnLoadedGenerator* pLoadedGenerator = foundGenerators[i];
		nRetVal = pLoadedGenerator->ExportedInterface.EnumerateProductionTrees(pContext, pGeneratorList, pErrors);
		if (nRetVal != XN_STATUS_OK && pErrors != NULL)
		{
			nRetVal = xnEnumerationErrorsAdd(pErrors, &pLoadedGenerator->Description, nRetVal);
			if (nRetVal != XN_STATUS_OK)
			{
				xnNodeInfoListFree(pGeneratorList);
				return (nRetVal);
			}
		}

		xnNodeInfoListAppend(pList, pGeneratorList);
		xnNodeInfoListFree(pGeneratorList);
	}

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::CreateRootNode(XnContext* pContext, XnNodeInfo* pTree, XnModuleInstance** ppInstance)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// look for this generator
	XnLoadedGenerator* pLoaded = NULL;
	nRetVal = m_AllGenerators.Get(*xnNodeInfoGetDescription(pTree), pLoaded);
	if (nRetVal == XN_STATUS_NO_MATCH)
	{
		return XN_STATUS_NODE_NOT_LOADED;
	}
	XN_IS_STATUS_OK(nRetVal);

	// create instance holder
	XnModuleInstance* pInstance;
	XN_VALIDATE_CALLOC(pInstance, XnModuleInstance, 1);
	pInstance->pLoaded = pLoaded;

	// create an instance
	const XnChar* strInstanceName = xnNodeInfoGetInstanceName(pTree);
	const XnChar* strCreationInfo = xnNodeInfoGetCreationInfo(pTree);
	XnNodeInfoList* pNeededNodes = xnNodeInfoGetNeededNodes(pTree);
	nRetVal = pLoaded->ExportedInterface.Create(pContext, strInstanceName, strCreationInfo, pNeededNodes, pLoaded->strConfigDir, &pInstance->hNode);
	XN_IS_STATUS_OK(nRetVal);

	*ppInstance = pInstance;

	return (XN_STATUS_OK);
}

void XnModuleLoader::DestroyModuleInstance(XnModuleInstance* pInstance)
{
	pInstance->pLoaded->ExportedInterface.Destroy(pInstance->hNode);
	xnOSFree(pInstance);
}

XnStatus XnModuleLoader::ValidateFunctionGroup(const XnChar* strName, void** aFunctions, XnUInt32 nSize)
{
	XnUInt32 nNotNullCount = 0;

	for (XnUInt32 i = 0; i < nSize; ++i)
	{
		if (aFunctions[i] != NULL)
			nNotNullCount++;
	}

	if (nNotNullCount != 0 && nNotNullCount != nSize)
	{
		xnLogWarning(XN_MASK_MODULE_LOADER, "Production Node has only some of the %s methods!", strName);
		return XN_STATUS_INVALID_GENERATOR;
	}

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnRegisterModule(const XnChar* strModule, const XnChar* strConfigDir)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnChar strFullPath[XN_FILE_MAX_PATH];
	nRetVal = xnOSGetFullPathName(strModule, strFullPath, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	XnBool bExists = FALSE;
	nRetVal = xnOSDoesFileExist(strFullPath, &bExists);
	XN_IS_STATUS_OK(nRetVal);

	if (!bExists)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_OS_FILE_NOT_FOUND, XN_MASK_OPEN_NI, "File '%s' does not exist!", strFullPath);
	}

	XnChar strConfigFullPathBuffer[XN_FILE_MAX_PATH] = {0};
	const XnChar* strConfigFullPath = NULL;

	if (strConfigDir != NULL)
	{
		nRetVal = xnOSGetFullPathName(strConfigDir, strConfigFullPathBuffer, XN_FILE_MAX_PATH);
		XN_IS_STATUS_OK(nRetVal);
		strConfigFullPath = strConfigFullPathBuffer;

		bExists = FALSE;
		nRetVal = xnOSDoesDirecotyExist(strConfigFullPath, &bExists);
		XN_IS_STATUS_OK(nRetVal);

		if (!bExists)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_OS_FILE_NOT_FOUND, XN_MASK_OPEN_NI, "Config directory '%s' does not exist!", strConfigFullPath);
		}
	}

	// TODO: try to load it to make sure its valid

	TiXmlDocument doc;
	nRetVal = loadModulesFile(doc);
	XN_IS_STATUS_OK(nRetVal);

	// check if it's already there
	XnBool bFound = FALSE;
	TiXmlElement* pModule = doc.RootElement()->FirstChildElement(XN_MODULE_ELEMENT_NAME);
	while (pModule != NULL)
	{
		const XnChar* strPath;
		nRetVal = xnXmlReadStringAttribute(pModule, "path", &strPath);
		XN_IS_STATUS_OK(nRetVal);

		if (strcmp(strPath, strFullPath) == 0)
		{
			bFound = TRUE;
			break;
		}

		pModule = pModule->NextSiblingElement(XN_MODULE_ELEMENT_NAME);
	}

	if (!bFound)
	{
		// Add it
		TiXmlElement newElem(XN_MODULE_ELEMENT_NAME);
		newElem.SetAttribute("path", strFullPath);

		if (strConfigDir != NULL)
		{
			newElem.SetAttribute("configDir", strConfigFullPath);
		}

		doc.RootElement()->InsertEndChild(newElem);

		nRetVal = saveModulesFile(doc);
		XN_IS_STATUS_OK(nRetVal);
	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUnregisterModule(const XnChar* strModule)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnChar strFullPath[XN_FILE_MAX_PATH];
	nRetVal = xnOSGetFullPathName(strModule, strFullPath, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	TiXmlDocument doc;
	nRetVal = loadModulesFile(doc);
	XN_IS_STATUS_OK(nRetVal);

	// find this module
	TiXmlElement* pModule = doc.RootElement()->FirstChildElement(XN_MODULE_ELEMENT_NAME);
	while (pModule != NULL)
	{
		const XnChar* strPath;
		nRetVal = xnXmlReadStringAttribute(pModule, "path", &strPath);
		XN_IS_STATUS_OK(nRetVal);

		if (xnOSStrCaseCmp(strPath, strFullPath) == 0)
		{
			doc.RootElement()->RemoveChild(pModule);
			break;
		}

		pModule = pModule->NextSiblingElement(XN_MODULE_ELEMENT_NAME);
	}

	nRetVal = saveModulesFile(doc);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnPrintRegisteredModules()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnModuleLoader loader;
	loader.SetLoadingMode(XnModuleLoader::LOADING_MODE_PRINT);

	XnVersion version;
	nRetVal = xnGetVersion(&version);
	XN_IS_STATUS_OK(nRetVal);
	
	XnChar strVersion[100];
	nRetVal = xnVersionToString(&version, strVersion, 100);
	XN_IS_STATUS_OK(nRetVal);

	printf("OpenNI version is %s.\n", strVersion);
	printf("\nRegistered modules:\n\n");

	return loader.Init();
}

#if !XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
XnModuleLoader::RegisteredModulesList XnModuleLoader::sm_modulesList;

XnStatus XnModuleLoader::RegisterModule(XnOpenNIModuleInterface* pInterface, const XnChar* strConfigDir, const XnChar* strName)
{
	RegisteredModule module  = { pInterface, strConfigDir, strName };
	return sm_modulesList.AddLast(module);
}

XN_C_API XnStatus xnRegisterModuleWithOpenNI(XnOpenNIModuleInterface* pInterface, const XnChar* strConfigDir, const XnChar* strName)
{
	return XnModuleLoader::RegisterModule(pInterface, strConfigDir, strName);
}
#endif


