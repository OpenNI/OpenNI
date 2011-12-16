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
