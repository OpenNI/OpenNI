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
#ifndef __XN_MODULE_CPP_REGISTRATION_H__
#define __XN_MODULE_CPP_REGISTRATION_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnModuleCppInterface.h"
#include <XnUtils.h>

using namespace xn;

//---------------------------------------------------------------------------
// Internal Macros
//---------------------------------------------------------------------------
/** Defines module instance name. */
#define _XN_MODULE_INST g_pTheModule

#define _CONCAT(a,b) a##b

inline XnModuleNodeHandle __ModuleNodeToHandle(xn::ModuleProductionNode* pNode)
{
	return ((XnModuleNodeHandle)pNode);
}

#define __XN_EXPORT_NODE_COMMON(ExportedClass, ExportedName, Type)											\
	/** Create a static global instance. */																	\
	static ExportedClass* ExportedName = new ExportedClass();												\
																											\
	void XN_CALLBACK_TYPE _CONCAT(ExportedClass,GetDescription)(XnProductionNodeDescription* pDescription)	\
	{																										\
		ExportedName->GetDescription(pDescription);															\
	}																										\
																											\
	XnStatus XN_CALLBACK_TYPE _CONCAT(ExportedClass,EnumerateProductionTrees)								\
		(XnContext* pContext, XnNodeInfoList* pTreesList, XnEnumerationErrors* pErrors)						\
	{																										\
		Context context(pContext);																			\
		NodeInfoList list(pTreesList);																		\
		EnumerationErrors errors(pErrors);																	\
		return ExportedName->EnumerateProductionTrees(context, list, pErrors == NULL ? NULL : &errors);		\
	}																										\
																											\
	XnStatus XN_CALLBACK_TYPE _CONCAT(ExportedClass,Create)(XnContext* pContext,							\
		const XnChar* strInstanceName,																		\
		const XnChar* strCreationInfo,																		\
		XnNodeInfoList* pNeededTrees,																		\
		const XnChar* strConfigurationDir,																	\
		XnModuleNodeHandle* phInstance)																		\
	{																										\
		xn::NodeInfoList* pNeeded = NULL;																	\
		if (pNeededTrees != NULL)																			\
		{																									\
			pNeeded = XN_NEW(xn::NodeInfoList, pNeededTrees);												\
		}																									\
		ModuleProductionNode* pNode;																		\
		Context context(pContext);																			\
		XnStatus nRetVal = ExportedName->Create(context, strInstanceName, strCreationInfo,					\
			pNeeded, strConfigurationDir, &pNode);															\
		if (nRetVal != XN_STATUS_OK)																		\
		{																									\
			XN_DELETE(pNeeded);																				\
			return (nRetVal);																				\
		}																									\
		*phInstance = __ModuleNodeToHandle(pNode);															\
		XN_DELETE(pNeeded);																					\
		return (XN_STATUS_OK);																				\
	}																										\
																											\
	void XN_CALLBACK_TYPE _CONCAT(ExportedClass,Destroy)(XnModuleNodeHandle hInstance)						\
	{																										\
		ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;										\
		ExportedName->Destroy(pNode);																		\
	}																										\
																											\
    void XN_CALLBACK_TYPE _CONCAT(ExportedClass,GetExportedInterface)(										\
		XnModuleExportedProductionNodeInterface* pInterface)												\
	{																										\
		pInterface->GetDescription = _CONCAT(ExportedClass,GetDescription);									\
		pInterface->EnumerateProductionTrees = _CONCAT(ExportedClass,EnumerateProductionTrees);				\
		pInterface->Create = _CONCAT(ExportedClass,Create);													\
		pInterface->Destroy = _CONCAT(ExportedClass,Destroy);												\
		pInterface->GetInterface.General = __ModuleGetGetInterfaceFunc(Type);								\
	}																										\
																											\
	static XnStatus _CONCAT(ExportedClass,RegisterResult) =													\
		_XN_MODULE_INST->AddExportedNode(_CONCAT(ExportedClass,GetExportedInterface));

#define _XN_EXPORT_NODE_COMMON(ExportedClass, Type)						\
	__XN_EXPORT_NODE_COMMON(ExportedClass, _g_##ExportedClass, Type)

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

void XN_CALLBACK_TYPE __ModuleGetProductionNodeInterface(XnModuleProductionNodeInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetDeviceInterface(XnModuleDeviceInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetGeneratorInterface(XnModuleGeneratorInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetMapGeneratorInterface(XnModuleMapGeneratorInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetDepthGeneratorInterface(XnModuleDepthGeneratorInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetImageGeneratorInterface(XnModuleImageGeneratorInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetIRGeneratorInterface(XnModuleIRGeneratorInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetUserGeneratorInterface(XnModuleUserGeneratorInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetHandsGeneratorInterface(XnModuleHandsGeneratorInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetGestureGeneratorInterface(XnModuleGestureGeneratorInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetSceneAnalyzerInterface(XnModuleSceneAnalyzerInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetAudioGeneratorInterface(XnModuleAudioGeneratorInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetRecorderInterface(XnModuleRecorderInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetPlayerInterface(XnModulePlayerInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetCodecInterface(XnModuleCodecInterface* pInterface);
void XN_CALLBACK_TYPE __ModuleGetScriptNodeInterface(XnModuleScriptNodeInterface* pInterface);

//---------------------------------------------------------------------------
// Utility Macros
//---------------------------------------------------------------------------

typedef void (XN_CALLBACK_TYPE *GetInterfaceFuncPtr)(void* pInterface);

static GetInterfaceFuncPtr __ModuleGetGetInterfaceFunc(XnProductionNodeType type)
{
	// start with concrete type
	if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_DEVICE))
		return (GetInterfaceFuncPtr)__ModuleGetDeviceInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_DEPTH))
		return (GetInterfaceFuncPtr)__ModuleGetDepthGeneratorInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_IMAGE))
		return (GetInterfaceFuncPtr)__ModuleGetImageGeneratorInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_IR))
		return (GetInterfaceFuncPtr)__ModuleGetIRGeneratorInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_USER))
		return (GetInterfaceFuncPtr)__ModuleGetUserGeneratorInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_GESTURE))
		return (GetInterfaceFuncPtr)__ModuleGetGestureGeneratorInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_SCENE))
		return (GetInterfaceFuncPtr)__ModuleGetSceneAnalyzerInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_AUDIO))
		return (GetInterfaceFuncPtr)__ModuleGetAudioGeneratorInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_RECORDER))
		return (GetInterfaceFuncPtr)__ModuleGetRecorderInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_PLAYER))
		return (GetInterfaceFuncPtr)__ModuleGetPlayerInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_HANDS))
		return (GetInterfaceFuncPtr)__ModuleGetHandsGeneratorInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_CODEC))
		return (GetInterfaceFuncPtr)__ModuleGetCodecInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_SCRIPT))
		return (GetInterfaceFuncPtr)__ModuleGetScriptNodeInterface;
	// and continue with abstract ones
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_MAP_GENERATOR))
		return (GetInterfaceFuncPtr)__ModuleGetMapGeneratorInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_GENERATOR))
		return (GetInterfaceFuncPtr)__ModuleGetGeneratorInterface;
	else if (xnIsTypeDerivedFrom(type, XN_NODE_TYPE_PRODUCTION_NODE))
		return (GetInterfaceFuncPtr)__ModuleGetProductionNodeInterface;

	// unknown
	XN_ASSERT(FALSE);
	return NULL;
}

/** Exports an OpenNI module from the DLL. */
#if XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
	#define XN_EXPORT_MODULE(ModuleClass)												 \
		/** Declare the static variable */												 \
		ModuleClass __moduleInstance;                                             		 \
		Module* _XN_MODULE_INST = &__moduleInstance;
#else
	#define XN_EXPORT_MODULE(ModuleClass)												 \
		/** Declare the static variable */												 \
		static ModuleClass __moduleInstance;                                             \
		static Module* _XN_MODULE_INST = &__moduleInstance;
#endif

/** Exports a node from the DLL. */
#define XN_EXPORT_NODE(ExportedClass, nodeType)								\
	_XN_EXPORT_NODE_COMMON(ExportedClass, nodeType)

/** Exports a device node from the DLL. */
#define XN_EXPORT_DEVICE(ExportedClass)										\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_DEVICE)

/** Exports a depth generator from the DLL. */
#define XN_EXPORT_DEPTH(ExportedClass)										\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_DEPTH)

/** Exports an image generator from the DLL. */
#define XN_EXPORT_IMAGE(ExportedClass)										\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_IMAGE)

/** Exports an IR generator from the DLL. */
#define XN_EXPORT_IR(ExportedClass)											\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_IR)

#define XN_EXPORT_USER(ExportedClass)										\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_USER)

#define XN_EXPORT_HANDS(ExportedClass)										\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_HANDS)

#define XN_EXPORT_GESTURE(ExportedClass)									\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_GESTURE)

#define XN_EXPORT_SCENE(ExportedClass)										\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_SCENE)

/** Exports an Audio generator from the DLL. */
#define XN_EXPORT_AUDIO(ExportedClass)										\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_AUDIO)

/** Exports a Recorder from the DLL. */
#define XN_EXPORT_RECORDER(ExportedClass)									\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_RECORDER)

/** Exports a Player from the DLL. */
#define XN_EXPORT_PLAYER(ExportedClass)										\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_PLAYER)

#define XN_EXPORT_CODEC(ExportedClass)										\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_CODEC)

#define XN_EXPORT_SCRIPT(ExportedClass)										\
	_XN_EXPORT_NODE_COMMON(ExportedClass, XN_NODE_TYPE_SCRIPT)

//---------------------------------------------------------------------------
// Exported C functions
//---------------------------------------------------------------------------
#if XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
	#include <XnModuleCFunctions.h>
	#define XN_MODULE_FUNC_TYPE XN_C_API_EXPORT
	extern Module* _XN_MODULE_INST;
#else
	#define XN_MODULE_FUNC_TYPE static
	static Module* _XN_MODULE_INST;
#endif

XN_MODULE_FUNC_TYPE XnStatus XN_C_DECL XN_MODULE_LOAD()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = _XN_MODULE_INST->Load();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_MODULE_FUNC_TYPE void XN_C_DECL XN_MODULE_UNLOAD()
{
	_XN_MODULE_INST->Unload();
}

XN_MODULE_FUNC_TYPE XnUInt32 XN_C_DECL XN_MODULE_GET_EXPORTED_NODES_COUNT()
{
	return _XN_MODULE_INST->GetExportedNodesCount();
}

XN_MODULE_FUNC_TYPE XnStatus XN_C_DECL XN_MODULE_GET_EXPORTED_NODES_ENTRY_POINTS(XnModuleGetExportedInterfacePtr* aEntryPoints, XnUInt32 nCount)
{
	return _XN_MODULE_INST->GetExportedNodes(aEntryPoints, nCount);
}

XN_MODULE_FUNC_TYPE void XN_C_DECL XN_MODULE_GET_OPEN_NI_VERSION(XnVersion* pVersion)
{
	pVersion->nMajor = XN_MAJOR_VERSION;
	pVersion->nMinor = XN_MINOR_VERSION;
	pVersion->nMaintenance = XN_MAINTENANCE_VERSION;
	pVersion->nBuild = XN_BUILD_VERSION;
}

#if !XN_PLATFORM_SUPPORTS_DYNAMIC_LIBS
#include <XnUtils.h>

static XnOpenNIModuleInterface moduleInterface = 
{
	XN_MODULE_LOAD,
	XN_MODULE_UNLOAD,
	XN_MODULE_GET_EXPORTED_NODES_COUNT,
	XN_MODULE_GET_EXPORTED_NODES_ENTRY_POINTS,
	XN_MODULE_GET_OPEN_NI_VERSION
};
static XnStatus registerResult = xnRegisterModuleWithOpenNI(&moduleInterface, NULL, __FILE__);
#endif

#endif // __XN_MODULE_CPP_REGISTRATION_H__
