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
