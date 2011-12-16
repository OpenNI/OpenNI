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
#ifndef _NINODEALLOC_H
#define _NINODEALLOC_H
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDataTypes.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/**
 * The base of the building block of the data types - XnBaseNode
 */
typedef struct XnBaseNode
{
	/** the next XnNode */
	XnBaseNode* m_pNext;
	/** the previous XnNode */
	XnBaseNode* m_pPrevious;
	/** the value of the XnNode */
	XnValue m_Data;
} XnBaseNode;

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------
/**
 * Provide an available xnBaseNode from the node pool.
 */ 
XN_C_API XnBaseNode* XN_C_DECL xnAllocateBaseNode();

/**
 * Release an xnBaseNode to the node pool.
 * @param	pNode	[in]	The base node to return to the pool.
 */ 
XN_C_API void XN_C_DECL xnDeallocateBaseNode(XnBaseNode* pNode);

#endif //_NINODEALLOC_H
