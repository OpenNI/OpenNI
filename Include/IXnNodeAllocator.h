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
#ifndef _ININODEALLOCATOR_H
#define _ININODEALLOCATOR_H

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnNode;

/**
 * Node Allocator - general interface for node allocation and deallocation.
 */
class INiNodeAllocator
{
public:
	virtual ~INiNodeAllocator() {}

	/**
	* Allocate a new XnNode
	* 
	* @return	NULL	if allocating a new node failed
	*/
	virtual XnNode *Allocate() = 0;

	/**
	* Release an XnNode
	* 
	* @param	pNode	[in]	The node to deallocate
	*/
	virtual void Deallocate(XnNode *pNode) = 0;
};

#endif //_ININODEALLOCATOR_H
