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
#ifndef _NISIMPLENODEALLOCATOR_H
#define _NISIMPLENODEALLOCATOR_H

#include <IXnNodeAllocator.h>
#include <XnNode.h>

class XnNodeAllocator : public INiNodeAllocator
{
public:
	/**
	* Allocate a new XnNode
	* 
	* @return	NULL	if allocating a new node failed
	*/
	virtual XnNode *Allocate() { return XnNode::Allocate();	}

	/**
	* Release an XnNode
	* 
	* @param	pNode	[in]	The node to deallocate
	*/
	virtual void Deallocate(XnNode *pNode) { XnNode::Deallocate(pNode); }
};

#endif //_NISIMPLENODEALLOCATOR_H
