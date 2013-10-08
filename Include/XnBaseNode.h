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
