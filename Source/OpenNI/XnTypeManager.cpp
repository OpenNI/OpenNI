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
#include "XnTypeManager.h"
#include <XnOSCpp.h>
#include <XnLog.h>
#include <XnOpenNI.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
TypeManager TypeManager::m_instance;

TypeManager& TypeManager::GetInstance()
{
	return m_instance;
}

TypeManager::TypeManager()
{
	xnOSCreateCriticalSection(&m_hLock);

	// create built-in types inheritance graph
	NodeTypeInfo productionNodeInfo("ProductionNode", XN_NODE_TYPE_PRODUCTION_NODE);
	AddType(productionNodeInfo);

	// Production Nodes
	AddNewType("Device", XN_NODE_TYPE_DEVICE, XN_NODE_TYPE_PRODUCTION_NODE);
	AddNewType("Recorder", XN_NODE_TYPE_RECORDER, XN_NODE_TYPE_PRODUCTION_NODE);
	AddNewType("Player", XN_NODE_TYPE_PLAYER, XN_NODE_TYPE_PRODUCTION_NODE);
	AddNewType("Codec", XN_NODE_TYPE_CODEC, XN_NODE_TYPE_PRODUCTION_NODE);
	AddNewType("Script", XN_NODE_TYPE_SCRIPT, XN_NODE_TYPE_PRODUCTION_NODE);
	AddNewType("Generator", XN_NODE_TYPE_GENERATOR, XN_NODE_TYPE_PRODUCTION_NODE);

	// Generators
	AddNewType("User", XN_NODE_TYPE_USER, XN_NODE_TYPE_GENERATOR);
	AddNewType("Hands", XN_NODE_TYPE_HANDS, XN_NODE_TYPE_GENERATOR);
	AddNewType("Gesture", XN_NODE_TYPE_GESTURE, XN_NODE_TYPE_GENERATOR);
	AddNewType("Audio", XN_NODE_TYPE_AUDIO, XN_NODE_TYPE_GENERATOR);
	AddNewType("MapGenerator", XN_NODE_TYPE_MAP_GENERATOR, XN_NODE_TYPE_GENERATOR);

	// Map Generators
	AddNewType("Depth", XN_NODE_TYPE_DEPTH, XN_NODE_TYPE_MAP_GENERATOR);
	AddNewType("Image", XN_NODE_TYPE_IMAGE, XN_NODE_TYPE_MAP_GENERATOR);
	AddNewType("IR", XN_NODE_TYPE_IR, XN_NODE_TYPE_MAP_GENERATOR);
	AddNewType("Scene", XN_NODE_TYPE_SCENE, XN_NODE_TYPE_MAP_GENERATOR);

	m_nNextExtendedNodeType = XN_NODE_TYPE_FIRST_EXTENSION;

	// make sure all types are registered
	for (XnInt32 i = 1; i < m_nNextExtendedNodeType; ++i)
	{
		XN_ASSERT(m_pTypesArray[i] != NULL);
	}
}

TypeManager::~TypeManager()
{
	for (XnInt32 i = 1; i < m_nNextExtendedNodeType; ++i)
	{
		XN_DELETE(m_pTypesArray[i]);
	}
}

XnStatus TypeManager::RegisterNewType(const XnChar* strName, XnProductionNodeType baseType, XnProductionNodeType* pNewType)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// we only need to lock when adding a new type
	XnAutoCSLocker locker(m_hLock);

	// check if type is already registered
	XnProductionNodeType type;
	if (XN_STATUS_OK == GetTypeByName(strName, &type))
	{
		*pNewType = type;
	}
	else
	{
		if (m_nNextExtendedNodeType >= XN_MAX_TYPES_COUNT)
		{
			XN_LOG_ERROR_RETURN(XN_STATUS_ERROR, XN_MASK_OPEN_NI, "OpenNI does not support more than %u types!", XN_MAX_TYPES_COUNT);
		}

		// add the new type
		nRetVal = AddNewType(strName, m_nNextExtendedNodeType, baseType);
		XN_IS_STATUS_OK(nRetVal);

		// take its type id
		*pNewType = m_nNextExtendedNodeType;

		// increment max type id
		++m_nNextExtendedNodeType;
	}
	
	return (XN_STATUS_OK);
}

XnStatus TypeManager::GetTypeName(XnProductionNodeType type, const XnChar** pstrName) const
{
	if (type > XN_MAX_TYPES_COUNT)
	{
		return XN_STATUS_NO_MATCH;
	}

	const NodeTypeInfo* pInfo = m_pTypesArray[type];
	if (pInfo == NULL)
	{
		return XN_STATUS_NO_MATCH;
	}

	*pstrName = pInfo->strName;
	return XN_STATUS_OK;
}

XnStatus TypeManager::GetTypeByName(const XnChar* strName, XnProductionNodeType* pType) const
{
	// take current count (for thread safety reasons)
	XnUInt32 nCount = m_nNextExtendedNodeType;

	// now search the array
	for (XnUInt32 i = 1; i < nCount; ++i)
	{
		if (strcmp(m_pTypesArray[i]->strName, strName) == 0)
		{
			*pType = m_pTypesArray[i]->type;
			return (XN_STATUS_OK);
		}
	}

	// not found
	return (XN_STATUS_NO_MATCH);
}

XnStatus TypeManager::IsTypeDerivedFrom(XnProductionNodeType type, XnProductionNodeType base, XnBool* pbIsDerived) const
{
	const NodeTypeInfo* pInfo = m_pTypesArray[type];
	if (pInfo == NULL)
	{
		return XN_STATUS_NO_MATCH;
	}

	*pbIsDerived = pInfo->inheritanceGraph.IsSet(base);

	return XN_STATUS_OK;
}

XnStatus TypeManager::GetTypeHierarchy(XnProductionNodeType type, const XnBitSet*& pHierarchy) const
{
	const NodeTypeInfo* pInfo = m_pTypesArray[type];
	if (pInfo == NULL)
	{
		return XN_STATUS_NO_MATCH;
	}

	pHierarchy = &pInfo->inheritanceGraph;
	return XN_STATUS_OK;
}

XnStatus TypeManager::AddType(NodeTypeInfo& info)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// add the type to it's inheritance graph
	nRetVal = info.inheritanceGraph.Set(info.type, TRUE);
	XN_IS_STATUS_OK(nRetVal);

	// add it to array
	XN_VALIDATE_NEW(m_pTypesArray[info.type], NodeTypeInfo);
	*m_pTypesArray[info.type] = info; 
	
	return (XN_STATUS_OK);
}

XnStatus TypeManager::AddNewType(const XnChar* strName, XnProductionNodeType type, XnProductionNodeType base)
{
	NodeTypeInfo newInfo(strName, type);
	NodeTypeInfo* pBaseInfo = m_pTypesArray[base];
	newInfo.inheritanceGraph = pBaseInfo->inheritanceGraph;
	return AddType(newInfo);
}

XnPredefinedProductionNodeType TypeManager::GetPredefinedBaseType(XnProductionNodeType type)
{
	const XnBitSet* pHierarchy;
	if (GetTypeHierarchy(type, pHierarchy) != XN_STATUS_OK)
	{
		return XN_NODE_TYPE_INVALID;
	}

	// start with concrete types
	if (pHierarchy->IsSet(XN_NODE_TYPE_DEVICE))
	{
		return XN_NODE_TYPE_DEVICE;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_DEPTH))
	{
		return XN_NODE_TYPE_DEPTH;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_IMAGE))
	{
		return XN_NODE_TYPE_IMAGE;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_AUDIO))
	{
		return XN_NODE_TYPE_AUDIO;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_IR))
	{
		return XN_NODE_TYPE_IR;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_USER))
	{
		return XN_NODE_TYPE_USER;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_RECORDER))
	{
		return XN_NODE_TYPE_RECORDER;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_PLAYER))
	{
		return XN_NODE_TYPE_PLAYER;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_GESTURE))
	{
		return XN_NODE_TYPE_GESTURE;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_SCENE))
	{
		return XN_NODE_TYPE_SCENE;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_HANDS))
	{
		return XN_NODE_TYPE_HANDS;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_CODEC))
	{
		return XN_NODE_TYPE_CODEC;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_SCRIPT))
	{
		return XN_NODE_TYPE_SCRIPT;
	}

	// and now, some abstract types
	else if (pHierarchy->IsSet(XN_NODE_TYPE_MAP_GENERATOR))
	{
		return XN_NODE_TYPE_MAP_GENERATOR;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_GENERATOR))
	{
		return XN_NODE_TYPE_GENERATOR;
	}
	else if (pHierarchy->IsSet(XN_NODE_TYPE_PRODUCTION_NODE))
	{
		return XN_NODE_TYPE_PRODUCTION_NODE;
	}
	else
	{
		// unknown
		XN_ASSERT(FALSE);
		return XN_NODE_TYPE_INVALID;
	}
}
