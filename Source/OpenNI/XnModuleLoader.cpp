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




//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnModuleLoader.h"
#include "XnInternalDefs.h"
#include <XnOpenNI.h>
#include <XnLog.h>
#include "XnXml.h"

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

//---------------------------------------------------------------------------
// XnDescriptionKeyManager class
//---------------------------------------------------------------------------
XnHashValue XnModuleLoader::XnDescriptionKeyManager::Hash(XnProductionNodeDescription const& key)
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
	return nTotalCRC % (1 << (sizeof(XnHashValue)*8)); 
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

#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	nRetVal = xnOSExpandEnvironmentStrings("%OPEN_NI_INSTALL_PATH%\\Data\\modules.xml", strFileName, nBufSize);
	XN_IS_STATUS_OK(nRetVal);
#elif (XN_PLATFORM == XN_PLATFORM_LINUX_X86 || XN_PLATFORM == XN_PLATFORM_LINUX_ARM)
	nRetVal = xnOSStrCopy(strFileName, "/var/lib/ni/modules.xml", nBufSize);
	XN_IS_STATUS_OK(nRetVal);
#else
	#error "Module Loader is not supported on this platform!"
#endif

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
XnModuleLoader::XnModuleLoader(XnContext* pContext) : m_pContext(pContext), m_loadingMode(LOADING_MODE_LOAD)
{}

XnModuleLoader::~XnModuleLoader()
{
	// free memory
	for (XnLoadedGeneratorsHash::Iterator it = m_AllGenerators.begin(); it != m_AllGenerators.end(); ++it)
	{
		xnOSFree(it.Value().strConfigDir);
		XN_DELETE(it.Value().pInterface);
	}
}

void XnModuleLoader::SetLoadingMode(LoadingMode mode)
{
	m_loadingMode = mode;
}

XnStatus XnModuleLoader::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// during loading, we change current dir to each module's dir (to help it load other DLLs).
	// we keep current dir, so that we can switch back once loading is complete.
	XnChar strWorkingDir[XN_FILE_MAX_PATH] = "";
	nRetVal = xnOSGetCurrentDir(strWorkingDir, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = LoadAllModules();
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSSetCurrentDir(strWorkingDir);
		return (nRetVal);
	}

	nRetVal = xnOSSetCurrentDir(strWorkingDir);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadAllModules()
{
	XnStatus nRetVal = XN_STATUS_OK;

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

	// set current dir to this file dir (to help it load other DLLs)
	XnChar strDir[XN_FILE_MAX_PATH];
	nRetVal = xnOSGetDirName(strFileName, strDir, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSSetCurrentDir(strDir);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_MODULE_LOADER, "Failed to load '%s' - directory doesn't exist!", strFileName);
		return (XN_STATUS_OK);
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

	XnModuleLoadPtr pLoadFunc;
	XnModuleUnloadPtr pUnloadFunc;
	XnModuleGetExportedNodesCountPtr pGetCountFunc;
	XnModuleGetExportedNodesEntryPointsPtr pGetEntryPointsFunc;
	XnModuleGetOpenNIVersionPtr pGetVersionFunc;

	// get the function pointers
	nRetVal = FindFuncAddress(strModuleFile, hLib, XN_STRINGIFY(XN_MODULE_LOAD), (XnFarProc*)&pLoadFunc);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FindFuncAddress(strModuleFile, hLib, XN_STRINGIFY(XN_MODULE_UNLOAD), (XnFarProc*)&pUnloadFunc);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FindFuncAddress(strModuleFile, hLib, XN_STRINGIFY(XN_MODULE_GET_EXPORTED_NODES_COUNT), (XnFarProc*)&pGetCountFunc);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FindFuncAddress(strModuleFile, hLib, XN_STRINGIFY(XN_MODULE_GET_EXPORTED_NODES_ENTRY_POINTS), (XnFarProc*)&pGetEntryPointsFunc);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FindFuncAddress(strModuleFile, hLib, XN_STRINGIFY(XN_MODULE_GET_OPEN_NI_VERSION), (XnFarProc*)&pGetVersionFunc);
	XN_IS_STATUS_OK(nRetVal);

	// get OpenNI Version
	XnVersion openNIVersion;
	pGetVersionFunc(&openNIVersion);

	if (m_loadingMode == LOADING_MODE_PRINT)
	{
		XnChar strOpenNIVersion[100];
		xnVersionToString(&openNIVersion, strOpenNIVersion, 100);
		printf("(compiled with OpenNI %s):\n", strOpenNIVersion);
	}

	// load Module
	nRetVal = pLoadFunc();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_MODULE_LOADER, "'%s' load function failed. Error code: 0x%x", strModuleFile, nRetVal);
		return (nRetVal);
	}

	// take the number of generators
	XnUInt32 nCount = pGetCountFunc();

	// allocate entry points array
	XnModuleGetExportedInterfacePtr* aEntryPoints;
	XN_VALIDATE_CALLOC(aEntryPoints, XnModuleGetExportedInterfacePtr, nCount);

	// fill it
	nRetVal = pGetEntryPointsFunc(aEntryPoints, nCount);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_MODULE_LOADER, "'%s' - failed to get exported nodes. Error code: 0x%x", strModuleFile, nRetVal);
		xnOSFree(aEntryPoints);
		return (nRetVal);
	}

	// now add every exported node
	for (XnUInt32 i = 0; i < nCount; ++i)
	{
		// get exported interface
		XnModuleExportedProductionNodeInterface ExportedInterface;
		aEntryPoints[i](&ExportedInterface);

		nRetVal = AddGenerator(&ExportedInterface, strConfigDir);
		if (nRetVal == XN_STATUS_INVALID_GENERATOR)
		{
			// if it failed, then this specific generator is not loaded, but the rest should be loaded anyway.
			xnLogWarning(XN_MASK_MODULE_LOADER, "Failed to add generator %d from module '%s'", i, strModuleFile);
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

XnStatus XnModuleLoader::AddGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, const XnChar* strConfigDir)
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
	XnLoadedGeneratorsHash::ConstIterator it = m_AllGenerators.end();
	if (m_AllGenerators.Find(loaded.Description, it) == XN_STATUS_OK)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_INVALID_GENERATOR, XN_MASK_MODULE_LOADER, "A Generator with the same description already exists!");
	}

	// Now load specific interface
	XnProductionNodeInterfaceContainer* pInterfaceContainer = NULL;

	nRetVal = LoadSpecificInterface(loaded.Description.Type, pExportedInterface, pInterfaceContainer);
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

XnStatus XnModuleLoader::LoadSpecificInterface(XnProductionNodeType Type, XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	switch (Type)
	{
	case XN_NODE_TYPE_DEVICE:
		nRetVal = LoadDeviceNode(pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_NODE_TYPE_DEPTH:
		nRetVal = LoadDepthGenerator(pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_NODE_TYPE_IMAGE:
		nRetVal = LoadImageGenerator(pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_NODE_TYPE_IR:
		nRetVal = LoadIRGenerator(pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_NODE_TYPE_GESTURE:
		nRetVal = LoadGestureGenerator(pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_NODE_TYPE_USER:
		nRetVal = LoadUserGenerator(pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_NODE_TYPE_HANDS:
		nRetVal = LoadHandsGenerator(pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
		break;		
	case XN_NODE_TYPE_SCENE:
		nRetVal = LoadSceneAnalyzer(pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_NODE_TYPE_AUDIO:
		nRetVal = LoadAudioGenerator(pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_NODE_TYPE_RECORDER:
		nRetVal = LoadRecorder(pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_NODE_TYPE_PLAYER:
		nRetVal = LoadPlayer(pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
		break;
	case XN_NODE_TYPE_CODEC:
		nRetVal = LoadCodec(pExportedInterface, pInterfaceContainer);
		XN_IS_STATUS_OK(nRetVal);
		break;
	default:
		XN_LOG_ERROR_RETURN(XN_STATUS_UNKNOWN_GENERATOR_TYPE, XN_MASK_MODULE_LOADER, "Unknown type: %u", Type);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadDeviceNode(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDeviceInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Device(&Interface.Device);

	// validate it
	nRetVal = ValidateDeviceInterface(&Interface.Device);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnDeviceInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnDeviceInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadDepthGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDepthGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Depth(&Interface.Depth);

	// validate it
	nRetVal = ValidateDepthGeneratorInterface(&Interface.Depth);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnDepthGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnDepthGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadImageGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnImageGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Image(&Interface.Image);

	// validate interface
	nRetVal = ValidateImageGeneratorInterface(&Interface.Image);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnImageGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnImageGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadIRGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnIRGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.IR(&Interface.IR);

	// validate interface
	nRetVal = ValidateIRGeneratorInterface(&Interface.IR);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnIRGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnIRGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadGestureGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnGestureGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Gesture(&Interface.Gesture);

	// validate interface
	nRetVal = ValidateGestureGeneratorInterface(&Interface.Gesture);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnGestureGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnGestureGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return XN_STATUS_OK;
}

XnStatus XnModuleLoader::LoadUserGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnUserGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.User(&Interface.User);

	// validate interface
	nRetVal = ValidateUserGeneratorInterface(&Interface.User);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnUserGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnUserGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return XN_STATUS_OK;
}

XnStatus XnModuleLoader::LoadHandsGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnHandsGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Hands(&Interface.Hands);

	// validate interface
	nRetVal = ValidateHandsGeneratorInterface(&Interface.Hands);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnHandsGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnHandsGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return XN_STATUS_OK;
}

XnStatus XnModuleLoader::LoadSceneAnalyzer(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnSceneAnalyzerInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Scene(&Interface.Scene);

	// validate interface
	nRetVal = ValidateSceneAnalyzerInterface(&Interface.Scene);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnSceneAnalyzerInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnSceneAnalyzerInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return XN_STATUS_OK;
}

XnStatus XnModuleLoader::LoadAudioGenerator(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnAudioGeneratorInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Audio(&Interface.Audio);

	// validate interface
	nRetVal = ValidateAudioGeneratorInterface(&Interface.Audio);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnAudioGeneratorInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnAudioGeneratorInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadRecorder(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnRecorderInterfaceContainer interface;

	// fill it up
	pExportedInterface->GetInterface.Recorder(&interface.recorder);

	// validate interface
	nRetVal = ValidateRecorderInterface(&interface.recorder);
	XN_IS_STATUS_OK(nRetVal);

	/*interface.recorder.pNodeNotifications points to interface.nodeNotifications,
	  so interface.nodeNotifications is already set. */
	nRetVal = ValidateNodeNotifications(&interface.nodeNotifications);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnRecorderInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnRecorderInterfaceContainer);
	*pContainer = interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadPlayer(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnPlayerInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Player(&Interface.Player);

	// validate interface
	nRetVal = ValidatePlayerInterface(&Interface.Player);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnPlayerInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnPlayerInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::LoadCodec(XnModuleExportedProductionNodeInterface* pExportedInterface, XnProductionNodeInterfaceContainer*& pInterfaceContainer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnCodecInterfaceContainer Interface;

	// fill it up
	pExportedInterface->GetInterface.Codec(&Interface.Codec);

	// validate interface
	nRetVal = ValidateCodecInterface(&Interface.Codec);
	XN_IS_STATUS_OK(nRetVal);

	// everything is OK. Allocate and store it
	XnCodecInterfaceContainer* pContainer;
	XN_VALIDATE_NEW(pContainer, XnCodecInterfaceContainer);
	*pContainer = Interface;

	pInterfaceContainer = pContainer;

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateProductionNodeInterface(XnModuleProductionNodeInterface* pInterface)
{
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, IsCapabilitySupported);

	// validate extended serialization capability
	if (pInterface->pExtendedSerializationInterface->InitNotifications != NULL)
	{
		// check the rest of the extended serialization functions are present
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pExtendedSerializationInterface, StopNotifications);
	}

	// validate lock aware capability
	if (pInterface->pLockAwareInterface->SetLockState != NULL)
	{
		// check the rest of the functions are present
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pLockAwareInterface, GetLockState);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pLockAwareInterface, RegisterToLockChange);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pLockAwareInterface, UnregisterFromLockChange);
	}

	// validate error state capability
	if (pInterface->pErrorStateInterface->GetErrorState != NULL)
	{
		// check the rest of the functions are present
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pErrorStateInterface, RegisterToErrorStateChange);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pErrorStateInterface, UnregisterFromErrorStateChange);
	}

	// we allow general set / get functions to be NULL.
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateDeviceInterface(XnModuleDeviceInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = ValidateProductionNodeInterface(pInterface->pProductionNode);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateGeneratorInterface(XnModuleGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ValidateProductionNodeInterface(pInterface->pProductionNodeInterface);
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_FUNC_NOT_NULL(pInterface, StartGenerating);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, StopGenerating);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterToGenerationRunningChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterFromGenerationRunningChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterToNewDataAvailable);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterFromNewDataAvailable);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, IsNewDataAvailable);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UpdateData);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetDataSize);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetTimestamp);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetFrameID);

	// validate mirror capability
	if (pInterface->pMirrorInterface->SetMirror != NULL)
	{
		// check the rest of the mirror functions are present
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pMirrorInterface, IsMirrored);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pMirrorInterface, RegisterToMirrorChange);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pMirrorInterface, UnregisterFromMirrorChange);
	}

	// validate alternative view point capability
	if (pInterface->pAlternativeViewPointInterface->IsViewPointSupported != NULL)
	{
		// check the rest of the alternative view point functions are present
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pAlternativeViewPointInterface, SetViewPoint);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pAlternativeViewPointInterface, IsViewPointAs);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pAlternativeViewPointInterface, ResetViewPoint);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pAlternativeViewPointInterface, RegisterToViewPointChange);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pAlternativeViewPointInterface, UnregisterFromViewPointChange);
	}

	// validate seeking capability
	if (pInterface->pSeekingInterface->GetMinTimestamp != NULL)
	{
		// check the rest of the seeking functions are present
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSeekingInterface, GetMaxTimestamp);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSeekingInterface, SeekToTimestamp);
	}

	// validate frame sync capability
	if (pInterface->pFrameSyncInterface->CanFrameSyncWith != NULL)
	{
		// check the rest of the functions are present
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pFrameSyncInterface, FrameSyncWith);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pFrameSyncInterface, StopFrameSyncWith);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pFrameSyncInterface, IsFrameSyncedWith);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pFrameSyncInterface, RegisterToFrameSyncChange);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pFrameSyncInterface, UnregisterFromFrameSyncChange);
	}

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateMapGeneratorInterface(XnModuleMapGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ValidateGeneratorInterface(pInterface->pGeneratorInterface);
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetSupportedMapOutputModes);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, SetMapOutputMode);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetMapOutputMode);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterToMapOutputModeChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterFromMapOutputModeChange);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateDepthGeneratorInterface(XnModuleDepthGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// validate base
	nRetVal = ValidateMapGeneratorInterface(pInterface->pMapInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetDeviceMaxDepth);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetFieldOfView);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterToFieldOfViewChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterFromFieldOfViewChange);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetDepthMap);

	// now check capabilities
	if (pInterface->pUserPositionInterface->GetSupportedUserPositionsCount != NULL)
	{
		// make sure the rest of the functions are implemented
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pUserPositionInterface, SetUserPosition);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pUserPositionInterface, GetUserPosition);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pUserPositionInterface, RegisterToUserPositionChange);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pUserPositionInterface, UnregisterFromUserPositionChange);
	}
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateImageGeneratorInterface(XnModuleImageGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// validate base
	nRetVal = ValidateMapGeneratorInterface(pInterface->pMapInterface);
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

XnStatus XnModuleLoader::ValidateIRGeneratorInterface(XnModuleIRGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// validate base
	nRetVal = ValidateMapGeneratorInterface(pInterface->pMapInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetIRMap);
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateGestureGeneratorInterface(XnModuleGestureGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate base
	nRetVal = ValidateGeneratorInterface(pInterface->pGeneratorInterface);
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

XnStatus XnModuleLoader::ValidateSceneAnalyzerInterface(XnModuleSceneAnalyzerInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate base
	nRetVal = ValidateMapGeneratorInterface(pInterface->pMapInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetLabelMap);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetFloor);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateUserGeneratorInterface(XnModuleUserGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate base
	nRetVal = ValidateGeneratorInterface(pInterface->pGeneratorInterface);
	XN_IS_STATUS_OK(nRetVal);

	// validate functions
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetNumberOfUsers);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetUsers);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetCoM);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetUserPixels);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, RegisterUserCallbacks);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, UnregisterUserCallbacks);

	// now check capabilities

	if (pInterface->pSkeletonInterface->IsJointAvailable != NULL)
	{
		// make sure the rest of the functions are implemented
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, IsProfileAvailable);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, SetSkeletonProfile);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, SetJointActive);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, IsJointActive);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, RegisterToJointConfigurationChange);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, UnregisterFromJointConfigurationChange);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, EnumerateActiveJoints);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, GetSkeletonJoint);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, GetSkeletonJointPosition);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, GetSkeletonJointOrientation);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, IsTracking);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, IsCalibrated);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, IsCalibrating);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, RequestCalibration);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, AbortCalibration);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, SaveCalibrationData);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, LoadCalibrationData);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, ClearCalibrationData);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, IsCalibrationData);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, StartTracking);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, StopTracking);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, Reset);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, NeedPoseForCalibration);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, GetCalibrationPose);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, SetSmoothing);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, RegisterCalibrationCallbacks);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pSkeletonInterface, UnregisterCalibrationCallbacks);
	}

	if (pInterface->pPoseDetectionInteface->GetNumberOfPoses != NULL)
	{
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pPoseDetectionInteface, GetNumberOfPoses);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pPoseDetectionInteface, GetAvailablePoses);

		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pPoseDetectionInteface, StartPoseDetection);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pPoseDetectionInteface, StopPoseDetection);

		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pPoseDetectionInteface, RegisterToPoseCallbacks);
		XN_VALIDATE_FUNC_NOT_NULL(pInterface->pPoseDetectionInteface, UnregisterFromPoseCallbacks);
	}

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateHandsGeneratorInterface(XnModuleHandsGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate base
	nRetVal = ValidateGeneratorInterface(pInterface->pGeneratorInterface);
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

XnStatus XnModuleLoader::ValidateAudioGeneratorInterface(XnModuleAudioGeneratorInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate base
	nRetVal = ValidateGeneratorInterface(pInterface->pGeneratorInterface);
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

XnStatus XnModuleLoader::ValidateRecorderInterface(XnModuleRecorderInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, SetOutputStream);

	// validate bases
	nRetVal = ValidateProductionNodeInterface(pInterface->pProductionNode);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ValidateNodeNotifications(pInterface->pNodeNotifications);
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidatePlayerInterface(XnModulePlayerInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	//validate functions
	nRetVal = ValidateProductionNodeInterface(pInterface->pProductionNode);
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

XnStatus XnModuleLoader::ValidateCodecInterface(XnModuleCodecInterface* pInterface)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = ValidateProductionNodeInterface(pInterface->pProductionNode);
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_FUNC_NOT_NULL(pInterface, GetCodecID);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, Init);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, CompressData);
	XN_VALIDATE_FUNC_NOT_NULL(pInterface, DecompressData);

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::ValidateNodeNotifications(XnNodeNotifications* pNodeNotifications)
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

XnStatus XnModuleLoader::Enumerate(XnProductionNodeType Type, XnNodeInfoList* pList, XnEnumerationErrors* pErrors)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	for (XnLoadedGeneratorsHash::ConstIterator it = m_AllGenerators.begin(); it != m_AllGenerators.end(); ++it)
	{
		const XnLoadedGenerator& LoadedGenerator = it.Value();
		
		// check if it's of the same type and it's not a mock node
		if (LoadedGenerator.Description.Type == Type)
		{
			XnNodeInfoList* pGeneratorList = NULL;
			nRetVal = xnNodeInfoListAllocate(&pGeneratorList);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = LoadedGenerator.ExportedInterface.EnumerateProductionTrees(m_pContext, pGeneratorList, pErrors);
			if (nRetVal != XN_STATUS_OK && pErrors != NULL)
			{
				nRetVal = xnEnumerationErrorsAdd(pErrors, &LoadedGenerator.Description, nRetVal);
				if (nRetVal != XN_STATUS_OK)
				{
					xnNodeInfoListFree(pGeneratorList);
					return (nRetVal);
				}
			}

			xnNodeInfoListAppend(pList, pGeneratorList);
			xnNodeInfoListFree(pGeneratorList);
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnModuleLoader::CreateRootNode(XnNodeInfo* pTree, XnModuleInstance** ppInstance)
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
	nRetVal = pLoaded->ExportedInterface.Create(m_pContext, strInstanceName, strCreationInfo, pNeededNodes, pLoaded->strConfigDir, &pInstance->hNode);
	XN_IS_STATUS_OK(nRetVal);

	*ppInstance = pInstance;

	return (XN_STATUS_OK);
}

void XnModuleLoader::DestroyModuleInstance(XnModuleInstance* pInstance)
{
	pInstance->pLoaded->ExportedInterface.Destroy(pInstance->hNode);
	xnOSFree(pInstance);
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

	XnModuleLoader loader(NULL);
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
