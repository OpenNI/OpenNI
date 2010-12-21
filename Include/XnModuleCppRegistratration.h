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




#ifndef __XN_MODULE_CPP_REGISTRATION_H__
#define __XN_MODULE_CPP_REGISTRATION_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnModuleCppInterface.h"

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

#define __XN_EXPORT_NODE_COMMON(ExportedClass, ExportedName, Type, GetInterfaceFunc)						\
	/** Create a static global instance. */																	\
	static ExportedClass ExportedName;																		\
																											\
	void XN_CALLBACK_TYPE _CONCAT(ExportedClass,GetDescription)(XnProductionNodeDescription* pDescription)	\
	{																										\
		ExportedName.GetDescription(pDescription);															\
	}																										\
																											\
	XnStatus XN_CALLBACK_TYPE _CONCAT(ExportedClass,EnumerateProductionTrees)								\
		(XnContext* pContext, XnNodeInfoList* pTreesList, XnEnumerationErrors* pErrors)						\
	{																										\
		Context context(pContext);																			\
		NodeInfoList list(pTreesList);																		\
		EnumerationErrors errors(pErrors);																	\
		return ExportedName.EnumerateProductionTrees(context, list, pErrors == NULL ? NULL : &errors);		\
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
		XnStatus nRetVal = ExportedName.Create(context, strInstanceName, strCreationInfo,					\
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
		ExportedName.Destroy(pNode);																		\
	}																										\
																											\
    void XN_CALLBACK_TYPE _CONCAT(ExportedClass,GetExportedInterface)(										\
		XnModuleExportedProductionNodeInterface* pInterface)												\
	{																										\
		pInterface->GetDescription = _CONCAT(ExportedClass,GetDescription);									\
		pInterface->EnumerateProductionTrees = _CONCAT(ExportedClass,EnumerateProductionTrees);				\
		pInterface->Create = _CONCAT(ExportedClass,Create);													\
		pInterface->Destroy = _CONCAT(ExportedClass,Destroy);												\
		pInterface->GetInterface.Type = GetInterfaceFunc;													\
	}																										\
																											\
	static XnStatus _CONCAT(ExportedClass,RegisterResult) =													\
		_XN_MODULE_INST->AddExportedNode(_CONCAT(ExportedClass,GetExportedInterface));

#define _XN_EXPORT_NODE_COMMON(ExportedClass, Type, GetInterfaceFunc)						\
	__XN_EXPORT_NODE_COMMON(ExportedClass, _g_##ExportedClass, Type, GetInterfaceFunc)

#define _XN_VALIDATE_CAPABILITY_INTERFACE_RET(capInterface, retVal)		\
	if (capInterface == NULL)											\
		return retVal;

#define _XN_VALIDATE_CAPABILITY_INTERFACE(capInterface)		\
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(capInterface, XN_STATUS_INVALID_OPERATION)

//---------------------------------------------------------------------------
// Utility Macros
//---------------------------------------------------------------------------

/** Exports an OpenNI module from the DLL. */
#define XN_EXPORT_MODULE(ModuleClass)												 \
	/** Declare the static variable */												 \
	static ModuleClass __moduleInstance;                                             \
	Module* _XN_MODULE_INST = &__moduleInstance;							 

/** Exports a device node from the DLL. */
#define XN_EXPORT_DEVICE(ExportedClass)													\
	_XN_EXPORT_NODE_COMMON(ExportedClass, Device, __ModuleGetDeviceInterface)

/** Exports a depth generator from the DLL. */
#define XN_EXPORT_DEPTH(ExportedClass)													\
	_XN_EXPORT_NODE_COMMON(ExportedClass, Depth, __ModuleGetDepthGeneratorInterface)

/** Exports an image generator from the DLL. */
#define XN_EXPORT_IMAGE(ExportedClass)													\
	_XN_EXPORT_NODE_COMMON(ExportedClass, Image, __ModuleGetImageGeneratorInterface)

/** Exports an IR generator from the DLL. */
#define XN_EXPORT_IR(ExportedClass)														\
	_XN_EXPORT_NODE_COMMON(ExportedClass, IR, __ModuleGetIRGeneratorInterface)

#define XN_EXPORT_USER(ExportedClass)													\
	_XN_EXPORT_NODE_COMMON(ExportedClass, User, __ModuleGetUserGeneratorInterface)

#define XN_EXPORT_HANDS(ExportedClass)													\
	_XN_EXPORT_NODE_COMMON(ExportedClass, Hands, __ModuleGetHandsGeneratorInterface)

#define XN_EXPORT_GESTURE(ExportedClass)												\
	_XN_EXPORT_NODE_COMMON(ExportedClass, Gesture, __ModuleGetGestureGeneratorInterface)

#define XN_EXPORT_SCENE(ExportedClass)													\
	_XN_EXPORT_NODE_COMMON(ExportedClass, Scene, __ModuleGetSceneAnalyzerInterface)

/** Exports an Audio generator from the DLL. */
#define XN_EXPORT_AUDIO(ExportedClass)													\
	_XN_EXPORT_NODE_COMMON(ExportedClass, Audio, __ModuleGetAudioGeneratorInterface)

/** Exports a Recorder from the DLL. */
#define XN_EXPORT_RECORDER(ExportedClass)												\
	_XN_EXPORT_NODE_COMMON(ExportedClass, Recorder, __ModuleGetRecorderInterface)

/** Exports a Player from the DLL. */
#define XN_EXPORT_PLAYER(ExportedClass)													\
	_XN_EXPORT_NODE_COMMON(ExportedClass, Player, __ModuleGetPlayerInterface)

#define XN_EXPORT_CODEC(ExportedClass)													\
	_XN_EXPORT_NODE_COMMON(ExportedClass, Codec, __ModuleGetCodecInterface)

//---------------------------------------------------------------------------
// Exported C functions
//---------------------------------------------------------------------------
extern Module* _XN_MODULE_INST;

XN_C_API_EXPORT XnStatus XN_C_DECL XN_MODULE_LOAD()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	nRetVal = _XN_MODULE_INST->Load();
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_C_API_EXPORT void XN_C_DECL XN_MODULE_UNLOAD()
{
	_XN_MODULE_INST->Unload();
}

XN_C_API_EXPORT XnUInt32 XN_C_DECL XN_MODULE_GET_EXPORTED_NODES_COUNT()
{
	return _XN_MODULE_INST->GetExportedNodesCount();
}

XN_C_API_EXPORT XnStatus XN_C_DECL XN_MODULE_GET_EXPORTED_NODES_ENTRY_POINTS(XnModuleGetExportedInterfacePtr* aEntryPoints, XnUInt32 nCount)
{
	return _XN_MODULE_INST->GetExportedNodes(aEntryPoints, nCount);
}

XN_C_API_EXPORT void XN_C_DECL XN_MODULE_GET_OPEN_NI_VERSION(XnVersion* pVersion)
{
	pVersion->nMajor = XN_MAJOR_VERSION;
	pVersion->nMinor = XN_MINOR_VERSION;
	pVersion->nMaintenance = XN_MAINTENANCE_VERSION;
	pVersion->nBuild = XN_BUILD_VERSION;
}

XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsCapabilitySupported(XnModuleNodeHandle hInstance, const XnChar* strCapabilityName)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	return pNode->IsCapabilitySupported(strCapabilityName);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleInitNotifications(XnModuleNodeHandle hInstance, XnNodeNotifications* pNotifications, void* pCookie)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	ModuleExtendedSerializationInterface* pInterface = pNode->GetExtendedSerializationInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->NotifyExState(pNotifications, pCookie);
}
XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleStopNotifications(XnModuleNodeHandle hInstance)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	ModuleExtendedSerializationInterface* pInterface = pNode->GetExtendedSerializationInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface,);
	pInterface->UnregisterExNotifications();
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetLockState(XnModuleNodeHandle hInstance, XnBool bLocked)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	ModuleLockAwareInterface* pInterface = pNode->GetLockAwareInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->SetLockState(bLocked);
}

XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleGetLockState(XnModuleNodeHandle hInstance)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	ModuleLockAwareInterface* pInterface = pNode->GetLockAwareInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	return pInterface->GetLockState();
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToLockChange(XnModuleNodeHandle hInstance, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	ModuleLockAwareInterface* pInterface = pNode->GetLockAwareInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->RegisterToLockChange(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromLockChange(XnModuleNodeHandle hInstance, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	ModuleLockAwareInterface* pInterface = pNode->GetLockAwareInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface,);
	pInterface->UnregisterFromLockChange(hCallback);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetErrorState(XnModuleNodeHandle hInstance)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	ModuleErrorStateInterface* pInterface = pNode->GetErrorStateInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, XN_STATUS_OK);
	return pInterface->GetErrorState();
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToErrorStateChange(XnModuleNodeHandle hInstance, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	ModuleErrorStateInterface* pInterface = pNode->GetErrorStateInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->RegisterToErrorStateChange(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromErrorStateChange(XnModuleNodeHandle hInstance, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	ModuleErrorStateInterface* pInterface = pNode->GetErrorStateInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface,);
	pInterface->UnregisterFromErrorStateChange(hCallback);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetIntProperty(XnModuleNodeHandle hInstance, const XnChar* strName, XnUInt64 nValue)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	return pNode->SetIntProperty(strName, nValue);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetRealProperty(XnModuleNodeHandle hInstance, const XnChar* strName, XnDouble dValue)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	return pNode->SetRealProperty(strName, dValue);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetStringProperty(XnModuleNodeHandle hInstance, const XnChar* strName, const XnChar* strValue)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	return pNode->SetStringProperty(strName, strValue);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetGeneralProperty(XnModuleNodeHandle hInstance, const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	return pNode->SetGeneralProperty(strName, nBufferSize, pBuffer);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetIntProperty(XnModuleNodeHandle hInstance, const XnChar* strName, XnUInt64* pnValue)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	return pNode->GetIntProperty(strName, *pnValue);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetRealProperty(XnModuleNodeHandle hInstance, const XnChar* strName, XnDouble* pdValue)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	return pNode->GetRealProperty(strName, *pdValue);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetStringProperty(XnModuleNodeHandle hInstance, const XnChar* strName, XnChar* csValue, XnUInt32 nBufSize)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	return pNode->GetStringProperty(strName, csValue, nBufSize);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetGeneralProperty(XnModuleNodeHandle hInstance, const XnChar* strName, XnUInt32 nBufferSize, void* pBuffer)
{
	ModuleProductionNode* pNode = (ModuleProductionNode*)hInstance;
	return pNode->GetGeneralProperty(strName, nBufferSize, pBuffer);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetOutputStream(XnModuleNodeHandle hInstance, void *pCookie, XnRecorderOutputStreamInterface *pStream)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hInstance;
	ModuleRecorder* pNode = dynamic_cast<ModuleRecorder*>(pProdNode);
	return pNode->SetOutputStream(pCookie, pStream);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetInputStream(XnModuleNodeHandle hInstance, void *pStreamCookie, XnPlayerInputStreamInterface *pStream)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hInstance;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	return pNode->SetInputStream(pStreamCookie, pStream);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleReadNext(XnModuleNodeHandle hInstance)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hInstance;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	return pNode->ReadNext();
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetRepeat(XnModuleNodeHandle hInstance, XnBool bRepeat)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hInstance;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	return pNode->SetRepeat(bRepeat);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSeekToTimeStamp(XnModuleNodeHandle hInstance, XnInt64 nTimeOffset, XnPlayerSeekOrigin origin)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hInstance;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	return pNode->SeekToTimeStamp(nTimeOffset, origin);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSeekToFrame(XnModuleNodeHandle hInstance, const XnChar* strNodeName, XnInt32 nFrameOffset, XnPlayerSeekOrigin origin)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hInstance;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	return pNode->SeekToFrame(strNodeName, nFrameOffset, origin);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleTellTimestamp(XnModuleNodeHandle hInstance, XnUInt64* pnTimestamp)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hInstance;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	return pNode->TellTimestamp(*pnTimestamp);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleTellFrame(XnModuleNodeHandle hInstance, const XnChar* strNodeName, XnUInt32* pnFrame)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hInstance;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	return pNode->TellFrame(strNodeName, *pnFrame);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetNumFrames(XnModuleNodeHandle hInstance, const XnChar* strNodeName, XnUInt32* pnFrames)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hInstance;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	return pNode->GetNumFrames(strNodeName, *pnFrames);
}

XN_C_API_EXPORT const XnChar* XN_CALLBACK_TYPE __ModuleGetSupportedFormat(XnModuleNodeHandle hInstance)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hInstance;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	return pNode->GetSupportedFormat();
}

XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsEOF(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	return pNode->IsEOF();
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToEndOfFileReached(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	return pNode->RegisterToEndOfFileReached(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromEndOfFileReached(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	pNode->UnregisterFromEndOfFileReached(hCallback);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetNodeNotifications(XnModuleNodeHandle hInstance, void *pNodeNotificationsCookie, XnNodeNotifications *pNodeNotifications)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hInstance;
	ModulePlayer* pNode = dynamic_cast<ModulePlayer*>(pProdNode);
	return pNode->SetNodeNotifications(pNodeNotificationsCookie, pNodeNotifications);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleOnNodeAdded(XnModuleNodeHandle hGenerator, const XnChar *strNodeName, XnProductionNodeType type, XnCodecID compression)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleRecorder* pNode = dynamic_cast<ModuleRecorder*>(pProdNode);
	return pNode->OnNodeAdded(strNodeName, type, compression);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleOnNodeRemoved(XnModuleNodeHandle hGenerator, const XnChar *strNodeName)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleRecorder* pNode = dynamic_cast<ModuleRecorder*>(pProdNode);
	return pNode->OnNodeRemoved(strNodeName);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleOnNodeIntPropChanged(XnModuleNodeHandle hGenerator, const XnChar *strNodeName, const XnChar* strPropName, XnUInt64 nValue)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleRecorder* pNode = dynamic_cast<ModuleRecorder*>(pProdNode);
	return pNode->OnNodeIntPropChanged(strNodeName, strPropName, nValue);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleOnNodeRealPropChanged(XnModuleNodeHandle hGenerator, const XnChar *strNodeName, const XnChar* strPropName, XnDouble dValue)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleRecorder* pNode = dynamic_cast<ModuleRecorder*>(pProdNode);
	return pNode->OnNodeRealPropChanged(strNodeName, strPropName, dValue);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleOnNodeStringPropChanged(XnModuleNodeHandle hGenerator, const XnChar *strNodeName, const XnChar* strPropName, const XnChar* strValue)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleRecorder* pNode = dynamic_cast<ModuleRecorder*>(pProdNode);
	return pNode->OnNodeStringPropChanged(strNodeName, strPropName, strValue);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleOnNodeGeneralPropChanged(XnModuleNodeHandle hGenerator, const XnChar *strNodeName, const XnChar* strPropName, XnUInt32 nBufferSize, const void* pBuffer)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleRecorder* pNode = dynamic_cast<ModuleRecorder*>(pProdNode);
	return pNode->OnNodeGeneralPropChanged(strNodeName, strPropName, nBufferSize, pBuffer);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleOnNodeNewData(XnModuleNodeHandle hGenerator, const XnChar *strNodeName, XnUInt64 nTimeStamp, XnUInt32 nFrame, const void *pData, XnUInt32 nSize)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleRecorder* pNode = dynamic_cast<ModuleRecorder*>(pProdNode);
	return pNode->OnNodeNewData(strNodeName, nTimeStamp, nFrame, pData, nSize);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleOnNodeStateReady(XnModuleNodeHandle hGenerator, const XnChar *strNodeName)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleRecorder* pNode = dynamic_cast<ModuleRecorder*>(pProdNode);
	return pNode->OnNodeStateReady(strNodeName);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetMirror(XnModuleNodeHandle hGenerator, XnBool bMirror)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleMirrorInterface* pInterface = pNode->GetMirrorInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->SetMirror(bMirror);
}

XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsMirrored(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleMirrorInterface* pInterface = pNode->GetMirrorInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	return pInterface->IsMirrored();
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToMirrorChange(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleMirrorInterface* pInterface = pNode->GetMirrorInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->RegisterToMirrorChange(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromMirrorChange(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleMirrorInterface* pInterface = pNode->GetMirrorInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface,);
	pInterface->UnregisterFromMirrorChange(hCallback);
}

XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsViewPointSupported(XnModuleNodeHandle hGenerator, XnNodeHandle hNode)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ProductionNode node(hNode);
	ModuleAlternativeViewPointInterface* pInterface = pNode->GetAlternativeViewPointInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	return pInterface->IsViewPointSupported(node);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetViewPoint(XnModuleNodeHandle hGenerator, XnNodeHandle hNode)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ProductionNode node(hNode);
	ModuleAlternativeViewPointInterface* pInterface = pNode->GetAlternativeViewPointInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->SetViewPoint(node);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleResetViewPoint(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleAlternativeViewPointInterface* pInterface = pNode->GetAlternativeViewPointInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->ResetViewPoint();
}

XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsViewPointAs(XnModuleNodeHandle hGenerator, XnNodeHandle hNode)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleAlternativeViewPointInterface* pInterface = pNode->GetAlternativeViewPointInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	ProductionNode node(hNode);
	return pInterface->IsViewPointAs(node);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToViewPointChange(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleAlternativeViewPointInterface* pInterface = pNode->GetAlternativeViewPointInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->RegisterToViewPointChange(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromViewPointChange(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleAlternativeViewPointInterface* pInterface = pNode->GetAlternativeViewPointInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface,);
	pInterface->UnregisterFromViewPointChange(hCallback);
}

XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleCanFrameSyncWith(XnModuleNodeHandle hGenerator, XnNodeHandle hNode)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleFrameSyncInterface* pInterface = pNode->GetFrameSyncInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	ProductionNode node(hNode);
	return pInterface->CanFrameSyncWith(node);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleFrameSyncWith(XnModuleNodeHandle hGenerator, XnNodeHandle hNode)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleFrameSyncInterface* pInterface = pNode->GetFrameSyncInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	ProductionNode node(hNode);
	return pInterface->FrameSyncWith(node);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleStopFrameSyncWith(XnModuleNodeHandle hGenerator, XnNodeHandle hNode)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleFrameSyncInterface* pInterface = pNode->GetFrameSyncInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	ProductionNode node(hNode);
	return pInterface->StopFrameSyncWith(node);
}

XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsFrameSyncedWith(XnModuleNodeHandle hGenerator, XnNodeHandle hNode)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleFrameSyncInterface* pInterface = pNode->GetFrameSyncInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	ProductionNode node(hNode);
	return pInterface->IsFrameSyncedWith(node);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToFrameSyncChange(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleFrameSyncInterface* pInterface = pNode->GetFrameSyncInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->RegisterToFrameSyncChange(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromFrameSyncChange(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	ModuleFrameSyncInterface* pInterface = pNode->GetFrameSyncInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface,);
	pInterface->UnregisterFromFrameSyncChange(hCallback);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleStartGenerating(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	return pNode->StartGenerating();
}

XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsGenerating(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	return pNode->IsGenerating();
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleStopGenerating(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	pNode->StopGenerating();
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToGenerationRunningChange(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	return pNode->RegisterToGenerationRunningChange(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromGenerationRunningChange(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	pNode->UnregisterFromGenerationRunningChange(hCallback);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToNewDataAvailable(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	return pNode->RegisterToNewDataAvailable(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromNewDataAvailable(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	pNode->UnregisterFromNewDataAvailable(hCallback);
}
	
XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsNewDataAvailable(XnModuleNodeHandle hGenerator, XnUInt64* pnTimestamp)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	return pNode->IsNewDataAvailable(*pnTimestamp);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleUpdateData(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	return pNode->UpdateData();
}

XN_C_API_EXPORT XnUInt32 XN_CALLBACK_TYPE __ModuleGetDataSize(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	return pNode->GetDataSize();
}

XN_C_API_EXPORT XnUInt64 XN_CALLBACK_TYPE __ModuleGetTimestamp(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	return pNode->GetTimestamp();
}

XN_C_API_EXPORT XnUInt32 XN_CALLBACK_TYPE __ModuleGetFrameID(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGenerator* pNode = dynamic_cast<ModuleGenerator*>(pProdNode);
	return pNode->GetFrameID();
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetCropping(XnModuleNodeHandle hGenerator, const XnCropping* pCropping)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleMapGenerator* pNode = dynamic_cast<ModuleMapGenerator*>(pProdNode);
	ModuleCroppingInterface* pInterface = pNode->GetCroppingInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->SetCropping(*pCropping);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetCropping(XnModuleNodeHandle hGenerator, XnCropping* pCropping)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleMapGenerator* pNode = dynamic_cast<ModuleMapGenerator*>(pProdNode);
	ModuleCroppingInterface* pInterface = pNode->GetCroppingInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->GetCropping(*pCropping);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToCroppingChange(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleMapGenerator* pNode = dynamic_cast<ModuleMapGenerator*>(pProdNode);
	ModuleCroppingInterface* pInterface = pNode->GetCroppingInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->RegisterToCroppingChange(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromCroppingChange(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleMapGenerator* pNode = dynamic_cast<ModuleMapGenerator*>(pProdNode);
	ModuleCroppingInterface* pInterface = pNode->GetCroppingInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface,);
	pInterface->UnregisterFromCroppingChange(hCallback);
}

XN_C_API_EXPORT XnUInt32 XN_CALLBACK_TYPE __ModuleGetSupportedMapOutputModesCount(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleMapGenerator* pNode = dynamic_cast<ModuleMapGenerator*>(pProdNode);
	return pNode->GetSupportedMapOutputModesCount();
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetSupportedMapOutputModes(XnModuleNodeHandle hGenerator, XnMapOutputMode* aModes, XnUInt32* pnCount)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleMapGenerator* pNode = dynamic_cast<ModuleMapGenerator*>(pProdNode);
	return pNode->GetSupportedMapOutputModes(aModes, *pnCount);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetMapOutputMode(XnModuleNodeHandle hGenerator, const XnMapOutputMode* pOutputMode)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleMapGenerator* pNode = dynamic_cast<ModuleMapGenerator*>(pProdNode);
	return pNode->SetMapOutputMode(*pOutputMode);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetMapOutputMode(XnModuleNodeHandle hGenerator, XnMapOutputMode* pOutputMode)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleMapGenerator* pNode = dynamic_cast<ModuleMapGenerator*>(pProdNode);
	return pNode->GetMapOutputMode(*pOutputMode);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToMapOutputModeChange(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleMapGenerator* pNode = dynamic_cast<ModuleMapGenerator*>(pProdNode);
	return pNode->RegisterToMapOutputModeChange(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromMapOutputModeChange(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleMapGenerator* pNode = dynamic_cast<ModuleMapGenerator*>(pProdNode);
	pNode->UnregisterFromMapOutputModeChange(hCallback);
}

XN_C_API_EXPORT XnUInt32 XN_CALLBACK_TYPE __ModuleGetSupportedUserPositionsCount(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleDepthGenerator* pNode = dynamic_cast<ModuleDepthGenerator*>(pProdNode);
	ModuleUserPositionInterface* pInterface = pNode->GetUserPositionInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, 0);
	return pInterface->GetSupportedUserPositionsCount();
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetUserPosition(XnModuleNodeHandle hGenerator, XnUInt32 nIndex, const XnBoundingBox3D* pPosition)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleDepthGenerator* pNode = dynamic_cast<ModuleDepthGenerator*>(pProdNode);
	ModuleUserPositionInterface* pInterface = pNode->GetUserPositionInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->SetUserPosition(nIndex, *pPosition);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetUserPosition(XnModuleNodeHandle hGenerator, XnUInt32 nIndex, XnBoundingBox3D* pPosition)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleDepthGenerator* pNode = dynamic_cast<ModuleDepthGenerator*>(pProdNode);
	ModuleUserPositionInterface* pInterface = pNode->GetUserPositionInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->GetUserPosition(nIndex, *pPosition);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToUserPositionChange(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleDepthGenerator* pNode = dynamic_cast<ModuleDepthGenerator*>(pProdNode);
	ModuleUserPositionInterface* pInterface = pNode->GetUserPositionInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->RegisterToUserPositionChange(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromUserPositionChange(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleDepthGenerator* pNode = dynamic_cast<ModuleDepthGenerator*>(pProdNode);
	ModuleUserPositionInterface* pInterface = pNode->GetUserPositionInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface,);
	pInterface->UnregisterFromUserPositionChange(hCallback);
}

XN_C_API_EXPORT XnDepthPixel XN_CALLBACK_TYPE __ModuleGetDeviceMaxDepth(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleDepthGenerator* pNode = dynamic_cast<ModuleDepthGenerator*>(pProdNode);
	return pNode->GetDeviceMaxDepth();
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetFieldOfView(XnModuleNodeHandle hGenerator, XnFieldOfView* pFOV)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleDepthGenerator* pNode = dynamic_cast<ModuleDepthGenerator*>(pProdNode);
	pNode->GetFieldOfView(*pFOV);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToFieldOfViewChange(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleDepthGenerator* pNode = dynamic_cast<ModuleDepthGenerator*>(pProdNode);
	return pNode->RegisterToFieldOfViewChange(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromFieldOfViewChange(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleDepthGenerator* pNode = dynamic_cast<ModuleDepthGenerator*>(pProdNode);
	pNode->UnregisterFromFieldOfViewChange(hCallback);
}

XN_C_API_EXPORT XnDepthPixel* XN_CALLBACK_TYPE __ModuleGetDepthMap(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleDepthGenerator* pNode = dynamic_cast<ModuleDepthGenerator*>(pProdNode);
	return pNode->GetDepthMap();
}

XN_C_API_EXPORT XnUInt8* XN_CALLBACK_TYPE __ModuleGetImageMap(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleImageGenerator* pNode = dynamic_cast<ModuleImageGenerator*>(pProdNode);
	return pNode->GetImageMap();
}

XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsPixelFormatSupported(XnModuleNodeHandle hGenerator, XnPixelFormat Format)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleImageGenerator* pNode = dynamic_cast<ModuleImageGenerator*>(pProdNode);
	return pNode->IsPixelFormatSupported(Format);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetPixelFormat(XnModuleNodeHandle hGenerator, XnPixelFormat Format)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleImageGenerator* pNode = dynamic_cast<ModuleImageGenerator*>(pProdNode);
	return pNode->SetPixelFormat(Format);
}

XN_C_API_EXPORT XnPixelFormat XN_CALLBACK_TYPE __ModuleGetPixelFormat(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleImageGenerator* pNode = dynamic_cast<ModuleImageGenerator*>(pProdNode);
	return pNode->GetPixelFormat();
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToPixelFormatChange(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleImageGenerator* pNode = dynamic_cast<ModuleImageGenerator*>(pProdNode);
	return pNode->RegisterToPixelFormatChange(handler, pCookie, *phCallback);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromPixelFormatChange(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleImageGenerator* pNode = dynamic_cast<ModuleImageGenerator*>(pProdNode);
	pNode->UnregisterFromPixelFormatChange(hCallback);
}

XN_C_API_EXPORT XnIRPixel* XN_CALLBACK_TYPE __ModuleGetIRMap(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleIRGenerator* pNode = dynamic_cast<ModuleIRGenerator*>(pProdNode);
	return pNode->GetIRMap();
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleAddGesture(XnModuleNodeHandle hGenerator, const XnChar* strGesture, XnBoundingBox3D* pArea)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGestureGenerator* pNode = dynamic_cast<ModuleGestureGenerator*>(pProdNode);
	return pNode->AddGesture(strGesture, pArea);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRemoveGesture(XnModuleNodeHandle hGenerator, const XnChar* strGesture)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGestureGenerator* pNode = dynamic_cast<ModuleGestureGenerator*>(pProdNode);
	return pNode->RemoveGesture(strGesture);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetActiveGestures(XnModuleNodeHandle hGenerator, XnChar** pstrGestures, XnUInt16* pnGestures)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGestureGenerator* pNode = dynamic_cast<ModuleGestureGenerator*>(pProdNode);
	return pNode->GetActiveGestures(pstrGestures, *pnGestures);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetAllActiveGestures(XnModuleNodeHandle hGenerator, XnChar** pstrGestures, XnUInt32 nNameLength, XnUInt16* pnGestures)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGestureGenerator* pNode = dynamic_cast<ModuleGestureGenerator*>(pProdNode);
	return pNode->GetAllActiveGestures(pstrGestures, nNameLength, *pnGestures);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleEnumerateGestures(XnModuleNodeHandle hGenerator, XnChar** pstrGestures, XnUInt16* pnGestures)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGestureGenerator* pNode = dynamic_cast<ModuleGestureGenerator*>(pProdNode);
	return pNode->EnumerateGestures(pstrGestures, *pnGestures);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleEnumerateAllGestures(XnModuleNodeHandle hGenerator, XnChar** pstrGestures, XnUInt32 nNameLength, XnUInt16* pnGestures)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGestureGenerator* pNode = dynamic_cast<ModuleGestureGenerator*>(pProdNode);
	return pNode->EnumerateAllGestures(pstrGestures, nNameLength, *pnGestures);
}
XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsGestureAvailable(XnModuleNodeHandle hGenerator, const XnChar* strGesture)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGestureGenerator* pNode = dynamic_cast<ModuleGestureGenerator*>(pProdNode);
	return pNode->IsGestureAvailable(strGesture);
}
XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsGestureProgressSupported(XnModuleNodeHandle hGenerator, const XnChar* strGesture)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGestureGenerator* pNode = dynamic_cast<ModuleGestureGenerator*>(pProdNode);
	return pNode->IsGestureProgressSupported(strGesture);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterGestureCallbacks(XnModuleNodeHandle hGenerator, XnModuleGestureRecognized RecognizedCB, XnModuleGestureProgress ProgressCB, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGestureGenerator* pNode = dynamic_cast<ModuleGestureGenerator*>(pProdNode);
	return pNode->RegisterGestureCallbacks(RecognizedCB, ProgressCB, pCookie, *phCallback);
}
XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterGestureCallbacks(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGestureGenerator* pNode = dynamic_cast<ModuleGestureGenerator*>(pProdNode);
	pNode->UnregisterGestureCallbacks(hCallback);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToGestureChange(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGestureGenerator* pNode = dynamic_cast<ModuleGestureGenerator*>(pProdNode);
	return pNode->RegisterToGestureChange(handler, pCookie, *phCallback);
}
XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromGestureChange(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleGestureGenerator* pNode = dynamic_cast<ModuleGestureGenerator*>(pProdNode);
	pNode->UnregisterFromGestureChange(hCallback);
}

XN_C_API_EXPORT const XnLabel* XN_CALLBACK_TYPE __ModuleGetLabelMap(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleSceneAnalyzer* pNode = dynamic_cast<ModuleSceneAnalyzer*>(pProdNode);
	return pNode->GetLabelMap();
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetFloor(XnModuleNodeHandle hGenerator, XnPlane3D* pPlane)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleSceneAnalyzer* pNode = dynamic_cast<ModuleSceneAnalyzer*>(pProdNode);
	return pNode->GetFloor(*pPlane);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterHandCallbacks(XnModuleNodeHandle hGenerator, XnModuleHandCreate CreateCB, XnModuleHandUpdate UpdateCB, XnModuleHandDestroy DestroyCB, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleHandsGenerator* pNode = dynamic_cast<ModuleHandsGenerator*>(pProdNode);
	return pNode->RegisterHandCallbacks(CreateCB, UpdateCB, DestroyCB, pCookie, *phCallback);
}
XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterHandCallbacks(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleHandsGenerator* pNode = dynamic_cast<ModuleHandsGenerator*>(pProdNode);
	pNode->UnregisterHandCallbacks(hCallback);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleStopTracking(XnModuleNodeHandle hGenerator, XnUserID user)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleHandsGenerator* pNode = dynamic_cast<ModuleHandsGenerator*>(pProdNode);
	return pNode->StopTracking(user);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleStopTrackingAll(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleHandsGenerator* pNode = dynamic_cast<ModuleHandsGenerator*>(pProdNode);
	return pNode->StopTrackingAll();
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleStartTracking(XnModuleNodeHandle hGenerator, const XnPoint3D* pPosition)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleHandsGenerator* pNode = dynamic_cast<ModuleHandsGenerator*>(pProdNode);
	return pNode->StartTracking(*pPosition);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetTrackingSmoothing(XnModuleNodeHandle hGenerator, XnFloat fSmoothingFactor)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleHandsGenerator* pNode = dynamic_cast<ModuleHandsGenerator*>(pProdNode);
	return pNode->SetSmoothing(fSmoothingFactor);
}

XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsJointAvailable(XnModuleNodeHandle hGenerator, XnSkeletonJoint eJoint)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	return pInterface->IsJointAvailable(eJoint);
}
XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsProfileAvailable(XnModuleNodeHandle hGenerator, XnSkeletonProfile eProfile)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	return pInterface->IsProfileAvailable(eProfile);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetSkeletonProfile(XnModuleNodeHandle hGenerator, XnSkeletonProfile eProfile)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->SetSkeletonProfile(eProfile);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetJointActive(XnModuleNodeHandle hGenerator, XnSkeletonJoint eJoint, XnBool bState)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->SetJointActive(eJoint, bState);
}
XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsJointActive(XnModuleNodeHandle hGenerator, XnSkeletonJoint eJoint)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	return pInterface->IsJointActive(eJoint);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToJointConfigurationChange(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->RegisterToJointConfigurationChange(handler, pCookie, *phCallback);
}
XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromJointConfigurationChange(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface,);
	pInterface->UnregisterFromJointConfigurationChange(hCallback);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleEnumerateActiveJoints(XnModuleNodeHandle hGenerator, XnSkeletonJoint* pJoints, XnUInt16* pnJoints)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->EnumerateActiveJoints(pJoints, *pnJoints);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetSkeletonJoint(XnModuleNodeHandle hGenerator, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointTransformation* pJoint)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->GetSkeletonJoint(user, eJoint, *pJoint);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetSkeletonJointPosition(XnModuleNodeHandle hGenerator, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointPosition* pJoint)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->GetSkeletonJointPosition(user, eJoint, *pJoint);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetSkeletonJointOrientation(XnModuleNodeHandle hGenerator, XnUserID user, XnSkeletonJoint eJoint, XnSkeletonJointOrientation* pJoint)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->GetSkeletonJointOrientation(user, eJoint, *pJoint);
}
XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsSkeletonTracking(XnModuleNodeHandle hGenerator, XnUserID user)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	return pInterface->IsTracking(user);
}
XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsSkeletonCalibrated(XnModuleNodeHandle hGenerator, XnUserID user)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	return pInterface->IsCalibrated(user);
}
XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsSkeletonCalibrating(XnModuleNodeHandle hGenerator, XnUserID user)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	return pInterface->IsCalibrating(user);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRequestSkeletonCalibration(XnModuleNodeHandle hGenerator, XnUserID user, XnBool bForce)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->RequestCalibration(user, bForce);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleAbortSkeletonCalibration(XnModuleNodeHandle hGenerator, XnUserID user)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->AbortCalibration(user);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSaveCalibrationData(XnModuleNodeHandle hGenerator, XnUserID user, XnUInt32 nSlot)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->SaveCalibrationData(user, nSlot);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleLoadCalibrationData(XnModuleNodeHandle hGenerator, XnUserID user, XnUInt32 nSlot)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->LoadCalibrationData(user, nSlot);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleClearCalibrationData(XnModuleNodeHandle hGenerator, XnUInt32 nSlot)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->ClearCalibrationData(nSlot);
}
XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleIsCalibrationData(XnModuleNodeHandle hGenerator, XnUInt32 nSlot)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	return pInterface->IsCalibrationData(nSlot);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleStartSkeletonTracking(XnModuleNodeHandle hGenerator, XnUserID user)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->StartTracking(user);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleStopSkeletonTracking(XnModuleNodeHandle hGenerator, XnUserID user)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->StopTracking(user);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleResetSkeleton(XnModuleNodeHandle hGenerator, XnUserID user)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->Reset(user);
}
XN_C_API_EXPORT XnBool XN_CALLBACK_TYPE __ModuleNeedPoseForSkeletonCalibration(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, FALSE);
	return pInterface->NeedPoseForCalibration();
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetSkeletonCalibrationPose(XnModuleNodeHandle hGenerator, XnChar* strPose)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->GetCalibrationPose(strPose);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetSkeletonSmoothing(XnModuleNodeHandle hGenerator, XnFloat fSmoothingFactor)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->SetSmoothing(fSmoothingFactor);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterCalibrationCallbacks(XnModuleNodeHandle hGenerator, XnModuleCalibrationStart CalibrationStartCB, XnModuleCalibrationEnd CalibrationEndCB, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->RegisterCalibrationCallbacks(CalibrationStartCB, CalibrationEndCB, pCookie, *phCallback);
}
XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterCalibrationCallbacks(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModuleSkeletonInterface* pInterface = pNode->GetSkeletonInterface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface,);
	pInterface->UnregisterCalibrationCallbacks(hCallback);
}
///
XN_C_API_EXPORT XnUInt32 XN_CALLBACK_TYPE __ModuleGetNumberOfPoses(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModulePoseDetectionInteface* pInterface = pNode->GetPoseDetectionInteface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface, 0);
	return pInterface->GetNumberOfPoses();
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetAvailablePoses(XnModuleNodeHandle hGenerator, XnChar** pstrPoses, XnUInt32* pnPoses)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModulePoseDetectionInteface* pInterface = pNode->GetPoseDetectionInteface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->GetAvailablePoses(pstrPoses, *pnPoses);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetAllAvailablePoses(XnModuleNodeHandle hGenerator, XnChar** pstrPoses, XnUInt32 nNameLength, XnUInt32* pnPoses)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModulePoseDetectionInteface* pInterface = pNode->GetPoseDetectionInteface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->GetAllAvailablePoses(pstrPoses, nNameLength, *pnPoses);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleStartPoseDetection(XnModuleNodeHandle hGenerator, const XnChar* strPose, XnUserID user)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModulePoseDetectionInteface* pInterface = pNode->GetPoseDetectionInteface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->StartPoseDetection(strPose, user);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleStopPoseDetection(XnModuleNodeHandle hGenerator, XnUserID user)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModulePoseDetectionInteface* pInterface = pNode->GetPoseDetectionInteface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->StopPoseDetection(user);
}

XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToPoseCallbacks(XnModuleNodeHandle hGenerator, XnModulePoseDetectionCallback PoseDetectionStartCB, XnModulePoseDetectionCallback PoseDetectionEndCB, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModulePoseDetectionInteface* pInterface = pNode->GetPoseDetectionInteface();
	_XN_VALIDATE_CAPABILITY_INTERFACE(pInterface);
	return pInterface->RegisterToPoseDetectionCallbacks(PoseDetectionStartCB, PoseDetectionEndCB, pCookie, *phCallback);
}
XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromPoseCallbacks(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	ModulePoseDetectionInteface* pInterface = pNode->GetPoseDetectionInteface();
	_XN_VALIDATE_CAPABILITY_INTERFACE_RET(pInterface,);
	pInterface->UnregisterFromPoseDetectionCallbacks(hCallback);
}

XN_C_API_EXPORT XnUInt16 XN_CALLBACK_TYPE __ModuleGetNumberOfUsers(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	return pNode->GetNumberOfUsers();
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetUsers(XnModuleNodeHandle hGenerator, XnUserID* pUsers, XnUInt16* pnUsers)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	return pNode->GetUsers(pUsers, *pnUsers);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetUserCoM(XnModuleNodeHandle hGenerator, XnUserID user, XnPoint3D* pCoM)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	return pNode->GetCoM(user, *pCoM);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetUserPixels(XnModuleNodeHandle hGenerator, XnUserID user, XnSceneMetaData* pScene)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	return pNode->GetUserPixels(user, pScene);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterUserCallbacks(XnModuleNodeHandle hGenerator, XnModuleUserHandler NewUserCB, XnModuleUserHandler LostUserCB, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	return pNode->RegisterUserCallbacks(NewUserCB, LostUserCB, pCookie, *phCallback);
}
XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterUserCallbacks(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleUserGenerator* pNode = dynamic_cast<ModuleUserGenerator*>(pProdNode);
	pNode->UnregisterUserCallbacks(hCallback);
}

XN_C_API_EXPORT XnUChar* XN_CALLBACK_TYPE __ModuleGetAudioBuffer(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleAudioGenerator* pNode = dynamic_cast<ModuleAudioGenerator*>(pProdNode);
	return pNode->GetAudioBuffer();
}
XN_C_API_EXPORT XnUInt32 XN_CALLBACK_TYPE __ModuleGetSupportedWaveOutputModesCount(XnModuleNodeHandle hGenerator)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleAudioGenerator* pNode = dynamic_cast<ModuleAudioGenerator*>(pProdNode);
	return pNode->GetSupportedWaveOutputModesCount();
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetSupportedWaveOutputModes(XnModuleNodeHandle hGenerator, XnWaveOutputMode* aSupportedModes, XnUInt32* pnCount)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleAudioGenerator* pNode = dynamic_cast<ModuleAudioGenerator*>(pProdNode);
	return pNode->GetSupportedWaveOutputModes(aSupportedModes, *pnCount);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleSetWaveOutputMode(XnModuleNodeHandle hGenerator, const XnWaveOutputMode* pOutputMode)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleAudioGenerator* pNode = dynamic_cast<ModuleAudioGenerator*>(pProdNode);
	return pNode->SetWaveOutputMode(*pOutputMode);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleGetWaveOutputMode(XnModuleNodeHandle hGenerator, XnWaveOutputMode* pOutputMode)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleAudioGenerator* pNode = dynamic_cast<ModuleAudioGenerator*>(pProdNode);
	return pNode->GetWaveOutputMode(*pOutputMode);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleRegisterToWaveOutputModeChanges(XnModuleNodeHandle hGenerator, XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle* phCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleAudioGenerator* pNode = dynamic_cast<ModuleAudioGenerator*>(pProdNode);
	return pNode->RegisterToWaveOutputModeChanges(handler, pCookie, *phCallback);
}
XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleUnregisterFromWaveOutputModeChanges(XnModuleNodeHandle hGenerator, XnCallbackHandle hCallback)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hGenerator;
	ModuleAudioGenerator* pNode = dynamic_cast<ModuleAudioGenerator*>(pProdNode);
	pNode->UnregisterFromWaveOutputModeChanges(hCallback);
}

XN_C_API_EXPORT XnCodecID XN_CALLBACK_TYPE __ModuleGetCodecID(XnModuleNodeHandle hCodec)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hCodec;
	ModuleCodec* pCodec = dynamic_cast<ModuleCodec*>(pProdNode);
	return pCodec->GetCodecID();
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleCodecInit(XnModuleNodeHandle hCodec, XnNodeHandle hNode)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hCodec;
	ModuleCodec* pCodec = dynamic_cast<ModuleCodec*>(pProdNode);
	return pCodec->Init(ProductionNode(hNode));
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleCompressData(XnModuleNodeHandle hCodec, const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hCodec;
	ModuleCodec* pCodec = dynamic_cast<ModuleCodec*>(pProdNode);
	return pCodec->CompressData(pSrc, nSrcSize, pDst, nDstSize, pnBytesWritten);
}
XN_C_API_EXPORT XnStatus XN_CALLBACK_TYPE __ModuleDecompressData(XnModuleNodeHandle hCodec, const void* pSrc, XnUInt32 nSrcSize, void* pDst, XnUInt32 nDstSize, XnUInt* pnBytesWritten)
{
	ModuleProductionNode* pProdNode = (ModuleProductionNode*)hCodec;
	ModuleCodec* pCodec = dynamic_cast<ModuleCodec*>(pProdNode);
	return pCodec->DecompressData(pSrc, nSrcSize, pDst, nDstSize, pnBytesWritten);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetExtendedSerializationInterface(XnModuleExtendedSerializationInterface* pInterface)
{
	pInterface->InitNotifications = __ModuleInitNotifications;
	pInterface->StopNotifications = __ModuleStopNotifications;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetLockAwareInterface(XnModuleLockAwareInterface* pInterface)
{
	pInterface->SetLockState = __ModuleSetLockState;
	pInterface->GetLockState = __ModuleGetLockState;
	pInterface->RegisterToLockChange = __ModuleRegisterToLockChange;
	pInterface->UnregisterFromLockChange = __ModuleUnregisterFromLockChange;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetErrorStateInterface(XnModuleErrorStateInterface* pInterface)
{
	pInterface->GetErrorState = __ModuleGetErrorState;
	pInterface->RegisterToErrorStateChange = __ModuleRegisterToErrorStateChange;
	pInterface->UnregisterFromErrorStateChange = __ModuleUnregisterFromErrorStateChange;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetProductionNodeInterface(XnModuleProductionNodeInterface* pInterface)
{
	pInterface->IsCapabilitySupported = __ModuleIsCapabilitySupported;
	pInterface->SetIntProperty = __ModuleSetIntProperty;
	pInterface->SetRealProperty = __ModuleSetRealProperty;
	pInterface->SetStringProperty = __ModuleSetStringProperty;
	pInterface->SetGeneralProperty = __ModuleSetGeneralProperty;
	pInterface->GetIntProperty = __ModuleGetIntProperty;
	pInterface->GetRealProperty = __ModuleGetRealProperty;
	pInterface->GetStringProperty = __ModuleGetStringProperty;
	pInterface->GetGeneralProperty = __ModuleGetGeneralProperty;

	__ModuleGetExtendedSerializationInterface(pInterface->pExtendedSerializationInterface);
	__ModuleGetLockAwareInterface(pInterface->pLockAwareInterface);
	__ModuleGetErrorStateInterface(pInterface->pErrorStateInterface);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetDeviceInterface(XnModuleDeviceInterface* pInterface)
{
	__ModuleGetProductionNodeInterface(pInterface->pProductionNode);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetMirrorInterface(XnModuleMirrorInterface* pInterface)
{
	pInterface->SetMirror = __ModuleSetMirror;
	pInterface->IsMirrored = __ModuleIsMirrored;
	pInterface->RegisterToMirrorChange = __ModuleRegisterToMirrorChange;
	pInterface->UnregisterFromMirrorChange = __ModuleUnregisterFromMirrorChange;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetAlternativeViewPointInterface(XnModuleAlternativeViewPointInterface* pInterface)
{
	pInterface->IsViewPointSupported = __ModuleIsViewPointSupported;
	pInterface->SetViewPoint = __ModuleSetViewPoint;
	pInterface->ResetViewPoint = __ModuleResetViewPoint;
	pInterface->IsViewPointAs = __ModuleIsViewPointAs;
	pInterface->RegisterToViewPointChange = __ModuleRegisterToViewPointChange;
	pInterface->UnregisterFromViewPointChange = __ModuleUnregisterFromViewPointChange;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetFrameSyncInterface(XnModuleFrameSyncInterface* pInterface)
{
	pInterface->CanFrameSyncWith = __ModuleCanFrameSyncWith;
	pInterface->FrameSyncWith = __ModuleFrameSyncWith;
	pInterface->StopFrameSyncWith = __ModuleStopFrameSyncWith;
	pInterface->IsFrameSyncedWith = __ModuleIsFrameSyncedWith;
	pInterface->RegisterToFrameSyncChange = __ModuleRegisterToFrameSyncChange;
	pInterface->UnregisterFromFrameSyncChange = __ModuleUnregisterFromFrameSyncChange;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetGeneratorInterface(XnModuleGeneratorInterface* pInterface)
{
	__ModuleGetProductionNodeInterface(pInterface->pProductionNodeInterface);
	pInterface->StartGenerating = __ModuleStartGenerating;
	pInterface->IsGenerating = __ModuleIsGenerating;
	pInterface->StopGenerating = __ModuleStopGenerating;
	pInterface->RegisterToGenerationRunningChange = __ModuleRegisterToGenerationRunningChange;
	pInterface->UnregisterFromGenerationRunningChange = __ModuleUnregisterFromGenerationRunningChange;
	pInterface->RegisterToNewDataAvailable = __ModuleRegisterToNewDataAvailable;
	pInterface->UnregisterFromNewDataAvailable = __ModuleUnregisterFromNewDataAvailable;
	pInterface->IsNewDataAvailable = __ModuleIsNewDataAvailable;
	pInterface->UpdateData = __ModuleUpdateData;
	pInterface->GetDataSize = __ModuleGetDataSize;
	pInterface->GetTimestamp = __ModuleGetTimestamp;
	pInterface->GetFrameID = __ModuleGetFrameID;
	__ModuleGetMirrorInterface(pInterface->pMirrorInterface);
	__ModuleGetAlternativeViewPointInterface(pInterface->pAlternativeViewPointInterface);
	__ModuleGetFrameSyncInterface(pInterface->pFrameSyncInterface);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetNodeNotifications(XnNodeNotifications *pInterface)
{
	pInterface->OnNodeAdded = __ModuleOnNodeAdded;
	pInterface->OnNodeRemoved = __ModuleOnNodeRemoved;
	pInterface->OnNodeIntPropChanged = __ModuleOnNodeIntPropChanged;
	pInterface->OnNodeRealPropChanged = __ModuleOnNodeRealPropChanged;
	pInterface->OnNodeStringPropChanged = __ModuleOnNodeStringPropChanged;
	pInterface->OnNodeGeneralPropChanged = __ModuleOnNodeGeneralPropChanged;
	pInterface->OnNodeStateReady = __ModuleOnNodeStateReady;
	pInterface->OnNodeNewData = __ModuleOnNodeNewData;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetRecorderInterface(XnModuleRecorderInterface* pInterface)
{
	pInterface->SetOutputStream = __ModuleSetOutputStream;
	__ModuleGetProductionNodeInterface(pInterface->pProductionNode);
	__ModuleGetNodeNotifications(pInterface->pNodeNotifications);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetPlayerInterface(XnModulePlayerInterface* pInterface)
{
	__ModuleGetProductionNodeInterface(pInterface->pProductionNode);
	pInterface->SetInputStream = __ModuleSetInputStream;
	pInterface->ReadNext = __ModuleReadNext;
	pInterface->SetNodeNotifications = __ModuleSetNodeNotifications;
	pInterface->SetRepeat = __ModuleSetRepeat;
	pInterface->SeekToTimeStamp = __ModuleSeekToTimeStamp;
	pInterface->SeekToFrame = __ModuleSeekToFrame;
	pInterface->TellTimestamp = __ModuleTellTimestamp;
	pInterface->TellFrame = __ModuleTellFrame;
	pInterface->GetNumFrames = __ModuleGetNumFrames;
	pInterface->GetSupportedFormat = __ModuleGetSupportedFormat;
	pInterface->IsEOF = __ModuleIsEOF;
	pInterface->RegisterToEndOfFileReached = __ModuleRegisterToEndOfFileReached;
	pInterface->UnregisterFromEndOfFileReached = __ModuleUnregisterFromEndOfFileReached;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetCroppingInterface(XnModuleCroppingInterface* pInterface)
{
	pInterface->SetCropping = __ModuleSetCropping;
	pInterface->GetCropping = __ModuleGetCropping;
	pInterface->RegisterToCroppingChange = __ModuleRegisterToCroppingChange;
	pInterface->UnregisterFromCroppingChange = __ModuleUnregisterFromCroppingChange;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetMapGeneratorInterface(XnModuleMapGeneratorInterface* pInterface)
{
	__ModuleGetGeneratorInterface(pInterface->pGeneratorInterface);
	pInterface->GetSupportedMapOutputModesCount = __ModuleGetSupportedMapOutputModesCount;
	pInterface->GetSupportedMapOutputModes = __ModuleGetSupportedMapOutputModes;
	pInterface->SetMapOutputMode = __ModuleSetMapOutputMode;
	pInterface->GetMapOutputMode = __ModuleGetMapOutputMode;
	pInterface->RegisterToMapOutputModeChange = __ModuleRegisterToMapOutputModeChange;
	pInterface->UnregisterFromMapOutputModeChange = __ModuleUnregisterFromMapOutputModeChange;
	__ModuleGetCroppingInterface(pInterface->pCroppingInterface);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetUserPositionInterface(XnModuleUserPositionCapabilityInterface* pInterface)
{
	pInterface->GetSupportedUserPositionsCount = __ModuleGetSupportedUserPositionsCount;
	pInterface->SetUserPosition = __ModuleSetUserPosition;
	pInterface->GetUserPosition = __ModuleGetUserPosition;
	pInterface->RegisterToUserPositionChange = __ModuleRegisterToUserPositionChange;
	pInterface->UnregisterFromUserPositionChange = __ModuleUnregisterFromUserPositionChange;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetDepthGeneratorInterface(XnModuleDepthGeneratorInterface* pInterface)
{
	__ModuleGetMapGeneratorInterface(pInterface->pMapInterface);
	pInterface->GetDeviceMaxDepth = __ModuleGetDeviceMaxDepth;
	pInterface->GetFieldOfView = __ModuleGetFieldOfView;
	pInterface->RegisterToFieldOfViewChange = __ModuleRegisterToFieldOfViewChange;
	pInterface->UnregisterFromFieldOfViewChange = __ModuleUnregisterFromFieldOfViewChange;
	pInterface->GetDepthMap = __ModuleGetDepthMap;
	__ModuleGetUserPositionInterface(pInterface->pUserPositionInterface);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetImageGeneratorInterface(XnModuleImageGeneratorInterface* pInterface)
{
	__ModuleGetMapGeneratorInterface(pInterface->pMapInterface);
	pInterface->GetImageMap = __ModuleGetImageMap;
	pInterface->IsPixelFormatSupported = __ModuleIsPixelFormatSupported;
	pInterface->SetPixelFormat = __ModuleSetPixelFormat;
	pInterface->GetPixelFormat = __ModuleGetPixelFormat;
	pInterface->RegisterToPixelFormatChange = __ModuleRegisterToPixelFormatChange;
	pInterface->UnregisterFromPixelFormatChange = __ModuleUnregisterFromPixelFormatChange;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetIRGeneratorInterface(XnModuleIRGeneratorInterface* pInterface)
{
	__ModuleGetMapGeneratorInterface(pInterface->pMapInterface);
	pInterface->GetIRMap = __ModuleGetIRMap;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetGestureGeneratorInterface(XnModuleGestureGeneratorInterface* pInterface)
{
	__ModuleGetGeneratorInterface(pInterface->pGeneratorInterface);
	pInterface->AddGesture = __ModuleAddGesture;
	pInterface->RemoveGesture = __ModuleRemoveGesture;
	pInterface->GetActiveGestures = __ModuleGetActiveGestures;
	pInterface->GetAllActiveGestures = __ModuleGetAllActiveGestures;
	pInterface->EnumerateGestures = __ModuleEnumerateGestures;
	pInterface->EnumerateAllGestures = __ModuleEnumerateAllGestures;
	pInterface->IsGestureAvailable = __ModuleIsGestureAvailable;
	pInterface->IsGestureProgressSupported = __ModuleIsGestureProgressSupported;
	pInterface->RegisterGestureCallbacks = __ModuleRegisterGestureCallbacks;
	pInterface->UnregisterGestureCallbacks = __ModuleUnregisterGestureCallbacks;
	pInterface->RegisterToGestureChange = __ModuleRegisterToGestureChange;
	pInterface->UnregisterFromGestureChange = __ModuleUnregisterFromGestureChange;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetSceneAnalyzerInterface(XnModuleSceneAnalyzerInterface* pInterface)
{
	__ModuleGetMapGeneratorInterface(pInterface->pMapInterface);
	pInterface->GetLabelMap = __ModuleGetLabelMap;
	pInterface->GetFloor = __ModuleGetFloor;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetSkeletonInterface(XnModuleSkeletonCapabilityInterface* pInterface)
{
	pInterface->IsJointAvailable = __ModuleIsJointAvailable;
	pInterface->IsProfileAvailable = __ModuleIsProfileAvailable;
	pInterface->SetSkeletonProfile = __ModuleSetSkeletonProfile;
	pInterface->SetJointActive = __ModuleSetJointActive;
	pInterface->IsJointActive = __ModuleIsJointActive;
	pInterface->RegisterToJointConfigurationChange = __ModuleRegisterToJointConfigurationChange;
	pInterface->UnregisterFromJointConfigurationChange = __ModuleUnregisterFromJointConfigurationChange;
	pInterface->EnumerateActiveJoints = __ModuleEnumerateActiveJoints;
	pInterface->GetSkeletonJoint = __ModuleGetSkeletonJoint;
	pInterface->GetSkeletonJointPosition = __ModuleGetSkeletonJointPosition;
	pInterface->GetSkeletonJointOrientation = __ModuleGetSkeletonJointOrientation;
	pInterface->IsTracking = __ModuleIsSkeletonTracking;
	pInterface->IsCalibrated = __ModuleIsSkeletonCalibrated;
	pInterface->IsCalibrating = __ModuleIsSkeletonCalibrating;
	pInterface->RequestCalibration = __ModuleRequestSkeletonCalibration;
	pInterface->AbortCalibration = __ModuleAbortSkeletonCalibration;
	pInterface->SaveCalibrationData = __ModuleSaveCalibrationData;
	pInterface->LoadCalibrationData = __ModuleLoadCalibrationData;
	pInterface->ClearCalibrationData = __ModuleClearCalibrationData;
	pInterface->IsCalibrationData = __ModuleIsCalibrationData;
	pInterface->StartTracking = __ModuleStartSkeletonTracking;
	pInterface->StopTracking = __ModuleStopSkeletonTracking;
	pInterface->Reset = __ModuleResetSkeleton;
	pInterface->NeedPoseForCalibration = __ModuleNeedPoseForSkeletonCalibration;
	pInterface->GetCalibrationPose = __ModuleGetSkeletonCalibrationPose;
	pInterface->SetSmoothing = __ModuleSetSkeletonSmoothing;
	pInterface->RegisterCalibrationCallbacks = __ModuleRegisterCalibrationCallbacks;
	pInterface->UnregisterCalibrationCallbacks = __ModuleUnregisterCalibrationCallbacks;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetPoseDetectionInteface(XnModulePoseDetectionCapabilityInterface* pInteface)
{
	pInteface->GetNumberOfPoses = __ModuleGetNumberOfPoses;
	pInteface->GetAvailablePoses = __ModuleGetAvailablePoses;
	pInteface->GetAllAvailablePoses = __ModuleGetAllAvailablePoses;
	pInteface->StartPoseDetection = __ModuleStartPoseDetection;
	pInteface->StopPoseDetection = __ModuleStopPoseDetection;
	pInteface->RegisterToPoseCallbacks = __ModuleRegisterToPoseCallbacks;
	pInteface->UnregisterFromPoseCallbacks = __ModuleUnregisterFromPoseCallbacks;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetUserGeneratorInterface(XnModuleUserGeneratorInterface* pInterface)
{
	__ModuleGetGeneratorInterface(pInterface->pGeneratorInterface);

	pInterface->GetNumberOfUsers = __ModuleGetNumberOfUsers;
	pInterface->GetUsers = __ModuleGetUsers;
	pInterface->GetCoM = __ModuleGetUserCoM;
	pInterface->GetUserPixels = __ModuleGetUserPixels;
	pInterface->RegisterUserCallbacks = __ModuleRegisterUserCallbacks;
	pInterface->UnregisterUserCallbacks = __ModuleUnregisterUserCallbacks;

	__ModuleGetSkeletonInterface(pInterface->pSkeletonInterface);
	__ModuleGetPoseDetectionInteface(pInterface->pPoseDetectionInteface);
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetHandsGeneratorInterface(XnModuleHandsGeneratorInterface* pInterface)
{
	__ModuleGetGeneratorInterface(pInterface->pGeneratorInterface);

	pInterface->StartTracking = __ModuleStartTracking;
	pInterface->StopTracking = __ModuleStopTracking;
	pInterface->StopTrackingAll = __ModuleStopTrackingAll;
	pInterface->RegisterHandCallbacks = __ModuleRegisterHandCallbacks;
	pInterface->UnregisterHandCallbacks = __ModuleUnregisterHandCallbacks;
	pInterface->SetSmoothing = __ModuleSetTrackingSmoothing;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetAudioGeneratorInterface(XnModuleAudioGeneratorInterface* pInterface)
{
	__ModuleGetGeneratorInterface(pInterface->pGeneratorInterface);

	pInterface->GetAudioBuffer = __ModuleGetAudioBuffer;
	pInterface->GetSupportedWaveOutputModesCount = __ModuleGetSupportedWaveOutputModesCount;
	pInterface->GetSupportedWaveOutputModes = __ModuleGetSupportedWaveOutputModes;
	pInterface->SetWaveOutputMode = __ModuleSetWaveOutputMode;
	pInterface->GetWaveOutputMode = __ModuleGetWaveOutputMode;
	pInterface->RegisterToWaveOutputModeChanges = __ModuleRegisterToWaveOutputModeChanges;
	pInterface->UnregisterFromWaveOutputModeChanges = __ModuleUnregisterFromWaveOutputModeChanges;
}

XN_C_API_EXPORT void XN_CALLBACK_TYPE __ModuleGetCodecInterface(XnModuleCodecInterface* pInterface)
{
	__ModuleGetProductionNodeInterface(pInterface->pProductionNode);

	pInterface->GetCodecID = __ModuleGetCodecID;
	pInterface->Init = __ModuleCodecInit;
	pInterface->CompressData = __ModuleCompressData;
	pInterface->DecompressData = __ModuleDecompressData;
}

#endif // __XN_MODULE_CPP_REGISTRATION_H__
