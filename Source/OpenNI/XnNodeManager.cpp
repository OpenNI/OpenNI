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
#include <XnOS.h>
#include <XnList.h>
#include <XnNode.h>
#include "XnNodeManager.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

const XnUInt32 XnNodeManager::nInitialSize = 20;

XnNodeManager::XnNodeManager()
{
	xnOSCreateCriticalSection(&m_hCriticalSection);
	//TODO: Handle critical section creation failure

	m_nCurrentAvailability = 0;
	m_nCurrentCapacity = 0;
	m_nCurrentOccupancy = 0;
	m_pFirstAvailable = NULL;

	m_eInitializationState = XN_NM_INIT_STATE_CREATE_INTERNAL_LIST;
	m_pAllNodes = XN_NEW(XnList, this);
	if (m_pAllNodes == NULL)
	{
		// OZOZ: Allocation failed in ctor...
	}

	m_eInitializationState = XN_NM_INIT_STATE_CREATE_FIRST_LINK;
	XnStatus rc = Resize(nInitialSize);
	if (rc != XN_STATUS_OK)
	{
		// OZOZ: Allocation failed in ctor...
	}

	m_eInitializationState = XN_NM_INIT_STATE_DONE;
}

XnNodeManager::~XnNodeManager()
{
	while (m_pAllNodes->begin() != m_pAllNodes->end())
	{
		XnValue RemovedValue;
		m_pAllNodes->Remove(m_pAllNodes->rbegin(), RemovedValue);
		XnNode* pNodes = (XnNode*)RemovedValue;
		XN_DELETE_ARR(pNodes);
	}

	XN_DELETE(m_pAllNodes);

	xnOSCloseCriticalSection(&m_hCriticalSection);
}

XnNodeManager* XnNodeManager::GetInstance()
{
	// NOTE: we *never* free the NodeManager instance, as it should always exist. Global variables dtors
	// might still use lists and hashs to do their work.
	// Instead, we let the OS free its memory and resources.
	static XnNodeManager* pNM = XN_NEW(XnNodeManager);
	return pNM;
}

XnNode* XnNodeManager::Allocate()
{
	XnNode* pResult = NULL;

	xnOSEnterCriticalSection(&m_hCriticalSection);

	if (m_eInitializationState == XN_NM_INIT_STATE_CREATE_INTERNAL_LIST)
	{
		pResult = &(m_InitialNodes[0]);
	}
	else if (m_eInitializationState == XN_NM_INIT_STATE_CREATE_FIRST_LINK)
	{
		pResult = &(m_InitialNodes[1]);
	}

	if (pResult != NULL)
	{
		xnOSLeaveCriticalSection(&m_hCriticalSection);
		return pResult;
	}

	// Check if resize is in order
	if (m_nCurrentAvailability == 1 ||
		XnFloat(m_nCurrentOccupancy)/m_nCurrentCapacity > 0.75)
	{
		XnStatus rc = Resize((XnUInt32)(m_nCurrentCapacity*0.5));
		if (rc != XN_STATUS_OK && m_nCurrentAvailability == 1)
		{
			// Couldn't resize, and there is only one node available
			// We'll need that one node to add to the list, to contain all new nodes
			// So there are actually no available nodes....
			xnOSLeaveCriticalSection(&m_hCriticalSection);
			return NULL;
		}
	}

	m_nCurrentOccupancy++;
	m_nCurrentAvailability--;

	// Return the first available
	XnNode* pOut = m_pFirstAvailable;
	m_pFirstAvailable = m_pFirstAvailable->Next();
	pOut->Next() = NULL;

	xnOSLeaveCriticalSection(&m_hCriticalSection);

	return pOut;
}

void XnNodeManager::Deallocate(XnNode *pNode)
{
	xnOSEnterCriticalSection(&m_hCriticalSection);

	m_nCurrentOccupancy--;
	m_nCurrentAvailability++;

	// Add the returned node as the first available
	pNode->Next() = m_pFirstAvailable;
	pNode->Previous() = NULL;
	m_pFirstAvailable = pNode;

	xnOSLeaveCriticalSection(&m_hCriticalSection);
}

XnStatus XnNodeManager::Resize(XnUInt32 nDeltaSize)
{
	// Allocate new nodes
	XnNode* pNewNodes = XN_NEW_ARR(XnNode, nDeltaSize);
	if (pNewNodes == NULL)
	{
		return XN_STATUS_ALLOC_FAILED;
	}

	// Connect them to each other
	for (XnUInt32 i = 0; i < nDeltaSize-1; ++i)
	{
		pNewNodes[i].Next() = &(pNewNodes[i+1]);
		pNewNodes[i].Previous() = NULL;
	}

	pNewNodes[nDeltaSize-1].Previous() = NULL;

	m_nCurrentAvailability += nDeltaSize;
	m_nCurrentCapacity += nDeltaSize;

	// Add the new nodes to the list
	m_pAllNodes->AddLast(XnValue(pNewNodes));
	// Replace first available with the first from this batch
	pNewNodes[nDeltaSize-1].Next() = m_pFirstAvailable;
	m_pFirstAvailable = &(pNewNodes[0]);

	return XN_STATUS_OK;
}
