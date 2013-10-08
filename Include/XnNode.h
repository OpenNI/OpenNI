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
