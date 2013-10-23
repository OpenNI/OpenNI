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
#ifndef _XN_NODE_MANAGER_H
#define _XN_NODE_MANAGER_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnNode.h>
#include <IXnNodeAllocator.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnList;

/**
 * Node manager - a general pool of XnNode instances.
 * The Node Manager is a Singleton, providing any interested party with XnNodes.
 */
class XnNodeManager : public INiNodeAllocator
{
public:
	/**
	* Get a reference to the single instance of the XnNodeManager
	*/
	static XnNodeManager* GetInstance();

	/**
	* Destructor. Destroy internal representations
	*/
	~XnNodeManager();

	/**
	* Provide an available XnNode from the pool
	* 
	* @return	NULL	if no nodes are available and allocating new ones failed
	*/
	virtual XnNode* Allocate();
	/**
	* Release an XnNode to the pool
	* 
	* @param	pNode	[in]	The node to return to the pool
	*/
	virtual void Deallocate(XnNode* pNode);

protected:
	/**
	* Add more nodes to the pool. 
	* 
	* @param	nDeltaSize	[in]	Number of new nodes required
	*/
	XnStatus Resize(XnUInt32 nDeltaSize);

	/** 2 nodes for the internal XnList implementation */
	XnNode m_InitialNodes[2];
	/** pointer to the first available node in the pool */
	XnNode* m_pFirstAvailable;

	/** List of nodes. Each node's valus is a XnNode[] */
	XnList* m_pAllNodes;

	/** Initial size of the pool */
	static const XnUInt32 nInitialSize;

	/** Current capacity of the pool */
	XnUInt32 m_nCurrentCapacity;
	/** Number of nodes from the pool currently used  */
	XnUInt32 m_nCurrentOccupancy;
	/** Number of nodes from the pool currently available */
	XnUInt32 m_nCurrentAvailability;

	/** Mutual exclusion lock */
	XN_CRITICAL_SECTION_HANDLE m_hCriticalSection;
private:
	/**
	* Constructor. Initialize internal representations
	*/
	XnNodeManager();
	/** Initialization state, to supply internal XnList implementation with the special Nodes */
	enum {
		XN_NM_INIT_STATE_CREATE_INTERNAL_LIST,
		XN_NM_INIT_STATE_CREATE_FIRST_LINK,
		XN_NM_INIT_STATE_DONE
	} m_eInitializationState;
};

#endif // _XN_NODE_MANAGER_H
