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
#ifndef _XN_NODE_H_
#define _XN_NODE_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDataTypes.h>
#include <XnBaseNode.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/**
 * The building block of the data types - XnNode
 */
class XnNode : protected XnBaseNode
{
public:
	static XnNode *Allocate() { return (XnNode*)xnAllocateBaseNode(); }
	static void Deallocate(XnNode *pNode) { xnDeallocateBaseNode((XnBaseNode*)pNode); }

	/**
	* Constructor. Initialize internal representations
	*/
	XnNode() { m_pNext = NULL; m_pPrevious = NULL; m_Data = NULL; }

	/**
	* Get the next XnNode (non-const version)
	*/
	XnNode*& Next() {return (XnNode*&)m_pNext;}
	/**
	* Get the next XnNode (const version)
	*/
	const XnNode* const Next() const {return (const XnNode*)m_pNext;}

	/**
	* Get the previous XnNode (non-const version)
	*/
	XnNode*& Previous() {return (XnNode*&)m_pPrevious;}
	/**
	* Get the previous XnNode (non-const version)
	*/
	const XnNode* const Previous() const {return (const XnNode*)m_pPrevious;}

	/**
	* Get the value in the node (non-const version)
	*/
	XnValue& Data() {return m_Data;}
	/**
	* Get the value in the node (non-const version)
	*/
	const XnValue& Data() const {return m_Data;}
};

#endif // _XN_NODE_H_
